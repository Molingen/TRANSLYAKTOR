#include <translator.hpp>
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

int main() {
    cout << "Трансляктор арифметических выражений\n";
    cout << "Поддерживаемые операции: +, -, *, (, ), /\n";
    cout << "Введите выражение (или 'exit' для выхода):\n\n";
    
    string line;
    while (true) {
        cout << "> ";
        if (!getline(cin, line)) {
            break;
        }
        
        if (line == "exit") {
            break;
        }
        
        if (line.empty()) {
            continue;
        }
        
        auto result = translator::computeExpression(line);
        
        if (result.success) {
            cout << "  Выражение корректно\n";
            cout << "  Токены:    " << translator::tokensToString(result.tokens) << "\n";
            cout << "  ОПН:       " << translator::tokensToString(result.rpnTokens) << "\n";
            cout << "  Результат: " << fixed << setprecision(6) << result.value << "\n";
        } else {
            cout << "  " << result.error << "\n";
        }
        
        cout << "\n";
    }
    
    return 0;
}