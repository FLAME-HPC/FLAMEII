/*!
 * \file xparser2/printer.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Utility class for writing text to an output stream
 */
#ifndef XPARSER__PRINTER_HPP_
#define XPARSER__PRINTER_HPP_
#include <ostream>
#include <map>
#include <string>

namespace xparser {

class Printer {
  public:
    Printer(std::ostream &s, char delimiter = '$');
    
    void Print(const char* text);
    void Print(const char* text,
               const std::map<std::string, std::string>& vars);
    void Print(const char* text,
               const char* var, const std::string& value);
    void Print(const char* text,
               const char* var1, const std::string& value1,
               const char* var2, const std::string& value2);

    // Print string as is. No indentation. No var replacement.
    void PrintRaw(const std::string& text);
    void PrintRaw(const char* text);
    
    void PrintFromFile(const char* filename);
    void PrintFromFile(const char* filename,
                       const std::map<std::string, std::string>& vars);
    void PrintFromFile(const char* filename,
                       const char* var, const std::string& value);
    void PrintFromFile(const char* filename,
                       const char* var1, const std::string& value1,
                       const char* var2, const std::string& value2);
    void PrintRawFromFile(const char* filename);

    void Indent();
    void Outdent();
    
  private:
    std::ostream &s_;
    std::string indent_;
    char delim_;
    bool at_start_of_line_;
  
    void write_(const char* data, int size);
    
    Printer(const Printer&);
    void operator=(const Printer&);
};

}  // namespace xparser
#endif  // XPARSER__PRINTER_HPP_
