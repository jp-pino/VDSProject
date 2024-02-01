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

bool Reachability::isReachable(const std::vector<bool> &stateVector) {
  // Compute Transition Relation τ
  auto τ = True();
  for (int i = 0; i < stateVector.size(); i++) {
    τ = and2(τ, or2(and2(next_states[i], transitions[i]),
                    and2(neg(next_states[i]), neg(transitions[i]))));
  }

  // Compute Characteristic Function for Initial State (CS0)
  auto cs0 = True();
  for (int i = 0; i < stateVector.size(); i++) {
    cs0 = and2(cs0, xnor2(states[i], init_state[i]));
  }

  auto cr_it = cs0;
  auto cr = cr_it;

  do {
    cr = cr_it;
    // Compute BBD for image of next states
    auto img_next = and2(cr, τ);
    for (int i = stateVector.size() - 1; i >= 0; i--) {
      img_next = or2(coFactorTrue(img_next, states[i]),
                     coFactorFalse(img_next, states[i]));
    }

    // Compute BDD for image of current states
    auto img = True();
    for (int i = 0; i < stateVector.size(); i++) {
      img = and2(img, xnor2(states[i], next_states[i]));
    }
    img = and2(img, img_next);

    for (int i = stateVector.size() - 1; i >= 0; i--) {
      img = or2(coFactorTrue(img, next_states[i]),
                coFactorFalse(img, next_states[i]));
    }

    cr_it = or2(cr, img);

  } while (cr_it != cr);

  spdlog::info(">>> cr: {} <<<", cr);

  for (int i = 0; i < stateVector.size(); i++) {
    cr = stateVector[i] ? coFactorTrue(cr, states[i])
                        : coFactorFalse(cr, states[i]);
  }

  return cr == True();
}

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
