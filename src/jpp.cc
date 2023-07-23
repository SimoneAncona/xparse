/**
 * @file jpp.cc
 * @author Simone Ancona
 * @brief
 * @version 1.4
 * @date 2023-07-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "jpp.hh"

Jpp::Json &Jpp::Json::operator[](size_t index)
{
    if (this->type > Jpp::JSON_OBJECT)
        throw std::out_of_range("Cannot use the subscript operator with an atomic value, use get_value");
    if (!is_resolved)
        parse(unresolved_string);
    return this->children[std::to_string(index)];
}

Jpp::Json &Jpp::Json::operator[](const std::string &property)
{
    if (this->type > Jpp::JSON_OBJECT)
        throw std::out_of_range("Cannot use the subscript operator with an atomic value, use get_value");
    if (this->type == Jpp::JSON_OBJECT && this->children.find(property) == this->children.end())
        this->children.emplace(property, Json(nullptr));
    if (!is_resolved)
        parse(unresolved_string);
    return this->children.at(property);
}

Jpp::Json &Jpp::Json::operator=(const std::string &str)
{
    this->children.clear();
    this->is_resolved = true;
    this->type = Jpp::JSON_STRING;
    this->value = str;

    return *this;
}

Jpp::Json &Jpp::Json::operator=(const char str[])
{
    this->children.clear();
    this->is_resolved = true;
    this->type = Jpp::JSON_STRING;
    this->value = std::string(str);

    return *this;
}

Jpp::Json &Jpp::Json::operator=(bool val)
{
    this->children.clear();
    this->is_resolved = true;
    this->type = Jpp::JSON_BOOLEAN;
    this->value = val;

    return *this;
}

Jpp::Json &Jpp::Json::operator=(double num)
{
    this->children.clear();
    this->is_resolved = true;
    this->type = Jpp::JSON_NUMBER;
    this->value = num;

    return *this;
}

Jpp::Json &Jpp::Json::operator=(int num)
{
    this->children.clear();
    this->is_resolved = true;
    this->type = Jpp::JSON_NUMBER;
    this->value = static_cast<double>(num);

    return *this;
}

Jpp::Json &Jpp::Json::operator=(std::vector<std::any> array)
{
    this->children.clear();
    this->type = Jpp::JSON_ARRAY;
    this->is_resolved = true;
    for (size_t i = 0; i < array.size(); ++i)
    {
        this->children.emplace(std::to_string(i), Json(array[i]));
    }
    return *this;
}

Jpp::Json &Jpp::Json::operator=(std::vector<std::pair<std::string, std::any>> object)
{
    this->children.clear();
    this->type = Jpp::JSON_OBJECT;
    this->is_resolved = true;
    for (size_t i = 0; i < object.size(); ++i)
    {
        this->children.emplace(object[i].first, Json(object[i].second));
    }
    return *this;
}

void Jpp::Json::parse(const std::string &json_string)
{
    size_t start = 0;
    this->is_resolved = true;
    if (json_string[start] == '{')
    {
        this->children = parse_object(json_string, start);
        this->unresolved_string = "";
        this->type = Jpp::JSON_OBJECT;
        return;
    }
    if (json_string[start] == '[')
    {
        this->children = parse_array(json_string, start);
        this->unresolved_string = "";
        this->type = Jpp::JSON_ARRAY;
        return;
    }
    throw std::runtime_error("Unexpected " + std::string(1, json_string[0]) + " at the beginning of the string");
}

Jpp::Json Jpp::Json::get_unresolved_object(std::string_view str, size_t &index, bool is_object)
{
    const char end = is_object ? '}' : ']';
    const char start = is_object ? '{' : '[';
    char is_string = false;
    bool escape = false;
    std::string unresolved;
    Jpp::Json unresolved_json;
    int level = 0;
    bool cycle = true;

    while (cycle)
    {
        if (!(is_space(str[index]) && !is_string))
            unresolved += str[index];
        index++;
        if (index >= str.length())
            throw std::runtime_error("Unexpected end of the string");

        switch (str[index])
        {
        case '"':
            if (escape || is_string == '\'')
            {
                escape = false;
                break;
            }
            if (is_string == '"')
            {
                escape = false;
                is_string = false;
                break;
            }
            escape = false;
            is_string = '"';
            break;
        case '\'':
            if (escape || is_string == '"')
            {
                escape = false;
                break;
            }
            if (is_string == '\'')
            {
                escape = false;
                is_string = false;
                break;
            }
            escape = false;
            is_string = '\'';
            break;
        case '\\':
            if (!is_string)
                throw std::runtime_error("Unexpected '\\' token at position: " + std::to_string(index));
            escape = !escape;
            break;
        case '{':
        case '[':
            if (str[index] != start)
                break;
            if (!is_string)
                level++;
            break;
        case '}':
        case ']':
            if (str[index] != end)
                break;
            if (!is_string)
            {
                if (level == 0)
                    cycle = false;
                else
                    level--;
                break;
            }
            break;
        default:
            escape = false;
            break;
        }
    }
    index++;
    unresolved += end;
    unresolved_json.type = is_object ? JSON_OBJECT : JSON_ARRAY;
    unresolved_json.is_resolved = false;
    unresolved_json.unresolved_string = unresolved;
    return unresolved_json;
}

std::map<std::string, Jpp::Json> Jpp::Json::parse_object(std::string_view str, size_t &index)
{
    std::map<std::string, Jpp::Json> object;
    Jpp::Token next;
    std::string current_property;
    Jpp::Json current_value;

    ++index;
    skip_white_spaces(str, index);

    while (true)
    {
        next = match_next(str, index);

        switch (next)
        {
        case Jpp::Token::END:
            throw std::invalid_argument("Unexpected the end of the string, expected a '}' at position: " + std::to_string(index));
        case Jpp::Token::ARRAY_START:
            throw std::runtime_error("Unexpected the start of an array, expected a property name at position: " + std::to_string(index));
        case Jpp::Token::ARRAY_END:
            throw std::runtime_error("Unexpected the end of an array, expected a property name at position: " + std::to_string(index));
        case Jpp::Token::ALPHA:
            throw std::runtime_error("Unexpected a boolean value, expected a property name at position: " + std::to_string(index));
        case Jpp::Token::NUMBER:
            throw std::runtime_error("Unexpected a number value, expected a property name at position: " + std::to_string(index));
        case Jpp::Token::OBJECT_START:
            throw std::runtime_error("Unexpected the start of an object, expected a property name at position: " + std::to_string(index));
        case Jpp::Token::OBJECT_END:
            return object;
        case Jpp::Token::SEPARATOR:
            throw std::runtime_error("Unexpected separator, expected a property name at position: " + std::to_string(index));
        case Jpp::Token::STRING:
            current_property = parse_string(str, index, str[index]);
            break;
        }

        skip_white_spaces(str, index);

        if (str[index] != ':')
            throw std::runtime_error("Expected ':' at position: " + std::to_string(index));

        ++index;

        skip_white_spaces(str, index);

        next = match_next(str, index);

        switch (next)
        {
        case Jpp::Token::END:
            throw std::runtime_error("Unexpected the end of the string, a value is expected at position: " + std::to_string(index));
        case Jpp::Token::ARRAY_START:
            current_value = get_unresolved_object(str, index, false);
            break;
        case Jpp::Token::ARRAY_END:
            throw std::runtime_error("Unexpected the end of an array, a value is expected at position: " + std::to_string(index));
        case Jpp::Token::OBJECT_START:
            current_value = get_unresolved_object(str, index, true);
            break;
        case Jpp::Token::OBJECT_END:
            throw std::runtime_error("Unexpected the end of the object, a value is expected at position: " + std::to_string(index));
        case Jpp::Token::ALPHA:
            if (str[index] == 'n')
                current_value = Jpp::Json(parse_null(str, index), Jpp::JSON_NULL);
            else
                current_value = Jpp::Json(parse_boolean(str, index), Jpp::JSON_BOOLEAN);
            break;
        case Jpp::Token::NUMBER:
            current_value = Jpp::Json(parse_number(str, index), Jpp::JSON_NUMBER);
            break;
        case Jpp::Token::STRING:
            current_value = Jpp::Json(parse_string(str, index, str[index]), Jpp::JSON_STRING);
            break;
        case Jpp::Token::SEPARATOR:
            throw std::runtime_error("Unexpected separator, a value is expected at position: " + std::to_string(index));
        }

        skip_white_spaces(str, index);

        next = match_next(str, index);
        if (next != Jpp::Token::SEPARATOR && next != Jpp::Token::OBJECT_END)
            throw std::runtime_error("Expected a ',' or the end of the object at position: " + std::to_string(index));

        ++index;

        skip_white_spaces(str, index);

        object.insert(std::pair<std::string, Jpp::Json>(current_property, current_value));

        if (next == Jpp::Token::OBJECT_END)
            return object;
    }
}

std::map<std::string, Jpp::Json> Jpp::Json::parse_array(std::string_view str, size_t &index)
{
    std::map<std::string, Jpp::Json> object;
    Jpp::Token next;
    size_t current_index = 0;
    Jpp::Json current_value;

    ++index;
    skip_white_spaces(str, index);

    while (true)
    {
        next = match_next(str, index);

        switch (next)
        {
        case Jpp::Token::END:
            throw std::runtime_error("Unexpected the end of the string, the end of the array is expected at position: " + std::to_string(index));
        case Jpp::Token::ARRAY_START:
            current_value = Jpp::Json(parse_array(str, index), Jpp::JSON_ARRAY);
            break;
        case Jpp::Token::ARRAY_END:
            return object;
        case Jpp::Token::OBJECT_START:
            current_value = Jpp::Json(parse_object(str, index), Jpp::JSON_OBJECT);
            break;
        case Jpp::Token::OBJECT_END:
            throw std::runtime_error("Unexpected '}' token, a value is expected at position: " + std::to_string(index));
        case Jpp::Token::ALPHA:
            if (str[index] == 'n')
                current_value = Jpp::Json(parse_null(str, index), Jpp::JSON_NULL);
            else
                current_value = Jpp::Json(parse_boolean(str, index), Jpp::JSON_BOOLEAN);
            break;
        case Jpp::Token::NUMBER:
            current_value = Jpp::Json(parse_number(str, index), Jpp::JSON_NUMBER);
            break;
        case Jpp::Token::STRING:
            current_value = Jpp::Json(parse_string(str, index, str[index]), Jpp::JSON_STRING);
            break;
        case Jpp::Token::SEPARATOR:
            throw std::runtime_error("Unexpected separator, a value is expected at position: " + std::to_string(index));
        }

        skip_white_spaces(str, index);

        next = match_next(str, index);
        if (next != Jpp::Token::SEPARATOR && next != Jpp::Token::ARRAY_END)
            throw std::runtime_error("Expected a ',' or the end of the array at position: " + std::to_string(index));

        ++index;

        skip_white_spaces(str, index);

        object.insert(std::pair<std::string, Jpp::Json>(std::to_string(current_index), current_value));
        ++current_index;

        if (next == Jpp::Token::ARRAY_END)
            return object;
    }
}

Jpp::Token Jpp::Json::match_next(std::string_view str, size_t &index)
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
    throw std::runtime_error("Unexpected " + std::string(1, str[index]) + " token at position: " + std::to_string(index));
}

std::string Jpp::Json::parse_string(std::string_view str, size_t &index, char start_with)
{
    std::string value;
    bool escape = false;

    ++index;
    while (true)
    {
        if (index >= str.length())
            throw std::runtime_error("Expected the end of the string");
        if (str[index] == '\n')
            throw std::runtime_error("Unexpected end of the line while parsing the string: '" + value + "' at position: " + std::to_string(index));
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

std::any Jpp::Json::parse_number(std::string_view str, size_t &index)
{
    size_t start = index;
    next_white_space_or_separator(str, index);
    size_t end = index;
    std::string_view substr = str.substr(start, end - start);

    return std::stod(substr.data());
}

std::any Jpp::Json::parse_boolean(std::string_view str, size_t &index)
{
    size_t start = index;
    next_white_space_or_separator(str, index);
    size_t end = index;
    std::string_view substr = str.substr(start, end - start);

    if (substr == "true")
        return true;
    if (substr == "false")
        return false;
    throw std::runtime_error("Unrecognized token: " + std::string(substr.data()) + " at position: " + std::to_string(index));
}

std::any Jpp::Json::parse_null(std::string_view str, size_t &index)
{
    size_t start = index;
    next_white_space_or_separator(str, index);
    size_t end = index;
    std::string_view substr = str.substr(start, end - start);

    if (substr == "null")
        return nullptr;

    throw std::runtime_error("Unrecognized token: " + std::string(substr.data()) + " at position: " + std::to_string(index));
}

std::string Jpp::Json::str_replace(std::string_view original, char old, std::string_view new_str)
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