#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <cctype>
#include "frontend.h"

void print_error_info(const std::string &s, const std::vector<int> &error_info) {
    for (decltype(error_info.size()) i = 0;
        i != error_info.size(); ++i) {
        if (error_info[i]) {
            std::cout << "The character " << s[i] << " at index " << i << " can not be recognized." << '\n';
        }
    }
}

int count_token_char(const frontend::Token &token) {
    int ret = 0;
    if (token.type_ == frontend::TokenType::INTEGER) {
        int value = token.value_;
        while (value) {
            ret += 1;
            value /= 10;
        }
    } else if (token.type_ == frontend::TokenType::END) {
        // Do nothing.
    } else {
        ++ret;
    }

    return ret;
}
void print_error_info(const std::string &s, const std::vector<frontend::Token> &tokens, const std::vector<int> &error_info) {
    int count_char = 0;
    // 不需要打印最后一个人为添加的 end token
    for (decltype(error_info.size()) i = 0; i + 1 <  error_info.size(); ++i) {
        if (error_info[i]) {
            std::cout << s << std::endl;
            for (int i = 0; i < count_char; ++i) {
                std::cout << " ";
            }
            std::cout << "^" << std::endl;
            std::cout << "Token " << tokens[i] << " at index " << i << " doesn't match.\n" << std::endl;
        }
        count_char += count_token_char(tokens[i]);
    }
}

void print_tokens(const std::vector<frontend::Token> &tokens) {
    for (const auto &token : tokens) {
        std::cout << token << std::endl;
    }
}
void test_parse_success() {
    std::cout << "----- " << __func__ << " -----" << std::endl;

    std::string s = "12+34/2-9*4";
    std::cout << "String: " << s << std::endl;

    frontend::Lexer lexer;
    bool success = lexer.lex_entire_string(s);

    frontend::Parser parser(lexer.get_tokens());
    if (parser.success()) {
        std::cout << "parse success" << std::endl;
        // parser.show_cst();
    } else {
        assert(0); // Shouldn't be here.
    }
}

void test_parse_fail() {
    std::cout << "----- " << __func__ << " -----" << std::endl;

    std::string s = "12++34*/2-9*4";
    std::cout << "String: " << s << std::endl;

    frontend::Lexer lexer;
    lexer.lex_entire_string(s);

    frontend::Parser parser(lexer.get_tokens());
    if (parser.success()) {
        assert(0); // Shouldn't be here.
    } else {
        print_error_info(s, parser.get_tokens(), parser.get_error_info());
    }

}

void test_parse_all() {
    std::cout << "-------------------- " << __func__ << " --------------------" << std::endl;
    test_parse_success();
    test_parse_fail();
}

void test_lex_success() {
    std::cout << "----- " << __func__ << " -----" << std::endl;

    std::string s = "12+34/2-9*4";
    std::cout << "String: " << s << std::endl;
    std::cout << "After lexing: " << std::endl;

    frontend::Lexer lexer;
    bool success = lexer.lex_entire_string(s);

    if (success) {
        print_tokens(lexer.get_tokens());
    } else {
        assert(0); // Shouldn't be here
    }
}

void test_lex_fail() {
    std::cout << "----- " << __func__ << " -----" << std::endl;

    std::string s = "1a+3e/2-9*$";
    std::cout << "String: " << s << std::endl;
    std::cout << "After lexing: " << std::endl;

    frontend::Lexer lexer;
    bool success = lexer.lex_entire_string(s);

    if (success) {
        assert(0); // Shouldn't be here
    } else {
        print_error_info(s, lexer.get_error_info());
    }
}

void test_lex_from_stdin() {
    std::cout << "----- " << __func__ << " -----" << std::endl;

    std::cout << "Please enter the string to be lexically analyzed: " << std::endl;
    std::string s;
    std::string response;

    frontend::Lexer lexer;
    while (std::cin >> s) {
        std::cout << "String: " << s << std::endl;
        std::cout << "After lexing: " << std::endl;

        bool success = lexer.lex_entire_string(s);

        if (success) {
            print_tokens(lexer.get_tokens());
        } else {
            print_error_info(s, lexer.get_error_info());
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
    // test_lex_all();
    test_parse_all();

    return 0;
}
