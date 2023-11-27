#include "Manager.h"

namespace ClassProject {

Manager::Manager() {
  auto& false_n = nodes[createVar("False")];
  false_n.high = false_n.id;
  false_n.low = false_n.id;
  false_n.top_var = false_n.id;

  auto& true_n = nodes[createVar("True")];
  true_n.high = true_n.id;
  true_n.low = true_n.id;
  true_n.top_var = true_n.id;
}

BDD_ID Manager::createVar(const std::string& label) {
  nodes.emplace_back(nodes.size(), label);
  return nodes.back().id;
}

const BDD_ID& Manager::True() {
  return nodes[1].id;
}

const BDD_ID& Manager::False() {
  return nodes[0].id;
}

bool Manager::isConstant(BDD_ID f) {
  return nodes[f].id == nodes[f].high && nodes[f].id == nodes[f].low &&
         nodes[f].id == nodes[f].top_var;
}

bool Manager::isVariable(BDD_ID x) {
  return !isConstant(x);
}

BDD_ID Manager::topVar(BDD_ID f) {
  return nodes[f].top_var;
}
}  // namespace ClassProject
