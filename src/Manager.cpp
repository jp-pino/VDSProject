#include "Manager.h"

#include <fmt/format.h>

#include <algorithm>
#include <fstream>
#include <iostream>

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#define GET_NAME(var) ((var).label())
#else
#define GET_NAME(var) ((var).id())
#endif

namespace ClassProject {

Manager::Manager() {
  createVar("False", nodes.size(), nodes.size(), nodes.size());
  createVar("True", nodes.size(), nodes.size(), nodes.size());
}

const Node Manager::createVar(const std::string& label) {
  return createVar(label, nodes.size(), True().id(), False().id());
}

const Node Manager::createVar(const std::string& label, const BDD_ID& top,
                              const BDD_ID& high, const BDD_ID& low) {
  nodes.push_back(std::make_shared<Node>(this, nodes.size(), label));
  auto node = nodes.back();
  node->top(top);
  node->high(high);
  node->low(low);
  unique_table[std::make_tuple(top, high, low)] = node->id();
  return *node;
}

const Node Manager::True() { return *nodes[1]; }

const Node Manager::False() { return *nodes[0]; }

const Node Manager::ite(const Node& i, const Node& t, const Node& e) {
  spdlog::trace("ite({}, {}, {})", i.id(), t.id(), e.id());

  // Terminal cases
  spdlog::trace("Checking terminal cases");
  if (i == True()) return t;
  if (i == False()) return e;
  if (t == e) return t;
  if (t == True() && e == False()) return i;

  // Check if ite has already been computed
  spdlog::trace("Checking if ite has already been computed");
  auto tuple_ite = std::make_tuple(i.id(), t.id(), e.id());
  if (computed_table.find(tuple_ite) != computed_table.end()) {
    pcache_hit++;
    return getNode(computed_table[tuple_ite]);
  }

  spdlog::trace("Computing ite");

  // Calculate top var
  auto top_vars = std::vector<BDD_ID>{i.top().id(), t.top().id(), e.top().id()};
  std::sort(top_vars.begin(), top_vars.end());
  top_vars.erase(
      std::remove_if(top_vars.begin(), top_vars.end(),
                     [this](BDD_ID var) { return getNode(var).isConstant(); }),
      top_vars.end());

  auto top = getNode(top_vars.front());
  auto high =
      ite(coFactorTrue(i, top), coFactorTrue(t, top), coFactorTrue(e, top));
  auto low =
      ite(coFactorFalse(i, top), coFactorFalse(t, top), coFactorFalse(e, top));

  // Reduce, if possible
  spdlog::trace("Reducing");
  if (high == low) return high;

  // Eliminate isomorphic sub-graphs
  spdlog::trace("Eliminating isomorphic sub-graphs");
  auto tuple_vgh = std::make_tuple(top.id(), high.id(), low.id());
  if (unique_table.find(tuple_vgh) != unique_table.end()) {
    ucache_hit++;
    computed_table[tuple_ite] = unique_table[tuple_vgh];
    return getNode(unique_table[tuple_vgh]);
  }

  // Create new node
  spdlog::trace("Creating new node");
  auto node = createVar(
      fmt::format("({} ? {} : {})", GET_NAME(top), GET_NAME(t), GET_NAME(e)),
      top.id(), high.id(), low.id());

  // Cache
  computed_table[tuple_ite] = node.id();

  return node;
}

const Node Manager::coFactorTrue(const Node& f, const Node& x) {
  if (f.isConstant() || x.isConstant() || f.top() > x) return f;

  if (f.top() == x) return f.high();

  auto T = coFactorTrue(f.high(), x);
  auto F = coFactorTrue(f.low(), x);

  return ite(f.top(), T, F);
}

const Node Manager::coFactorFalse(const Node& f, const Node& x) {
  if (f.isConstant() || x.isConstant() || f.top() > x) return f;

  if (f.top() == x) return f.low();

  auto T = coFactorFalse(f.high(), x);
  auto F = coFactorFalse(f.low(), x);

  return ite(f.top(), T, F);
}

const Node Manager::and2(const Node& a, const Node& b) {
  spdlog::trace(">>>>>>> and2({}, {})", GET_NAME(a), GET_NAME(b));
  auto node = nodes[ite(a, b, False()).id()];
  if (node->isConstant() || node->isVariable()) return *node;
  node->label(fmt::format("({} * {})", GET_NAME(a), GET_NAME(b)));
  return *node;
}

const Node Manager::or2(const Node& a, const Node& b) {
  spdlog::trace(">>>>>>> or2({}, {})", GET_NAME(a), GET_NAME(b));
  auto node = nodes[ite(a, True(), b).id()];
  if (node->isConstant() || node->isVariable()) return *node;
  node->label(fmt::format("({} + {})", GET_NAME(a), GET_NAME(b)));
  return *node;
}

const Node Manager::xor2(const Node& a, const Node& b) {
  spdlog::trace(">>>>>>> xor2({}, {})", GET_NAME(a), GET_NAME(b));
  auto node = nodes[ite(a, neg(b), b).id()];
  if (node->isConstant() || node->isVariable()) return *node;
  node->label(fmt::format("({} x {})", GET_NAME(a), GET_NAME(b)));
  return *node;
}

const Node Manager::neg(const Node& a) {
  spdlog::trace(">>>>>>> neg({})", GET_NAME(a));
  auto node = nodes[ite(a, False(), True()).id()];
  if (node->isConstant() || node->isVariable()) return *node;
  node->label(fmt::format("!({})", GET_NAME(a)));
  return *node;
}

const Node Manager::nand2(const Node& a, const Node& b) {
  spdlog::trace(">>>>>>> nand2({}, {})", GET_NAME(a), GET_NAME(b));
  auto node = nodes[neg(and2(a, b)).id()];
  if (node->isConstant() || node->isVariable()) return *node;
  node->label(fmt::format("!({} * {})", GET_NAME(a), GET_NAME(b)));
  return *node;
}

const Node Manager::nor2(const Node& a, const Node& b) {
  spdlog::trace(">>>>>>> nor2({}, {})", GET_NAME(a), GET_NAME(b));
  auto node = nodes[neg(or2(a, b)).id()];
  if (node->isConstant() || node->isVariable()) return *node;
  node->label(fmt::format("!({} + {})", GET_NAME(a), GET_NAME(b)));
  return *node;
}

const Node Manager::xnor2(const Node& a, const Node& b) {
  spdlog::trace(">>>>>>> xnor2({}, {})", GET_NAME(a), GET_NAME(b));
  auto node = nodes[neg(xor2(a, b)).id()];
  if (node->isConstant() || node->isVariable()) return *node;
  node->label(fmt::format("!({} x {})", GET_NAME(a), GET_NAME(b)));
  return *node;
}

void Manager::dump() {
  spdlog::info("Unique table size: {}", nodes.size());
  spdlog::info("Computed table size: {}", computed_table.size());

  for (auto node : nodes) {
    node->dump();
  }
}

void Manager::visualizeBDD_internal(std::ofstream& file, const Node& root) {
  file << fmt::format("n{} [label=\"{}\"]\n", root.id(), root.label());

  if (root.isConstant()) return;

  visualizeBDD_internal(file, root.low());
  file << fmt::format("n{} -> n{} [style=solid]\n", root.id(), root.low().id());

  visualizeBDD_internal(file, root.high());
  file << fmt::format("n{} -> n{} [style=solid]\n", root.id(),
                      root.high().id());
}

void Manager::visualizeBDD(std::string filepath, const Node& root,
                           bool test_result) {
  std::ofstream file(filepath);

  file << "strict digraph A {\n";
  file << "label=\"" << (test_result ? "PASSED" : "FAILED") << ": " << filepath
       << "\"\n";
  file << "graph [bgcolor=transparent]\n";
  file << "node [fillcolor=white, shape=box, fontname=Arial]\n";
  visualizeBDD_internal(file, root);
  file << "}\n";
}

void Manager::mermaidGraph_internal(std::ofstream& file, const Node& root,
                                    std::set<Node>& printed_nodes) {
  if (printed_nodes.find(root) != printed_nodes.end()) return;
  file << fmt::format("n{}[\"{}\"]\n", root.id(), root.label());
  printed_nodes.insert(root);

  if (root.isConstant()) return;

  mermaidGraph_internal(file, root.low(), printed_nodes);
  file << fmt::format("n{} -- 0 --> n{};\n", root.id(), root.low().id());

  mermaidGraph_internal(file, root.high(), printed_nodes);
  file << fmt::format("n{} -- 1 --> n{};\n", root.id(), root.high().id());
}

void Manager::mermaidGraph(std::string filepath, const Node& root) {
  std::ofstream file(filepath);
  std::set<Node> printed_nodes;

  file << "graph TD;\n";
  mermaidGraph_internal(file, root, printed_nodes);
}

const Node Manager::getNode(const BDD_ID& id) const { return *nodes[id]; }

void Manager::findNodes(const Node& root, std::set<Node>& nodes_of_root) {
  nodes_of_root.insert(root);

  if (root.isConstant()) return;

  findNodes(root.low(), nodes_of_root);
  findNodes(root.high(), nodes_of_root);
}

void Manager::findNodes(const BDD_ID& root, std::set<BDD_ID>& nodes_of_root) {
  std::set<Node> nodes;
  findNodes(getNode(root), nodes);
  for (auto node : nodes) {
    nodes_of_root.insert(node.id());
  }
}

void Manager::findVars(const Node& root, std::set<Node>& vars_of_root) {
  if (root.isConstant()) return;

  vars_of_root.insert(root.top());

  findVars(root.low(), vars_of_root);
  findVars(root.high(), vars_of_root);
}

void Manager::findVars(const BDD_ID& root, std::set<BDD_ID>& vars_of_root) {
  std::set<Node> vars;
  findVars(getNode(root), vars);
  for (auto var : vars) {
    vars_of_root.insert(var.id());
  }
}

std::vector<Node> Manager::findVars(const Node& root) {
  std::set<Node> vars_of_root;
  findVars(root, vars_of_root);
  return std::vector<Node>(vars_of_root.begin(), vars_of_root.end());
}

size_t Manager::uniqueTableSize() { return nodes.size(); }

}  // namespace ClassProject
