#include "Reachability.h"

#include <fmt/format.h>

#include <cmath>

namespace ClassProject {

Reachability::Reachability(unsigned int stateSize, unsigned int inputSize)
    : states(stateSize, 0),
      inputs(inputSize, 0),
      next_states(stateSize, 0),
      init_state(stateSize, false),
      transitions(stateSize, 0) {
  if (stateSize == 0) throw std::runtime_error(">>> stateSize is zero! <<<");

  for (unsigned int i = 0; i < stateSize; i++) {
    transitions[i] = states[i] = createVar(fmt::format("s{}", i));
    next_states[i] = createVar(fmt::format("s{}'", i));
  }

  for (unsigned int i = 0; i < inputSize; i++) {
    inputs[i] = createVar(fmt::format("i{}", i));
  }
}

bool Reachability::isReachable(const std::vector<bool> &stateVector) {
  if (stateVector.size() != init_state.size()) {
    throw std::runtime_error(
        ">>> StateVector size does not match with number of state bits! <<<");
  }

  auto cr_it = cs0;
  auto cr = cr_it;
  do {
    cr = cr_it;
    // Compute BBD for image of next states
    auto img_next = existential_quantification(and2(cr, τ), states);

    // Compute BDD for image of current states
    auto img = True();
    for (size_t i = 0; i < stateVector.size(); i++) {
      img = and2(img, xnor2(states[i], next_states[i]));
    }
    img = existential_quantification(and2(img, img_next), next_states);

    cr_it = or2(cr, img);
  } while (cr_it != cr);

  spdlog::debug("cr: {}", cr);

  for (size_t i = 0; i < stateVector.size(); i++) {
    cr = stateVector[i] ? coFactorTrue(cr, states[i])
                        : coFactorFalse(cr, states[i]);
  }

  spdlog::debug("cr (after state restriction): {}", cr);
  if (inputs.size() == 0 || isConstant(cr)) return cr == True();

  int possible_inputs = std::pow(2, inputs.size());

  spdlog::debug("possible inputs: {}", possible_inputs);

  for (int input = 0; input < possible_inputs; input++) {
    auto temp_cr = cr;

    for (size_t i = 0; i < inputs.size(); i++) {
      temp_cr = input & (1 << i) ? coFactorTrue(temp_cr, inputs[i])
                                 : coFactorFalse(temp_cr, inputs[i]);
    }

    spdlog::debug("cr (input {}): {}", input, temp_cr);
    if (isConstant(temp_cr)) return true;
  }

  return false;
}

int Reachability::stateDistance(const std::vector<bool> &stateVector) {
  if (stateVector.size() != init_state.size()) {
    throw std::runtime_error(
        ">>> StateVector size does not match with number of state bits! <<<");
  }

  auto cr_it = cs0;
  auto cr = cr_it;
  auto distance = 0;
  do {
    cr = cr_it;
    // Compute BBD for image of next states
    auto img_next = existential_quantification(and2(cr, τ), states);

    // Compute BDD for image of current states
    auto img = True();
    for (size_t i = 0; i < stateVector.size(); i++) {
      img = and2(img, xnor2(states[i], next_states[i]));
    }
    img = existential_quantification(and2(img, img_next), next_states);

    cr_it = or2(cr, img);
    distance++;

    spdlog::debug("cr: {}", cr);

    auto cr_solved = cr;
    for (size_t i = 0; i < stateVector.size(); i++) {
      cr_solved = stateVector[i] ? coFactorTrue(cr_solved, states[i])
                                 : coFactorFalse(cr_solved, states[i]);
    }
    spdlog::debug("cr_solved: {}", cr_solved);
    if (cr_solved == True()) break;
  } while (cr_it != cr);
  return distance;
}

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

  // Compute Transition Relation τ
  τ = True();
  for (size_t i = 0; i < transitions.size(); i++) {
    τ = and2(τ, or2(and2(next_states[i], transitions[i]),
                    and2(neg(next_states[i]), neg(transitions[i]))));
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
  for (size_t i = 0; i < stateVector.size(); i++) {
    cs0 = and2(cs0, xnor2(states[i], init_state[i]));
  }
}

BDD_ID Reachability::existential_quantification(const BDD_ID &f,
                                                const std::vector<BDD_ID> &v) {
  auto eq = f;
  for (int64_t i = v.size() - 1; i >= 0; i--) {
    eq = or2(coFactorTrue(eq, v[i]), coFactorFalse(eq, v[i]));
  }
  return eq;
}

}  // namespace ClassProject
