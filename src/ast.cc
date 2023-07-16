#include "ast.hh"

Xpp::AST::AST()
{
    this->atomic = false;
}

Xpp::AST::AST(std::string rule_name, std::vector<AST> children)
{
    this->atomic = false;
    this->rule_name = rule_name;
    this->children = children;
}

Xpp::AST::AST(std::string rule_name, std::string atomic_value)
{
    this->atomic = true;
    this->rule_name = rule_name;
    this->value = atomic_value;
}

bool Xpp::AST::is_atomic()
{
    return atomic;
}

std::string Xpp::AST::get_rule_name()
{
    return rule_name;
}

std::string Xpp::AST::get_value()
{
    if (!atomic)
        throw std::runtime_error("Cannot get the value of a non-atomic node");
    return value;
}

std::vector<Xpp::AST> &Xpp::AST::get_children()
{
    if (atomic)
        throw std::runtime_error("Cannot get the children of an atomic node");
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