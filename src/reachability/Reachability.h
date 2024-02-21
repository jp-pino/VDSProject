#pragma once

#include <tuple>
#include <vector>

#include "../Manager.h"
#include "IReachability.h"

namespace ClassProject {

class Reachability : public IReachability {
 private:
  std::vector<Node> states;
  std::vector<Node> inputs;
  std::vector<Node> next_states;
  std::vector<bool> init_state;
  std::vector<Node> transitionFunctions;
  Node tau;
  Node cs0;

 public:
  /**
   * The constructor initializes a default state machine with the given number
   * of variables. All state variables should be created within the constructor.
   * The default transition function for each state bit is the identity
   * function. For the default initial state, all bits are assumed to be set to
   * false. Hence, after calling the constructor, the only reachable state
   * should be the initial state.
   *
   * @param stateSize vector specifying the number of bits
   * @param inputSize number of boolean input bits, defaults to zero
   * @throws std::runtime_error if stateSize is zero
   */
  explicit Reachability(unsigned int stateSize, unsigned int inputSize = 0);

  inline const std::vector<Node> &getStates() const override { return states; }

  inline const std::vector<Node> &getInputs() const override { return inputs; }

  // Overridden methods
  bool isReachable(const std::vector<bool> &stateVector) override;
  int stateDistance(const std::vector<bool> &stateVector) override;
  void setTransitionFunctions(
      const std::vector<Node> &transitionFunctions) override;
  void setInitState(const std::vector<bool> &stateVector) override;

 private:
  /**
   * @brief Test reachability of a state
   *
   * @param cr Node Characteristic function to restrict
   * @param stateVector std::vector<bool> State to test reachability
   * @return bool True if the state is reachable, False otherwise
   */
  bool test_reachability(const Node &cr, const std::vector<bool> &stateVector);
};

}  // namespace ClassProject
