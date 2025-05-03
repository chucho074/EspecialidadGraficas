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
#include "stdHeaders.h"
#include "mathObjects.h"
#include <cassert>

#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x=nullptr;
#define SAFE_DELETE(x) if(x != nullptr) delete(x); x=nullptr;
#define UNREFERENCED_PARAMETER(x) (void)x

static String 
trim(const String & str) {
  size_t start = str.find_first_not_of(' ');
  size_t end = str.find_last_not_of(' ');
  return str.substr(start, end - start + 1);
}

static Vector<String> 
split(const String & str, char delim) {
  Vector <String> tokens;
  size_t start = 0;
  size_t end = str.find(delim);
  while (end != String::npos) {
    tokens.push_back(trim(str.substr(start, end - start)));
    start = end + 1;
    end = str.find(delim, start);
  }
  tokens.push_back(str.substr(start, end));
  return tokens;
}

