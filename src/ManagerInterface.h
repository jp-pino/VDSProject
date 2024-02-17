//
// Created by ludwig on 01.03.16
//
#pragma once

#include <spdlog/spdlog.h>

#include <set>
#include <string>
#include <vector>

namespace ClassProject {

typedef size_t BDD_ID;
class Node;

class ManagerInterface {
 public:
  virtual const Node createVar(const std::string& label) = 0;

  virtual const Node True() = 0;

  virtual const Node False() = 0;

  virtual const Node ite(const Node& i, const Node& t, const Node& e) = 0;

  virtual const Node coFactorTrue(const Node& f, const Node& x) = 0;

  virtual const Node coFactorFalse(const Node& f, const Node& x) = 0;

  virtual const Node and2(const Node& a, const Node& b) = 0;

  virtual const Node or2(const Node& a, const Node& b) = 0;

  virtual const Node xor2(const Node& a, const Node& b) = 0;

  virtual const Node neg(const Node& a) = 0;

  virtual const Node nand2(const Node& a, const Node& b) = 0;

  virtual const Node nor2(const Node& a, const Node& b) = 0;

  virtual const Node xnor2(const Node& a, const Node& b) = 0;

  virtual void findNodes(const Node& root, std::set<Node>& nodes_of_root) = 0;
  virtual void findNodes(const BDD_ID& root,
                         std::set<BDD_ID>& nodes_of_root) = 0;

  virtual void findVars(const Node& root, std::set<Node>& vars_of_root) = 0;
  virtual void findVars(const BDD_ID& root, std::set<BDD_ID>& vars_of_root) = 0;

  virtual std::vector<Node> findVars(const Node& root) = 0;

  virtual size_t uniqueTableSize() = 0;
  virtual size_t ucache_hits() = 0;
  virtual size_t pcache_hits() = 0;

  virtual void visualizeBDD(std::string filepath, const Node& root,
                            bool test_result) = 0;

  virtual const Node getNode(const BDD_ID& id) const = 0;
};

class Node {
  mutable ManagerInterface* manager;

  BDD_ID _id;
  BDD_ID _high;
  BDD_ID _low;
  BDD_ID _top;
  std::string _label;

 public:
  Node(ManagerInterface* manager, BDD_ID id, std::string label = "")
      : manager(manager), _id(id), _label(label){};

  BDD_ID id() const { return _id; }
  const Node high() const { return manager->getNode(_high); }
  const Node low() const { return manager->getNode(_low); }
  const Node top() const { return manager->getNode(_top); }
  std::string label() const { return _label; }

  Node high(BDD_ID high) {
    _high = high;
    return this->high();
  }

  Node high(const Node high) {
    _high = high._id;
    return this->high();
  }

  Node low(BDD_ID low) {
    _low = low;
    return this->low();
  }

  Node low(const Node low) {
    _low = low._id;
    return this->low();
  }

  Node top(BDD_ID top) {
    _top = top;
    return this->top();
  }

  Node top(const Node top) {
    _top = top._id;
    return this->top();
  }

  std::string label(std::string label) {
    _label = label;
    return this->label();
  }

  bool isConstant() const {
    return _id == _high && _id == _low && _id == this->_top;
  }
  bool isVariable() const { return !isConstant() && _top == _id; }

  void dump() const {
    spdlog::info(
        "Node: {}"
        "\n  Label: {}"
        "\n  Top: {}"
        "\n  High: {}"
        "\n  Low: {}",
        (long)id(), label(), top().id(), high().id(), low().id());
  }

  bool operator==(const Node& rhs) const {
    return _high == rhs._high && _low == rhs._low && _top == rhs._top;
  }

  bool operator==(const BDD_ID& rhs) const {
    auto node = manager->getNode(rhs);
    return *this == node;
  }

  bool operator!=(const Node& rhs) const { return !(*this == rhs); }

  bool operator>(const BDD_ID& rhs) const { return _id > rhs; }
  bool operator>=(const BDD_ID& rhs) const { return _id >= rhs; }
  bool operator<(const BDD_ID& rhs) const { return _id < rhs; }
  bool operator<=(const BDD_ID& rhs) const { return _id <= rhs; }

  bool operator>(const Node& rhs) const { return _id > rhs.id(); }
  bool operator>=(const Node& rhs) const { return _id >= rhs.id(); }
  bool operator<(const Node& rhs) const { return _id < rhs.id(); }
  bool operator<=(const Node& rhs) const { return _id <= rhs.id(); }

  /**
   * @brief Or operator
   *
   * Creates a new node with the result of the or operation
   *
   * @param rhs
   * @return Node&
   */
  const Node operator+(const Node& rhs) const {
    return manager->or2(*this, rhs);
  }
  const Node operator|(const Node& rhs) const { return *this + rhs; }
  Node& operator+=(const Node& rhs) {
    *this = *this + rhs;
    return *this;
  }

  /**
   * @brief And operator
   *
   * Creates a new node with the result of the and operation
   *
   * @param rhs
   * @return Node&
   */
  const Node operator*(const Node& rhs) const {
    return manager->and2(*this, rhs);
  }
  const Node operator&(const Node& rhs) const { return *this * rhs; }
  Node& operator*=(const Node& rhs) {
    *this = *this * rhs;
    return *this;
  }

  /**
   * @brief Xor operator
   *
   * Creates a new node with the result of the xor operation
   *
   * @param rhs
   * @return Node&
   */
  const Node operator^(const Node& rhs) const {
    return manager->xor2(*this, rhs);
  }

  /**
   * @brief Negation operator
   *
   * Creates a new node with the result of the negation operation
   *
   * @return Node&
   */
  const Node operator!() const { return manager->neg(*this); }

  /**
   * @brief Cofactor True operator
   *
   * Creates a new node with the result of the Cofactor True operation
   *
   * @param rhs
   * @return Node&
   */
  const Node operator<<(const Node& rhs) const {
    return manager->coFactorTrue(*this, rhs);
  }

  /**
   * @brief Cofactor False operator
   *
   * Creates a new node with the result of the Cofactor False operation
   *
   * @param rhs
   * @return Node&
   */
  const Node operator>>(const Node& rhs) const {
    return manager->coFactorFalse(*this, rhs);
  }
};
}  // namespace ClassProject
