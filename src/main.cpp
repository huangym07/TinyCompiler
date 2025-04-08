#include <iostream>
#include <vector>
#include <string>

template<class TokenType, class ErrorInfoType>
class Lexer {
  public:
    std::vector<TokenType> getTokens() { return tokens; }
    std::vector<ErrorInfoType> getErrorInfo() { return ei; }

    void lex(const std::string& s);

  private:
    std::vector<TokenType> tokens;
    std::vector<ErrorInfoType> ei;
    enum class State {
        START,
        NUM,
    };
};

// 词法分析结果 Token
struct Token {
    char token_class;   // 类型
    int token_value;    // 值
};

// 词法分析
// input: 
//      std::string s       待词法分析字符串
// 起始状态     数字->      数字状态    
// 数字状态     数字->      数字状态
// 数字状态     非数字->    识别数字
// 起始状态     四则算符->  识别算符
// 起始状态     其他->      识别出错
// 算法描述：
//      初始为起始状态，当前字符为数字，转移到数字状态
//
//      数字状态遇见数字，转移到数字状态
//      数字状态遇见非数字，转移到识别数字状态（终止状态）
//      记录当前数字，返回到起始状态，并将字符指针减 1
//
//      初始状态遇见算符，转移到识别算符状态（终止状态）
//      记录当前 token，并返回初始状态
//
//      初始状态遇见其他字符，转移到识别错误状态（终止状态）
//      记录当前错误位置，并返回初始状态
template<>
void Lexer<Token, int>::lex(const std::string& s) {
    int n = s.size();

    ei.resize(0);
    ei.resize(n);
    tokens.resize(0);
    
    State state = State::START;
    int num = 0;
    for (int i = 0; i < n; i++) {
        char ch = s[i];
        switch (state) {
            case State::START:
                if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
                    Token token = {'o', ch};
                    tokens.push_back(token);
                } else if (ch >= '0' && ch <= '9') {
                    num = ch - '0';
                    state = State::NUM;
                } else {
                    ei[i] = 1;
                }
                break;
            case State::NUM:
                if (ch >= '0' && ch <= '9') {
                    num = num * 10 + ch - '0';
                } else {
                    Token token = {'n', num};
                    tokens.push_back(token);
                    num = 0;
                    state = State::START;
                    i--;
                }
                break;
        }
    }
    if (state == State::NUM) {
        Token token = {'n', num};
        tokens.push_back(token);
    }
}



int main() {
    Lexer<Token, int> lexer;

    std::string s;
    while (std::cin >> s) {
        lexer.lex(s);
        std::vector<Token> tokens = lexer.getTokens();
        std::vector<int> ei = lexer.getErrorInfo();

        std::cout << "Result after lexing: " << '\n';
        bool has_error = false;
        for (int i = 0; i < ei.size(); i++) {
            if (ei[i] == 1) {
                std::cout << "下标为 " << i << " 的字符 " << s[i] << " 无法识别" << '\n';
                has_error = true;
            }
        }
        
        if (!has_error) {
            for (auto& token : tokens) {
                char type = token.token_class;
                int value = token.token_value;
                if (type == 'n') {
                    std::cout << "(" << type << ", " << value  << ")" << '\n';
                } else {
                    std::cout << "(" << type << ", " << (char)value  << ")" << '\n';
                }
            }
        }
    }
}
