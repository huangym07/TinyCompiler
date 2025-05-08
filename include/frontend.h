#ifndef __FRONTEND_H__
#define __FRONTEND_H__

#include <iostream>
#include <vector>
#include <string>

namespace frontend {
    enum class TokenType {
        PLUS,
        MINUS,
        TIMES,
        DIVIDE,
        INTEGER,
        END             // 该类型的 token (end token) 添加在词法分析得到的 token 序列最后，用来使语法分析正确终止
    };

    class Token {
        friend std::ostream& operator << (std::ostream &os, const Token &token);
    public:
        Token() = default;
        Token(const TokenType &type, int value) : type_(type), value_(value) {}
    public:
        TokenType type_;
        int value_;
    };

    class Lexer {
    public:
        Lexer() = default;

        bool lex_entire_string(const std::string &);

        const std::vector<Token>& get_tokens() const { return tokens_; }
        const std::vector<int>& get_error_info() const { return error_info_; }

        bool success() const { return lex_success; }

    private:
        std::vector<Token> tokens_;

        std::vector<int> error_info_;
        
        bool lex_success;
    };


    enum class CstNodeType {
        // non-leaf node type
        E, A, B, F, T,  
        // leaf node type
        PLUS, MINUS, TIMES, DIVIDE, INTEGER,    // token type
        EMPTY                                   // non-token type
    };

    class CstNode {
        friend std::ostream &operator << (std::ostream &os, const CstNode &cst_node);
    public:
        CstNode() = default;
    public:
        CstNodeType type_;
        std::vector<std::vector<CstNode>::size_type> son_node_indexs_;
        Token token_;
    };

    class Parser {
    public:
        Parser(const std::vector<Token> &tokens);

        void show_cst() const;
        const std::vector<int>& get_error_info() const { return error_info_; }
        const std::vector<Token>& get_tokens() const { return tokens_; }
        bool success() const { return parse_success; }

    private:
        // Create a cst node
        // return the node index
        // token 无法匹配则记录错误 token 位置，丢弃 token，尝试下一个 token
        std::vector<CstNode>::size_type CreateNode(const CstNodeType &type);

    private:
        std::vector<Token> tokens_;
        std::vector<Token>::size_type current_token_index_;

        std::vector<CstNode> cst_;
        std::vector<CstNode>::size_type cst_root_index_;

        std::vector<int> error_info_;

        bool parse_success;
    };
}

#endif // __FRONTEND_H__
