/*!
 * \file src/io/io_xml_model.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLModel: reading of model XML file
 */
#include "io_xml_model.hpp"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <fstream>

namespace flame { namespace io {

void printNode(boost::property_tree::ptree::value_type const& v, int level) {
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v2, v.second ) {
        for(int i = 0; i < level; i++) std::cout << "\t";
        std::cout << v2.first << "\n";
        printNode(v2, level+1);
    }
}

int IOXMLModel::readFile(std::string file_name) {
    std::ifstream myfile;
    myfile.open(file_name.c_str());
    if (myfile.is_open()) {

        using boost::property_tree::ptree;
        ptree pt;
        read_xml(myfile, pt);

        try {
            pt.get_child("xmodel");
        } catch (std::exception& e) {
            std::cout << "File does not have root 'xmodel': " << file_name << "\n";
            return 2;
        }

        BOOST_FOREACH( ptree::value_type const& v, pt.get_child("xmodel") ) {
            std::cout << v.first << "\n";
            printNode(v, 1);
        }

        myfile.close();
        return 0;
    } else {
        std::cout << "Can't open file " << file_name << "\n";
        return 1;
    }
}

}}  // namespace flame::io
