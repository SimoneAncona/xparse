/**
 * @file json.cc
 * @author Simone Ancona
 * @brief
 * @version 1.1.3
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "jpp.hh"

bool Jpp::Json::is_array()
{
    return this->type == JSON_ARRAY;
}

bool Jpp::Json::is_object()
{
    return this->type == JSON_OBJECT;
}

bool Jpp::Json::is_string()
{
    return this->type == JSON_STRING;
}

bool Jpp::Json::is_boolean()
{
    return this->type == JSON_BOOLEAN;
}

bool Jpp::Json::is_number()
{
    return this->type == JSON_NUMBER;
}

Jpp::json_type_t Jpp::Json::get_type()
{
    return this->type;
}

Jpp::Json::Json()
{
    this->type = JSON_OBJECT;
}

Jpp::Json::~Json()
{
}

Jpp::Json::Json(std::map<std::string, Jpp::Json> children, Jpp::json_type_t type)
{
    this->children = children;
    this->type = type;
}

Jpp::Json::Json(std::any value, Jpp::json_type_t type)
{
    this->value = value;
    this->type = type;
}

Jpp::Json::Json(std::string str)
{
    this->value = str;
    this->type = JSON_STRING;
}

Jpp::Json::Json(double num)
{
    this->value = num;
    this->type = JSON_NUMBER;
}

Jpp::Json::Json(bool val)
{
    this->value = val;
    this->type = JSON_BOOLEAN;
}

Jpp::Json::Json(nullptr_t null)
{
    this->value = null;
    this->type = JSON_NULL;
}

std::any Jpp::Json::get_value()
{
    return this->value;
}

std::map<std::string, Jpp::Json> Jpp::Json::get_children()
{
    return this->children;
}

Jpp::Json &Jpp::Json::operator[](size_t index)
{
    if (this->type > JSON_OBJECT)
        throw std::out_of_range("Cannot use the subscript operator with an atomic value, use get_value");
    return this->children.at(std::to_string(index));
}

Jpp::Json &Jpp::Json::operator[](std::string property)
{
    if (this->type > JSON_OBJECT)
        throw std::out_of_range("Cannot use the subscript operator with an atomic value, use get_value");
    if (this->type == JSON_OBJECT && this->children.find(property) == this->children.end())
        this->children[property] = Json(nullptr);
    return this->children.at(property);
}

Jpp::Json &Jpp::Json::operator=(std::string str)
{
    this->type = JSON_STRING;
    this->value = str;

    return *this;
}

Jpp::Json &Jpp::Json::operator=(const char str[])
{
    this->type = JSON_STRING;
    this->value = std::string(str);

    return *this;
}

Jpp::Json &Jpp::Json::operator=(bool val)
{
    this->type = JSON_BOOLEAN;
    this->value = val;
    
    return *this;
}

Jpp::Json &Jpp::Json::operator=(double num)
{
    this->type = JSON_NUMBER;
    this->value = num;
    
    return *this;
}

Jpp::Json &Jpp::Json::operator=(int num)
{
    this->type = JSON_NUMBER;
    this->value = static_cast<double>(num);
    
    return *this;
}

std::map<std::string, Jpp::Json>::iterator Jpp::Json::begin()
{
    return children.begin();
}

std::map<std::string, Jpp::Json>::iterator Jpp::Json::end()
{
    return children.end();
}

std::map<std::string, Jpp::Json>::reverse_iterator Jpp::Json::rbegin()
{
    return children.rbegin();
}

std::map<std::string, Jpp::Json>::reverse_iterator Jpp::Json::rend()
{
    return children.rend();
}

void Jpp::Json::parse(std::string json_string)
{
    trim_string(json_string);
    size_t start = 0;
    if (json_string[start] == '{')
    {
        this->children = Jpp::parse_object(json_string, start);
        this->type = JSON_OBJECT;
        return;
    }
    if (json_string[start] == '[')
    {
        this->children = Jpp::parse_array(json_string, start);
        this->type = JSON_ARRAY;
        return;
    }
    throw std::runtime_error("Unexpected " + std::string(1, json_string[0]) + " at the beginning of the string");
}

void Jpp::trim_string(std::string &str)
{
    size_t start = 0;
    size_t end = str.length() - 1;
    while (str[start] == ' ' || str[start] == '\n')
    {
        ++start;
        if (start == end)
            throw std::runtime_error("The string is empty");
    }

    while (str[end] == ' ' || str[end] == '\n')
        --end;

    str = str.substr(start, end - start + 1);
}

std::map<std::string, Jpp::Json> Jpp::parse_object(std::string str, size_t &index)
{
    std::map<std::string, Jpp::Json> object;
    Jpp::Token next;
    std::string current_property;
    Jpp::Json current_value;

    ++index;
    Jpp::skip_white_spaces(str, index);

    while (true)
    {
        next = Jpp::match_next(str, index);

        switch (next)
        {
        case Jpp::Token::END:
            throw std::invalid_argument("Unexpected the end of the string, expected a '}'");
        case Jpp::Token::ARRAY_START:
            throw std::runtime_error("Unexpected the start of an array, expected a property name");
        case Jpp::Token::ARRAY_END:
            throw std::runtime_error("Unexpected the end of an array, expected a property name");
        case Jpp::Token::ALPHA:
            throw std::runtime_error("Unexpected a boolean value, expected a property name");
        case Jpp::Token::NUMBER:
            throw std::runtime_error("Unexpected a number value, expected a property name");
        case Jpp::Token::OBJECT_START:
            throw std::runtime_error("Unexpected the start of an object, expected a property name");
        case Jpp::Token::OBJECT_END:
            return object;
        case Jpp::Token::SEPARATOR:
            throw std::runtime_error("Unexpected separator, expected a property name");
        case Jpp::Token::STRING:
            current_property = Jpp::parse_string(str, index, str[index]);
            break;
        }

        Jpp::skip_white_spaces(str, index);

        if (str[index] != ':')
            throw std::runtime_error("Expected ':'");

        ++index;

        Jpp::skip_white_spaces(str, index);

        next = Jpp::match_next(str, index);

        switch (next)
        {
        case Jpp::Token::END:
            throw std::runtime_error("Unexpected the end of the string, a value is expected");
        case Jpp::Token::ARRAY_START:
            current_value = Jpp::Json(Jpp::parse_array(str, index), JSON_ARRAY);
            break;
        case Jpp::Token::ARRAY_END:
            throw std::runtime_error("Unexpected the end of an array, a value is expected");
        case Jpp::Token::OBJECT_START:
            current_value = Jpp::Json(Jpp::parse_object(str, index), JSON_OBJECT);
            break;
        case Jpp::Token::OBJECT_END:
            throw std::runtime_error("Unexpected the end of the object, a value is expected");
        case Jpp::Token::ALPHA:
            if (str[index] == 'n')
                current_value = Jpp::Json(Jpp::parse_null(str, index), JSON_NULL);
            else
                current_value = Jpp::Json(Jpp::parse_boolean(str, index), JSON_BOOLEAN);
            break;
        case Jpp::Token::NUMBER:
            current_value = Jpp::Json(Jpp::parse_number(str, index), JSON_NUMBER);
            break;
        case Jpp::Token::STRING:
            current_value = Jpp::Json(Jpp::parse_string(str, index, str[index]), JSON_STRING);
            break;
        case Jpp::Token::SEPARATOR:
            throw std::runtime_error("Unexpected separator, a value is expected");
        }

        Jpp::skip_white_spaces(str, index);

        next = Jpp::match_next(str, index);
        if (next != Jpp::Token::SEPARATOR && next != Jpp::Token::OBJECT_END)
            throw std::runtime_error("Expected a ',' or the end of the object");

        ++index;

        Jpp::skip_white_spaces(str, index);

        object.insert(std::pair<std::string, Jpp::Json>(current_property, current_value));

        if (next == Jpp::Token::OBJECT_END)
            return object;
    }
}

std::map<std::string, Jpp::Json> Jpp::parse_array(std::string str, size_t &index)
{
    std::map<std::string, Jpp::Json> object;
    Jpp::Token next;
    size_t current_index = 0;
    Jpp::Json current_value;

    ++index;
    Jpp::skip_white_spaces(str, index);

    while (true)
    {
        next = Jpp::match_next(str, index);

        switch (next)
        {
        case Jpp::Token::END:
            throw std::runtime_error("Unexpected the end of the string, the end of the array is expected");
        case Jpp::Token::ARRAY_START:
            current_value = Jpp::Json(Jpp::parse_array(str, index), JSON_ARRAY);
            break;
        case Jpp::Token::ARRAY_END:
            return object;
        case Jpp::Token::OBJECT_START:
            current_value = Jpp::Json(Jpp::parse_object(str, index), JSON_OBJECT);
            break;
        case Jpp::Token::OBJECT_END:
            throw std::runtime_error("Unexpected '}' token, a value is expected");
        case Jpp::Token::ALPHA:
             if (str[index] == 'n')
                current_value = Jpp::Json(Jpp::parse_null(str, index), JSON_NULL);
            else
                current_value = Jpp::Json(Jpp::parse_boolean(str, index), JSON_BOOLEAN);
            break;
        case Jpp::Token::NUMBER:
            current_value = Jpp::Json(Jpp::parse_number(str, index), JSON_NUMBER);
            break;
        case Jpp::Token::STRING:
            current_value = Jpp::Json(Jpp::parse_string(str, index, str[index]), JSON_STRING);
            break;
        case Jpp::Token::SEPARATOR:
            throw std::runtime_error("Unexpected separator, a value is expected");
        }

        Jpp::skip_white_spaces(str, index);

        next = Jpp::match_next(str, index);
        if (next != Jpp::Token::SEPARATOR && next != Jpp::Token::ARRAY_END)
            throw std::runtime_error("Expected a ',' or the end of the array");

        ++index;

        Jpp::skip_white_spaces(str, index);

        object.insert(std::pair<std::string, Jpp::Json>(std::to_string(current_index), current_value));
        ++current_index;

        if (next == Jpp::Token::ARRAY_END)
            return object;
    }
}

Jpp::Token Jpp::match_next(std::string str, size_t &index)
{
    if (index >= str.length())
        return Jpp::Token::END;
    switch (str[index])
    {
    case '{':
        return Jpp::Token::OBJECT_START;
    case '}':
        return Jpp::Token::OBJECT_END;
    case ',':
        return Jpp::Token::SEPARATOR;
    case '"':
    case '\'':
        return Jpp::Token::STRING;
    case '[':
        return Jpp::Token::ARRAY_START;
    case ']':
        return Jpp::Token::ARRAY_END;
    }
    if (str[index] >= '0' && str[index] <= '9')
        return Jpp::Token::NUMBER;
    if (isalpha(str[index]))
        return Jpp::Token::ALPHA;
    throw std::runtime_error("Unexpected " + std::string(1, str[index]) + " token");
}

void Jpp::skip_white_spaces(std::string str, size_t &index)
{
    while (index < str.length() && isspace(str[index]))
        ++index;
}

std::string Jpp::parse_string(std::string str, size_t &index, char start_with)
{
    std::string value;
    bool escape = false;

    ++index;
    while (true)
    {
        if (index >= str.length())
            throw std::runtime_error("Expected the end of the string");
        if (str[index] == '\n')
            throw std::runtime_error("Unexpected end of the line while parsing the string: '" + value + "'");
        if (str[index] == '\\' && !escape)
        {
            escape = true;
            ++index;
            continue;
        }

        if (str[index] == start_with && !escape)
        {
            ++index;
            return value;
        }

        if (escape)
        {
            escape = false;
            switch (str[index])
            {
            case 'n':
                value += '\n';
                ++index;
                continue;
            case 't':
                value += '\t';
                ++index;
                continue;
            case 'r':
                value += '\r';
                ++index;
                continue;
            case 'v':
                value += '\v';
                ++index;
                continue;
            case 'b':
                value += '\b';
                ++index;
                continue;
            }
        }
        value += str[index];
        ++index;
        escape = false;
    }
}

std::any Jpp::parse_number(std::string str, size_t &index)
{
    size_t start = index;
    Jpp::next_white_space_or_separator(str, index);
    size_t end = index;
    std::string substr = str.substr(start, end - start);

    return std::stod(substr);
}

std::any Jpp::parse_boolean(std::string str, size_t &index)
{
    size_t start = index;
    Jpp::next_white_space_or_separator(str, index);
    size_t end = index;
    std::string substr = str.substr(start, end - start);

    if (substr == "true")
        return true;
    if (substr == "false")
        return false;
    throw std::runtime_error("Unrecognized token: " + substr);
}

std::any Jpp::parse_null(std::string str, size_t &index)
{
    size_t start = index;
    Jpp::next_white_space_or_separator(str, index);
    size_t end = index;
    std::string substr = str.substr(start, end - start);

    if (substr == "null")
        return nullptr;

    throw std::runtime_error("Unrecognized token: " + substr);
}

std::string Jpp::Json::to_string()
{
    switch (this->type)
    {
    case JSON_OBJECT:
        return Jpp::json_object_to_string(*this);
    case JSON_ARRAY:
        return Jpp::json_array_to_string(*this);
    case JSON_STRING:
        return "\"" +
               Jpp::str_replace(
                   Jpp::str_replace(std::any_cast<std::string>(this->value), '"', "\\\""), '\n', "\\n") +
               "\"";
    case JSON_BOOLEAN:
        return std::any_cast<bool>(this->value) ? "true" : "false";
    case JSON_NUMBER:
        return std::to_string(std::any_cast<double>(this->value));
    case JSON_NULL:
        return "null";
    }
    return "";
}

std::string Jpp::json_object_to_string(Jpp::Json json)
{
    std::map<std::string, Jpp::Json> children = json.get_children();
    if (children.size() == 0) return "{}";
    std::map<std::string, Jpp::Json>::iterator it;
    std::string str = "{";


    for (it = children.begin(); it != std::prev(children.end()); ++it)
    {
        str += "\"" + it->first + "\":";
        str += it->second.to_string();
        str += ", ";
    }

    str += "\"" + std::prev(children.end())->first + "\":";
    str += std::prev(children.end())->second.to_string();

    return str + "}";
}

std::string Jpp::json_array_to_string(Jpp::Json json)
{
    std::map<std::string, Jpp::Json> children = json.get_children();
    if (children.size() == 0) return "[]";
    std::map<std::string, Jpp::Json>::iterator it;
    std::string str = "[";

    for (it = children.begin(); it != std::prev(children.end()); ++it)
    {
        str += it->second.to_string();
        str += ",";
    }

    str += std::prev(children.end())->second.to_string();

    return str + "]";
}

void Jpp::next_white_space_or_separator(std::string str, size_t &index)
{
    while (index < str.length() && !isspace(str[index]) && str[index] != '[' && str[index] != '{' && str[index] != ',' && str[index] != ']' && str[index] != '}')
        ++index;
}

std::string Jpp::str_replace(std::string original, char old, std::string new_str)
{
    std::string str = "";
    for (char ch : original)
    {
        if (ch == old)
            str += new_str;
        else
            str += ch;
    }
    return str;
}