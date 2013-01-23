/*!
 * \file xparser2/xparser2.cpp
 * \author Shawn Chin, Simon Coakley
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Xparser
 *
 * Files generated:
 *  1. main.cpp
 *    - register agents
 *    - register agent functions
 *      - state change
 *      - mem read/write
 *      - message send/recv
 *    - register messages
 *    - registed datatypes
 * 2. message_datatypes.hpp
 * 3. Makefile
 * 4. data.xsd (TODO)
 *    - schema to validate input data
 */
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <boost/filesystem/operations.hpp>
#include "flame2/exceptions/io.hpp"
#include "flame2/io/io_manager.hpp"
#include "codegen/gen_snippets.hpp"
#include "codegen/gen_datastruct.hpp"
#include "codegen/gen_makefile.hpp"
#include "codegen/gen_headerfile.hpp"
#include "codegen/gen_maincpp.hpp"
#include "codegen/gen_model.hpp"
#include "codegen/gen_agent.hpp"
#include "codegen/gen_agentfunc.hpp"
#include "codegen/gen_message_registration.hpp"
#include "file_generator.hpp"

namespace gen = xparser::codegen;  // namespace shorthand
namespace model = flame::model;

// Functions defined further down
void build_output(model::XModel* model);
void generate_agents(model::XModel *model, gen::GenMainCpp *maincpp);
void generate_agent_functions(model::XMachine *agent, gen::GenMainCpp *maincpp);
void generate_agent_func_def(model::XModel *model,
    gen::GenMainCpp *maincpp, std::string file_name);
void generate_messages(model::XModel *model,
    gen::GenMainCpp *maincpp, std::string file_name);

// Print error message then quit with given return code
void die(const std::string& message, int rc = 2) {
  std::cerr << "ERROR: " << message << std::endl;
  exit(rc);
}

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " MODEL_FILE" << std::endl;
    exit(1);
  }

  if (!boost::filesystem::is_regular_file(argv[1])) {
    die(std::string(argv[1]) + " is not a valid file.");
  }

  // Load and validate model
  model::XModel model;
  try {
    flame::io::IOManager::GetInstance().loadModel(argv[1], &model);
  } catch(const flame::exceptions::flame_io_exception& e) {
    die(std::string("Invalid Model file\n") + e.what());
  }
  assert(model.validate() == 0);  // just in case exception not properly thrown

  // Generate output files based on model
  try {
    build_output(&model);
  } catch(const flame::exceptions::flame_exception& e) {
    die(e.what());
  }

  return 0;
}

void build_output(model::XModel* model) {
  // File generator to manage file writing
  xparser::FileGenerator filegen;
  gen::GenMakefile makefile;  // Makefile generator
  gen::GenMainCpp maincpp;    // main.cpp generator

  // initialise model and environment
  gen::GenModel genmodel;
  maincpp.Insert(genmodel);

  // output to main.cpp code to define agents
  generate_agents(model, &maincpp);

  // Write out header file for agent function definitions
  generate_agent_func_def(model, &maincpp, "agent_function_definitions.hpp");
  makefile.AddHeaderFile("agent_function_definitions.hpp");

  // Define and register messages
  generate_messages(model, &maincpp, "message_datatypes.hpp");
  makefile.AddHeaderFile("message_datatypes.hpp");

  // Umbrella header file which all model function files should include
  static const char* common_header_name = "flame_api.hpp";
  gen::GenHeaderFile common_header;  // flame_model.hpp generator
  common_header.AddRequiredHeader("flame2.hpp");
  common_header.AddRequiredHeader("message_datatypes.hpp");
#ifndef TESTBUILD
  printf("Writing file: %s\n", common_header_name);
#endif
  filegen.Output(common_header_name, common_header);
  makefile.AddHeaderFile(common_header_name);

  // output completed main.cpp file
#ifndef TESTBUILD
  printf("Writing file: %s\n", "main.cpp");
#endif
  filegen.Output("main.cpp", maincpp);
  makefile.AddSourceFile("main.cpp");

  // Add user agent function files to Makefile
  std::vector<std::string> * ffs = model->getFunctionFiles();
  std::vector<std::string>::iterator ff = ffs->begin();
  for (; ff != ffs->end(); ++ff) makefile.AddSourceFile((*ff));

  // Output Makefile now that all hpp and cpp files have been added
#ifndef TESTBUILD
  printf("Writing file: %s\n", "Makefile");
#endif
  filegen.Output("Makefile", makefile);
}


