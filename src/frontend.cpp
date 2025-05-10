#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cctype>
#include <cassert>
#include "frontend.h"

namespace frontend {
    std::ostream& operator << (std::ostream &os, const Token &token) {
        switch (token.type_) {
            case TokenType::PLUS:
            case TokenType::MINUS:
            case TokenType::TIMES:
            case TokenType::DIVIDE:
                os << "(o, " << static_cast<char>(token.value_) << ")";
                break;
            
            case TokenType::INTEGER:
                os << "(n, " << token.value_ << ")";
                break;
            
            case TokenType::END:
                // do nothing
                break;
            
            default:
                os << "Unknown token type";
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
            if (s[i] == '+') {
                tokens_.push_back({TokenType::PLUS, s[i]});
            } else if (s[i] == '-') {
                tokens_.push_back({TokenType::MINUS, s[i]});
            } else if (s[i] == '*') {
                tokens_.push_back({TokenType::TIMES, s[i]});
            } else if (s[i] == '/') {
                tokens_.push_back({TokenType::DIVIDE, s[i]});
            } else if (isdigit(s[i])) {
                num = 0;
                while (i != s.size() && isdigit(s[i])) {
                    num = num * 10 + s[i] - '0';
                    ++i;
                }
                --i;
                tokens_.push_back({TokenType::INTEGER, num});
            } else {
                error_info_[i] = 1;
                lex_success = false;
            }
        }
        
        return lex_success;
    }

    std::ostream& operator << (std::ostream &os, const CstNode &cst_node) {
        switch (cst_node.type_) {
            // non-leaf node
            case CstNodeType::E:
                os << "(E, )";
                break;
            case CstNodeType::A:
                os << "(A, )";
                break;
            case CstNodeType::B:
                os << "(B, )";
                break;
            case CstNodeType::F:
                os << "(F, )";
                break;
            case CstNodeType::T:
                os << "(T, )";
                break;

            // leaf node
            // token node
            case CstNodeType::PLUS:
            case CstNodeType::MINUS:
            case CstNodeType::TIMES:
            case CstNodeType::DIVIDE:
            case CstNodeType::INTEGER:
                os << cst_node.token_;
                break;
            // non-token/empty node
            case CstNodeType::EMPTY:
                os << "(empty, epsilon)";
                break;

            default:
                os << "Unknown cst node type";
                break;
        }

        return os;
    }

    Parser::Parser(const std::vector<Token> &tokens)
        : tokens_(tokens), current_token_index_(0),  error_info_(tokens.size() + 1, 0), parse_success(true) {
        // 在词法分析得到的 token 序列最后，添加 end token 用来使语法分析正确停止
        tokens_.push_back({TokenType::END, '#'});
        cst_root_ = CreateNode(CstNodeType::E);
    }

