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

/*! \brief Utility class for writing text to an output stream
 *
 * Handles indentation of lines so users can pass a printer instance around
 * and dump out code without having to worry about the indentation level.
 *
 * Also provides basic basic variable substitutions so text templates can
 * be used.
 */
class Printer {
  public:
    /*! \brief Constructor
     * \param s Output stream to print output to
     * \param delimiter variable delimiter for identifying variables in input
     */
    Printer(std::ostream &s, char delimiter = '$');

    //! Prints text with the corrent indentation inserted at each new line.
    void Print(const char* text);

    /*! \brief Prints text to output stream
     * \param text Output text
     * \param map Map of possible replacement candidates
     *
     * Prints text with the corrent indentation inserted at each new line.
     *
     * Also performs variable subsitution using the given map. Variables in
     * the input text are demarcated using the delimiter character provided
     * in the constructor (defaults to \c $).
     *
     * Throws flame::exceptions::invalid_argument if an invalid variable is
     * found.
     */
    void Print(const char* text,
               const std::map<std::string, std::string>& vars);

    //! Alternative form of Print() that does not require a map
    void Print(const char* text,
               const char* var, const std::string& value);

    //! Alternative form of Print() that does not require a map
    void Print(const char* text,
               const char* var1, const std::string& value1,
               const char* var2, const std::string& value2);
               
    //! Alternative form of Print() that does not require a map
    void Print(const char* text,
               const char* var1, const std::string& value1,
               const char* var2, const std::string& value2,
               const char* var3, const std::string& value3);

    //! Prints text as it stands. No indentation or variable substitution.
    void PrintRaw(const std::string& text);
    //! Prints text as it stands. No indentation or variable substitution.
    void PrintRaw(const char* text);

    //! Like Print() but takes the input text from a file
    void PrintFromFile(const std::string& filename);
    //! Like Print() but takes the input text from a file
    void PrintFromFile(const std::string& filename,
                       const std::map<std::string, std::string>& vars);
    //! Like Print() but takes the input text from a file
    void PrintFromFile(const std::string& filename,
                       const char* var, const std::string& value);
    //! Like Print() but takes the input text from a file
    void PrintFromFile(const std::string& filename,
                       const char* var1, const std::string& value1,
                       const char* var2, const std::string& value2);
    //! Like PrintRaw() but takes the input text from a file
    void PrintRawFromFile(const std::string& filename);

    //! Increase the indentation level
    void Indent();

    /*! \brief Reduce the indentation level
     *
     * Throws flame::exceptions::invalid_operation if called when the
     * indentation level is already 0.
     */
    void Outdent();

  private:
    std::ostream &s_;  //! Output stream
    std::string indent_;  //! String representing the indent
    char delim_;  //! Delimiter character used to identify variables in text
    bool at_start_of_line_;  //! Flag to indicate start of line (needs indent)

    /*! \brief Write chars to output stream
     * \param data Address to start reading chars from
     * \param size The number of chars to write
     */
    void write_(const char* data, int size);

    // Disable copy constructor and assignment
    Printer(const Printer&);
    void operator=(const Printer&);
};

}  // namespace xparser
#endif  // XPARSER__PRINTER_HPP_