void generate_agent_func_def(model::XModel *model,
    gen::GenMainCpp *maincpp, std::string file_name) {
  xparser::FileGenerator filegen;
  gen::GenHeaderFile func_def_hpp;
  gen::AgentFunctionHeaderSnippets agent_func_headers;
  gen::RegisterAgentFuncSnippets register_agent_func;
  boost::ptr_vector<flame::model::XFunction>::iterator func;
  boost::ptr_vector<flame::model::XMachine>::iterator agent;
  boost::ptr_vector<flame::model::XMachine> *agents = model->getAgents();

  // for each agent function
  for (agent = agents->begin(); agent != agents->end(); ++agent) {
    boost::ptr_vector<model::XFunction> * funcs = agent->getFunctions();
    for (func = funcs->begin(); func != funcs->end(); ++func) {
      agent_func_headers.Add(func->getName());   // func declaration
      register_agent_func.Add(func->getName());  // func registrations
    }
  }

  // append generated content to appropriate files
  maincpp->Insert(register_agent_func);
  func_def_hpp.Insert(agent_func_headers);

#ifndef TESTBUILD
  printf("Writing file: %s\n", file_name.c_str());
#endif
  filegen.Output(file_name, func_def_hpp);
}

void generate_messages(model::XModel *model,
    gen::GenMainCpp *maincpp, std::string file_name) {
  // file generator
  xparser::FileGenerator filegen;
  // header file generator
  gen::GenHeaderFile msg_datatype_h;

  boost::ptr_vector<model::XVariable>::iterator v;
  boost::ptr_vector<model::XMessage>::iterator m;
  boost::ptr_vector<model::XMessage> *messages = model->getMessages();
  // for each model messsage
  for (m = messages->begin(); m != messages->end(); ++m) {
    gen::GenMessageRegistration msg_reg(m->getName());
    gen::GenDataStruct msg_datatype(m->getName() + "_message");

    // populate message vars
    boost::ptr_vector<model::XVariable> *vars = m->getVariables();
    for (v = vars->begin(); v != vars->end(); ++v) {
      msg_reg.AddVar(v->getType(), v->getName());
      msg_datatype.AddVar(v->getType(), v->getName());
    }

    // Append to main.cpp
    maincpp->Insert(msg_reg);

    // Append to message_datatype.hpp
    msg_datatype_h.Insert(msg_datatype);
  }

  // output header to file using file_name
#ifndef TESTBUILD
  printf("Writing file: %s\n", file_name.c_str());
#endif
  filegen.Output(file_name, msg_datatype_h);
}

void generate_agent_functions(model::XMachine *agent,
    gen::GenMainCpp *maincpp) {
  // iterate throught agent functions
  boost::ptr_vector<model::XFunction> *funcs = agent->getFunctions();
  boost::ptr_vector<model::XFunction>::iterator f = funcs->begin();
  for (; f != funcs->end(); ++f) {
    gen::GenAgentFunc gen_func(agent->getName(), f->getName(),
        f->getCurrentState(), f->getNextState());
    // loop outputs
    boost::ptr_vector<model::XIOput>::iterator ioput;
    boost::ptr_vector<model::XIOput> * outputs = f->getOutputs();
    for (ioput = outputs->begin(); ioput != outputs->end(); ++ioput)
      gen_func.AddOutput(ioput->getMessageName());

    // loop inputs
    boost::ptr_vector<model::XIOput> * inputs = f->getInputs();
    for (ioput = inputs->begin(); ioput != inputs->end(); ++ioput)
      gen_func.AddInput(ioput->getMessageName());

    // memory access
    std::vector<std::string>::const_iterator s;
    // specify read-write vars
    std::vector<std::string> *rw = f->getReadWriteVariables();
    for (s = rw->begin(); s != rw->end(); ++s) gen_func.AddReadWriteVar(*s);
    // specify read-only vars
    std::vector<std::string> *ro = f->getReadOnlyVariables();
    for (s = ro->begin(); s != ro->end(); ++s) gen_func.AddReadOnlyVar(*s);

    // append func def to main.cpp
    maincpp->Insert(gen_func);
  }
}

void generate_agents(model::XModel *model,
    xparser::codegen::GenMainCpp *maincpp) {
  boost::ptr_vector<model::XMachine>::iterator agent;
  boost::ptr_vector<model::XMachine> *agents = model->getAgents();
  for (agent = agents->begin(); agent != agents->end(); ++agent) {
    std::string agent_name = agent->getName();
    gen::GenAgent gen_agent(agent_name);

    // iterate through agent memory var
    boost::ptr_vector<model::XVariable> *vars = agent->getVariables();
    boost::ptr_vector<model::XVariable>::iterator v = vars->begin();
    for (; v != vars->end(); ++v) gen_agent.AddVar(v->getType(), v->getName());
    maincpp->Insert(gen_agent);

    // generate agent functions
    generate_agent_functions(&(*agent), maincpp);
  }
}

