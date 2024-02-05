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
      transitionFunctions(stateSize, 0) {
  if (stateSize == 0) throw std::runtime_error(">>> stateSize is zero! <<<");

  for (unsigned int i = 0; i < stateSize; i++) {
    transitionFunctions[i] = states[i] = createVar(fmt::format("s{}", i));
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
  spdlog::debug(">>> isReachable");

  auto cr_it = cs0;
  auto cr = cr_it;
  do {
    spdlog::debug("cr: {}", cr);
    cr = cr_it;
    // Compute BBD for image of next states
    auto img_next = existential_quantification(
        existential_quantification(and2(cr, tau), states), inputs);

    // Compute BDD for image of current states
    auto img = True();
    for (size_t i = 0; i < stateVector.size(); i++) {
      img = and2(img, xnor2(states[i], next_states[i]));
    }
    img = existential_quantification(
        existential_quantification(and2(img, img_next), next_states), inputs);

    cr_it = or2(cr, img);
  } while (cr_it != cr);

  spdlog::debug("FINAL cr: {}", cr);

  return test_reachability(cr, stateVector);
}

int Reachability::stateDistance(const std::vector<bool> &stateVector) {
  if (stateVector.size() != init_state.size()) {
    throw std::runtime_error(
        ">>> StateVector size does not match with number of state bits! <<<");
  }
  spdlog::debug(">>> stateDistance");

  auto cr_it = cs0;
  auto cr = cr_it;
  auto distance = 0;
  do {
    cr = cr_it;
    // Compute BBD for image of next states
    auto img_next = existential_quantification(
        existential_quantification(and2(cr, tau), states), inputs);

    // Compute BDD for image of current states
    auto img = True();
    for (size_t i = 0; i < stateVector.size(); i++) {
      img = and2(img, xnor2(states[i], next_states[i]));
    }
    img = existential_quantification(
        existential_quantification(and2(img, img_next), next_states), inputs);

    cr_it = or2(cr, img);
    distance++;

    // Check if the state is reachable at this iteration
  } while (!test_reachability(cr, stateVector) && cr_it != cr);

  return test_reachability(cr, stateVector) ? distance : -1;
}

void Reachability::setTransitionFunctions(
    const std::vector<BDD_ID> &transitionFunctions) {
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
    tau = and2(
        tau, or2(and2(next_states[i], this->transitionFunctions[i]),
                 and2(neg(next_states[i]), neg(this->transitionFunctions[i]))));
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
    cs0 = and2(cs0, xnor2(states[i], init_state[i]));
  }
}

BDD_ID Reachability::existential_quantification(const BDD_ID &f,
                                                const std::vector<BDD_ID> &v) {
  auto temp = f;
  for (int64_t i = v.size() - 1; i >= 0; i--) {
    temp = or2(coFactorTrue(temp, v[i]), coFactorFalse(temp, v[i]));
  }
  return temp;
}

BDD_ID Reachability::restrict(const BDD_ID &f, const std::vector<bool> &k,
                              const std::vector<BDD_ID> &v) {
  auto temp = f;
  for (int64_t i = k.size() - 1; i >= 0; i--) {
    temp = k[i] ? coFactorTrue(temp, v[i]) : coFactorFalse(temp, v[i]);
  }
  return temp;
}

std::tuple<BDD_ID, std::unordered_map<BDD_ID, bool>> Reachability::try_restrict(
    const BDD_ID &f) {
  const std::vector<BDD_ID> vars = findVars(f);
  if (vars.size() == 0 || isConstant(f)) {
    spdlog::warn(">>> No variables to restrict! <<<");
    return {f, {}};
  };

  size_t possible_inputs = std::pow(2, vars.size());

  BDD_ID lowest = f;
  size_t lowest_input = 0;
  size_t lowest_var_count = vars.size();

  for (size_t input = 0; input < possible_inputs; input++) {
    auto temp = f;

    // Convert input to a vector of bools
    boost::dynamic_bitset<> input_bits(vars.size(), input);
    std::vector<bool> input_vector;
    for (size_t i = 0; i < vars.size(); i++) {
      input_vector.push_back(input_bits[i]);
    }

    temp = restrict(temp, input_vector, vars);

    if (findVars(temp).size() < lowest_var_count && temp != False()) {
      lowest = temp;
      lowest_input = input;
      lowest_var_count = findVars(temp).size();
    }

    if (lowest == True()) break;
  }

  // Convert lowest_input to a map of BDD_IDs -> bools
  boost::dynamic_bitset<> input_bits(vars.size(), lowest_input);
  std::unordered_map<BDD_ID, bool> input_map;
  for (size_t i = 0; i < vars.size(); i++) {
    input_map[vars[i]] = input_bits[i];
  }

  return {lowest, input_map};
}

bool Reachability::test_reachability(const BDD_ID &cr,
                                     const std::vector<bool> &stateVector) {
  auto temp = restrict(cr, stateVector, states);
  auto [result, input_map] = try_restrict(temp);

  spdlog::debug(
      "\n  incoming cr:             {}"
      "\n  restrict cr on states:   {}"
      "\n  try_restrict cr on vars: {}",
      cr, temp, result);

  return result == True();
}

}  // namespace ClassProject
