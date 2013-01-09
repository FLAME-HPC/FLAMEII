/*!
 * \file xparser2/xparser2.cpp
 * \author Shawn Chin, Simon Coakley
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Xparser
 */
#include <fstream>
#include <map>
#include <boost/lexical_cast.hpp>
#include "flame2/io/io_manager.hpp"
#include "printer.hpp"
#include "codegen/gen_datastruct.hpp"
#include "xparser2.hpp"

void printCondition(std::string cname, flame::model::XCondition * condition, xparser::Printer * p) {
    std::map<std::string, std::string> variables;
    variables["cname"] = cname;

    if (condition->isNot) p->Print("$cname$->isNot = true;\n", variables);
    if (condition->isValues) {
        p->Print("$cname$->isValues = true;\n", variables);
        /* Handle lhs */
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
        /* Handle operator */
        variables["op"] = condition->op;
        p->Print("$cname$->op = \"$op$\";\n", variables);
        /* Handle rhs */
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
        printCondition(cname + "->lhsCondition", condition->lhsCondition, p);
        variables["op"] = condition->op;
        p->Print("$cname$->op = \"$op$\";\n", variables);
        p->Print("$cname$->rhsCondition = new model::XCondition;\n", variables);
        printCondition(cname + "->rhsCondition", condition->rhsCondition, p);
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

int main(int argc, const char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0];
        std::cerr << " MODEL_FILE" << std::endl;
        exit(1);
     }

    std::string model_path = argv[1];

    // Load and validate model
    flame::model::XModel model;
    flame::io::IOManager::GetInstance().loadModel(model_path, &model);
    if (model.validate() != 0) {
        std::cerr << "Invalid model" << std::endl;
        exit(2);
    }

    // Open file for writing
    std::ofstream outfile;
    outfile.open ("output.txt");

    // create printer instance
    //xparser::Printer p(outfile);
    xparser::Printer p(std::cout);

    p.Print("#include \"flame2/exe/task_manager.hpp\"\n");
    p.Print("#include \"flame2/exe/scheduler.hpp\"\n");
    p.Print("#include \"flame2/exe/splitting_fifo_task_queue.hpp\"\n");
    p.Print("#include \"flame2/io/io_manager.hpp\"\n");
    p.Print("#include \"flame2/model/xmodel.hpp\"\n");
    p.Print("#include \"flame2/mb/message_board_manager.hpp\"\n");

    p.Print("\nint main(int argc, const char* argv[]) {\n");
    p.Indent();

    p.Print("// Define tasks\n");
    p.Print("flame::exe::TaskManager& task_mgr = flame::exe::TaskManager::GetInstance();\n");
    p.Print("model::XMachine * agent = 0;\n");
    p.Print("model::XFunction * function = 0;\n");
    p.Print("model::XIOput * ioput = 0;\n");
    p.Print("flame::model::XCondition * condition = 0;\n");
    p.Print("// Create model\n");
    p.Print("model::XModel model;\n");
    std::map<std::string, std::string> variables;
    boost::ptr_vector<flame::model::XMachine>::iterator agent;
    boost::ptr_vector<flame::model::XVariable>::iterator variable;
    boost::ptr_vector<flame::model::XFunction>::iterator func;
    boost::ptr_vector<flame::model::XIOput>::iterator ioput;
    boost::ptr_vector<flame::model::XMessage>::iterator message;
    // Agents
    boost::ptr_vector<flame::model::XMachine> * agents = model.getAgents();
    for (agent = agents->begin(); agent != agents->end(); ++agent) {
        variables["agent_name"] = (*agent).getName();
        p.Print("agent = model.addAgent(\"$agent_name$\");\n", variables);
        // Agent memory
        boost::ptr_vector<flame::model::XVariable> * vars = (*agent).getVariables();
        for (variable = vars->begin(); variable != vars->end(); ++variable) {
            variables["var_name"] = (*variable).getName();
            variables["var_type"] = (*variable).getType();
            p.Print("agent->addVariable(\"$var_type$\", \"$var_name$\");\n", variables);
        }
        // Agent functions
        boost::ptr_vector<flame::model::XFunction> * funcs = (*agent).getFunctions();
        for (func = funcs->begin(); func != funcs->end(); ++func) {
            variables["func_name"] = (*func).getName();
            variables["func_current_state"] = (*func).getCurrentState();
            variables["func_next_state"] = (*func).getNextState();
            p.Print("// Function: $func_name$\n", variables);
            p.Print("function = agent->addFunction();\n");
            p.Print("function->setName(\"$func_name$\");\n", variables);
            p.Print("function->setCurrentState(\"$func_current_state$\");\n", variables);
            p.Print("function->setNextState(\"$func_next_state$\");\n", variables);
            // Condition
            if ((*func).getCondition()) {
                p.Print("condition = function->addCondition();\n");
                printCondition("condition", (*func).getCondition(), &p);
            }
            // Outputs
            boost::ptr_vector<flame::model::XIOput> * outputs = (*func).getOutputs();
            for (ioput = outputs->begin(); ioput != outputs->end(); ++ioput) {
                variables["message_name"] = (*ioput).getMessageName();
                p.Print("ioput = function->addOutput();\n");
                p.Print("ioput->setMessageName(\"$message_name$\");\n", variables);
            }
            // Inputs
            boost::ptr_vector<flame::model::XIOput> * inputs = (*func).getInputs();
            for (ioput = inputs->begin(); ioput != inputs->end(); ++ioput) {
                variables["message_name"] = (*ioput).getMessageName();
                p.Print("ioput = function->addInput();\n");
                p.Print("ioput->setMessageName(\"$message_name$\");\n", variables);
                if ((*ioput).getFilter()) {
                    p.Print("condition = ioput->addFilter();\n");
                    printCondition("condition", (*ioput).getFilter(), &p);
                }
                if ((*ioput).isRandom()) p.Print("ioput->setRandom(true);\n");
                if ((*ioput).isSort()) {
                    p.Print("ioput->setSort(true);\n");
                    variables["sort_key"] = (*ioput).getSortKey();
                    variables["sort_order"] = (*ioput).getSortOrder();
                    p.Print("ioput->setSortKey(\"$sort_key$\");\n", variables);
                    p.Print("ioput->setSortOrder(\"$sort_order$\");\n", variables);
                }
            }
            // Memory Access info
            if ((*func).getMemoryAccessInfoAvailable()) {
                p.Print("function->setMemoryAccessInfoAvailable(true);\n");
                std::vector<std::string>::iterator var;
                std::vector<std::string> * readOnly = (*func).getReadOnlyVariables();
                for (var = readOnly->begin(); var != readOnly->end(); ++var) {
                    variables["read_only_var"] = (*var);
                    p.Print("function->addReadOnlyVariable($read_only_var$);\n", variables);
                }
                std::vector<std::string> * readWrite = (*func).getReadWriteVariables();
                for (var = readWrite->begin(); var != readWrite->end(); ++var) {
                    variables["read_write_var"] = (*var);
                    p.Print("function->addReadWriteVariable($read_write_var$);\n", variables);
                }
            }
        }
    }

    p.Print("// Validate model\n");
    p.Print("model.validate();\n");
    p.Print("// Register model memory\n");
    p.Print("model.registerWithMemoryManager();\n");
    p.Print("// Register model tasks\n");
    p.Print("model.registerWithTaskManager();\n");
    // Register messages
    p.Print("// Register messages\n");
    p.Print("flame::mb::MessageBoardManager& mb_mbr = flame::mb::MessageBoardManager::GetInstance();\n");
    boost::ptr_vector<flame::model::XMessage> * messages = model.getMessages();
    for (message = messages->begin(); message != messages->end(); ++message) {
        variables["message_name"] = (*message).getName();
        p.Print("mb_mbr.RegisterMessage<$message_name$_message>(\"$message_name$\");\n", variables);
    }

    p.Outdent();
    p.Print("}\n\n");

    //xparser::Printer p(std::cout);
    for (message = messages->begin(); message != messages->end(); ++message) {
        xparser::codegen::GenDataStruct msg((*message).getName() + "_message");
        boost::ptr_vector<flame::model::XVariable> * vars = (*message).getVariables();
        for (variable = vars->begin(); variable != vars->end(); ++variable)
            msg.AddVar((*variable).getType(), (*variable).getName());
        msg.Generate(p);
    }

    /*
    1. main.cpp
      - register agents
      - register agent functions
        - state change
        - mem read/write
        - message send/recv
      - register messages
      - registed datatypes (?)
    2. model_datatypes.cpp
      - generate message datatypes
      - generate user-defined datatypes
    3. data.xsd
      - schema to validate input data
    4. Makefile
    */

    /*
    std::map<std::string, std::string> variables;
    variables["hello"] = "world";
    variables["hey"] = "jude";
    p.Print("$hello$ peace, said $hey$.\n", variables);
    p.Print("I have no memory of this code\n");
    p.Print("say $one$\n", "one", "hello");
    p.Print("struct {\n");
    p.Indent();
    p.Print("double x;\n");
    p.Print("double y;\n");
    p.Outdent();
    p.Print("} msg;\n");
    */

    // close file when done
    outfile.close();

    return 0;



}
