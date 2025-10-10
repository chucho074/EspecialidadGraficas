/**
 * @file    PrerequisiteGraficas.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    22/02/25
 * @brief   .
 */

/**
 * @include
 */
#pragma once
//#include "stdHeaders.h"
#include "mathObjects.h"
#include <cassert>

#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x=nullptr;
#define SAFE_DELETE(x) if(x != nullptr) delete(x); x=nullptr;
#define UNREFERENCED_PARAMETER(x) (void)x

static String 
trim(const String & str) {
  size_t start = str.find_first_not_of(" \t\n\r\f\v");
  if (start == String::npos) { 
    return ""; 
  }
  size_t end = str.find_last_not_of(" \t\n\r\f\v");
  return str.substr(start, end - start + 1);
}

static String
trim_and_reduce_spaces(const String& str) {
  String result;
  bool in_space = false;

  size_t start = str.find_first_not_of(" \t\n\r\f\v");
  if (start == String::npos) return "";

  size_t end = str.find_last_not_of(" \t\n\r\f\v");

  for (size_t i = start; i <= end; ++i) {
    if (isspace(str[i])) {
      if (!in_space) {
        result += ' ';
        in_space = true;
      }
    }
    else {
      result += str[i];
      in_space = false;
    }
  }

  return result;
}

static Vector<String> 
split(const String & str, char delim) {
  Vector <String> tokens;
  size_t start = 0;
  size_t end = str.find(delim);
  while (end != String::npos) {
    tokens.push_back(trim_and_reduce_spaces(str.substr(start, end - start)));
    start = end + 1;
    end = str.find(delim, start);
  }
  //tokens.push_back(str.substr(start, end));
  tokens.push_back(trim_and_reduce_spaces(str.substr(start)));
  return tokens;
}

