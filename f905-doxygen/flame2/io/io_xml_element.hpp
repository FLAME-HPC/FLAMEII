/*!
 * \file flame2/io/io_xml_element.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLElement: access element info
 */
#ifndef IO__IO_XML_ELEMENT_HPP_
#define IO__IO_XML_ELEMENT_HPP_
#include <string>

namespace flame { namespace io { namespace xml {

class IOXMLElement {
  public:
    IOXMLElement() {}
    /*!
     * \brief Get element name
     * \param[in] node The element
     * \return The element name
     */
    std::string getElementName(xmlNode * node);
    /*!
     * \brief Get element value
     * \param[in] node The element
     * \return The element value
     */
    std::string getElementValue(xmlNode * node);
    /*!
     * \brief Read unknown element
     * \param[in] node The element
     */
    void readUnknownElement(xmlNode * node);
};

}}}  // namespace flame::io::xml
#endif  // IO__XML_ELEMENT_HPP_
