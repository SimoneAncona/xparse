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
#include <regex>

namespace Xpp
{
    class Parser
    {
    private:
        Jpp::Json grammar;

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