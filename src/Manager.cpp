#include "Manager.h"

#include <fmt/format.h>

#include <algorithm>
#include <fstream>
#include <iostream>

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#define GET_NAME(var) ((var)->label)
#else
#define GET_NAME(var) ((var)->id)
#endif

namespace ClassProject {

Manager::Manager() {
  nodes.push_back(std::make_shared<Node>(nodes.size(), "False"));
  auto node = nodes.back();
  node->top = node->id;
  node->high = node->id;
  node->low = node->id;
  unique_table[std::make_tuple(node->top, node->high, node->low)] = 0;

  nodes.push_back(std::make_shared<Node>(nodes.size(), "True"));
  node = nodes.back();
  node->top = node->id;
  node->high = node->id;
  node->low = node->id;
  unique_table[std::make_tuple(node->top, node->high, node->low)] = 1;
}

BDD_ID Manager::createVar(const std::string& label) {
  return createVar(label, nodes.size(), True(), False());
}

BDD_ID Manager::createVar(const std::string& label, const BDD_ID& top,
                          const BDD_ID& high, const BDD_ID& low) {
  nodes.push_back(std::make_shared<Node>(nodes.size(), label));
  auto node = nodes.back();
  node->top = top;
  node->high = high;
  node->low = low;
  unique_table[std::make_tuple(top, high, low)] = node->id;
  return node->id;
}

const BDD_ID& Manager::True() { return nodes[1]->id; }

const BDD_ID& Manager::False() { return nodes[0]->id; }

bool Manager::isConstant(BDD_ID f) { return nodes[f]->isConstant(); }

bool Manager::isVariable(BDD_ID x) { return nodes[x]->isVariable(); }

BDD_ID Manager::topVar(BDD_ID f) { return nodes[f]->top; }

BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
  spdlog::trace("ite({}, {}, {})", i, t, e);

  // Terminal cases
  spdlog::trace("Checking terminal cases");
  if (i == True()) return t;
  if (i == False()) return e;
  if (t == e) return t;
  if (t == True() && e == False()) return i;

  // Check if ite has already been computed
  spdlog::trace("Checking if ite has already been computed");
  auto tuple_ite = std::make_tuple(i, t, e);
  if (computed_table.find(tuple_ite) != computed_table.end()) {
    pcache_hit++;
    return computed_table[tuple_ite];
  }

  spdlog::trace("Computing ite");

  // Calculate top var
  auto top_vars = std::vector<BDD_ID>{topVar(i), topVar(t), topVar(e)};
  std::sort(top_vars.begin(), top_vars.end());
  top_vars.erase(std::remove_if(top_vars.begin(), top_vars.end(),
                                [this](BDD_ID var) { return isConstant(var); }),
                 top_vars.end());

  auto top = nodes[top_vars.front()];
  auto high = ite(coFactorTrue(i, top->id), coFactorTrue(t, top->id),
                  coFactorTrue(e, top->id));
  auto low = ite(coFactorFalse(i, top->id), coFactorFalse(t, top->id),
                 coFactorFalse(e, top->id));

  // Reduce, if possible
  spdlog::trace("Reducing");
  if (high == low) return high;

  // Eliminate isomorphic sub-graphs
  spdlog::trace("Eliminating isomorphic sub-graphs");
  auto tuple_vgh = std::make_tuple(top->id, high, low);
  if (unique_table.find(tuple_vgh) != unique_table.end()) {
    ucache_hit++;
    computed_table[tuple_ite] = unique_table[tuple_vgh];
    return unique_table[tuple_vgh];
  }

  // Create new node
  spdlog::trace("Creating new node");
  auto id = createVar(fmt::format("({} ? {} : {})", GET_NAME(top),
                                  GET_NAME(getNode(t)), GET_NAME(getNode(e))),
                      top->id, high, low);

  // Cache
  computed_table[tuple_ite] = id;

  return id;
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
  auto f_node = nodes[f];

  if (isConstant(f) || isConstant(x) || f_node->top > x) return f;

  if (f_node->top == x) return f_node->high;

  auto T = coFactorTrue(f_node->high, x);
  auto F = coFactorTrue(f_node->low, x);

  return ite(f_node->top, T, F);
}

BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
  auto f_node = nodes[f];

  if (isConstant(f) || isConstant(x) || f_node->top > x) return f;

  if (f_node->top == x) return f_node->low;

  auto T = coFactorFalse(f_node->high, x);
  auto F = coFactorFalse(f_node->low, x);

  return ite(f_node->top, T, F);
}

BDD_ID Manager::coFactorTrue(BDD_ID f) { return nodes[f]->high; }
BDD_ID Manager::coFactorFalse(BDD_ID f) { return nodes[f]->low; }

BDD_ID Manager::and2(BDD_ID a, BDD_ID b) {
  spdlog::trace(">>>>>>> and2({}, {})", GET_NAME(nodes[a]), GET_NAME(nodes[b]));
  auto node = nodes[ite(a, b, False())];
  if (node->isConstant() || node->isVariable()) return node->id;
  node->label =
      fmt::format("({} * {})", GET_NAME(nodes[a]), GET_NAME(nodes[b]));
  return node->id;
}

BDD_ID Manager::or2(BDD_ID a, BDD_ID b) {
  spdlog::trace(">>>>>>> or2({}, {})", GET_NAME(nodes[a]), GET_NAME(nodes[b]));
  auto node = nodes[ite(a, True(), b)];
  if (node->isConstant() || node->isVariable()) return node->id;
  node->label =
      fmt::format("({} + {})", GET_NAME(nodes[a]), GET_NAME(nodes[b]));
  return node->id;
}

