#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include <tuple>
#include <vector>

#include "../Manager.h"
#include "ReachabilityInterface.h"

namespace ClassProject {

class Reachability : public ReachabilityInterface {
 private:
  std::vector<BDD_ID> states;
  std::vector<BDD_ID> inputs;
  std::vector<BDD_ID> next_states;
  std::vector<bool> init_state;
  std::vector<BDD_ID> transitionFunctions;
  BDD_ID tau;
  BDD_ID cs0;

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

  inline const std::vector<BDD_ID> &getStates() const override {
    return states;
  }

  inline const std::vector<BDD_ID> &getInputs() const override {
    return inputs;
  }

  bool isReachable(const std::vector<bool> &stateVector) override;
  int stateDistance(const std::vector<bool> &stateVector) override;
  void setTransitionFunctions(
      const std::vector<BDD_ID> &transitionFunctions) override;
  void setInitState(const std::vector<bool> &stateVector) override;

 private:
  BDD_ID existential_quantification(const BDD_ID &f,
                                    const std::vector<BDD_ID> &v);

  BDD_ID restrict(const BDD_ID &f, const std::vector<bool> &decision,
                  const std::vector<BDD_ID> &vars);
  std::tuple<BDD_ID, std::unordered_map<BDD_ID, bool>> try_restrict(
      const BDD_ID &f);

  bool test_reachability(const BDD_ID &cr,
                         const std::vector<bool> &stateVector);
};

}  // namespace ClassProject
#endif
