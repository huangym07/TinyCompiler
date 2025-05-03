#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <cctype>
#include "parser.h"

void print_error_info(const std::string &s, const std::vector<int> &error_info) {
    for (decltype(error_info.size()) i = 0;
        i != error_info.size(); ++i) {
        if (error_info[i]) {
            std::cout << "The character " << s[i] << " at index " << i << " can not be recognized." << '\n';
        }
    }
}

void print_tokens(const std::vector<parser::Token> &tokens) {
    for (const auto &token : tokens) {
        if (token.type == parser::TokenType::NUMBER) {
            std::cout << "(" << token.type << ", " << token.value << ")" << '\n';
        } else if (token.type == parser::TokenType::OPERATOR) {
            std::cout << "(" << token.type << ", " << static_cast<char>(token.value) << ")" << '\n';
        } else {
            assert(1); // Shouldn't be here
        }
    }
}

void test_lex_success() {
    std::cout << "----- " << __func__ << " -----" << std::endl;

    std::string s = "12+34/2-9*4";
    std::cout << "String: " << s << std::endl;
    std::cout << "After lexing: " << std::endl;

    parser::Lexer lexer;
    bool success = lexer.lex_entire_string(s);

    if (success) {
        const auto &tokens = lexer.get_tokens();
        print_tokens(tokens);
    } else {
        assert(1); // Shouldn't be here
    }
}

void test_lex_fail() {
    std::cout << "----- " << __func__ << " -----" << std::endl;

    std::string s = "1a+3e/2-9*$";
    std::cout << "String: " << s << std::endl;
    std::cout << "After lexing: " << std::endl;

    parser::Lexer lexer;
    bool success = lexer.lex_entire_string(s);

    if (success) {
        assert(1); // Shouldn't be here
    } else {
        const auto &error_info = lexer.get_error_info();
        print_error_info(s, error_info);
    }
}

void test_lex_from_stdin() {
    std::cout << "----- " << __func__ << " -----" << std::endl;

    std::cout << "Please enter the string to be lexically analyzed: " << std::endl;
    std::string s;
    std::string response;

    parser::Lexer lexer;
    while (std::cin >> s) {
        std::cout << "String: " << s << std::endl;
        std::cout << "After lexing: " << std::endl;

        bool success = lexer.lex_entire_string(s);

        if (success) {
            const auto &tokens = lexer.get_tokens();
            print_tokens(tokens);
        } else {
            const auto &error_info = lexer.get_error_info();
            print_error_info(s, error_info);
        }

        std::cout << "Continue? Enter yes or no: " << std::endl;
        std::cin >> response;
        if (!std::cin || tolower(response[0]) == 'n') {
            break;
        }
    }
}

void test_lex_all() {
    std::cout << "-------------------- " << __func__ << " --------------------" << std::endl;
    test_lex_success();
    test_lex_fail();
    test_lex_from_stdin();
}

int main() {
    test_lex_all();

    return 0;
}
