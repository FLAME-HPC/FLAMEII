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

/*!
 * \brief Helper class to write XML using libxml2
 */
class XMLWriter {
  public:
    /*!
     * \brief Constructor
     */
    explicit XMLWriter();
    /*!
     * \brief Set pointer to text to write to
     * \param[in] writer The text writer pointer
     */
    void setWriterPtr(xmlTextWriterPtr writer);
    /*!
     * \brief End file and close all tags
     */
    void endXMLDoc();
    /*!
     * \brief End tag or tags
     * \param[in] count Number of tags to close, default to 1
     */
    void writeXMLEndTag(int count = 1);
    /*!
     * \brief Start a tag
     * \param[in] name The name of the tag
     */
    void writeXMLTag(std::string name);
    /*!
     * \brief Start a tag that holds an int value
     * \param[in] name The name of the tag
     * \param[in] value The int value
     */
    void writeXMLTag(std::string name, int value);
    /*!
     * \brief Start a tag that holds a double value
     * \param[in] name The name of the tag
     * \param[in] value The double value
     */
    void writeXMLTag(std::string name, double value);
    /*!
     * \brief Start a tag that holds a string
     * \param[in] name The name of the tag
     * \param[in] value The string value
     */
    void writeXMLTag(std::string name, std::string value);
    /*!
     * \brief Start a tag that holds an attribute
     * \param[in] name The name of the tag
     * \param[in] value The attribute string value
     */
    void writeXMLTagAttribute(std::string name, std::string value);
  private:
    xmlTextWriterPtr writer_;
};
}}  // namespace flame::io
#endif  // IO__XML_WRITER_HPP_
