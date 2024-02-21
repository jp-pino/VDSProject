#include "Reachability.h"

#include <fmt/format.h>

#include <boost/dynamic_bitset.hpp>
#include <cmath>

namespace ClassProject {

Reachability::Reachability(unsigned int stateSize, unsigned int inputSize)
    : states(stateSize, False()),
      inputs(inputSize, False()),
      next_states(stateSize, False()),
      init_state(stateSize, false),
      transitionFunctions(stateSize, False()),
      tau(True()),
      cs0(True()) {
  if (stateSize == 0) throw std::runtime_error(">>> stateSize is zero! <<<");

  for (unsigned int i = 0; i < stateSize; i++) {
    transitionFunctions[i] = states[i] = createVar(fmt::format("s{}", i));
    next_states[i] = createVar(fmt::format("s{}'", i));
  }

  for (unsigned int i = 0; i < inputSize; i++) {
    inputs[i] = createVar(fmt::format("i{}", i));
  }

  setInitState(std::vector<bool>(stateSize, false));
  setTransitionFunctions(std::vector<Node>(stateSize, False()));
}

bool Reachability::isReachable(const std::vector<bool> &stateVector) {
  return stateDistance(stateVector) >= 0;
}

int Reachability::stateDistance(const std::vector<bool> &stateVector) {
  if (stateVector.size() != init_state.size()) {
    throw std::runtime_error(
        ">>> StateVector size does not match with number of state bits! <<<");
  }

  auto prev = cs0;
  auto cr = cs0;
  auto distance = 0;
  spdlog::debug("cr: {}", cr.id());
  do {
    // Compute BBD for image of next states
    auto img_next = (cr * tau)
                        .existential_quantification(states)
                        .existential_quantification(inputs);

    // Rename next state as current state
    auto img = True();
    for (size_t i = 0; i < stateVector.size(); i++) {
      img *= states[i].tseitin(next_states[i]);
    }

    img = (img * img_next)
              .existential_quantification(next_states)
              .existential_quantification(inputs);

    prev = cr;
    cr += img;
    distance++;

    // Check if the state is reachable at this iteration
    spdlog::info("cr: {}", cr.id());
    spdlog::info("prev: {}", prev.id());
  } while (!test_reachability(prev, stateVector) && cr != prev);

  return test_reachability(cr, stateVector) ? distance - 1 : -1;
}

void Reachability::setTransitionFunctions(
    const std::vector<Node> &transitionFunctions) {
  if (this->transitionFunctions.size() != transitionFunctions.size()) {
    throw std::runtime_error(
        ">>> The number of given transition functions does not match the "
        "number of state bits! <<<");
  }

  for (auto &tf : transitionFunctions) {
    if (tf > uniqueTableSize()) {
      throw std::runtime_error(">>> An unknown ID is provided! <<<");
    }
  }

  this->transitionFunctions = transitionFunctions;

  // Compute Transition Relation tau
  tau = True();
  for (size_t i = 0; i < this->transitionFunctions.size(); i++) {
    tau *= next_states[i].tseitin(this->transitionFunctions[i]);
  }
}

void Reachability::setInitState(const std::vector<bool> &stateVector) {
  if (stateVector.size() != init_state.size()) {
    throw std::runtime_error(
        ">>> StateVector size does not match with number of state bits! <<<");
  }
  init_state = stateVector;

  // Compute Characteristic Function for Initial State (CS0)
  cs0 = True();
  for (size_t i = 0; i < init_state.size(); i++) {
    cs0 *= states[i].tseitin(init_state[i] ? True() : False());
  }
}

bool Reachability::test_reachability(const Node &cr,
                                     const std::vector<bool> &stateVector) {
  auto result = cr.restrict(states, stateVector);

  spdlog::debug(
      "\n  incoming cr:             {}"
      "\n  restrict cr on states:   {}",
      cr.id(), result.id());

  spdlog::debug("  result == True(): {}", result == True());
  return result == True();
}

}  // namespace ClassProject
