/*!
 * \file flame2/io/plugins/xml_writer.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMLWriter: helps write XML
 */

#ifndef IO__XML_WRITER_HPP_
#define IO__XML_WRITER_HPP_
#include <libxml/xmlwriter.h>
#include <string>

namespace flame { namespace io {

class XMLWriter {
  public:
    explicit XMLWriter();
    void setWriterPtr(xmlTextWriterPtr writer);
    void endXMLDoc();
    void writeXMLEndTag(int count = 1);
    void writeXMLTag(std::string name);
    void writeXMLTag(std::string name, int value);
    void writeXMLTag(std::string name, double value);
    void writeXMLTag(std::string name, std::string value);
    void writeXMLTagAttribute(std::string name, std::string value);
  private:
    xmlTextWriterPtr writer_;
};
}}  // namespace flame::io
#endif  // IO__XML_WRITER_HPP_
