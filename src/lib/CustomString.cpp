#include "CustomString.h"

#include <cstdarg>
#include <cstring>
#include <string>
#include <vector>

/* ------------------------------------------------ */
/* String class implementation [Private] */
/* ------------------------------------------------ */
class String::String_Impl {
public:
  String_Impl(void) {}
  ~String_Impl(void) {}

  void set(std::string in) { _str = in; }

  void append(std::string in) { _str += in; }

  const char* get(void) const {
    return _str.c_str();
  }

  void clear(void) { _str.clear(); }

  usize length(void) { return std::strlen(_str.c_str()); }

  void resize(usize newSize) { _str.resize(static_cast<usize>(newSize)); }

  bool compare(std::string in, StringCompare mode) const {
    switch (mode) {
    case StringCompare::EQUAL:
      if (in == _str) {
        return true;
      }
      return false;
      break;

    case StringCompare::NOT_EQUAL:
      if (in != _str) {
        return true;
      }
      return false;
      break;

    case StringCompare::LESS:
      if (in < _str) {
        return true;
      }
      return false;
      break;

    case StringCompare::GREAT:
      if (in > _str) {
        return true;
      }
      return false;
      break;

    case StringCompare::LESS_OR_EQUAL:
      if (in <= _str) {
        return true;
      }
      return false;
      break;

    case StringCompare::GREAT_OR_EQUAL:
      if (in >= _str) {
        return true;
      }
      return false;
      break;
    }
    // If the switch/case statement somehow fails, it'll return false
    return false;
  }

private:
  std::string _str;
};

/* ------------------------------------------------ */
/* String class implementation [Public] */
/* ------------------------------------------------ */
#if defined(_WIN32) && !defined(GH_STATIC_LIB)
String::String(void) : _impl(new String_Impl()) {}
String::String(const char *format, ...) : _impl(new String_Impl()) {
  va_list args;
  va_start(args, format);
  preset(format, args, false);
  va_end(args);
}

String::~String(void) {
  delete _impl;
  _impl = nullptr;
}
#else
String::String(void) : _impl(LIB_PIMPL_CREATE<String_Impl>()) {}
String::String(const char *format, ...) : _impl(LIB_PIMPL_CREATE<String_Impl>()) {
  va_list args;
  va_start(args, format);
  preset(format, args, false);
  va_end(args);
}

String::~String(void) = default;
#endif

void String::preset(const char *format, va_list args, bool newLine) {
  va_list args_copy;
  va_copy(args_copy, args);
  usize size = vsnprintf(nullptr, 0, format, args_copy);
  va_end(args_copy);

  // Buffer
  std::vector<char> buffer(size + 1);
  vsnprintf(buffer.data(), buffer.size(), format, args);

  // Remove null terminator
  buffer.pop_back();
  if (newLine) {
    buffer.push_back('\n');
  }
  _impl->set(std::string{buffer.begin(), buffer.end()});
}

void String::Set(const char *format, ...) {
  va_list args;
  va_start(args, format);
  va_list args_copy;
  va_copy(args_copy, args);
  usize size = vsnprintf(nullptr, 0, format, args_copy);
  va_end(args_copy);

  std::vector<char> buffer(size + 1);
  vsnprintf(buffer.data(), buffer.size(), format, args);
  va_end(args);

  buffer.pop_back();
  _impl->set(std::string{buffer.begin(), buffer.end()});
}

void String::Append(const char* format, ...) {
  va_list args;
  va_start(args, format);
  va_list args_copy;
  va_copy(args_copy, args);
  usize size = vsnprintf(nullptr, 0, format, args_copy);
  va_end(args_copy);

  std::vector<char> buffer(size + 1);
  vsnprintf(buffer.data(), buffer.size(), format, args);
  va_end(args);

  buffer.pop_back();
  _impl->append(std::string{buffer.begin(), buffer.end()});
}

void String::SetV(const char *format, va_list args, bool newLine) {
  preset(format, args, newLine);
}

const char* String::Get(void) const { return _impl->get(); }

void String::Clear(void) { _impl->clear(); }

bool String::Compare(const char *input, StringCompare mode) const {
  return _impl->compare(input,mode);
}

bool String::Compare(String &input, StringCompare mode) const {
  return _impl->compare(input.Get(), mode);
}

usize String::Length(void) { return _impl->length(); }

void String::Resize(usize newSize) { _impl->resize(newSize); }