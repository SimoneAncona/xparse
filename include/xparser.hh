/**
 * @file xparser.hh
 * @author Simone Ancona
 * @brief A simple parser for C++
 * @version 1.0
 * @date 2023-07-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "jpp.hh"
#include "ast.hh"
#include "rel.hh"
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <stdexcept>
#include <regex>
#include <stack>
#include <set>

namespace Xpp
{
    struct Rule
    {
        std::string name;
        std::vector<RuleExpression> expressions;
    };

    struct TerminalRule
    {
        std::string name;
        std::string regex;
    };

    struct Token
    {
        TerminalRule from;
        size_t index;
        size_t column;
        size_t line;
        std::string value;
    };

    enum SyntaxErrorType
    {
        EXPECTED_TOKEN,
        UNEXPECTED_TOKEN,
        UNMATCHED_RULE,
    };

    struct SyntaxError
    {
        SyntaxErrorType type;
        std::string message;
        size_t index;
        size_t column;
        size_t line;
    };

    struct Index
    {
        size_t token_index;
        size_t char_index;
    };

    class SyntaxErrorException : public std::exception
    {
    private:
        const char *message;
    public:

        SyntaxErrorException(const std::string message) noexcept
        {
            this->message = message.c_str();
        }

        SyntaxErrorException(const char *message) noexcept
        {
            this->message = message;
        }

        inline const char *what() noexcept
        {
            return message;
        }
    };

    bool token_compare(Token, Token);

    class Parser
    {
    private:
        Jpp::Json grammar;
        std::vector<Rule> rules;
        std::vector<TerminalRule> terminals = {{"integer", "[-|+]?\\d+"}, {"identifier", "[_a-zA-Z][_a-zA-Z0-9]*"}, {"real", "[+|-]?\\d+(\\.\\d+)?"}};
        const std::vector<std::string> implicit_terminals = {"alnum", "digit", "alpha", "space", "hexDigit", "octDigit", "eof", "newLine", "any"};
        std::stack<SyntaxError> error_stack;

        Index parse_index;
        std::string input;

        void generate_from_json();
        void generate_terminal_rules(const std::map<std::string, Jpp::Json> &);
        void generate_rules(const std::map<std::string, Jpp::Json> &);
        std::vector<RuleExpression> parse_expressions(const std::map<std::string, Jpp::Json> &, std::set<std::pair<std::string, std::string>> &, const std::string &);
        void get_reference_names(RuleExpression &, std::set<std::pair<std::string, std::string>> &, const std::string &);
        Rule *find_rule(const std::string &);
        TerminalRule *find_terminal_rule(const std::string &);
        std::string get_string_from_file(const std::ifstream &);
        std::vector<Token> tokenize(const std::string &);
        Xpp::AST parse(const std::vector<Token> &);
        std::vector<Token> get_tokens(const std::string &, TerminalRule);
        std::pair<size_t, size_t> get_column_line(std::string_view, long long);
        void analyze_rule(Xpp::AST &, const std::vector<Token> &, const Rule &);
        bool analyze_expression(Xpp::AST &, const std::vector<Token> &, RuleExpression &, const std::string &);
        bool analyze_alternative(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &, const std::string &);
        bool analyze_reference(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &, const std::string &);
        bool analyze_single_reference(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &, const std::string &);
        bool analyze_zero_or_one(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &, const std::string &);
        bool analyze_zero_or_more(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &, const std::string &);
        bool analyze_one_or_more(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &, const std::string &);
        bool analyze_exact_quantity(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &, const std::string &);
        bool analyze_exact_range(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &, const std::string &);
        bool analyze_constant(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &, const std::string &);

    public:
        /**
         * @brief Construct a new Parser object
         *
         */
        Parser() = default;

        /**
         * @brief Construct a new Parser object specifying the grammar with a JSON object
         *
         */
        Parser(const Jpp::Json &);

        /**
         * @brief Construct a new Parser object specifying the grammar with a JSON string
         *
         */
        Parser(const std::string &);

        /**
         * @brief Construct a new Parser object specifying the input file stream
         *
         */
        Parser(const std::ifstream &);

        /**
         * @brief Destroy the Parser object
         *
         */
        ~Parser() = default;

        /**
         * @brief Get the ast object
         *
         * @return AST
         */
        AST generate_ast(const std::string &);

        /**
         * @brief Get the error stack
         *
         * @return std::stack<SyntaxError>&
         */
        std::stack<SyntaxError> &get_error_stack() noexcept;

        /**
         * @brief Get the last error
         *
         * @return SyntaxError
         */
        SyntaxError get_last_error();
    };
};