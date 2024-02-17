// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014
#pragma once

#include <spdlog/spdlog.h>

#include <boost/functional/hash.hpp>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "ManagerInterface.h"

namespace ClassProject {

typedef std::tuple<BDD_ID, BDD_ID, BDD_ID> Key;

class Node;

struct TupleHasher {
  std::size_t operator()(const Key& key) const {
    std::size_t seed = 0;

    // std::get<2>(key) << 21 | std::get<1>(key) << 42 | std::get<0>(key);
    boost::hash_combine(seed, boost::hash_value(std::get<0>(key)));
    boost::hash_combine(seed, boost::hash_value(std::get<1>(key)));
    boost::hash_combine(seed, boost::hash_value(std::get<2>(key)));

    return seed;
  }
};

class Manager : public ManagerInterface {
 private:
  size_t ucache_hit = 0, pcache_hit = 0;
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
  std::vector<std::shared_ptr<Node>> nodes;
  // T       H       L
  std::unordered_map<Key, BDD_ID, TupleHasher> unique_table;
  // std::map<Key, BDD_ID> unique_table;

  /**
   * @brief Computed Table
   * Used to improve run time. It stores for every triple (f, g, h) a pointer to
   * function ite(f, g, h) in the unique table. In this way, repeated
   * ite-computations of the same operands are avoided.
   */
  // I       T       E
  std::unordered_map<Key, BDD_ID, TupleHasher> computed_table;
  // std::map<Key, BDD_ID> computed_table;

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
  const Node createVar(const std::string& label) override;
  const Node createVar(const std::string& label, const BDD_ID& top,
                       const BDD_ID& high, const BDD_ID& low);

  /**
   * @brief Get the ID of the constant True
   * @return ID of the constant True
   */
  const Node True() override;

  /**
   * @brief Get the ID of the constant False
   * @return ID of the constant False
   */
  const Node False() override;

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
  const Node ite(const Node& i, const Node& t, const Node& e) override;

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
  const Node coFactorTrue(const Node& f, const Node& x) override;

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
  const Node coFactorFalse(const Node& f, const Node& x) override;

  /**
   * @brief Compute the AND of two nodes
   * @param a ID of the first node
   * @param b ID of the second node
   * @return ID of the result node
   */
  const Node and2(const Node& a, const Node& b) override;

  /**
   * @brief Compute the OR of two nodes
   * @param a ID of the first node
   * @param b ID of the second node
   * @return ID of the result node
   */
  const Node or2(const Node& a, const Node& b) override;

  /**
   * @brief Compute the XOR of two nodes
   * @param a ID of the first node
   * @param b ID of the second node
   * @return ID of the result node
   */
  const Node xor2(const Node& a, const Node& b) override;

  /**
   * @brief Compute the negation of a node
   * @param a ID of the node
   * @return ID of the result node
   */
  const Node neg(const Node& a) override;

  /**
   * @brief Compute the NAND of two nodes
   * @param a ID of the first node
   * @param b ID of the second node
   * @return ID of the result node
   */
  const Node nand2(const Node& a, const Node& b) override;

  /**
   * @brief Compute the NOR of two nodes
   * @param a ID of the first node
   * @param b ID of the second node
   * @return ID of the result node
   */
  const Node nor2(const Node& a, const Node& b) override;

  /**
   * @brief Compute the XNOR of two nodes
   * @param a ID of the first node
   * @param b ID of the second node
   * @return ID of the result node
   */
  const Node xnor2(const Node& a, const Node& b) override;

  void dump();

  void findNodes(const Node& root, std::set<Node>& nodes_of_root) override;
  void findNodes(const BDD_ID& root, std::set<BDD_ID>& nodes_of_root) override;

  void findVars(const Node& root, std::set<Node>& vars_of_root) override;
  void findVars(const BDD_ID& root, std::set<BDD_ID>& vars_of_root) override;
  std::vector<Node> findVars(const Node& root) override;

  size_t uniqueTableSize() override;

  size_t ucache_hits() override { return ucache_hit; }
  size_t pcache_hits() override { return pcache_hit; }

  // Not implemented yet

  void visualizeBDD_internal(std::ofstream& file, const Node& root);
  void visualizeBDD(std::string filepath, const Node& root,
                    bool test_result) override;

  void mermaidGraph_internal(std::ofstream& file, const Node& root,
                             std::set<Node>& printed_nodes);
  void mermaidGraph(std::string filepath, const Node& root);

  const Node getNode(const BDD_ID& id) const;
};

}  // namespace ClassProject
