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

Xpp::AST::AST(std::string rule_name, std::vector<AST> children)
{
    this->terminal = false;
    this->rule_name = rule_name;
    this->children = children;
}

Xpp::AST::AST(std::string rule_name, std::string terminal_value)
{
    this->terminal = true;
    this->rule_name = rule_name;
    this->value = terminal_value;
}

inline bool Xpp::AST::is_terminal()
{
    return terminal;
}

inline std::string Xpp::AST::get_rule_name()
{
    return rule_name;
}

inline std::string Xpp::AST::get_value()
{
    if (!terminal)
        throw std::runtime_error("Cannot get the value of a non-terminal node");
    return value;
}

inline std::vector<Xpp::AST> &Xpp::AST::get_children()
{
    if (terminal)
        throw std::runtime_error("Cannot get the children of an terminal node");
    return children;
}

inline Xpp::AST &Xpp::AST::operator[](size_t index)
{
    return children[index];
}

inline std::vector<Xpp::AST>::iterator Xpp::AST::begin()
{
    return children.begin();
}

inline std::vector<Xpp::AST>::iterator Xpp::AST::end()
{
    return children.end();
}

Jpp::Json Xpp::AST::to_json()
{
    Jpp::Json json;

    return json;
}