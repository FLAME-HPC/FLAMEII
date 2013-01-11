#include <cstdio>
#include <iostream>
#include <string>
#include <sys/time.h>


// returns time in seconds
static double get_time(void) {
  struct timeval now;
  gettimeofday(&now, NULL);
  return now.tv_sec + (now.tv_usec * 1.e-6);
}

int main(int argc, const char* argv[]) {

  if (argc < 3 || argc > 4) {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " POP_DATA NUM_ITERATIONS [NUM_CORES]" << std::endl;
    exit(1);
  }

  std::string pop_path = argv[1];
  size_t num_iters = (size_t)atoi(argv[2]);
  if (num_iters == 0) {
    std::cerr << "Invalid value for NUM_ITERATIONS" << std::endl;
    exit(2);
  }

  size_t num_cores = 1; // default to single core run
  if (argc > 3) {
    num_cores = (size_t)atoi(argv[3]);
    if (num_cores == 0) {
      std::cerr << "Invalid value for NUM_CORES" << std::endl;
      exit(3);
    }
  }

  model::Model model;

  model.addAgent("Circle");
  model.addAgentVariable("Circle", "int", "id");
  model.addAgentVariable("Circle", "double", "x");
  model.addAgentVariable("Circle", "double", "y");
  model.addAgentVariable("Circle", "double", "fx");
  model.addAgentVariable("Circle", "double", "fy");
  model.addAgentVariable("Circle", "double", "radius");
  model.addAgentFunction("Circle", "outputdata", "start", "1");
  model.addAgentFunctionReadOnlyVariable("Circle", "outputdata", "start", "1", "x");
  model.addAgentFunctionReadOnlyVariable("Circle", "outputdata", "start", "1", "y");
  model.addAgentFunctionReadOnlyVariable("Circle", "outputdata", "start", "1", "radius");
  model.addAgentFunctionReadOnlyVariable("Circle", "outputdata", "start", "1", "id");
  model.addAgentFunction("Circle", "inputdata", "1", "2");
  model.addAgentFunctionReadOnlyVariable("Circle", "inputdata", "1", "2", "x");
  model.addAgentFunctionReadOnlyVariable("Circle", "inputdata", "1", "2", "y");
  model.addAgentFunctionReadOnlyVariable("Circle", "inputdata", "1", "2", "radius");
  model.addAgentFunctionReadOnlyVariable("Circle", "inputdata", "1", "2", "id");
  model.addAgentFunctionReadWriteVariable("Circle", "inputdata", "1", "2", "fx");
  model.addAgentFunctionReadWriteVariable("Circle", "inputdata", "1", "2", "fy");
  model.addAgentFunction("Circle", "move", "2", "end");
  model.addAgentFunctionReadOnlyVariable("Circle", "move", "2", "end", "fx");
  model.addAgentFunctionReadOnlyVariable("Circle", "move", "2", "end", "fy");
  model.addAgentFunctionReadWriteVariable("Circle", "move", "2", "end", "x");
  model.addAgentFunctionReadWriteVariable("Circle", "move", "2", "end", "y");
  model.addMessage("location");
  model.addMessageVariable("location", "int", "id");
  model.addMessageVariable("location", "double", "x");
  model.addMessageVariable("location", "double", "y");

  // Create simulation using model and path to initial pop
  flame::sim::Simulation s(&model, pop_path);

  double start_time, stop_time, total_time;
  start_time = get_time();

  // Run simulation
  s.start(num_iters);

  stop_time = get_time();
  total_time = stop_time - start_time;
  printf("Execution time - %d:%02d:%03d [mins:secs:msecs]\n",
           (int)(total_time/60), 
           ((int)total_time)%60, 
           (((int)(total_time * 1000.0)) % 1000));
  
  return 0;
}
