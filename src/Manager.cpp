#include "Manager.h"

#include <algorithm>

namespace ClassProject {

Manager::Manager() {
  auto& false_n = nodes[createVar("False")];
  false_n.high = false_n.id;
  false_n.low = false_n.id;
  false_n.top = false_n.id;

  auto& true_n = nodes[createVar("True")];
  true_n.high = true_n.id;
  true_n.low = true_n.id;
  true_n.top = true_n.id;
}

BDD_ID Manager::createVar(const std::string& label) {
  nodes.emplace_back(nodes.size(), label);
  return nodes.back().id;
}

const BDD_ID& Manager::True() { return nodes[1].id; }

const BDD_ID& Manager::False() { return nodes[0].id; }

bool Manager::isConstant(BDD_ID f) { return nodes[f].isConstant(); }

bool Manager::isVariable(BDD_ID x) { return !isConstant(x); }

BDD_ID Manager::topVar(BDD_ID f) { return nodes[f].top; }

BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
  if (i == True()) return t;
  if (i == False()) return e;
  if (t == e) return t;
  if (t == True() && e == False()) return i;

  auto top_vars = std::vector<BDD_ID>{topVar(i), topVar(t), topVar(e)};
  std::sort(top_vars.begin(), top_vars.end());
  auto& top = nodes[top_vars.back()];

  auto high = ite(coFactorTrue(i, top.id), coFactorTrue(t, top.id),
                  coFactorTrue(e, top.id));
  auto low = ite(coFactorFalse(i, top.id), coFactorFalse(t, top.id),
                 coFactorFalse(e, top.id));

  if (high == low) return high;

  auto precomputed = std::find_if(nodes.begin(), nodes.end(), [&](auto& node) {
    return node.top == top.id && node.high == high && node.low == low;
  });

  if (precomputed != nodes.end()) return precomputed->id;

  std::string label =
      "ite(" + top.label + "," + nodes[t].label + "," + nodes[e].label + ")";
  return createVar(label);
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
  auto& f_node = nodes[f];

  if (isConstant(f) || isConstant(x) || f_node.top > x) return f;

  if (f_node.top == x) return f_node.high;

  auto T = coFactorTrue(f_node.high, x);
  auto F = coFactorTrue(f_node.low, x);

  return ite(f_node.top, T, F);
}

BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
  auto& f_node = nodes[f];

  if (isConstant(f) || isConstant(x) || f_node.top > x) return f;

  if (f_node.top == x) return f_node.low;

  auto T = coFactorFalse(f_node.high, x);
  auto F = coFactorFalse(f_node.low, x);

  return ite(f_node.top, T, F);
}

BDD_ID Manager::coFactorTrue(BDD_ID f) { return nodes[f].high; }

BDD_ID Manager::coFactorFalse(BDD_ID f) { return nodes[f].low; }

}  // namespace ClassProject
