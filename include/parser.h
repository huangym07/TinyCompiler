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


    enum class CstNodeType {
        E, A, B, F, T, 
        OPERATOR,
        NUMBER,
        EMPTY,
    };
    std::ostream& operator << (std::ostream &os, const CstNodeType &type);

    struct CstNode {
        std::vector<std::vector<CstNode>::size_type> son_node_indexs;
        bool is_leaf;
        CstNodeType type;
        int value;
    };

    class Parser {
    public:
        Parser(const std::vector<Token> &tokens);

        const std::vector<CstNode>& get_cst() const { return cst_; }

        const std::vector<int>& get_error_info() const { return error_info_; }
        bool get_success() const { return parse_success; }
        void show_cst() const;

    private:
        // 从当前 tokne 开始构建子树
        // 返回子树根结点的编号
        // token 无法匹配则记录错误 token 位置，丢弃 token，尝试下一个 token
        std::vector<CstNode>::size_type E();
        std::vector<CstNode>::size_type A();
        std::vector<CstNode>::size_type B();
        std::vector<CstNode>::size_type F();
        std::vector<CstNode>::size_type T();

        // 从当前 token 开始构建叶子结点
        // 返回叶子结点的编号
        // 当前 token 应与当前结点需要的 token 匹配
        // token 无法匹配则记录错误 token 位置，丢弃 token，尝试下一个 token
        std::vector<CstNode>::size_type Match(const Token &should_be_token);

    private:
        std::vector<Token>::size_type current_token_index_;
        std::vector<Token> tokens_;
        std::vector<CstNode> cst_;
        std::vector<CstNode>::size_type cst_root_index_;
        std::vector<int> error_info_;
        bool parse_success;
    };
}

#endif // __LEX_H__
