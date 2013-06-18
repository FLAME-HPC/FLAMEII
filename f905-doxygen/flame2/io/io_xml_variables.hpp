/*!
 * \file flame2/io/io_xml_variables.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLVariables: reading of variables from XML
 */
#ifndef IO__IO_XML_VARIABLES_HPP_
#define IO__IO_XML_VARIABLES_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include "flame2/model/xvariable.hpp"
#include "io_xml_element.hpp"

namespace flame { namespace io { namespace xml {

class IOXMLVariables {
  public:
    IOXMLVariables() {}
    /*!
     * \brief Read variables
     * \param[in] node The element
     * \param[out] variables The variables
     */
    void readVariables(xmlNode * node,
        boost::ptr_vector<model::XVariable> * variables);

  private:
    //! \brief Read elements
    IOXMLElement ioxmlelement;
    /*!
     * \brief Read a variable
     * \param[in] node The element
     * \param[out] variables The variables
     */
    void readVariable(xmlNode * node,
        boost::ptr_vector<model::XVariable> * variables);
};

}}}  // namespace flame::io::xml
#endif  // IO__XML_VARIABLES_HPP_