    std::unique_ptr<CstNode> Parser::CreateNode(const CstNodeType &type) {
        assert(current_token_index_ <= tokens_.size());

        std::unique_ptr<CstNode> cst_node(new CstNode);
        cst_node->type_ = type;

        bool success = false;
        while (current_token_index_ != tokens_.size() && !success) {
            success = true;
            const Token &token = tokens_[current_token_index_];
            switch (type) {
                case CstNodeType::E: {
                    if (token.type_ == TokenType::INTEGER) {        // E->TA
                        cst_node->children_.push_back(CreateNode(CstNodeType::T));
                        cst_node->children_.push_back(CreateNode(CstNodeType::A));
                    } else {
                        success = false;
                    }
                    break;
                }

                case CstNodeType::A: {
                    if (token.type_ == TokenType::PLUS || token.type_ == TokenType::MINUS) {        // A->+TA or A->-TA
                        cst_node->children_.push_back(CreateNode(token.type_ == TokenType::PLUS ? CstNodeType::PLUS : CstNodeType::MINUS));
                        cst_node->children_.push_back(CreateNode(CstNodeType::T));
                        cst_node->children_.push_back(CreateNode(CstNodeType::A));
                    } else if (token.type_ == TokenType::END) {                                      // A->\epsilon
                        cst_node->children_.push_back(CreateNode(CstNodeType::EMPTY));
                    } else {
                        success = false;
                    }
                    break;
                }

                case CstNodeType::B: {
                    if (token.type_ == TokenType::TIMES || token.type_ == TokenType::DIVIDE) {      // B->*FB or B->/FB
                        cst_node->children_.push_back(CreateNode(token.type_ == TokenType::TIMES ? CstNodeType::TIMES : CstNodeType::DIVIDE));
                        cst_node->children_.push_back(CreateNode(CstNodeType::F));
                        cst_node->children_.push_back(CreateNode(CstNodeType::B));
                    } else if (token.type_ == TokenType::PLUS || token.type_ == TokenType::MINUS 
                        || token.type_ == TokenType::END) {                                         // B->\epsilon
                        cst_node->children_.push_back(CreateNode(CstNodeType::EMPTY));
                    } else {
                        success = false;
                    }
                    break;
                }

                case CstNodeType::F: {
                    if (token.type_ == TokenType::INTEGER) {        // F->i
                        cst_node->children_.push_back(CreateNode(CstNodeType::INTEGER));
                    } else {
                        success = false;
                    }
                    break;
                }

                case CstNodeType::T: {
                    if (token.type_ == TokenType::INTEGER) {        // T->i
                        cst_node->children_.push_back(CreateNode(CstNodeType::F));
                        cst_node->children_.push_back(CreateNode(CstNodeType::B));
                    } else {
                        success = false;
                    }
                    break;
                }

                case CstNodeType::PLUS:   
                case CstNodeType::MINUS:   
                case CstNodeType::TIMES:   
                case CstNodeType::DIVIDE:   
                case CstNodeType::INTEGER: {
                    if (    (token.type_ == TokenType::PLUS && type == CstNodeType::PLUS)
                        ||  (token.type_ == TokenType::MINUS && type == CstNodeType::MINUS)
                        ||  (token.type_ == TokenType::TIMES && type == CstNodeType::TIMES)
                        ||  (token.type_ == TokenType::DIVIDE && type == CstNodeType::DIVIDE)
                        ||  (token.type_ == TokenType::INTEGER && type == CstNodeType::INTEGER)) {          // Current token matches token type
                        cst_node->token_ = token;
                        ++current_token_index_;
                    } else {            // Doesn't match.
                        success = false;
                    }
                    break;
                }

                case CstNodeType::EMPTY: {
                    // Skip. 
                    break;
                }

                default:
                    assert(0);  // Shouldn't be here.
                    break;
            }

            if (!success) {
                parse_success = false;
                error_info_[current_token_index_] = 1;
                ++current_token_index_;
            }
        }

        if (success) return cst_node;
        return nullptr;
    }

    void Parser::show_cst() const {
        if (parse_success) {
            std::cout << "Concrete Syntax Tree: " << std::endl;
            if (cst_root_) PrintTree(cst_root_, 0, false);
            else assert(0); // Should be here.
        } else {
            std::cout << "Parse failed." << std::endl;
        }
    }
    void Parser::PrintTree(const std::unique_ptr<CstNode> &node, int depth = 0, bool is_last = false) const {
        if (!node) assert(0); // Shouldn't be nullptr.
        std::string indent = (depth > 0) ? std::string((depth - 1) * 6, ' ') : "";
        std::cout << indent;
        
        if (depth > 0) {
            std::cout << (is_last ? "+-- " : "|-- ");
        }
        
        std::cout << *node << std::endl;
        
        for (decltype(node->children_.size()) i = 0; i != node->children_.size(); ++i) {
            PrintTree(node->children_[i], depth + 1, i + 1 == node->children_.size());
        }
    }
}
