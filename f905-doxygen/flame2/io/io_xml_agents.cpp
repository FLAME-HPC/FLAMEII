/*!
 * \file flame2/io/io_xml_agent.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLAgents: reading of agents from XML
 */
#include <string>
#include <vector>
#include "flame2/config.hpp"
#include "io_xml_agents.hpp"
#include "io_xml_element.hpp"

namespace flame { namespace io { namespace xml {

void IOXMLAgents::readAgents(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of xagents */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "xagent")
        readAgent(cur_node, model);
      else
        ioxmlelement.readUnknownElement(cur_node);
    }
  }
}

void IOXMLAgents::readAgent(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;
  model::XMachine * xm = 0;

  /* Loop through each child of xagent */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "name")
        xm = model->addAgent(ioxmlelement.getElementValue(cur_node));
      else if (name == "description") {}
      else if (name == "memory")
        ioxmlvariables.readVariables(cur_node, xm->getVariables());
      else if (name == "functions")
        readTransitions(cur_node, xm);
      else
        ioxmlelement.readUnknownElement(cur_node);
    }
  }
}

void IOXMLAgents::readInputs(xmlNode * node,
    model::XFunction * xfunction) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of inputs */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "input")
        readInput(cur_node, xfunction);
      else
        ioxmlelement.readUnknownElement(cur_node);
    }
  }
}

void IOXMLAgents::readInput(xmlNode * node,
    model::XFunction * xfunction) {
  xmlNode *cur_node = NULL;
  model::XIOput * input = xfunction->addInput();

  /* Loop through each child of input */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "messageName") {
        input->setMessageName(ioxmlelement.getElementValue(cur_node));
      } else if (name == "filter") {
        readCondition(cur_node, input->addFilter());
      } else if (name == "sort") {
        readSort(cur_node, input);
      } else if (name == "random") {
        /* Indicate that random is set */
        input->setRandomSet(true);
        /* Read the string ready to be validated later */
        input->setRandomString(ioxmlelement.getElementValue(cur_node));
      } else {
        ioxmlelement.readUnknownElement(cur_node);
      }
    }
  }
}

void IOXMLAgents::readOutputs(xmlNode * node,
    model::XFunction * xfunction) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of outputs */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "output")
        readOutput(cur_node, xfunction);
      else
        ioxmlelement.readUnknownElement(cur_node);
    }
  }
}

void IOXMLAgents::readOutput(xmlNode * node,
    model::XFunction * xfunction) {
  xmlNode *cur_node = NULL;
  model::XIOput * output = xfunction->addOutput();

  /* Loop through each child of output */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "messageName")
        output->setMessageName(ioxmlelement.getElementValue(cur_node));
      else
        ioxmlelement.readUnknownElement(cur_node);
    }
  }
}

void IOXMLAgents::readTransition(xmlNode * node,
    model::XMachine * machine) {
  xmlNode *cur_node = NULL;
  model::XFunction * xfunction = machine->addFunction();

  // Set memory access information to be unavailable
  xfunction->setMemoryAccessInfoAvailable(false);

  /* Loop through each child of transition */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "name")
        xfunction->setName(ioxmlelement.getElementValue(cur_node));
      else if (name == "description") {}
      else if (name == "currentState")
        xfunction->setCurrentState(ioxmlelement.getElementValue(cur_node));
      else if (name == "nextState")
        xfunction->setNextState(ioxmlelement.getElementValue(cur_node));
      else if (name == "condition")
        /* Create condition from function */
        readCondition(cur_node, xfunction->addCondition());
      else if (name == "outputs") readOutputs(cur_node, xfunction);
      else if (name == "inputs") readInputs(cur_node, xfunction);
      else if (name == "memoryAccess")
        readMemoryAccess(cur_node, xfunction);
      else
        ioxmlelement.readUnknownElement(cur_node);
    }
  }
}

void IOXMLAgents::readTransitions(xmlNode * node,
    model::XMachine * machine) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of transitions */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "function")
        readTransition(cur_node, machine);
      else
        ioxmlelement.readUnknownElement(cur_node);
    }
  }
}

void IOXMLAgents::readSort(xmlNode * node,
    model::XIOput * xioput) {
  xmlNode *cur_node = NULL;
  xioput->setSort(true);

  /* Loop through each child of message */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "key")
        xioput->setSortKey(ioxmlelement.getElementValue(cur_node));
      else if (name == "order")
        xioput->setSortOrder(ioxmlelement.getElementValue(cur_node));
      else
        ioxmlelement.readUnknownElement(cur_node);
    }
  }
}

