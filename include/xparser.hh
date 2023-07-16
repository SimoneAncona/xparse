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

    class Parser
    {
    private:
        Jpp::Json grammar;
        std::vector<Rule> rules;
        std::vector<TerminalRule> terminals;

        void generate_rules();
        std::string get_string_from_file();

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
        Parser(std::ifstream);

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
    };
};