BDD_ID Manager::xor2(BDD_ID a, BDD_ID b) {
  spdlog::trace(">>>>>>> xor2({}, {})", GET_NAME(nodes[a]), GET_NAME(nodes[b]));
  auto node = nodes[ite(a, neg(b), b)];
  if (node->isConstant() || node->isVariable()) return node->id;
  node->label =
      fmt::format("({} x {})", GET_NAME(nodes[a]), GET_NAME(nodes[b]));
  return node->id;
}

BDD_ID Manager::neg(BDD_ID a) {
  spdlog::trace(">>>>>>> neg({})", GET_NAME(nodes[a]));
  auto node = nodes[ite(a, False(), True())];
  if (node->isConstant() || node->isVariable()) return node->id;
  node->label = fmt::format("!({})", GET_NAME(nodes[a]));
  return node->id;
}

BDD_ID Manager::nand2(BDD_ID a, BDD_ID b) {
  spdlog::trace(">>>>>>> nand2({}, {})", GET_NAME(nodes[a]),
                GET_NAME(nodes[b]));
  auto node = nodes[neg(and2(a, b))];
  if (node->isConstant() || node->isVariable()) return node->id;
  node->label =
      fmt::format("!({} * {})", GET_NAME(nodes[a]), GET_NAME(nodes[b]));
  return node->id;
}

BDD_ID Manager::nor2(BDD_ID a, BDD_ID b) {
  spdlog::trace(">>>>>>> nor2({}, {})", GET_NAME(nodes[a]), GET_NAME(nodes[b]));
  auto node = nodes[neg(or2(a, b))];
  if (node->isConstant() || node->isVariable()) return node->id;
  node->label =
      fmt::format("!({} + {})", GET_NAME(nodes[a]), GET_NAME(nodes[b]));
  return node->id;
}

BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b) {
  spdlog::trace(">>>>>>> xnor2({}, {})", GET_NAME(nodes[a]),
                GET_NAME(nodes[b]));
  auto node = nodes[neg(xor2(a, b))];
  if (node->isConstant() || node->isVariable()) return node->id;
  node->label =
      fmt::format("!({} x {})", GET_NAME(nodes[a]), GET_NAME(nodes[b]));
  return node->id;
}

void Manager::dump() {
  spdlog::info("Unique table size: {}", nodes.size());
  spdlog::info("Computed table size: {}", computed_table.size());

  for (auto node : nodes) {
    node->dump();
  }
}

void Manager::visualizeBDD_internal(std::ofstream& file, BDD_ID& root) {
  auto node = nodes[root];

  file << fmt::format("n{} [label=\"{}\"]\n", node->id, node->label);

  if (isConstant(root)) return;

  visualizeBDD_internal(file, node->low);
  file << fmt::format("n{} -> n{} [style=solid]\n", node->id, node->low);

  visualizeBDD_internal(file, node->high);
  file << fmt::format("n{} -> n{} [style=solid]\n", node->id, node->high);
}

void Manager::visualizeBDD(std::string filepath, BDD_ID& root,
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

void Manager::mermaidGraph_internal(std::ofstream& file, BDD_ID& root,
                                    std::set<BDD_ID>& printed_nodes) {
  auto node = nodes[root];

  if (printed_nodes.find(node->id) != printed_nodes.end()) return;
  file << fmt::format("n{}[\"{}\"]\n", node->id, node->label);
  printed_nodes.insert(node->id);

  if (isConstant(root)) return;

  mermaidGraph_internal(file, node->low, printed_nodes);
  file << fmt::format("n{} -- 0 --> n{};\n", node->id, node->low);

  mermaidGraph_internal(file, node->high, printed_nodes);
  file << fmt::format("n{} -- 1 --> n{};\n", node->id, node->high);
}

void Manager::mermaidGraph(std::string filepath, BDD_ID& root) {
  std::ofstream file(filepath);
  std::set<BDD_ID> printed_nodes;

  file << "graph TD;\n";
  mermaidGraph_internal(file, root, printed_nodes);
}

const std::shared_ptr<Node> Manager::getNode(const BDD_ID& id) const {
  return nodes[id];
}

std::string Manager::getTopVarName(const BDD_ID& root) {
  auto node = nodes[root];
  return nodes[node->top]->label;
}

void Manager::findNodes(const BDD_ID& root, std::set<BDD_ID>& nodes_of_root) {
  auto node = nodes[root];

  nodes_of_root.insert(root);

  if (isConstant(root)) return;

  findNodes(node->low, nodes_of_root);
  findNodes(node->high, nodes_of_root);
}

void Manager::findVars(const BDD_ID& root, std::set<BDD_ID>& vars_of_root) {
  auto node = nodes[root];

  if (isConstant(root)) return;

  vars_of_root.insert(node->top);

  findVars(node->low, vars_of_root);
  findVars(node->high, vars_of_root);
}

std::vector<BDD_ID> Manager::findVars(const BDD_ID& root) {
  std::set<BDD_ID> vars_of_root;
  findVars(root, vars_of_root);
  return std::vector<BDD_ID>(vars_of_root.begin(), vars_of_root.end());
}

size_t Manager::uniqueTableSize() { return nodes.size(); }

}  // namespace ClassProject
