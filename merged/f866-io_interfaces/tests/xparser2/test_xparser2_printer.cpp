/*!
 * \file tests/xparser2/test_xparser2_utils.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for xparser2
 */
#define BOOST_TEST_DYN_LINK
#include <map>
#include <string>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include "flame2/exceptions/base.hpp"
#include "xparser2/printer.hpp"

BOOST_AUTO_TEST_SUITE(XPARSER2)

BOOST_AUTO_TEST_CASE(xp_printer) {
  std::ostringstream s_empty;
  xparser::Printer empty(&s_empty);
  BOOST_CHECK_EQUAL(s_empty.str(), "");

  // no var subst
  std::ostringstream s_plain;
  xparser::Printer plain(&s_plain);
  plain.Print("hello world");
  BOOST_CHECK_EQUAL(s_plain.str(), "hello world");

  // one var
  std::ostringstream s_var1;
  xparser::Printer var1(&s_var1);
  var1.Print("hello $WHO$?", "WHO", "world");
  BOOST_CHECK_EQUAL(s_var1.str(), "hello world?");

  // two var
  std::ostringstream s_var2;
  xparser::Printer var2(&s_var2);
  var2.Print("hello $WHO$ $WHAT$", "WHO", "world", "WHAT", "domination");
  BOOST_CHECK_EQUAL(s_var2.str(), "hello world domination");

  // using a variable map
  std::map<std::string, std::string> v;
  v["A"] = "aye";
  v["B"] = "bee";
  v["C"] = "see";
  std::ostringstream s_varmap;
  xparser::Printer varmap(&s_varmap);
  varmap.Print("$C$ \n $A$ $B$ \n\n", v);
  BOOST_CHECK_EQUAL(s_varmap.str(), "see \n aye bee \n\n");

  // exception on invalid var
  std::ostringstream s_varmap_err;
  xparser::Printer varmap_err(&s_varmap_err);
  BOOST_CHECK_THROW(varmap_err.Print("$C$ $UNKNOWN$", v),
                    flame::exceptions::invalid_argument);

  // escaping delimiter
  std::ostringstream s_delim;
  xparser::Printer delim(&s_delim);
  delim.Print("How much?\n");
  delim.Print("$$100 only.\n");
  BOOST_CHECK_EQUAL(s_delim.str(), "How much?\n$100 only.\n");

  // changing delimiter
  std::ostringstream s_chg_delim;
  xparser::Printer chg_delim(&s_chg_delim, '|');
  chg_delim.Print("$C$ \n |A| $B$ \n\n", v);
  BOOST_CHECK_EQUAL(s_chg_delim.str(), "$C$ \n aye $B$ \n\n");

  // indentation
  static const char * text = "hello\nworld\n";
  std::ostringstream s_indent;
  xparser::Printer i(&s_indent);
  i.Print("$A$\n", v);
  i.Indent();
  i.Print(text);
  i.Outdent();
  i.Print("$C$\n", v);
  BOOST_CHECK_EQUAL(s_indent.str(), "aye\n  hello\n  world\nsee\n");

  // raw printing
  std::ostringstream s_raw;
  xparser::Printer r(&s_raw);
  r.PrintRaw("$A$\n");
  r.Indent();
  r.PrintRaw(text);
  r.Outdent();
  r.Print("$C$\n", v);
  BOOST_CHECK_EQUAL(s_raw.str(), "$A$\nhello\nworld\nsee\n");

  // from file
  std::ostringstream s_file;
  xparser::Printer f(&s_file);
  f.Indent();
  f.PrintFromFile("xparser2/data/dummy_template.txt", v);
  BOOST_CHECK_EQUAL(s_file.str(), "  hello\n  aye\n  world $bee\n");

  // raw from file
  std::ostringstream s_fileraw;
  xparser::Printer fr(&s_fileraw);
  fr.Indent();
  fr.PrintRawFromFile("xparser2/data/dummy_template.txt");
  BOOST_CHECK_EQUAL(s_fileraw.str(), "  hello\n$A$\nworld $$$B$\n");
}

BOOST_AUTO_TEST_SUITE_END()
