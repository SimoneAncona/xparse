/**
 * @file xparser.cc
 * @author Simone Ancona
 * @version 1.0
 * @date 2023-07-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "xparser.hh"

Xpp::Parser::Parser(Jpp::Json grammar)
{
    this->grammar = grammar;
}

Xpp::Parser::Parser(std::string grammar)
{
    this->grammar.parse(grammar);
}

Xpp::AST Xpp::Parser::generate_ast(std::string)
{

}