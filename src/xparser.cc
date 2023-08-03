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

Xpp::Parser::Parser(const Jpp::Json &grammar)
{
    this->grammar = grammar;
    this->generate_from_json();
}

Xpp::Parser::Parser(const std::string &grammar)
{
    this->grammar.parse(grammar);
    this->generate_from_json();
}

Xpp::Parser::Parser(const std::ifstream &file)
{
    this->grammar.parse(this->get_string_from_file(file));
    this->generate_from_json();
}

Xpp::AST Xpp::Parser::generate_ast(const std::string &input_string)
{
    return parse(tokenize(input_string));
}

std::stack<Xpp::SyntaxError> &Xpp::Parser::get_error_stack() noexcept
{
    return error_stack;
}

Xpp::SyntaxError Xpp::Parser::get_last_error()
{
    return error_stack.top();
}

std::string Xpp::Parser::get_string_from_file(const std::ifstream &file)
{
    std::stringstream buff;
    buff << file.rdbuf();
    return buff.str();
}

void Xpp::Parser::generate_from_json()
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

void Xpp::Parser::generate_terminal_rules(const std::map<std::string, Jpp::Json> &terminalsArray)
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

void Xpp::Parser::generate_rules(const std::map<std::string, Jpp::Json> &rulesArray)
{
    std::set<std::pair<std::string, std::string>> referenced_rule_names;
    std::string rule_name;
    for (auto ruleJSON : rulesArray)
    {
        try
        {
            rule_name = std::any_cast<std::string>(ruleJSON.second["name"].get_value());
            this->rules.push_back(Xpp::Rule{rule_name, parse_expressions(ruleJSON.second["expressions"].get_children(), referenced_rule_names, rule_name)});
        }
        catch (const std::runtime_error e)
        {
            throw std::runtime_error("Error while parsing the array of rules, go to https://github.com/SimoneAncona/xparser#define-a-grammar for more:\n\t" + std::string(e.what()));
        }
    }

    for (auto rule : referenced_rule_names)
    {
        if (find_rule(rule.first) == nullptr && find_terminal_rule(rule.first) == nullptr && std::find(implicit_terminals.begin(), implicit_terminals.end(), rule.first) == std::end(implicit_terminals))
            throw std::runtime_error("Undefined reference to the rule '" + rule.first + "' in the rule '" + rule.second + "'");
    }

    if (rules.size() == 0)
        throw std::runtime_error("No rules were specified. You must specify at least one rule");
}

std::vector<Xpp::RuleExpression> Xpp::Parser::parse_expressions(const std::map<std::string, Jpp::Json> &expressions, std::set<std::pair<std::string, std::string>> &referenced_rules, const std::string &rule_name)
{
    std::vector<Xpp::RuleExpression> parsed_expressions;
    Xpp::RuleExpression temp_expression;

    for (auto exp : expressions)
    {
        temp_expression = Xpp::RuleExpression(any_cast<std::string>(exp.second.get_value()));
        get_reference_names(temp_expression, referenced_rules, rule_name);
        parsed_expressions.push_back(temp_expression);
    }

    return parsed_expressions;
}

void Xpp::Parser::get_reference_names(Xpp::RuleExpression &exp, std::set<std::pair<std::string, std::string>> &references, const std::string &rule_name)
{
    for (auto el : exp.get_elements())
    {
        if (el.type == ExpressionElementType::RULE_REFERENCE)
            references.insert(std::pair<std::string, std::string>(el.references[0].reference_to, rule_name));
    }
}

Xpp::Rule *Xpp::Parser::find_rule(const std::string &name)
{
    for (auto &rule : rules)
    {
        if (rule.name == name)
            return &rule;
    }
    return nullptr;
}

Xpp::TerminalRule *Xpp::Parser::find_terminal_rule(const std::string &name)
{
    for (auto &t : terminals)
    {
        if (t.name == name)
            return &t;
    }
    return nullptr;
}

std::vector<Xpp::Token> Xpp::Parser::tokenize(const std::string &str)
{
    std::vector<Xpp::Token> tokens;
    std::vector<Xpp::Token> temp;

    for (auto t : terminals)
    {
        temp = get_tokens(str, t);
        for (auto tm : temp)
        {
            tokens.push_back(tm);
        }
    }

    std::sort(tokens.begin(), tokens.end(), Xpp::token_compare);

    return tokens;
}

