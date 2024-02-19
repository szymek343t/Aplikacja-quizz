#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

// Struktura przechowująca pytanie i odpowiedzi
struct Question {
    string text;
    vector<string> answers;
    vector<int> correctAnswers;
};

// Funkcja do wyświetlania pytań i pobierania odpowiedzi od użytkownika
vector<int> askQuestion(const Question& question) {
    cout << question.text << endl;
    for (int i = 0; i < question.answers.size(); i++) {
        cout << i + 1 << ". " << question.answers[i] << endl;
    }
    cout << "Podaj odpowiedzi (oddzielone spacja): ";
    string input;
    getline(cin, input);
    vector<int> selectedAnswers;
    size_t pos = 0;
    while ((pos = input.find(' ')) != string::npos) {
        int answerIndex = stoi(input.substr(0, pos)) - 1;
        selectedAnswers.push_back(answerIndex);
        input.erase(0, pos + 1);
    }
    int answerIndex = stoi(input) - 1;
    selectedAnswers.push_back(answerIndex);
    return selectedAnswers;
}

// Funkcja do losowego mieszania pytań w teście
vector<Question> shuffleQuestions(const vector<Question>& questions) {
    vector<Question> shuffledQuestions = questions;
    random_shuffle(shuffledQuestions.begin(), shuffledQuestions.end());
    return shuffledQuestions;
}

// Funkcja do zapisywania testu do pliku
void saveTest(const vector<Question>& questions, const string& filename) {
    ofstream file(filename);
    file << questions.size() << endl;
    for (const auto& question : questions) {
        file << question.text << endl;
        file << question.answers.size() << endl;
        for (const auto& answer : question.answers) {
            file << answer << endl;
        }
        file << question.correctAnswers.size() << endl;
        for (const auto& correctAnswer : question.correctAnswers) {
            file << correctAnswer << endl;
        }
    }
    file.close();
}

// Funkcja do wczytywania testu z pliku
vector<Question> loadTest(const string& filename) {
    ifstream file(filename);
    int numQuestions;
    file >> numQuestions;
    vector<Question> questions(numQuestions);
    for (int i = 0; i < numQuestions; i++) {
        Question& question = questions[i];
        getline(file >> ws, question.text);
        int numAnswers;
        file >> numAnswers;
        question.answers.resize(numAnswers);
        for (auto& answer : question.answers) {
            getline(file >> ws, answer);
        }
        int numCorrectAnswers;
        file >> numCorrectAnswers;
        question.correctAnswers.resize(numCorrectAnswers);
        for (auto& correctAnswer : question.correctAnswers) {
            file >> correctAnswer;
        }
    }
    file.close();
    return questions;
}

// Funkcja do rozwiązywania testu
void takeTest(const vector<Question>& questions, int timeLimitSeconds) {
    cout << "Masz " << timeLimitSeconds << " sekund na rozwiazanie testu." << endl;
    cout << "Wcisnij Enter, aby rozpoczac test." << endl;
    cin.get();
    vector<Question> shuffledQuestions = shuffleQuestions(questions);
    int numQuestions = shuffledQuestions.size();
    int numCorrectAnswers = 0;
    time_t startTime = time(nullptr);
    for (int i = 0; i < numQuestions; i++) {
        vector<int> selectedAnswers = askQuestion(shuffledQuestions[i]);
        bool isCorrect = true;
        for (const auto& correctAnswer : shuffledQuestions[i].correctAnswers) {
            if (find(selectedAnswers.begin(), selectedAnswers.end(), correctAnswer) == selectedAnswers.end()) {
                isCorrect = false;
                break;
            }
        }
        if (isCorrect) {
            numCorrectAnswers++;
            cout << "Odpowiedz poprawna." << endl;
        }
        else {
            cout << "Odpowiedz bledna." << endl;
        }
    }
    time_t endTime = time(nullptr);
    int timeElapsedSeconds = static_cast<int>(difftime(endTime, startTime));
    int timeRemainingSeconds = max(0, timeLimitSeconds - timeElapsedSeconds);
    cout << "Twoj wynik to " << numCorrectAnswers << "/" << numQuestions << "." << endl;
    cout << "Czas rozwiazania testu: " << timeElapsedSeconds << " sekund." << endl;
    cout << "Czas pozostaly na rozwiazanie testu: " << timeRemainingSeconds << " sekund." << endl;
}

vector<Question> createTest() {
    vector<Question> questions;

    // Pobieranie liczby pytań od użytkownika
    int numQuestions;
    cout << "Podaj liczbe pytan: ";
    cin >> numQuestions;
    cin.ignore(); // Ignorowanie znaku nowej linii pozostawionego w strumieniu wejścia przez cin

    // Pobieranie specyfikacji pytań i odpowiedzi od użytkownika
    for (int i = 0; i < numQuestions; i++) {
        Question question;
        cout << "Podaj tresc pytania #" << i + 1 << ": ";
        getline(cin, question.text);
        int numAnswers;
        cout << "Podaj liczbe odpowiedzi: ";
        cin >> numAnswers;
        cin.ignore(); // Ignorowanie znaku nowej linii pozostawionego w strumieniu wejścia przez cin
        for (int j = 0; j < numAnswers; j++) {
            string answer;
            cout << "Podaj tresc odpowiedzi #" << j + 1 << ": ";
            getline(cin, answer);
            question.answers.push_back(answer);
        }
        int numCorrectAnswers;
        cout << "Podaj liczbe poprawnych odpowiedzi: ";
        cin >> numCorrectAnswers;
        cin.ignore(); // Ignorowanie znaku nowej linii pozostawionego w strumieniu wejścia przez cin
        for (int j = 0; j < numCorrectAnswers; j++) {
            int correctAnswer;
            cout << "Podaj numer poprawnej odpowiedzi #" << j + 1 << ": ";
            cin >> correctAnswer;
            question.correctAnswers.push_back(correctAnswer - 1);
        }
        questions.push_back(question);
    }

    return questions;
}


int main() {
    vector<Question> questions;
    int choice;

    while (true) {
        cout << "Wybierz opcje:" << endl;
        cout << "1. Utworz nowy test" << endl;
        cout << "2. Rozwiaz istniejacy test" << endl;
        cout << "3. Zapisz test do pliku" << endl;
        cout << "4. Wczytaj test z pliku" << endl;
        cout << "5. Wyjscie" << endl;
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            questions = createTest();
            break;
        case 2:
            if (questions.empty()) {
                cout << "Nie ma zadnych pytan. Utworz najpierw test." << endl;
            }
            else {
                int timeLimitSeconds;
                cout << "Podaj limit czasu na rozwiazanie testu (w sekundach): ";
                cin >> timeLimitSeconds;
                cin.ignore();
                takeTest(questions, timeLimitSeconds);
            }
            break;
        case 3:
            if (questions.empty()) {
                cout << "Nie ma zadnych pytan. Utworz najpierw test." << endl;
            }
            else {
                string filename;
                cout << "Podaj nazwe pliku: ";
                getline(cin, filename);
                saveTest(questions, filename);
            }
            break;
        case 4:
        {
            string filename;
            cout << "Podaj nazwe pliku: ";
            getline(cin, filename);
            questions = loadTest(filename);
        }
        break;
        case 5:
            return 0;
        default:
            cout << "Nieprawidlowa opcja. Sprobuj jeszcze raz." << endl;
            break;
        }
    }
}
