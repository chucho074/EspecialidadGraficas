/**
 * @file    gbStdHeaders.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    07/12/24
 * @brief   .
 */


 /**
  * @include
  */
#pragma once
  /*****************************************************************************/
  /*
  * C type objects
  */
  /*****************************************************************************/
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
//#include <cmath>

/*****************************************************************************/
/*
* Memory management
*/
/*****************************************************************************/
#include <memory>

/*****************************************************************************/
/*
* STL containers
*/
/*****************************************************************************/
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <set>
#include <list>
#include <forward_list>
#include <deque>
#include <queue>
#include <bitset>
#include <array>
#include <unordered_map>
#include <unordered_set>

/*****************************************************************************/
/*
* STL algorithms and functions
*/
/*****************************************************************************/
#include <algorithm>
#include <functional>
#include <limits>
#include <iterator>
#include <numeric>

/*****************************************************************************/
/*
* C++ Stream
*/
/*****************************************************************************/
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;


/*****************************************************************************/
/**
 * Basic unsigned types
 */
 /****************************************************************************/
using uint8 = uint8_t;      //8-bit unsigned int
using uint16 = uint16_t;    //16-bit unsigned int
using uint32 = uint32_t;    //32-bit unsigned int
using uint64 = uint64_t;    //64-bit unsigned int


/*****************************************************************************/
/**
 * Basic signed types
 */
 /****************************************************************************/
using int8 = int8_t;        //8-bit signed int
using int16 = int16_t;      //16-bit signed int
using int32 = int32_t;      //32-bit signed int
using int64 = int64_t;      //64-bit signed int


/*****************************************************************************/
/**
 *
 */
 /****************************************************************************/
using size_T = size_t;


using String = std::string;

using StringView = std::string_view;

using wString = std::wstring;

using ifstream = std::ifstream;

using ofstream = std::ofstream;

using fstream = std::fstream;

/**
 * @brief      Vector.
 * @param      T
 * @param      Alloc
 */
template<typename T, typename Alloc = std::allocator<T>>
using Vector = std::vector<T, Alloc>;

/**
 * @brief      Stack.
 * @param      T
 * @param      Alloc
 */
template<typename T, typename Alloc = std::allocator<T>>
using Stack = std::stack<T, Alloc>;

/**
 * @brief      Queue.
 * @param      T
 * @param      Alloc
 */
template<typename T, typename Alloc = std::allocator<T>>
using Queue = std::queue<T, Alloc>;

/**
 * @brief      List.
 * @param      T
 * @param      Alloc
 */
template<typename T, typename Alloc = std::allocator<T>>
using List = std::list<T, Alloc>;

/**
 * @brief      Pair.
 */
template<class K, class T>
using Pair = std::pair<const K, T>;

/**
 * @brief      Maps.
 * @tparam     Key
 * @tparam     T
 * @tparam     Compare
 * @tparam     Alloc
 */
template<class Key,
  class T,
  class Compare = std::less<Key>,
  class Alloc = std::allocator<Pair<const Key, T>>>
using Map = std::map<Key, T, Compare, Alloc>;

/**
 * @brief      Shared Pointers.
 * @tparam     T
 */
template <typename T>
using SPtr = std::shared_ptr<T>;

/**
 * @brief      Weak Pointers.
 * @tparam     T
 */
template <typename T>
using WPtr = std::weak_ptr<T>;

/**
 * @brief      Unique Pointers.
 * @tparam     T
 */
template<typename T>
using UPtr = std::unique_ptr<T>;

//Make unique function
using std::make_unique;

//Cast to dynamic pointer
using std::dynamic_pointer_cast;

//Cast to static pointer
using std::static_pointer_cast;

//Making shared 
using std::make_shared;

//Make pair
using std::make_pair;

//FileSystem
namespace fsys = std::filesystem;

//Path
using Path = std::filesystem::path;

//Wstring & String converters functions
/*String
to_string(wString wstr) {
  int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
  std::string str(count, 0);
  WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
  return str;
}

wString
to_wstring(String str) {
  int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
  std::wstring wstr(count, 0);
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
  return wstr;
}*/

//Console output
#define ConsoleOut std::cout

//Console input
#define ConsoleIn std::cin;

//Console line
#define ConsoleLine std::endl;

//
#define toString(x) std::to_string(x)

using std::ios;
using std::fmodf;
using std::max;
using std::min;
using std::abs;
using std::function;