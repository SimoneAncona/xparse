# Xparser

## Introduction

Xparser is a versatile parsing library that empowers developers with robust parsing capabilities.

## Installation

You can easily install Xparser by downloading the source codes and integrating them into your project.

## Getting Started

### Define a grammar

In order to use Xparser, you need to define your grammar using a simple JSON file.

> A grammar allows Xparser to transform a sequence of characters into a syntax tree.

All JSON Xparser grammar files must have the following structure:

```json
{
    "name": "nameOfYourGrammar",
    "terminals": [
        {
            "name": "nameOfTerminalRule",
            "regex": "ECMAScript regular expression"
        }
    ],
    "rules": [
        {
            "name": "ruleName",
            "expressions": [
                "[b]def<identifier>():"
            ]
        }
    ]
}
```

You can also specify the JSON schema so that you don't run into errors:

```json
{
    "$schema": "https://raw.githubusercontent.com/SimoneAncona/xparser/main/schemas/schema.json",
}
```
For more information <a href="#grammars">click here</a>

### Using Grammars

As mentioned above, we use grammars to generate an abstract syntax tree or AST, you can do it in your C++ project:

```cpp
#include "xparser.hh"
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iostream>

std::string read_json_file(std::string filename);

int main(int argc, char** argv)
{
    Xpp::Parser parser(read_json_file("myGrammar.json"));       // import the grammar file
    Xpp::AST ast = parser.generate_ast("parse this string");    // parse a string and generate the AST

    std::cout << ast.to_json().to_string() << std::endl;  // see the JSON string representation of the AST
    return 0;
}

std::string read_json_file(std::string filename)
{
    ifstream file;
    stringstream buff;
    file.open(filename);
    if (file.fail())
        throw std::runtime_error("Cannot open the file: " + filename);
    buff << file.rdbuf();
    return buff.str();
}
```

<a name="grammars"></a>
## Grammars

### Terminal Values

A terminal is always a final node in the AST, a terminal value can be a literal number, a literal string or an identifier. There are 3 types of terminal values:
- Predefined: terminals that are built-in such as `integer` or `identifier`
- User-defined: terminals that are defined in the `terminals` property of the grammar JSON file.
- Constant: terminals that are defined in rule expressions, we will see later what this means.

A terminal is defined by a name and a regular expression, except for those constants.

#### Predefined Terminal Values

There are 8 built-in terminals:
- `integer`: that is equivalent to `[-|+]?\d+` regular expression.
- `identifier`: that is equivalent to `[_a-zA-Z][_a-zA-Z0-9]*`.
- `real`: that is equivalent to `[+|-]?\d+(\.\d+)?`
- `alpha`: that is equivalent to `[a-zA-Z]`
- `alnum`: equivalent to `[a-zA-Z0-9]`
- `digit`: equivalent to `[0-9]`
- `hexDigit`: equivalent to `[0-9a-fA-F]`
- `octalDigit`: equivalent to `[0-7]`

#### User-defined Terminal

User-defined terminals are defined in the JSON grammar file under the `terminals` property. A terminal is defined by specifying the name and the ECMAScript regular expression like this:
```json
{
    "terminals": [
        
    ]
}
``` 
