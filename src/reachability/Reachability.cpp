#include "Reachability.h"

#include <fmt/format.h>

#include <boost/dynamic_bitset.hpp>
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

  auto top_vars = findVars(cr);
  return restrict_by_test(restrict(cr, stateVector, states), top_vars);
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

    auto top_vars = findVars(cr);
    if (restrict_by_test(restrict(cr, stateVector, states), top_vars)) break;
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

BDD_ID Reachability::restrict(const BDD_ID &f,
                              const std::vector<bool> &decision,
                              const std::vector<BDD_ID> &vars) {
  auto eq = f;
  for (int64_t i = decision.size() - 1; i >= 0; i--) {
    eq = decision[i] ? coFactorTrue(eq, vars[i]) : coFactorFalse(eq, vars[i]);
  }
  return eq;
}

bool Reachability::restrict_by_test(const BDD_ID &f,
                                    const std::vector<BDD_ID> &vars) {
  if (vars.size() == 0 || isConstant(f)) return f == True();

  int possible_inputs = std::pow(2, vars.size());

  for (int input = 0; input < possible_inputs; input++) {
    auto eq = f;

    // Convert input to a vector of bools
    boost::dynamic_bitset<> input_bits(vars.size(), input);
    std::vector<bool> input_vector;
    for (size_t i = 0; i < vars.size(); i++) {
      input_vector.push_back(input_bits[i]);
    }

    eq = restrict(eq, input_vector, vars);

    if (eq == True()) return true;
  }

  return false;
}

}  // namespace ClassProject
