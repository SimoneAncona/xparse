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
        UNEXPECTED_TOKEN
    };

    struct SyntaxError
    {
        SyntaxErrorType type;
        std::string message;
        size_t index;
        size_t column;
        size_t line;
    };

    bool token_compare(Token, Token);

    class Parser
    {
    private:
        Jpp::Json grammar;
        std::vector<Rule> rules;
        std::vector<TerminalRule> terminals = {{"any", "."}, {"integer", "[-|+]?\\d+"}, {"identifier", "[_a-zA-Z][_a-zA-Z0-9]*"}, {"real", "[+|-]?\\d+(\\.\\d+)?"}, {"alpha", "[a-zA-Z]"}, {"alnum", "[a-zA-Z0-9]"}, {"digit", "[0-9]"}, {"hexDigit", "[0-9a-fA-F]"}, {"octalDigit", "[0-7]"}, {"space", "[^\\S]"}, {"newLine", "\\r?\\n"}};
        std::stack<SyntaxError> error_stack;

        size_t index;

        void generate_from_json();
        void generate_terminal_rules(std::map<std::string, Jpp::Json>);
        void generate_rules(std::map<std::string, Jpp::Json>);
        std::vector<RuleExpression> parse_expressions(std::map<std::string, Jpp::Json>);
        std::string get_string_from_file(std::ifstream &);
        std::vector<Token> tokenize(std::string);
        Xpp::AST parse(std::vector<Token>);
        std::vector<Token> get_tokens(std::string, TerminalRule);
        std::pair<size_t, size_t> get_column_line(std::string, long long);

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
        Parser(Jpp::Json);

        /**
         * @brief Construct a new Parser object specifying the grammar with a JSON string
         *
         */
        Parser(std::string);

        /**
         * @brief Construct a new Parser object specifying the input file stream
         *
         */
        Parser(std::ifstream &);

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
        AST generate_ast(std::string);

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