/*
void generateConditionFunction(flame::model::XCondition * condition, xparser::Printer * p) {
    std::map<std::string, std::string> variables;

    if (condition->isNot) p->Print("!");
    p->Print("(");
    if (condition->isValues) {
        // Handle lhs
        if (condition->lhsIsAgentVariable) {
            variables["lhs"] = condition->lhs;
            p->Print("a.$lhs$", variables);
        } else if (condition->lhsIsMessageVariable) {
            variables["lhs"] = condition->lhs;
            p->Print("m.$lhs$", variables);
        } else if (condition->lhsIsValue) {
            variables["lhsDouble"] = boost::lexical_cast<std::string>(condition->lhsDouble);
            p->Print("$lhsDouble$", variables);
        }
        // Handle operator
        variables["op"] = condition->op;
        p->Print(" $op$ ", variables);
        // Handle rhs
        if (condition->rhsIsAgentVariable) {
            variables["rhs"] = condition->rhs;
            p->Print("a.$rhs$", variables);
        } else if (condition->rhsIsMessageVariable) {
            variables["rhs"] = condition->rhs;
            p->Print("m.$rhs$", variables);
        } else if (condition->rhsIsValue) {
            variables["rhsDouble"] = boost::lexical_cast<std::string>(condition->rhsDouble);
            p->Print("$rhsDouble$", variables);
        }
    }
    if (condition->isConditions) {
        generateConditionFunction(condition->lhsCondition, p);
        variables["op"] = condition->op;
        p->Print(" $op$ ", variables);
        generateConditionFunction(condition->rhsCondition, p);
    }
    if (condition->isTime) {
        p->Print("iteration_loop");
        // If time period is not 'iteration' then
        p->Print("%$time_period_iterations$");

        p->Print(" == ");

        if (condition->timePhaseIsVariable) {
            variables["timePhaseVariable"] = condition->timePhaseVariable;
            p->Print("a.$timePhaseVariable$", variables);
        } else {
            variables["timePhaseValue"] = boost::lexical_cast<std::string>(condition->timePhaseValue);
            p->Print("$timePhaseValue$", variables);
        }
        // ToDo (SC) Handle time duration?
    }
    p->Print(")");
}

void generateConditionCreation(std::string cname, flame::model::XCondition * condition, xparser::Printer * p) {
    std::map<std::string, std::string> variables;
    variables["cname"] = cname;

    if (condition->isNot) p->Print("$cname$->isNot = true;\n", variables);
    if (condition->isValues) {
        p->Print("$cname$->isValues = true;\n", variables);
        // Handle lhs
        if (condition->lhsIsAgentVariable) {
            p->Print("$cname$->lhsIsAgentVariable = true;\n", variables);
            variables["lhs"] = condition->lhs;
            p->Print("$cname$->lhs = \"$lhs$\";\n", variables);
        }
        else if (condition->lhsIsMessageVariable) {
            p->Print("$cname$->lhsIsMessageVariable = true;\n", variables);
            variables["lhs"] = condition->lhs;
            p->Print("$cname$->lhs = \"$lhs$\";\n", variables);
        }
        else if (condition->lhsIsValue) {
            p->Print("$cname$->lhsIsValue = true;\n", variables);
            variables["lhs"] = boost::lexical_cast<std::string>(condition->lhsDouble);
            p->Print("$cname$->lhsDouble = $lhs$;\n", variables);
        }
        // Handle operator
        variables["op"] = condition->op;
        p->Print("$cname$->op = \"$op$\";\n", variables);
        // Handle rhs
        if (condition->rhsIsAgentVariable) {
            p->Print("$cname$->rhsIsAgentVariable = true;\n", variables);
            variables["rhs"] = condition->lhs;
            p->Print("$cname$->rhs = \"$rhs$\";\n", variables);
        }
        else if (condition->rhsIsMessageVariable) {
            p->Print("$cname$->rhsIsMessageVariable = true;\n", variables);
            variables["rhs"] = condition->lhs;
            p->Print("$cname$->rhs = \"$rhs$\";\n", variables);
        }
        else if (condition->rhsIsValue) {
            p->Print("$cname$->rhsIsValue = true;\n", variables);
            variables["rhs"] = boost::lexical_cast<std::string>(condition->rhsDouble);
            p->Print("$cname$->rhsDouble = $rhs$;\n", variables);
        }
    }
    if (condition->isConditions) {
        p->Print("$cname$->isConditions = true;\n", variables);
        p->Print("$cname$->lhsCondition = new model::XCondition;\n", variables);
        generateConditionCreation(cname + "->lhsCondition", condition->lhsCondition, p);
        variables["op"] = condition->op;
        p->Print("$cname$->op = \"$op$\";\n", variables);
        p->Print("$cname$->rhsCondition = new model::XCondition;\n", variables);
        generateConditionCreation(cname + "->rhsCondition", condition->rhsCondition, p);
    }
    if (condition->isTime) {
        p->Print("$cname$->isTime = true;\n", variables);
        variables["timePeriod"] = condition->timePeriod;
        p->Print("$cname$->timePeriod = \"$timePeriod$\";\n", variables);
        if (condition->timePhaseIsVariable) {
            p->Print("$cname$->timePhaseIsVariable = true;\n", variables);
            variables["timePhaseVariable"] = condition->timePhaseVariable;
            p->Print("$cname$->timePhaseVariable = \"$timePhaseVariable$\";\n", variables);
        } else {
            variables["timePhaseValue"] = boost::lexical_cast<std::string>(condition->timePhaseValue);
            p->Print("$cname$->timePhaseValue = $timePhaseValue$;\n", variables);
        }
        if (condition->foundTimeDuration) {
            p->Print("$cname$->foundTimeDuration = true;\n", variables);
            variables["timeDuration"] = boost::lexical_cast<std::string>(condition->timeDuration);
            p->Print("$cname$->timeDuration = $timeDuration$;\n", variables);
        }
    }
}

void generateConditionFunctionFiles(flame::model::XModel * model) {
    boost::ptr_vector<flame::model::XMachine>::iterator agent;
    boost::ptr_vector<flame::model::XFunction>::iterator func;
    std::map<std::string, std::string> variables;

    // Condition functions cpp
    std::ofstream condition_filter_methodscppfile;
    condition_filter_methodscppfile.open ("flame_generated_condition_filter_methods.cpp");
    // create printer instance
    xparser::Printer p2(condition_filter_methodscppfile);
    p2.Print("#include \"flame2.hpp\"\n");
    p2.Print("#include \"flame_generated_message_datatypes.hpp\"\n");
    // Generate function condition functions
    boost::ptr_vector<flame::model::XMachine> * agents = model->getAgents();
    for (agent = agents->begin(); agent != agents->end(); ++agent) {
        variables["agent_name"] = (*agent).getName();
        boost::ptr_vector<flame::model::XFunction> * funcs = (*agent).getFunctions();
        for (func = funcs->begin(); func != funcs->end(); ++func) {
            // Conditions
            if ((*func).getCondition()) {
                variables["func_name"] = (*func).getName();
                variables["func_current_state"] = (*func).getCurrentState();
                variables["func_next_state"] = (*func).getNextState();
                p2.Print("\nbool $agent_name$_$func_name$_$func_current_state$_$func_next_state$_condition() {\n", variables);
                p2.Indent();
                p2.Print("return ");
                generateConditionFunction((*func).getCondition(), &p2);
                p2.Print(";\n");
                p2.Outdent();
                p2.Print("}\n");
            }
            // Filters
            // ToDo SC
        }
    }
    // close file when done
    condition_filter_methodscppfile.close();

    // Condition function header
    std::ofstream condition_filter_methodshppfile;
    condition_filter_methodshppfile.open ("flame_generated_condition_filter_methods.hpp");
    xparser::Printer p3(condition_filter_methodshppfile);
    p3.Print("#ifndef FLAME_GENERATED_CONDITION_FILTER_METHODS_HPP_\n");
    p3.Print("#define FLAME_GENERATED_CONDITION_FILTER_METHODS_HPP_\n");
    p3.Print("#include \"flame2.hpp\"\n\n");
    // Generate function condition functions
    for (agent = agents->begin(); agent != agents->end(); ++agent) {
        variables["agent_name"] = (*agent).getName();
        boost::ptr_vector<flame::model::XFunction> * funcs = (*agent).getFunctions();
        for (func = funcs->begin(); func != funcs->end(); ++func) {
            // Conditions
            if ((*func).getCondition()) {
                variables["func_name"] = (*func).getName();
                variables["func_current_state"] = (*func).getCurrentState();
                variables["func_next_state"] = (*func).getNextState();
                p3.Print("bool $agent_name$_$func_name$_$func_current_state$_$func_next_state$_condition();\n", variables);
            }
        }
    }
    p3.Print("\n#endif  // FLAME_GENERATED_CONDITION_FILTER_METHODS_HPP_\n");
    condition_filter_methodshppfile.close();
}
 */
