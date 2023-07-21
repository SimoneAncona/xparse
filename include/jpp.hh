/**
 * @file json.hh
 * @author Simone Ancona
 * @brief A JSON parser for C++
 * @version 1.2.2
 * @date 2023-07-20
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

#define JSON_ARRAY (char)0x00
#define JSON_OBJECT (char)0x01
#define JSON_STRING (char)0xF0
#define JSON_BOOLEAN (char)0xF1
#define JSON_NUMBER (char)0xF2
#define JSON_NULL (char)0xF3

namespace Jpp
{
    typedef char json_type_t;

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
        json_type_t type;
        std::map<std::string, Json> children;
        std::any value;

    public:
        Json();
        Json(std::map<std::string, Json>, json_type_t);
        Json(std::any, json_type_t);
        Json(std::string);
        Json(double);
        Json(bool);
        Json(nullptr_t);
        ~Json() = default;

        /**
         * @brief Get the type object
         * 
         * @return json_type_t 
         * @since v1.0
         */
        json_type_t get_type();

        /**
         * @brief Get the value object
         * 
         * @return std::any 
         * @since v1.0
         */
        std::any get_value();

        /**
         * @brief Check if the JSON value is an array
         * 
         * @return true 
         * @return false 
         * @since v1.0
         */
        bool is_array();

        /**
         * @brief Check if the JSON value is an object
         * 
         * @return true 
         * @return false 
         * @since v1.0
         */
        bool is_object();

        /**
         * @brief Check if the JSON is an atomic string value
         * 
         * @return true 
         * @return false 
         * @since v1.0
         */
        bool is_string();

        /**
         * @brief Check if the JSON is an atomic boolean value
         * 
         * @return true 
         * @return false 
         * @since v1.0
         */
        bool is_boolean();

        /**
         * @brief Check if the JSON is an atomic double value
         * 
         * @return true 
         * @return false 
         * @since v1.0
         */
        bool is_number();

        /**
         * @brief Parse a JSON string
         * @since v1.0
         */
        void parse(std::string);

        /**
         * @brief Get the children object
         * 
         * @return std::map<std::string, Json> 
         * @since v1.0
         */
        std::map<std::string, Json> get_children();

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
        Json &operator[](std::string);

        /**
         * @return Json& 
         * @since v1.0
         */
        Json &operator=(std::string);

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
         * @brief Convert the JSON object to its JSON representation.
         * 
         * @return std::string 
         */
        std::string to_string();

        /**
         * @brief Begin iterator
         * 
         * @return std::map<std::string, Json>::iterator
         * @since v1.1
         */
        inline std::map<std::string, Json>::iterator begin()
        {
            return children.begin();
        }

        /**
         * @brief End iterator
         * 
         * @return std::map<std::string, Json>::iterator
         * @since v1.1
         */
        inline std::map<std::string, Json>::iterator end()
        {
            return children.end();
        }

        /**
         * @brief Reverse begin iterator
         * 
         * @return std::map<std::string, Json>::iterator
         * @since v1.1
         */
        inline std::map<std::string, Json>::reverse_iterator rbegin()
        {
            return children.rbegin();
        }

        /**
         * @brief Reverse end iterator
         * 
         * @return std::map<std::string, Json>::iterator
         * @since v1.1
         */
        inline std::map<std::string, Json>::reverse_iterator rend()
        {
            return children.rend();
        }

        /**
         * @brief Get the vector if the JSON object is an array
         * 
         * @return std::vector<Json> 
         * @since v1.2
         */
        std::vector<Json> get_vector();
    };

    void trim_string(std::string &);
    std::map<std::string, Json> parse_object(std::string, size_t &);
    std::map<std::string, Json> parse_array(std::string, size_t &);
    std::string parse_string(std::string, size_t &, char);
    std::any parse_number(std::string, size_t &);
    std::any parse_boolean(std::string, size_t &);
    std::any parse_null(std::string, size_t &);

    Token match_next(std::string, size_t &);
    void next_white_space_or_separator(std::string, size_t &);
    void skip_white_spaces(std::string, size_t &);

    std::string json_object_to_string(Json);
    std::string json_array_to_string(Json);
    std::string str_replace(std::string, char, std::string);
};