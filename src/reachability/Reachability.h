#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include <tuple>
#include <vector>

#include "../Manager.h"
#include "ReachabilityInterface.h"

namespace ClassProject {

class Reachability : public ReachabilityInterface {
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
   * @brief Existential quantification operator
   * @param f Node Characteristic function
   * @param v std::vector<Node> Vector of variables to quantify
   * @return Node Disjunction of the cofactors of f w.r.t list of variables
   */
  const Node existential_quantification(const Node &f,
                                        const std::vector<Node> &v);

  /**
   * @brief Restrict operator
   *
   * Shannon cofactor of f w.r.t variable v
   *
   * @param f Node Characteristic function to restrict
   * @param k std::vector<bool> Constants to restrict the
   * function
   * @param v std::vector<Node> List of variables to restrict the function
   * @return Node of the resulting BDD after the restriction
   */
  const Node restrict(const Node &f, const std::vector<bool> &k,
                      const std::vector<Node> &v);

  /**
   * @brief Try to restrict a BDD to a constant
   *
   * The function tries to restrict the given BDD to the smallest BDD that's not
   * False and returns the combination of variables that lead to this result.
   * The variables it uses are the top variables of the BDD.
   *
   * @param f Node Characteristic function to restrict
   * @return std::tuple<Node, std::unordered_map<Node, bool>> Resulting BDD
   * and the values of the variables that result in the restriction
   */
  std::tuple<Node, std::unordered_map<Node, bool>> try_restrict(const Node &f);

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
#endif
