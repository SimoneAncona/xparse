/**
 * @file ast.cc
 * @author Simone Ancona
 * @version 1.0
 * @date 2023-07-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ast.hh"

Xpp::AST::AST()
{
    this->terminal = false;
}

Xpp::AST::AST(const std::string &rule_name, std::vector<AST> children)
{
    this->terminal = false;
    this->rule_name = rule_name;
    this->children = children;
}

Xpp::AST::AST(const std::string &rule_name, const std::string &terminal_value)
{
    this->terminal = true;
    this->rule_name = rule_name;
    this->value = terminal_value;
}

bool Xpp::AST::is_terminal()
{
    return terminal;
}

std::string Xpp::AST::get_rule_name()
{
    return rule_name;
}

std::string Xpp::AST::get_value()
{
    if (!terminal)
        throw std::runtime_error("Cannot get the value of a non-terminal node");
    return value;
}

std::vector<Xpp::AST> &Xpp::AST::get_children()
{
    if (terminal)
        throw std::runtime_error("Cannot get the children of an terminal node");
    return children;
}

Xpp::AST &Xpp::AST::operator[](size_t index)
{
    return children[index];
}

std::vector<Xpp::AST>::iterator Xpp::AST::begin()
{
    return children.begin();
}

std::vector<Xpp::AST>::iterator Xpp::AST::end()
{
    return children.end();
}

Jpp::Json Xpp::AST::to_json()
{
    Jpp::Json json;

    return json;
}