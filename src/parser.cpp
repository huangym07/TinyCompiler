#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cassert>
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
        
        if (lex_success) tokens_.push_back({TokenType::END, '#'});
        return lex_success;
    }


    std::ostream& operator << (std::ostream &os, const CstNodeType &type) {
        switch (type) {
            case CstNodeType::E:
                os << "E";
                break;
            case CstNodeType::A:
                os << "A";
                break;
            case CstNodeType::B: 
                os << "B";
                break;
            case CstNodeType::T:
                os << "T";
                break;
            case CstNodeType::OPERATOR:
                os << "OPERATOR";
                break;
            case CstNodeType::NUMBER:
                os << "NUMBER";
                break;
            case CstNodeType::EMPTY:
                os << "EMPTY";
                break;
            default:
                os << "Unknown CstNodeType";
                break;
        }

        return os;
    }

    Parser::Parser(const std::vector<Token> &tokens)
        : current_token_index_(0), tokens_(tokens), error_info_(tokens.size(), 0), parse_success(true) {
        cst_root_index_ = E();
    }

    std::vector<CstNode>::size_type Parser::E() {
        assert(current_token_index_ <= tokens_.size());

#ifndef NDEBUG
        std::cout << __func__ << " and current_token_index is " << current_token_index_ << std::endl;
#endif  

        CstNode cst_node;
        cst_node.is_leaf = false;
        cst_node.type = CstNodeType::E;

        while (current_token_index_ != tokens_.size()) {
            const Token &token = tokens_[current_token_index_];
            if (token.type == TokenType::NUMBER) {                      // token in Predict(E->TA)
                cst_node.son_node_indexs.push_back(T());
                cst_node.son_node_indexs.push_back(A());
                cst_.push_back(cst_node);
                break;
            } else {                                                    // error: token 无法识别，记录错误位置，丢弃当前 token
                                                                        // 以当前状态继续尝试下一个 token
                error_info_[current_token_index_] = 1;
                ++current_token_index_;
                parse_success = false;
            }
        }
        
#ifndef NDEBUG
        std::cout << __func__ << " and current_token_index is " << current_token_index_ << std::endl;
#endif  
        return cst_.size() - 1;
    }

    std::vector<CstNode>::size_type Parser::A() {
        assert(current_token_index_ <= tokens_.size());

#ifndef NDEBUG
        std::cout << __func__ << " and current_token_index is " << current_token_index_ << std::endl;
#endif  

        CstNode cst_node;
        cst_node.is_leaf = false;
        cst_node.type = CstNodeType::A;

        while (current_token_index_ != tokens_.size()) {
            const Token &token = tokens_[current_token_index_];
            if (token.type == TokenType::OPERATOR) {                            // token in Predict(A->+TA)
                if (token.value == '+') {
                    Token should_be_token = {TokenType::OPERATOR, '+'};
                    cst_node.son_node_indexs.push_back(Match(should_be_token));
                } else if (token.value == '-') {                                // token in Predict(A->-TA)
                    Token should_be_token = {TokenType::OPERATOR, '-'};
                    cst_node.son_node_indexs.push_back(Match(should_be_token));
                } else {                                                        // error
                    error_info_[current_token_index_] = 1;
                    ++current_token_index_;
                    parse_success = false;
                    continue;
                }
                cst_node.son_node_indexs.push_back(T());
                cst_node.son_node_indexs.push_back(A());
                cst_.push_back(cst_node);
                break;
            } else if (token.type == TokenType::END) {
                Token should_be_token = {TokenType::END, '#'};
                cst_node.son_node_indexs.push_back(Match(should_be_token));
                break;
            } else {                                                            // error: token 无法识别，记录错误位置，丢弃当前 token
                                                                                // 以当前状态继续尝试下一个 token
                error_info_[current_token_index_] = 1;
                ++current_token_index_;
                parse_success = false;
            }
        }
        
#ifndef NDEBUG
        std::cout << __func__ << " and current_token_index is " << current_token_index_ << std::endl;
#endif  
        return cst_.size() - 1;
    }

    std::vector<CstNode>::size_type Parser::B() {
        assert(current_token_index_ <= tokens_.size());

#ifndef NDEBUG
        std::cout << __func__ << " and current_token_index is " << current_token_index_ << std::endl;
#endif  

        CstNode cst_node;
        cst_node.is_leaf = false;
        cst_node.type = CstNodeType::B;

        while (current_token_index_ != tokens_.size()) {
            const Token &token = tokens_[current_token_index_];
            if (token.type == TokenType::OPERATOR) {                            // token in Predict(B->*FB) or Predict(B->/FB) or Predict(B->\epsilon)
                if (token.value == '+' || token.value == '-') {
#ifndef NDEBUG
                    std::cout << __func__ << " +-" << std::endl;
#endif  
                    Token should_be_token = {TokenType::END, '#'};
                    cst_node.son_node_indexs.push_back(Match(should_be_token));
                } else if (token.value == '*') {
#ifndef NDEBUG
                    std::cout << __func__ << " *" << std::endl;
#endif  
                    Token should_be_token = {TokenType::OPERATOR, '*'};
                    cst_node.son_node_indexs.push_back(Match(should_be_token));
                    cst_node.son_node_indexs.push_back(F());
                    cst_node.son_node_indexs.push_back(B());
                } else {
#ifndef NDEBUG
                    std::cout << __func__ << " /" << std::endl;
#endif  
                    Token should_be_token = {TokenType::OPERATOR, '/'};
                    cst_node.son_node_indexs.push_back(Match(should_be_token));
                    cst_node.son_node_indexs.push_back(F());
                    cst_node.son_node_indexs.push_back(B());
                }
                cst_.push_back(cst_node);
                break;
            } else if (token.type == TokenType::END) {                          // token in Predict(B->\epsilon)

#ifndef NDEBUG
                std::cout << __func__ << " END" << std::endl;
#endif  
                Token should_be_token = {TokenType::END, '#'};
                cst_node.son_node_indexs.push_back(Match(should_be_token));
                cst_.push_back(cst_node);
                break;
            } else {                                                            // error: token 无法识别，记录错误位置，丢弃当前 token
                                                                                // 以当前状态继续尝试下一个 token
                error_info_[current_token_index_] = 1;
                ++current_token_index_;
                parse_success = false;
            }
        }
        
#ifndef NDEBUG
        std::cout << __func__ << " and current_token_index is " << current_token_index_ << std::endl;
#endif  
        return cst_.size() - 1;
    }

    std::vector<CstNode>::size_type Parser::F() {
        assert(current_token_index_ <= tokens_.size());

#ifndef NDEBUG
        std::cout << __func__ << " and current_token_index is " << current_token_index_ << std::endl;
#endif  

        CstNode cst_node;
        cst_node.is_leaf = false;
        cst_node.type = CstNodeType::F;

        while (current_token_index_ != tokens_.size()) {
            const Token &token = tokens_[current_token_index_];
            if (token.type == TokenType::NUMBER) {                              // token in Predict(F->i)
                Token should_be_token = {TokenType::NUMBER, 0};
                cst_node.son_node_indexs.push_back(Match(should_be_token));
                cst_.push_back(cst_node);
                break;
            } else {                                                            // error: token 无法识别，记录错误位置，丢弃当前 token
                                                                                // 以当前状态继续尝试下一个 token
                error_info_[current_token_index_] = 1;
                ++current_token_index_;
                parse_success = false;
            }
        }
        
#ifndef NDEBUG
        std::cout << __func__ << " and current_token_index is " << current_token_index_ << std::endl;
#endif
        return cst_.size() - 1;
    }

    std::vector<CstNode>::size_type Parser::T() {
        assert(current_token_index_ <= tokens_.size());

#ifndef NDEBUG
        std::cout << __func__ << " and current_token_index is " << current_token_index_ << std::endl;
#endif

        CstNode cst_node;
        cst_node.is_leaf = false;
        cst_node.type = CstNodeType::T;

        while (current_token_index_ != tokens_.size()) {
            const Token &token = tokens_[current_token_index_];
            if (token.type == TokenType::NUMBER) {                              // token in Predict(T->FB)
                cst_node.son_node_indexs.push_back(F());
                cst_node.son_node_indexs.push_back(B());
                cst_.push_back(cst_node);
                break;
            } else {                                                            // error
                error_info_[current_token_index_] = 1;
                ++current_token_index_;
                parse_success = false;
            }

        }

#ifndef NDEBUG
        std::cout << __func__ << " and current_token_index is " << current_token_index_ << std::endl;
#endif
        return cst_.size() - 1;
    }

    std::vector<CstNode>::size_type Parser::Match(const Token &should_be_token) {
        assert(current_token_index_ <= tokens_.size());
        
#ifndef NDEBUG
        std::cout << __func__ << " and current_token_index is " << current_token_index_ << std::endl;
#endif

        CstNode cst_node;
        cst_node.is_leaf = true;

        while (current_token_index_ != tokens_.size()) {
            const Token &token = tokens_[current_token_index_];
            if ((token.type == should_be_token.type && token.value == should_be_token.value)
                || (token.type == should_be_token.type && should_be_token.type == TokenType::NUMBER)
                || should_be_token.type == TokenType::END) {
                if (should_be_token.type == TokenType::OPERATOR) {
                    cst_node.type = CstNodeType::OPERATOR;
                    ++current_token_index_;
                } else if (should_be_token.type == TokenType::NUMBER) {
                    cst_node.type = CstNodeType::NUMBER;
                    ++current_token_index_;
                } else {        // #
                    cst_node.type = CstNodeType::EMPTY;
                }
                cst_node.value = token.value;
                cst_.push_back(cst_node);
                break;
            } else {            // error
                error_info_[current_token_index_] = 1;
                ++current_token_index_;
                parse_success = false;
            }
        }

#ifndef NDEBUG
        std::cout << __func__ << " and current_token_index is " << current_token_index_ << std::endl;
#endif
        return cst_.size() - 1;
    }

}