bool Xpp::token_compare(Xpp::Token t1, Xpp::Token t2)
{
    return t1.index < t2.index;
}

std::vector<Xpp::Token> Xpp::Parser::get_tokens(const std::string &str, Xpp::TerminalRule rule)
{
    std::vector<Xpp::Token> tokens;
    std::smatch m;
    std::pair<size_t, size_t> column_line;
    std::string::const_iterator s_begin = str.cbegin();
    size_t index = 0;

    while (std::regex_search(s_begin, str.cend(), m, std::regex(rule.regex)))
    {
        index = (str.length() - m.suffix().length()) - m.str().length();
        column_line = Xpp::Parser::get_column_line(str, index);
        tokens.push_back(Xpp::Token{rule, index, column_line.first, column_line.second, m.str()});
        s_begin = m.suffix().first;
    }

    return tokens;
}

std::pair<size_t, size_t> Xpp::Parser::get_column_line(std::string_view str, long long index)
{
    size_t column = 0;
    size_t line = 0;
    long long i = 0;
    for (auto ch : str)
    {
        if (i == index)
            return std::pair<size_t, size_t>(column, line);
        if (ch == '\n')
        {
            line++;
            column = 0;
        }
        i++;
        column++;
    }
    return std::pair<size_t, size_t>(0, 0);
}

Xpp::AST Xpp::Parser::parse(const std::vector<Xpp::Token> &tokens)
{
    Xpp::AST ast(rules[0].name, std::vector<Xpp::AST>{});
    this->parse_index = {0, 0};
    try
    {
        analyze_rule(ast, tokens, rules[0]);
    }
    catch (Xpp::SyntaxErrorException &e)
    {
        throw Xpp::SyntaxErrorException("An error occurred while parsing the string:\n\t" + std::string(e.what()) + "\nUse 'get_error_stack' or 'get_last_error' for more.");
    }

    return ast;
}

void Xpp::Parser::analyze_rule(Xpp::AST &ast, const std::vector<Xpp::Token> &tokens, const Rule &rule)
{
    bool error = true;
    for (auto rule_exp : rule.expressions)
    {
        if (!analyze_expression(ast, tokens, rule_exp, rule.name) && error)
            error = false;
    }
    if (error)
        throw Xpp::SyntaxErrorException(get_last_error().message);
}

bool Xpp::Parser::analyze_expression(Xpp::AST &ast, const std::vector<Xpp::Token> &tokens, Xpp::RuleExpression &exp, const std::string &rule_name)
{
    for (auto el : exp)
    {
        switch (el.type)
        {
        case ExpressionElementType::CONSTANT_TERMINAL:
            return analyze_constant(ast, tokens, el, rule_name);
        case ExpressionElementType::ALTERNATIVE:
            return analyze_alternative(ast, tokens, el, rule_name);
        case ExpressionElementType::RULE_REFERENCE:
            return analyze_reference(ast, tokens, el, rule_name);
        }
    }
    return false;
}

bool Xpp::Parser::analyze_constant(Xpp::AST &ast, const std::vector<Xpp::Token> &tokens, const Xpp::ExpressionElement &el, const std::string &rule_name)
{
    for (size_t i = 0; i < el.value.length(); i++)
    {
        if (el.value[i] != tokens[parse_index.token_index].value[parse_index.char_index])
        {
            error_stack.push({EXPECTED_TOKEN, "'" + std::string(1, el.value[i]) + "' was expected", tokens[parse_index.token_index].index, tokens[parse_index.token_index].column, tokens[parse_index.token_index].line});
            return false;
        }
        parse_index.char_index++;
        if (parse_index.char_index >= tokens[parse_index.token_index].value.length())
            parse_index = {parse_index.token_index + 1, 0};
    }
    ast.push_child({rule_name, el.value});
    return true;
}

bool Xpp::Parser::analyze_reference(Xpp::AST &ast, const std::vector<Xpp::Token> &tokens, const Xpp::ExpressionElement &el, const std::string &rule_name)
{
    switch (el.references[0].quantifier.type)
    {
        case NONE:
            return analyze_single_reference(ast, tokens, el, rule_name);
        case ZERO_OR_ONE:
            return analyze_zero_or_one(ast, tokens, el, rule_name);
        case ZERO_OR_MORE:
            return analyze_zero_or_more(ast, tokens, el, rule_name);
        case ONE_OR_MORE:
            return analyze_one_or_more(ast, tokens, el, rule_name);
        case EXACT_VALUE:
            return analyze_exact_quantity(ast, tokens, el, rule_name);
        case EXACT_RANGE:
            return analyze_exact_range(ast, tokens, el, rule_name);
    }
    return false;
}

