/**
 * @file jpp.hh
 * @author Simone Ancona
 * @brief A JSON parser for C++
 * @version 1.4
 * @date 2023-07-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <string>
#include <map>
#include <stdexcept>
#include <any>
#include <cctype>
#include <vector>

#define l_object std::vector<std::pair<std::string, std::any>>
#define l_array std::vector<std::any>

namespace Jpp
{
    enum JsonType
    {
        JSON_ARRAY,
        JSON_OBJECT,
        JSON_STRING,
        JSON_BOOLEAN,
        JSON_NUMBER,
        JSON_NULL
    };

    enum Token
    {
        OBJECT_START,
        ARRAY_START,
        OBJECT_END,
        ARRAY_END,
        NUMBER,
        STRING,
        ALPHA,
        SEPARATOR,
        END,
    };

    /**
     * @brief The Json class allows to parse a json string
     *
     */
    class Json
    {
    private:
        JsonType type;
        std::map<std::string, Json> children;
        std::any value;

        std::map<std::string, Json> parse_object(std::string_view, size_t &);
        std::map<std::string, Json> parse_array(std::string_view, size_t &);
        std::string parse_string(std::string_view, size_t &, char);
        std::any parse_number(std::string_view, size_t &);
        std::any parse_boolean(std::string_view, size_t &);
        std::any parse_null(std::string_view, size_t &);
        bool is_resolved;
        std::string unresolved_string;

        Token match_next(std::string_view, size_t &);

        inline bool is_space(char ch) noexcept
        {
            return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\v';
        }

        inline void next_white_space_or_separator(std::string_view str, size_t &index) noexcept
        {
            while (index < str.length() && !is_space(str[index]) && str[index] != '[' && str[index] != '{' && str[index] != ',' && str[index] != ']' && str[index] != '}')
                ++index;
        }

        inline void skip_white_spaces(std::string_view str, size_t &index) noexcept
        {
            while (index < str.length() && is_space(str[index]))
                ++index;
        }

        inline std::string json_object_to_string(Json json)
        {
            std::map<std::string, Jpp::Json> children = json.get_children();
            if (children.size() == 0)
                return "{}";
            if (!is_resolved)
                return unresolved_string;
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

        inline std::string json_array_to_string(Json json)
        {
            std::map<std::string, Jpp::Json> children = json.get_children();
            if (children.size() == 0)
                return "[]";
            if (!is_resolved)
                return unresolved_string;
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
        std::string str_replace(std::string_view, char, std::string_view);

        Json get_unresolved_object(std::string_view, size_t &, bool);

    public:
        /**
         * @brief Construct a new Json object
         * @since v1.0
         */
        inline Json() noexcept
        {
            this->type = JSON_OBJECT;
            this->is_resolved = true;
        }

        /**
         * @brief Construct a new Json object
         * 
         * @param children 
         * @param type 
         * @since v1.0
         */
        inline Json(std::map<std::string, Json> children, JsonType type) noexcept
        {
            this->children = children;
            this->type = type;
            this->is_resolved = true;
        }

        /**
         * @brief Construct a new Json object
         * 
         * @param value 
         * @param type 
         * @since v1.0
         */
        inline Json(std::any value, JsonType type) noexcept
        {
            this->value = value;
            this->type = type;
            this->is_resolved = true;
        }

        /**
         * @brief Construct a new Json object
         * 
         * @param values 
         * @since v1.4
         */
        inline Json(std::vector<std::any> values)
        {
            this->type = JSON_ARRAY;
            this->is_resolved = true;
            for (size_t i = 0; i < values.size(); ++i)
            {
                this->children.emplace(std::to_string(i), Json(values[i]));
            }
        }

        /**
         * @brief Construct a new Json object
         * 
         * @param key_values 
         * @since v1.4
         */
        inline Json(std::vector<std::pair<std::string, std::any>> key_values)
        {
            this->type = JSON_OBJECT;
            this->is_resolved = true;
            for (size_t i = 0; i < key_values.size(); ++i)
            {
                this->children.emplace(key_values[i].first, Json(key_values[i].second));
            }
        }

        /**
         * @brief Construct a new Json object
         * 
         * @param value 
         * @since v1.4
         */
        inline Json(std::any value)
        {
            this->is_resolved = true;
            size_t hash = value.type().hash_code();
            if (hash == typeid(int).hash_code())
            {
                this->value = static_cast<double>(std::any_cast<int>(value));
                this->type = JSON_NUMBER;
                return;
            }
            if (hash == typeid(const char *).hash_code())
            {
                this->value = std::string(std::any_cast<const char *>(value));
                this->type = JSON_STRING;
                return;
            }
            if (hash == typeid(std::string).hash_code())
            {
                this->value = value;
                this->type = JSON_STRING;
                return;
            }
            if (hash == typeid(bool).hash_code())
            {
                this->value = value;
                this->type = JSON_BOOLEAN;
                return;
            }
            if (hash == typeid(double).hash_code())
            {
                this->value = value;
                this->type = JSON_NUMBER;
                return;
            }
            if (hash == typeid(nullptr_t).hash_code())
            {
                this->value = value;
                this->type = JSON_NULL;
                return;
            }
            throw std::runtime_error("Unknown type: " + std::string(value.type().name()));
        }

        /**
         * @brief Construct a new Json object
         * 
         * @param str 
         * @since v1.0
         */
        inline Json(std::string str) noexcept
        {
            this->value = str;
            this->type = JSON_STRING;
            this->is_resolved = true;
        }

        /**
         * @brief Construct a new Json object
         * 
         * @param num 
         * @since v1.0
         */
        inline Json(double num) noexcept
        {
            this->value = num;
            this->type = JSON_NUMBER;
            this->is_resolved = true;
        }

        /**
         * @brief Construct a new Json object
         * 
         * @param val 
         * @since v1.0
         */
        inline Json(bool val) noexcept
        {
            this->value = val;
            this->type = JSON_BOOLEAN;
            this->is_resolved = true;
        }

        /**
         * @brief Construct a new Json object
         * 
         * @param null 
         * @since v1.0
         */
        inline Json(nullptr_t null) noexcept
        {
            this->value = null;
            this->type = JSON_NULL;
            this->is_resolved = true;
        }

        ~Json() = default;

        /**
         * @brief Get the type object
         *
         * @return JsonType
         * @since v1.0
         */
        inline JsonType get_type() noexcept
        {
            return this->type;
        }

        /**
         * @brief Get the value object
         *
         * @return std::any
         * @since v1.0
         */
        inline std::any get_value() noexcept
        {
            return this->value;
        }

        /**
         * @brief Check if the JSON value is an array
         *
         * @return true
         * @return false
         * @since v1.0
         */
        inline bool is_array() noexcept
        {
            return this->type == JSON_ARRAY;
        }

        /**
         * @brief Check if the JSON value is an object
         *
         * @return true
         * @return false
         * @since v1.0
         */
        inline bool is_object() noexcept
        {
            return this->type == JSON_OBJECT;
        }

        /**
         * @brief Check if the JSON is an atomic string value
         *
         * @return true
         * @return false
         * @since v1.0
         */
        inline bool is_string() noexcept
        {
            return this->type == JSON_STRING;
        }

        /**
         * @brief Check if the JSON is an atomic boolean value
         *
         * @return true
         * @return false
         * @since v1.0
         */
        inline bool is_boolean() noexcept
        {
            return this->type == JSON_BOOLEAN;
        }

        /**
         * @brief Check if the JSON is an atomic double value
         *
         * @return true
         * @return false
         * @since v1.0
         */
        inline bool is_number() noexcept
        {
            return this->type == JSON_NUMBER;
        }

        /**
         * @brief Parse a JSON string
         * @since v1.0
         */
        void parse(const std::string &);

        /**
         * @brief Get the children object
         *
         * @return std::map<std::string, Json>
         * @since v1.0
         */
        inline std::map<std::string, Json> get_children()
        {
            if (!is_resolved)
                parse(unresolved_string);
            return this->children;
        }

        /**
         * @brief Access to a position of the array
         * @example
         *  Jpp::Json json;
         *  json.parse("[0, 1, 2, 3]");
         *  json[0] // returns an any value
         * @return Json&
         * @since v1.0
         */
        Json &operator[](size_t);

        /**
         * @brief Access to a value of the object with the given property name
         *
         * @return Json&
         * @since v1.0
         */
        Json &operator[](const std::string &);

        /**
         * @return Json&
         * @since v1.0
         */
        Json &operator=(const std::string &);

        /**
         * @return Json&
         * @since v1.0
         */
        Json &operator=(double);

        /**
         * @return Json&
         * @since v1.0
         */
        Json &operator=(int);

        /**
         * @return Json&
         * @since v1.0
         */
        Json &operator=(bool);

        /**
         * @return Json&
         * @since v1.0
         */
        Json &operator=(const char[]);

        /**
         * @return Json&
         * @since v1.4
         */
        Json &operator=(std::vector<std::any>);

        /**
         * @return Json& 
         * @since v1.4
         */
        Json &operator=(std::vector<std::pair<std::string, std::any>>);

        /**
         * @brief Convert the JSON object to its JSON representation.
         *
         * @return std::string
         */
        inline std::string to_string()
        {
            switch (this->type)
            {
            case Jpp::JSON_OBJECT:
                return json_object_to_string(*this);
            case Jpp::JSON_ARRAY:
                return json_array_to_string(*this);
            case Jpp::JSON_STRING:
                return "\"" +
                       str_replace(
                           str_replace(std::any_cast<std::string>(this->value), '"', "\\\""), '\n', "\\n") +
                       "\"";
            case Jpp::JSON_BOOLEAN:
                return std::any_cast<bool>(this->value) ? "true" : "false";
            case Jpp::JSON_NUMBER:
                return std::to_string(std::any_cast<double>(this->value));
            case Jpp::JSON_NULL:
                return "null";
            }
            return "";
        }

        /**
         * @brief Begin iterator
         *
         * @return std::map<std::string, Json>::iterator
         * @since v1.1
         */
        inline std::map<std::string, Json>::iterator begin() noexcept
        {
            return children.begin();
        }

        /**
         * @brief End iterator
         *
         * @return std::map<std::string, Json>::iterator
         * @since v1.1
         */
        inline std::map<std::string, Json>::iterator end() noexcept
        {
            return children.end();
        }

        /**
         * @brief Reverse begin iterator
         *
         * @return std::map<std::string, Json>::iterator
         * @since v1.1
         */
        inline std::map<std::string, Json>::reverse_iterator rbegin() noexcept
        {
            return children.rbegin();
        }

        /**
         * @brief Reverse end iterator
         *
         * @return std::map<std::string, Json>::iterator
         * @since v1.1
         */
        inline std::map<std::string, Json>::reverse_iterator rend() noexcept
        {
            return children.rend();
        }

        /**
         * @brief Get the vector if the JSON object is an array
         *
         * @return std::vector<Json>
         * @since v1.2
         */
        inline std::vector<Json> get_vector()
        {
            if (type != JSON_ARRAY)
                throw std::runtime_error("Cannot convert a non-array JSON to a vector");
            std::vector<Jpp::Json> vct;
            for (auto json : children)
            {
                vct.push_back(json.second);
            }
            return vct;
        }
    };
};