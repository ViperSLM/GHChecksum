/* Custom string class to help get around DLL issues */

#ifndef GH_CUSTOMSTRING_H
#define GH_CUSTOMSTRING_H
#include "GHChecksum_API.h"

#include <cstdio>
#include <cstdint>

typedef size_t usize;

namespace GHChecksum {

/* StringCompare enum */
enum class StringCompare {
  EQUAL,           // ==
  NOT_EQUAL,       // !=
  LESS,            // <
  GREAT,           // >
  LESS_OR_EQUAL,   // <=
  GREAT_OR_EQUAL   // >=
};


class APP_API String {
public:
  String(void);
  String(const char *format, ...);

  ~String(void);

  /* Sets the string's value using a printf-style input
   (Also, I like C's printf() better than C++'s std::cout) */
  void Set(const char *format, ...);

  /* SetV is used in some situations. */
  void SetV(const char *format, va_list args, bool newLine = false);

  /* Returns the string's internal value as a C-style string */
  const char* Get(void) const;

/* Appends/Adds input into current string value */
  void Append(const char *format, ...);

  /* Clears the string's stored value */
  void Clear(void); 

  /* Returns a boolean value for the specified comparison */
  bool Compare(const char *input, StringCompare mode) const;
  bool Compare(String &input, StringCompare mode) const;

  /* Returns the length of the string
  (can be useful in for loops, for example) */
  usize Length(void);

  /* Resize string */
  void Resize(usize newSize);

private:
  void preset(const char *format, va_list args, bool newLine);

  /* Pointer-to-Implementation, so I can avoid having include C++'s <string> header inside of this file */
  class String_Impl;

#if defined(_WIN32) && !defined(GH_STATIC_LIB)
  String_Impl *_impl = nullptr;
#else
  LIB_PIMPL<String_Impl> _impl;
#endif
};

}

#endif