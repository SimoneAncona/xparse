# Xparser

## Introduction

Xparser is a versatile parsing library that empowers developers with robust parsing capabilities.

## Installation

You can easily install Xparser by downloading the source codes and integrating them into your project.

## Getting Started

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
