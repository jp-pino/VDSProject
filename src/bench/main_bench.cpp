//
// Written by Carolina P. Nogueira 2016
// Refactored by Deutschmann 28.09.2021
//

#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <string>

#include "BenchParser.hpp"
#include "BenchmarkLib.h"
#include "CircuitToBDD.hpp"
#include "Manager.h"

#define DEBUG 0

int main(int argc, char *argv[]) {
  spdlog::cfg::load_env_levels();

#if DEBUG
  std::string bench_file = "../../../benchmarks/iscas85/c17.bench";
#else
  if (2 > argc) {
    std::cout << "Must specify a filename!" << std::endl;
    return -1;
  }

  std::string bench_file = argv[1];
#endif

  /* Parse the circuit from file and generate topological sorted circuit */
  BenchParser parsed_circuit(bench_file);

  std::cout << "- Initializating BDD manager... ";
  auto BDD_manager = make_shared<ClassProject::Manager>();
  std::cout << "Done!" << std::endl;
  std::cout << "- Initializating circuit to BDD converter... ";
  auto circuit2BDD = make_unique<CircuitToBDD>(BDD_manager);
  std::cout << "Done!" << std::endl;

  double user_time, vm1, rss1, vm2, rss2;

  std::cout << "- Generating BDD from circuit:" << std::endl;
  std::cout << "  - Initializing... " << std::flush;
  process_mem_usage(vm1, rss1);
  user_time = userTime();
  std::cout << "Done!" << std::endl;
  std::cout << "  - Generating BDD... " << std::flush;
  circuit2BDD->GenerateBDD(parsed_circuit.GetSortedCircuit(), bench_file);
  std::cout << "Done!" << std::endl;
  user_time = userTime() - user_time;
  std::cout << " BDD generated successfully!" << std::endl << std::endl;

  circuit2BDD->PrintBDD(parsed_circuit.GetListOfOutputLabels());

  std::cout << "**** Performance ****" << std::endl;
  std::cout << " Runtime: " << user_time << std::endl;
  process_mem_usage(vm2, rss2);
  std::cout << " VM: " << vm2 - vm1 << "; RSS: " << rss2 - rss1 << endl << endl;

  return 0;
}
