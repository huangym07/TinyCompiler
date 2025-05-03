#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include "parser.h"

namespace parser {
    std::ostream& operator << (std::ostream &os, const TokenType &type) {
        switch (type) {
            case TokenType::NUMBER:
                os << "n";
                break;
            case TokenType::OPERATOR:
                os << "o";
                break;
            default:
                os << "Unknown TokenType";
                break;
        }

        return os;
    }

    bool Lexer::lex_entire_string(const std::string &s) {
        lex_success = true;

        tokens_.clear();
        error_info_.clear();
        error_info_.assign(s.size(), 0);

        int num = 0;

        for (decltype(s.size()) i = 0; i != s.size(); ++i) {
            if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/') {
                Token token = {TokenType::OPERATOR, s[i]};
                tokens_.push_back(token);
            } else if (isdigit(s[i])) {
                num = 0;
                while (i != s.size() && isdigit(s[i])) {
                    num = num * 10 + s[i] - '0';
                    ++i;
                }
                --i;

                Token token = {TokenType::NUMBER, num};
                tokens_.push_back(token);
            } else {
                error_info_[i] = 1;
                lex_success = false;
            }
        }

        return lex_success;
    }
}
