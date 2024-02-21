#pragma once

#include <spdlog/spdlog.h>

#include <set>
#include <string>
#include <vector>

namespace ClassProject {

typedef size_t BDD_ID;

class IManager;
class Cofactor;
class Node {
  mutable IManager* manager;

  BDD_ID _id;
  BDD_ID _high;
  BDD_ID _low;
  BDD_ID _top;
  std::string _label;

 public:
  Node(IManager* manager, BDD_ID id, std::string label = "")
      : manager(manager), _id(id), _label(label){};

  const BDD_ID id() const;
  const Node high() const;
  const Node low() const;
  const Node top() const;
  const std::string label() const;

  Node high(BDD_ID high);
  Node high(const Node& high);

  Node low(BDD_ID low);
  Node low(const Node& low);

  Node top(BDD_ID top);
  Node top(const Node& top);

  std::string label(std::string label);

  bool isConstant() const;
  bool isVariable() const;

  void dump() const;

  // Comparison operators
  bool operator==(const Node& rhs) const;
  bool operator==(const BDD_ID& rhs) const;
  bool operator!=(const Node& rhs) const;
  bool operator>(const BDD_ID& rhs) const;
  bool operator>=(const BDD_ID& rhs) const;
  bool operator<(const BDD_ID& rhs) const;
  bool operator<=(const BDD_ID& rhs) const;
  bool operator>(const Node& rhs) const;
  bool operator>=(const Node& rhs) const;
  bool operator<(const Node& rhs) const;
  bool operator<=(const Node& rhs) const;

  /**
   * @brief Or operator
   *
   * Creates a new node with the result of the or operation
   *
   * @param rhs
   * @return Node&
   */
  const Node operator+(const Node& rhs) const;
  const Node operator|(const Node& rhs) const;
  Node& operator+=(const Node& rhs);
  Node& operator|=(const Node& rhs);

  /**
   * @brief Nor operator
   *
   * Creates a new node with the result of the nor operation
   *
   * @param rhs
   * @return Node&
   */
  const Node nor(const Node& rhs) const;

  /**
   * @brief And operator
   *
   * Creates a new node with the result of the and operation
   *
   * @param rhs
   * @return Node&
   */
  const Node operator*(const Node& rhs) const;
  const Node operator&(const Node& rhs) const;
  Node& operator*=(const Node& rhs);
  Node& operator&=(const Node& rhs);

  /**
   * @brief Nand operator
   *
   * Creates a new node with the result of the nand operation
   *
   * @param rhs
   * @return Node&
   */
  const Node nand(const Node& rhs) const;

  /**
   * @brief Xor operator
   *
   * Creates a new node with the result of the xor operation
   *
   * @param rhs
   * @return Node&
   */
  const Node operator^(const Node& rhs) const;
  Node& operator^=(const Node& rhs);

  /**
   * @brief Xnor operator
   *
   * Creates a new node with the result of the xnor operation
   *
   * @param rhs
   * @return Node&
   */
  const Node xnor(const Node& rhs) const;

  /**
   * @brief Negation operator
   *
   * Creates a new node with the result of the negation operation
   *
   * @return Node&
   */
  const Node operator!() const;
  const Node neg() const;

  /**
   * @brief Cofactor operator
   *
   * Creates a new node with the result of the Cofactor (True or False)
   * operation
   *
   * @param rhs
   * @return Node&
   */
  const Node operator|(const Cofactor& rhs) const;

  /**
   * @brief Existential quantification operator
   *
   * @param v std::vector<Node> Vector of variables to quantify
   * @return Node Disjunction of the cofactors of f w.r.t list of variables
   */
  const Node existential_quantification(const std::vector<Node>& v) const;

  /**
   * @brief Universal quantification operator
   *
   * @param v std::vector<Node> Vector of variables to quantify
   * @return Node Conjunction of the cofactors of f w.r.t list of variables
   */
  const Node universal_quantification(const std::vector<Node>& v) const;

  /**
   * @brief Restrict operator
   *
   * Shannon cofactor of f w.r.t variable v
   *
   * @param v std::vector<Node> List of variables to restrict the function
   * @param k std::vector<bool> Constants to restrict the
   * function
   * @return Node of the resulting BDD after the restriction
   */
  const Node restrict(const std::vector<Node>& v,
                      const std::vector<bool>& k) const;

  /**
   * @brief Tseitin construction
   *
   * @param node RHS of the Tseitin construction
   * @return Node of the resulting BDD after the Tseitin construction
   */
  const Node tseitin(const Node& node) const;

  std::set<Node> findNodes() const;
  std::set<Node> findVars() const;
};

struct Cofactor {
  bool type;
  Node node;

  Cofactor(const Node& node, bool type = true) : node(node), type(type){};
};
}  // namespace ClassProject
