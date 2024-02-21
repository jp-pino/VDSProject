#include "Node.h"

#include "ManagerInterface.h"

namespace ClassProject {

const BDD_ID Node::id() const { return _id; }
const Node Node::high() const { return manager->getNode(_high); }
const Node Node::low() const { return manager->getNode(_low); }
const Node Node::top() const { return manager->getNode(_top); }
const std::string Node::label() const { return _label; }

Node Node::high(BDD_ID high) {
  _high = high;
  return this->high();
}

Node Node::high(const Node high) {
  _high = high._id;
  return this->high();
}

Node Node::low(BDD_ID low) {
  _low = low;
  return this->low();
}

Node Node::low(const Node low) {
  _low = low._id;
  return this->low();
}

Node Node::top(BDD_ID top) {
  _top = top;
  return this->top();
}

Node Node::top(const Node top) {
  _top = top._id;
  return this->top();
}

std::string Node::label(std::string label) {
  _label = label;
  return this->label();
}

bool Node::isConstant() const {
  return _id == _high && _id == _low && _id == this->_top;
}
bool Node::isVariable() const { return !isConstant() && _top == _id; }

void Node::dump() const {
  spdlog::info(
      "Node: {}"
      "\n  Label: {}"
      "\n  Top: {}"
      "\n  High: {}"
      "\n  Low: {}",
      (long)id(), label(), top().id(), high().id(), low().id());
}

bool Node::operator==(const Node& rhs) const {
  return _high == rhs._high && _low == rhs._low && _top == rhs._top;
}

bool Node::operator==(const BDD_ID& rhs) const {
  auto node = manager->getNode(rhs);
  return *this == node;
}

bool Node::operator!=(const Node& rhs) const { return !(*this == rhs); }

bool Node::operator>(const BDD_ID& rhs) const { return _id > rhs; }
bool Node::operator>=(const BDD_ID& rhs) const { return _id >= rhs; }
bool Node::operator<(const BDD_ID& rhs) const { return _id < rhs; }
bool Node::operator<=(const BDD_ID& rhs) const { return _id <= rhs; }

bool Node::operator>(const Node& rhs) const { return _id > rhs.id(); }
bool Node::operator>=(const Node& rhs) const { return _id >= rhs.id(); }
bool Node::operator<(const Node& rhs) const { return _id < rhs.id(); }
bool Node::operator<=(const Node& rhs) const { return _id <= rhs.id(); }

const Node Node::operator+(const Node& rhs) const {
  return manager->or2(*this, rhs);
}
const Node Node::operator|(const Node& rhs) const { return *this + rhs; }
Node& Node::operator+=(const Node& rhs) {
  *this = *this + rhs;
  return *this;
}
const Node Node::nor(const Node& rhs) const {
  return manager->nor2(*this, rhs);
}

const Node Node::operator*(const Node& rhs) const {
  return manager->and2(*this, rhs);
}
const Node Node::operator&(const Node& rhs) const { return *this * rhs; }
Node& Node::operator*=(const Node& rhs) {
  *this = *this * rhs;
  return *this;
}
const Node Node::nand(const Node& rhs) const {
  return manager->nand2(*this, rhs);
}

const Node Node::operator^(const Node& rhs) const {
  return manager->xor2(*this, rhs);
}
Node& Node::operator^=(const Node& rhs) {
  *this = *this ^ rhs;
  return *this;
}

const Node Node::xnor(const Node& rhs) const {
  return manager->xnor2(*this, rhs);
}

const Node Node::operator!() const { return manager->neg(*this); }

const Node Node::operator|(const Cofactor& rhs) const {
  if (rhs.type) return manager->coFactorTrue(*this, rhs.node);
  return manager->coFactorFalse(*this, rhs.node);
}

const Node Node::existential_quantification(const std::vector<Node>& v) const {
  auto temp = *this;
  for (int64_t i = v.size() - 1; i >= 0; i--) {
    temp = (temp | Cofactor(v[i], true)) + (temp | Cofactor(v[i], false));
  }
  return temp;
}

const Node Node::universal_quantification(const std::vector<Node>& v) const {
  auto temp = *this;
  for (int64_t i = v.size() - 1; i >= 0; i--) {
    temp = (temp | Cofactor(v[i], true)) * (temp | Cofactor(v[i], false));
  }
  return temp;
}

const Node Node::restrict(const std::vector<bool>& k,
                          const std::vector<Node>& v) const {
  auto temp = *this;
  for (int64_t i = k.size() - 1; i >= 0; i--) {
    temp = temp | Cofactor(v[i], k[i]);
  }
  return temp;
}

const Node Node::tseitin(const Node& node) const { return this->xnor(node); }

}  // namespace ClassProject
