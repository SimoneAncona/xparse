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

namespace Xpp
{
    class Parser
    {
    private:
        Jpp::Json grammar;

    public:
        Parser() = default;
        Parser(Jpp::Json);
        Parser(std::string);

        AST generate_ast();
    };
};