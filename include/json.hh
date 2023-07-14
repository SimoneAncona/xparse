#include <string>
#include <map>
#include <stdexcept>
#include <any>
#include <format>
#include <cctype>

#define JSON_ARRAY (char)0x00
#define JSON_OBJECT (char)0x01
#define JSON_STRING (char)0xF2
#define JSON_BOOLEAN (char)0xF3
#define JSON_NUMBER (char)0xF4

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
        ~Json();
        json_type_t get_type();
        std::any get_value();
        bool is_array();
        bool is_object();
        bool is_string();
        bool is_boolean();
        bool is_number();
        void parse(std::string);
        std::map<std::string, Json> get_children();
        Json &operator[](size_t);
        Json &operator[](std::string);
        std::string to_string();
        template <typename T>
        T &get_atomic();
    };

    void trim_string(std::string &);
    std::map<std::string, Json> parse_object(std::string, size_t &);
    std::map<std::string, Json> parse_array(std::string, size_t &);
    std::string parse_string(std::string, size_t &, char);
    std::any parse_number(std::string, size_t &);
    std::any parse_boolean(std::string, size_t &);

    Token match_next(std::string, size_t &);
    void next_white_space_or_separator(std::string, size_t &);
    void skip_white_spaces(std::string, size_t &);

    std::string json_object_to_string(Json);
    std::string json_array_to_string(Json);
    std::string str_replace(std::string, char, std::string);
};