bool Xpp::Parser::analyze_single_reference(Xpp::AST &ast, const std::vector<Xpp::Token> &tokens, const Xpp::ExpressionElement &el, const std::string &rule_name)
{
    Xpp::Rule *rule = find_rule(el.references[0].reference_to);
    Xpp::TerminalRule *terminal = find_terminal_rule(el.references[0].reference_to);
    if (rule == nullptr)
    {
        if (tokens[parse_index.token_index].from.name == terminal->name)
        {
            ast.push_child({rule_name, tokens[parse_index.token_index].value});
            this->parse_index = {parse_index.token_index + 1, 0};
            return true;
        }
        return false;
    }

    try
    {
        analyze_rule(ast, tokens, *rule);
        return true;
    }
    catch (const Xpp::SyntaxErrorException e)
    {
        error_stack.push({UNMATCHED_RULE, "Cannot match '" + rule->name + "' rule. Use 'get_error_stack' to get the error stack.", tokens[parse_index.token_index].index, tokens[parse_index.token_index].column, tokens[parse_index.token_index].line});
        return false;
    }
    return false;
}

bool Xpp::Parser::analyze_alternative(Xpp::AST &ast, const std::vector<Xpp::Token> &tokens, const Xpp::ExpressionElement &el, const std::string &rule_name)
{
    Index last_index = parse_index;
    for (auto ref : el.references)
    {
        parse_index = last_index;
        if (analyze_reference(ast, tokens, {RULE_REFERENCE, "", {ref}}, rule_name))
        {
            return true;
        }
    }

    error_stack.push({UNMATCHED_RULE, "No match found on the alternative in the rule '" + rule_name + "'. Use 'get_error_stack' to get the error stack.", tokens[parse_index.token_index].index, tokens[parse_index.token_index].column, tokens[parse_index.token_index].line});
    return false;
}

bool Xpp::Parser::analyze_zero_or_one(Xpp::AST &ast, const std::vector<Xpp::Token> &tokens, const Xpp::ExpressionElement &el, const std::string &rule_name)
{
    Index last_index = parse_index;
    if (analyze_single_reference(ast, tokens, el, rule_name))
    {
        return true;
    }
    parse_index = last_index;
    return true;
}

bool Xpp::Parser::analyze_zero_or_more(Xpp::AST &ast, const std::vector<Xpp::Token> &tokens, const Xpp::ExpressionElement &el, const std::string &rule_name)
{
    Index last_index = parse_index;
    while (analyze_single_reference(ast, tokens, el, rule_name))
    {
        last_index = parse_index;
    }
    parse_index = last_index;
    return true;
}

bool Xpp::Parser::analyze_one_or_more(Xpp::AST &ast, const std::vector<Xpp::Token> &tokens, const Xpp::ExpressionElement &el, const std::string &rule_name)
{
    Index last_index = parse_index;
    bool error = true;
    while (analyze_single_reference(ast, tokens, el, rule_name))
    {
        last_index = parse_index;
        error = false;
    }
    parse_index = last_index;
    if (error)
    {
        error_stack.push({UNMATCHED_RULE, "'" + rule_name + "' was expected at least once. Use 'get_error_stack' to get the error stack.", tokens[parse_index.token_index].index, tokens[parse_index.token_index].column, tokens[parse_index.token_index].line});
        return false;
    }
    return true;
}

bool Xpp::Parser::analyze_exact_quantity(Xpp::AST &ast, const std::vector<Xpp::Token> &tokens, const Xpp::ExpressionElement &el, const std::string &rule_name)
{
    Index last_index = parse_index;
    size_t i = 0;
    while (analyze_single_reference(ast, tokens, el, rule_name))
    {
        i++;
    }
    parse_index = last_index;
    return true;
}

bool Xpp::Parser::analyze_exact_range(Xpp::AST &ast, const std::vector<Xpp::Token> &tokens, const Xpp::ExpressionElement &el, const std::string &rule_name)
{
    Index last_index = parse_index;
    size_t i = 0;
    while (analyze_single_reference(ast, tokens, el, rule_name))
    {
        i++;
    }
    parse_index = last_index;
    return true;
} 