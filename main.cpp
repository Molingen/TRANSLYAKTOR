#include <translator.hpp>
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

int main() {
    cout << "Arithmetic expression translator\n";
    cout << "Supported operations: +, -, *, (, ), /\n";
    cout << "Enter expression (or 'exit' for quit):\n\n";
    
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
            cout << "  The expression is correct\n";
            cout << "  Tokens:    " << translator::tokensToString(result.tokens) << "\n";
            cout << "  RPN:       " << translator::tokensToString(result.rpnTokens) << "\n";
            cout << "  Result: " << fixed << setprecision(6) << result.value << "\n";
        } else {
            cout << "  " << result.error << "\n";
        }
        
        cout << "\n";
    }
    
    return 0;
}