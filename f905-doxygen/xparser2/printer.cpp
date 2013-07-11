/*!
 * \file xparser2/printer.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Utility class for writing text to an output stream
 */
#include <map>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include "flame2/exceptions/base.hpp"
#include "printer.hpp"

namespace {  // utility functions for use within this file only

// Allocates a char* buffer with size equal to the file size, copies in
// the file content and returns the buffer.
// The deallocation of the buffer is left to the caller, e.g.
//    const char *buffer = get_cstr_from_file(filename);
//    // ...
//    delete[] buffer;
const char* get_cstr_from_file(const std::string& filename) {
  // open file for reading
  std::ifstream in(filename.c_str());
  if (in.fail()) {
    throw flame::exceptions::invalid_argument(
      std::string("Error reading file : ") + filename);
  }

  // get file size
  in.seekg(0, std::ifstream::end);
  int length = in.tellg();
  // create string buffer and append NULL char
  char *buffer = new char[length + 1];
  buffer[length] = '\0';

  // read file into buffer
  in.seekg(0, std::ifstream::beg);
  in.read(buffer, length);
  in.close();

  return buffer;
}
}

namespace xparser {

Printer::Printer(std::ostream *s, char delimiter)
  : s_(s), indent_(), delim_(delimiter), at_start_of_line_(true) {
}

void Printer::Indent() {
  indent_ += "  ";
}

void Printer::Outdent() {
  if (indent_.empty()) {
    throw flame::exceptions::invalid_operation(
      "Outdent() without matching Indent().");
  }
  indent_.resize(indent_.size() - 2);  // reduce indent string by one level
}

void Printer::write_(const char* data, int size) {
  if (at_start_of_line_ && (size > 0) && (data[0] != '\n')) {
    at_start_of_line_ = false;
    s_->write(indent_.data(), indent_.size());  // place indent string
  }
  s_->write(data, size);
}

int Printer::PrintVariable(const char* text, const char* end, int* pos,
    const std::map<std::string, std::string>& vars) {
  int endpos = end - text;
  std::string varname(text + *pos, endpos - *pos);
  if (varname.empty()) {
    // Two delimiters in a row reduce to a literal delimiter character.
    write_(&delim_, 1);
  } else {
    // lookup var map and perform substitution
    std::map<std::string, std::string>::const_iterator \
        iter = vars.find(varname);
    if (iter == vars.end()) {
      throw flame::exceptions::invalid_argument(
          std::string("Unknown var - ") + varname);
    } else {
      write_(iter->second.data(), iter->second.size());
    }
  }

  *pos = endpos + 1;
  return endpos;
}

void Printer::Print(const char* text,
                    const std::map<std::string, std::string>& vars) {
  int size = strlen(text);
  int pos = 0;

  // Iterate through string so we can indent lines and perform var substitution
  for (int i = 0; i < size; ++i) {
    if (text[i] == '\n') {  // found new line
      // Write what we have so far, including the newline
      write_(text + pos, i - pos + 1);
      pos = i + 1;

      // next write_() should insert an indent
      at_start_of_line_ = true;

    } else if (text[i] == delim_) {  // found substitution candidate
      // First, dump previously seen text
      write_(text + pos, i - pos);
      pos = i + 1;

      // Locate closing delimiter
      const char* end = strchr(text + pos, delim_);
      if (end == NULL) {
        throw flame::exceptions::invalid_argument(
          "Closing delimiter not found");
      }

      // handle var and advance past the var placeholder
      i = PrintVariable(text, end, &pos, vars);
    }
  }

  // Write the rest.
  write_(text + pos, size - pos);
}

void Printer::Print(const char* text) {
  static std::map<std::string, std::string> empty;
  Print(text, empty);
}

void Printer::Print(const char* text,
                    const char* var, const std::string& value) {
  std::map<std::string, std::string> vars;
  vars[var] = value;
  Print(text, vars);
}

void Printer::Print(const char* text,
                    const char* var1, const std::string& value1,
                    const char* var2, const std::string& value2) {
  std::map<std::string, std::string> vars;
  vars[var1] = value1;
  vars[var2] = value2;
  Print(text, vars);
}

void Printer::Print(const char* text,
                    const char* var1, const std::string& value1,
                    const char* var2, const std::string& value2,
                    const char* var3, const std::string& value3) {
  std::map<std::string, std::string> vars;
  vars[var1] = value1;
  vars[var2] = value2;
  vars[var3] = value3;
  Print(text, vars);
}

void Printer::PrintRaw(const std::string& text) {
  write_(text.data(), text.size());
}

void Printer::PrintRaw(const char* text) {
  write_(text, strlen(text));
}

void Printer::PrintFromFile(const std::string& filename) {
  const char* buffer = get_cstr_from_file(filename);
  Print(buffer);
  delete[] buffer;
}

void Printer::PrintFromFile(const std::string& filename,
                            const std::map<std::string, std::string>& vars) {
  const char* buffer = get_cstr_from_file(filename);
  Print(buffer, vars);
  delete[] buffer;
}

/*
void Printer::PrintFromFile(const std::string& filename,
                            const char* var, const std::string& value) {
  const char* buffer = get_cstr_from_file(filename);
  Print(buffer, var, value);
  delete[] buffer;
}

void Printer::PrintFromFile(const std::string& filename,
                            const char* var1, const std::string& value1,
                            const char* var2, const std::string& value2) {
  const char* buffer = get_cstr_from_file(filename);
  Print(buffer, var1, value1, var2, value2);
  delete[] buffer;
}
*/

void Printer::PrintRawFromFile(const std::string& filename) {
  const char* buffer = get_cstr_from_file(filename);
  PrintRaw(buffer);
  delete[] buffer;
}

}  // namespace xparser
