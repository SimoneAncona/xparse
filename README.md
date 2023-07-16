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
    "atomic": [
        {
            "name": "nameOfAtomicRule",
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