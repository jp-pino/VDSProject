// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014
#pragma once

#include <spdlog/spdlog.h>

#include <map>
#include <string>
#include <vector>

#include "ManagerInterface.h"

namespace ClassProject {
struct Node {
  BDD_ID id;
  BDD_ID high;
  BDD_ID low;
  BDD_ID top;
  std::string label;

  Node(BDD_ID id, std::string label) : id(id), label(label){};

  bool isConstant() { return id == high && id == low && id == top; }
  bool isVariable() { return !isConstant(); }

  bool operator==(const Node& rhs) const {
    return high == rhs.high && low == rhs.low && top == rhs.top;
  }
};

class Manager : public ManagerInterface {
 private:
  /**
   * @brief Unique table
   * The unique table is a vector of nodes
   * The index of the vector is the id of the node
   *
   * Contains for every node of the ROBDD an ID as well as a triple consisting
   * of:
   * - the top variable for this node
   * - the ID of the low successor
   * - the ID of the high successor
   */
  std::vector<Node> unique_table;

  /**
   * @brief Computed Table
   * Used to improve run time. It stores for every triple (f, g, h) a pointer to
   * function ite(f, g, h) in the unique table. In this way, repeated
   * ite-computations of the same operands are avoided.
   */
  std::map<std::tuple<BDD_ID, BDD_ID, BDD_ID>, BDD_ID> computed_table;

 public:
  /**
   * @brief Constructor
   * Creates the constant nodes True and False
   */
  Manager();

  /**
   * @brief Create a new variable
   * Creates a new variable with the given label and returns its ID.
   *
   * @param label Label of the variable
   * @return ID of the new variable
   */
  BDD_ID createVar(const std::string& label) override;

  /**
   * @brief Get the ID of the constant True
   * @return ID of the constant True
   */
  const BDD_ID& True() override;

  /**
   * @brief Get the ID of the constant False
   * @return ID of the constant False
   */
  const BDD_ID& False() override;

  /**
   * @brief Check if a node is a constant
   * @param f ID of the node
   * @return True if the node is a constant, False otherwise
   */
  bool isConstant(BDD_ID f) override;

  /**
   * @brief Check if a node is a variable
   * @param x ID of the node
   * @return True if the node is a variable, False otherwise
   */
  bool isVariable(BDD_ID x) override;

  /**
   * @brief Get the top variable of a node
   * @param f ID of the node
   * @return ID of the top variable of the node
   */
  BDD_ID topVar(BDD_ID f) override;

  /**
   * @brief Compute the if-then-else of three nodes
   *
   * Implements the if-then-else algorithm, which most of the following
   * functions are based on. Returns the existing or new node that represents
   * the given expression. Please refer to the lecture slides for a detailed
   * description
   *
   * @param i ID of the if node
   * @param t ID of the then node
   * @param e ID of the else node
   * @return ID of the result node
   */
  BDD_ID ite(BDD_ID i, BDD_ID t, BDD_ID e) override;

  /**
   * @brief Compute the true co-factor of a node
   *
   * Returns the positive co-factor of the function represented by ID f w.r.t.
   * variable x. The second parameter is optional. If x is not specified, the
   * co-factor is determined w.r.t. the top variable of f. Example: f = a + (b
   * ∗c) with alphabetical variable order coFactorTrue(f) = 1 =
   * coFactorTrue(f, a) coFactorTrue(f, c) = a + b
   *
   * @param f ID of the node
   * @param x ID of the variable
   * @return ID of the true co-factor of the node
   */
  BDD_ID coFactorTrue(BDD_ID f, BDD_ID x) override;

  /**
   * @brief Compute the false co-factor of a node
   *
   * Returns the negative co-factor of the function represented by ID f w.r.t.
   * variable x. The second parameter is optional. If x is not specified, the
   * co-factor is determined w.r.t. the top variable of f. Example: f = a +
   * (b ∗ c) with alphabetical variable order coFactorFalse(f) = b ∗c =
   * coFactorFalse(f, a) coFactorFalse(f, c) = a + b
   *
   * @param f ID of the node
   * @param x ID of the variable
   * @return ID of the false co-factor of the node
   */
  BDD_ID coFactorFalse(BDD_ID f, BDD_ID x) override;

  BDD_ID coFactorTrue(BDD_ID f) override;
  BDD_ID coFactorFalse(BDD_ID f) override;

  /**
   * @brief Compute the AND of two nodes
   * @param a ID of the first node
   * @param b ID of the second node
   * @return ID of the result node
   */
  BDD_ID and2(BDD_ID a, BDD_ID b) override;

  /**
   * @brief Compute the OR of two nodes
   * @param a ID of the first node
   * @param b ID of the second node
   * @return ID of the result node
   */
  BDD_ID or2(BDD_ID a, BDD_ID b) override;

  /**
   * @brief Compute the XOR of two nodes
   * @param a ID of the first node
   * @param b ID of the second node
   * @return ID of the result node
   */
  BDD_ID xor2(BDD_ID a, BDD_ID b) override;

  /**
   * @brief Compute the negation of a node
   * @param a ID of the node
   * @return ID of the result node
   */
  BDD_ID neg(BDD_ID a) override;

  /**
   * @brief Compute the NAND of two nodes
   * @param a ID of the first node
   * @param b ID of the second node
   * @return ID of the result node
   */
  BDD_ID nand2(BDD_ID a, BDD_ID b) override;

  /**
   * @brief Compute the NOR of two nodes
   * @param a ID of the first node
   * @param b ID of the second node
   * @return ID of the result node
   */
  BDD_ID nor2(BDD_ID a, BDD_ID b) override;

  /**
   * @brief Compute the XNOR of two nodes
   * @param a ID of the first node
   * @param b ID of the second node
   * @return ID of the result node
   */
  BDD_ID xnor2(BDD_ID a, BDD_ID b) override;

  void dump();

  std::string getTopVarName(const BDD_ID& root) override;

  void findNodes(const BDD_ID& root, std::set<BDD_ID>& nodes_of_root) override;

  void findVars(const BDD_ID& root, std::set<BDD_ID>& vars_of_root) override;

  size_t uniqueTableSize() override;

  // Not implemented yet

  void visualizeBDD_internal(std::ofstream& file, BDD_ID& root);
  void visualizeBDD(std::string filepath, BDD_ID& root) override;

  void mermaidGraph_internal(std::ofstream& file, BDD_ID& root,
                             std::set<BDD_ID>& printed_nodes);
  void mermaidGraph(std::string filepath, BDD_ID& root);

  const Node getNode(const BDD_ID& id) const;
};
}  // namespace ClassProject
