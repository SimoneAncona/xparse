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
    this->generate_from_json();
}

Xpp::Parser::Parser(std::string grammar)
{
    this->grammar.parse(grammar);
    this->generate_from_json();
}

Xpp::Parser::Parser(std::ifstream &file)
{
    this->grammar.parse(this->get_string_from_file(file));
    this->generate_from_json();
}

inline Xpp::AST Xpp::Parser::generate_ast(std::string input_string)
{
    return parse(tokenize(input_string));
}

inline std::stack<Xpp::SyntaxError> &Xpp::Parser::get_error_stack() noexcept
{
    return error_stack;
}

inline Xpp::SyntaxError Xpp::Parser::get_last_error()
{
    return error_stack.top();
}

inline std::string Xpp::Parser::get_string_from_file(std::ifstream &file)
{
    std::stringstream buff;
    buff << file.rdbuf();
    return buff.str();
}

inline void Xpp::Parser::generate_from_json()
{
    auto children = grammar.get_children();
    auto terminals = children.find("terminals");
    if (terminals == children.end())
        throw std::runtime_error("The 'terminals' property is required in the JSON grammar.");
    auto rules = children.find("rules");
    if (rules == children.end())
        throw std::runtime_error("The 'rules' property is required in the JSON grammar.");

    if (!terminals->second.is_array())
        throw std::runtime_error("The 'terminals' property must be an array");
    if (!rules->second.is_array())
        throw std::runtime_error("The 'rules' property must be an array");

    auto terminalsArray = terminals->second.get_children();
    auto rulesArray = rules->second.get_children();

    generate_terminal_rules(terminalsArray);
    generate_rules(rulesArray);
}

inline void Xpp::Parser::generate_terminal_rules(std::map<std::string, Jpp::Json> terminalsArray)
{
    for (auto terminal : terminalsArray)
    {
        try
        {
            this->terminals.push_back(Xpp::TerminalRule{std::any_cast<std::string>(terminal.second["name"].get_value()), std::any_cast<std::string>(terminal.second["regex"].get_value())});
        }
        catch (const std::runtime_error e)
        {
            throw std::runtime_error("Error while parsing the array of terminals, go to https://github.com/SimoneAncona/xparser#define-a-grammar for more");
        }
    }
}

inline void Xpp::Parser::generate_rules(std::map<std::string, Jpp::Json> rulesArray)
{
    Xpp::Rule rule;
    for (auto ruleJSON : rulesArray)
    {
        try
        {
            rule = Xpp::Rule{std::any_cast<std::string>(ruleJSON.second["name"].get_value()), parse_expressions(ruleJSON.second["expressions"].get_children())};
            this->rules.push_back(rule);
        }
        catch (const std::runtime_error e)
        {
            throw std::runtime_error("Error while parsing the array of rules, go to https://github.com/SimoneAncona/xparser#define-a-grammar for more:\n\t" + std::string(e.what()));
        }
    }
}

std::vector<Xpp::RuleExpression> Xpp::Parser::parse_expressions(std::map<std::string, Jpp::Json> expressions)
{
    std::vector<Xpp::RuleExpression> parsed_expressions;

    for (auto exp : expressions)
    {
        parsed_expressions.push_back(Xpp::RuleExpression(any_cast<std::string>(exp.second.get_value())));
    }

    return parsed_expressions;
}

std::vector<Xpp::Token> Xpp::Parser::tokenize(std::string str)
{
    std::vector<Xpp::Token> tokens;

    return tokens;
}

Xpp::AST Xpp::Parser::parse(std::vector<Xpp::Token> tokens)
{
    Xpp::AST ast;

    return ast;
}