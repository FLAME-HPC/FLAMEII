/*!
 * \file flame2/io/xml_writer.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMLWriter: helps write XML
 */
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <string>
#include <vector>
#include <set>
#include <cstdio>
#include <utility>
#include <iostream>
#include <sstream>
#include "flame2/config.hpp"
#include "flame2/mem/vector_wrapper.hpp"
#include "flame2/exceptions/io.hpp"
#include "xml_writer.hpp"

namespace exc = flame::exceptions;

namespace flame { namespace io { namespace xml {

// initialise text writer pointer to be zero
XMLWriter::XMLWriter()
    : writer_(0) {}

void XMLWriter::setWriterPtr(xmlTextWriterPtr writer) {
  writer_ = writer;
}

void XMLWriter::writeXMLEndTag(int count) {
  // count default is 1
  // for each count close an element
  int ii;
  for (ii = 0; ii < count; ++ii)
    if (xmlTextWriterEndElement(writer_) < 0)
      throw exc::flame_io_exception("Failed while writing XML end element");
}

void XMLWriter::writeXMLTag(std::string name) {
  // write a tag
  if (xmlTextWriterStartElement(writer_, BAD_CAST name.c_str()) < 0)
    throw exc::flame_io_exception("Failed while writing XML start element");
}

void XMLWriter::writeXMLTag(std::string name, int value) {
  // write tag with an int value
  if (xmlTextWriterWriteFormatElement(
      writer_, BAD_CAST name.c_str(), "%d", value) < 0)
    throw exc::flame_io_exception("Failed while writing XML tag");
}

void XMLWriter::writeXMLTag(std::string name, double value) {
  // write tag with a double value
  if (xmlTextWriterWriteFormatElement(
      writer_, BAD_CAST name.c_str(), "%f", value) < 0)
    throw exc::flame_io_exception("Failed while writing XML tag");
}

void XMLWriter::writeXMLTag(std::string name, std::string value) {
  // write tag with a string value
  if (xmlTextWriterWriteFormatElement(writer_, BAD_CAST name.c_str(), "%s",
      value.c_str()) < 0)
    throw exc::flame_io_exception("Failed while writing XML tag");
}

void XMLWriter::writeXMLTagAttribute(std::string name, std::string value) {
  // write an attribute
  if (xmlTextWriterWriteAttribute(writer_, BAD_CAST name.c_str(),
      BAD_CAST value.c_str()) < 0)
    throw exc::flame_io_exception("Failed while writing XML attribute");
}

void XMLWriter::endXMLDoc() {
  // close a doc and end tags
  if (xmlTextWriterEndDocument(writer_) < 0)
    throw exc::flame_io_exception("Could not close XML doc");
}

}}}  // namespace flame::io::xml
