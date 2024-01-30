#include "Reachability.h"

#include <fmt/format.h>

namespace ClassProject {

Reachability::Reachability(unsigned int stateSize, unsigned int inputSize)
    : states(stateSize, 0),
      inputs(inputSize, 0),
      next_states(stateSize, 0),
      init_state(stateSize, false),
      transitions(stateSize, 0) {
  if (stateSize == 0) throw std::runtime_error(">>> stateSize is zero! <<<");

  for (int i = 0; i < stateSize; i++) {
    transitions[i] = states[i] = createVar(fmt::format("s{}", i));
    next_states[i] = createVar(fmt::format("s{}'", i));
  }

  for (int i = 0; i < inputSize; i++) {
    inputs[i] = createVar(fmt::format("i{}", i));
  }
}

bool Reachability::isReachable(const std::vector<bool> &stateVector) {}

int Reachability::stateDistance(const std::vector<bool> &stateVector) {}

void Reachability::setTransitionFunctions(
    const std::vector<BDD_ID> &transitionFunctions) {
  if (transitionFunctions.size() != transitions.size()) {
    throw std::runtime_error(
        ">>> The number of given transition functions does not match the "
        "number of state bits! <<<");
  }

  for (auto &tf : transitionFunctions) {
    if (tf > uniqueTableSize()) {
      throw std::runtime_error(">>> An unknown ID is provided! <<<");
    }
  }

  transitions = transitionFunctions;
}

void Reachability::setInitState(const std::vector<bool> &stateVector) {
  if (stateVector.size() != init_state.size()) {
    throw std::runtime_error(
        ">>> StateVector size does not match with number of state bits! <<<");
  }
  init_state = stateVector;
}

}  // namespace ClassProject
