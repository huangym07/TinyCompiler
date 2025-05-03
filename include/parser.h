#ifndef __LEX_H__
#define __LEX_H__

#include <iostream>
#include <vector>
#include <string>

namespace parser {
    enum class TokenType {
        NUMBER,
        OPERATOR,
        END
    };
    std::ostream& operator << (std::ostream &os, const TokenType &type);

    struct Token {
        TokenType type;
        int value;
    };

    class Lexer {
    public:
        Lexer() = default;

        bool lex_entire_string(const std::string &);

        const std::vector<Token>& get_tokens() const { return tokens_; }
        const std::vector<int>& get_error_info() const { return error_info_; }

        bool success() const { return lex_success; }

    private:
        bool lex_success;
        std::vector<Token> tokens_;
        std::vector<int> error_info_;
    };
}

#endif // __LEX_H__