void IOXMLAgents::readConditionTime(model::XConditionTime * xconditiontime,
    xmlNode *cur_node) {
  xmlNode *cur_node_2 = NULL;
  for (cur_node_2 = cur_node->children;
      cur_node_2; cur_node_2 = cur_node_2->next) {
    /* If node is an XML element */
    if (cur_node_2->type == XML_ELEMENT_NODE) {
      std::string name_2 = ioxmlelement.getElementName(cur_node_2);
      /* Handle each time element */
      if (name_2 == "period") {
        xconditiontime->setTimePeriod(ioxmlelement.getElementValue(cur_node_2));
      } else if (name_2 == "phase") {
        xconditiontime->setTimePhaseVariable(
            ioxmlelement.getElementValue(cur_node_2));
      } else if (name_2 == "duration") {
        xconditiontime->setFoundTimeDuration(true);
        xconditiontime->setTimeDurationString(
            ioxmlelement.getElementValue(cur_node_2));
      } else {
        ioxmlelement.readUnknownElement(cur_node_2);
      }
    }
  }
}

void IOXMLAgents::readConditionLhs(model::XCondition * xc,
    xmlNode *cur_node) {
  /* Set up and read lhs */
  xc->setLhsCondition(new model::XCondition);
  xc->values.setTempValue("");
  readCondition(cur_node, xc->lhsCondition());
  /* Handle if lhs is a value or a condition */
  if (xc->lhsCondition()->values.tempValue() != "") {
    /* lhs is a value */
    xc->values.setLhs(xc->lhsCondition()->values.tempValue());
    xc->values.setLhsIsValue(true);
    delete xc->lhsCondition();
    xc->setLhsCondition(0);
  } else {
    /* lhs is a nested condition */
    xc->setLhsIsCondition(true);
  }
}

void IOXMLAgents::readConditionRhs(model::XCondition * xc,
    xmlNode *cur_node) {
  /* Set up and read rhs */
  xc->setRhsCondition(new model::XCondition);
  xc->values.setTempValue("");
  readCondition(cur_node, xc->rhsCondition());
  /* Handle if rhs is a value or a condition */
  if (xc->rhsCondition()->values.tempValue() != "") {
    /* rhs is a value */
    xc->values.setRhs(xc->rhsCondition()->values.tempValue());
    xc->values.setRhsIsValue(true);
    delete xc->rhsCondition();
    xc->setRhsCondition(0);
  } else {
    /* rhs is a nested condition */
    xc->setRhsIsCondition(true);
  }
}

void IOXMLAgents::readCondition(xmlNode * node,
    model::XCondition * xc) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of message */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child and call appropriate
       * processing function */
      if (name == "not") { xc->setIsNot(true);
        readCondition(cur_node, xc);
      } else if (name == "time") {
        xc->setIsTime(true);
        readConditionTime(&xc->time, cur_node);
      } else if (name == "lhs") {
        readConditionLhs(xc, cur_node);
      } else if (name == "op") {
        xc->setOp(ioxmlelement.getElementValue(cur_node));
      } else if (name == "rhs") {
        readConditionRhs(xc, cur_node);
      } else if (name == "value") {
        xc->values.setTempValue(ioxmlelement.getElementValue(cur_node));
      } else {
        ioxmlelement.readUnknownElement(cur_node);
      }
    }
  }
}

void IOXMLAgents::readMemoryAccessVariables(xmlNode * node,
    std::vector<std::string> * variables) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of message */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child and call appropriate
       * processing function */
      if (name == "variableName") {
        variables->push_back(ioxmlelement.getElementValue(cur_node));
      } else {
        ioxmlelement.readUnknownElement(cur_node);
      }
    }
  }
}

void IOXMLAgents::readMemoryAccess(
    xmlNode * node, model::XFunction * xfunction) {
  xmlNode *cur_node = NULL;

  // Set memory access information to be available
  xfunction->setMemoryAccessInfoAvailable(true);

  /* Loop through each child of message */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child and call appropriate
       * processing function */
      if (name == "readOnly") {
        readMemoryAccessVariables(cur_node,
            xfunction->getReadOnlyVariables());
      } else if (name == "readWrite") {
        readMemoryAccessVariables(cur_node,
            xfunction->getReadWriteVariables());
      } else {
        ioxmlelement.readUnknownElement(cur_node);
      }
    }
  }
}

}}}  // namespace flame::io::xml
