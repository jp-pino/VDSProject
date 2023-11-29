#include "Manager.h"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace ClassProject {

Manager::Manager() {
  auto& false_n = unique_table[createVar("False")];
  false_n.high = false_n.id;
  false_n.low = false_n.id;
  false_n.top = false_n.id;

  auto& true_n = unique_table[createVar("True")];
  true_n.high = true_n.id;
  true_n.low = true_n.id;
  true_n.top = true_n.id;
}

BDD_ID Manager::createVar(const std::string& label) {
  unique_table.emplace_back(unique_table.size(), label);
  unique_table.back().top = unique_table.back().id;
  unique_table.back().high = True();
  unique_table.back().low = False();
  return unique_table.back().id;
}

const BDD_ID& Manager::True() { return unique_table[1].id; }

const BDD_ID& Manager::False() { return unique_table[0].id; }

bool Manager::isConstant(BDD_ID f) { return unique_table[f].isConstant(); }

bool Manager::isVariable(BDD_ID x) { return !isConstant(x); }

BDD_ID Manager::topVar(BDD_ID f) { return unique_table[f].top; }

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
  auto precomputed = computed_table.find(std::make_tuple(i, t, e));
  if (precomputed != computed_table.end()) return precomputed->second;

  spdlog::trace("Computing ite");

  // Calculate top var
  auto top_vars = std::vector<BDD_ID>{topVar(i), topVar(t), topVar(e)};
  std::sort(top_vars.begin(), top_vars.end());
  top_vars.erase(std::remove_if(top_vars.begin(), top_vars.end(),
                                [this](BDD_ID var) { return isConstant(var); }),
                 top_vars.end());

  auto& top = unique_table[top_vars.front()];

  auto high = ite(coFactorTrue(i, top.id), coFactorTrue(t, top.id),
                  coFactorTrue(e, top.id));
  auto low = ite(coFactorFalse(i, top.id), coFactorFalse(t, top.id),
                 coFactorFalse(e, top.id));

  // Reduce, if possible
  spdlog::trace("Reducing");
  if (high == low) return high;

  // Eliminate isomorphic sub-graphs
  spdlog::trace("Eliminating isomorphic sub-graphs");
  auto isomorphic =
      std::find_if(unique_table.begin(), unique_table.end(), [&](Node& node) {
        return node.top == top.id && node.high == high && node.low == low;
      });
  if (isomorphic != unique_table.end()) return isomorphic->id;

  // Create new node
  spdlog::trace("Creating new node");
  std::string label = "ite(" + top.label + "," + unique_table[t].label + "," +
                      unique_table[e].label + ")";
  auto& node = unique_table[createVar(label)];
  node.high = high;
  node.low = low;
  node.top = top.id;

  // Cache
  computed_table[std::make_tuple(i, t, e)] = node.id;

  return node.id;
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
  auto& f_node = unique_table[f];

  if (isConstant(f) || isConstant(x) || f_node.top > x) return f;

  if (f_node.top == x) return f_node.high;

  auto T = coFactorTrue(f_node.high, x);
  auto F = coFactorTrue(f_node.low, x);

  return ite(f_node.top, T, F);
}

BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
  auto& f_node = unique_table[f];

  if (isConstant(f) || isConstant(x) || f_node.top > x) return f;

  if (f_node.top == x) return f_node.low;

  auto T = coFactorFalse(f_node.high, x);
  auto F = coFactorFalse(f_node.low, x);

  return ite(f_node.top, T, F);
}

BDD_ID Manager::coFactorTrue(BDD_ID f) { return unique_table[f].high; }
BDD_ID Manager::coFactorFalse(BDD_ID f) { return unique_table[f].low; }

BDD_ID Manager::and2(BDD_ID a, BDD_ID b) {
  spdlog::trace(">>>>>>> and2({}, {})", a, b);
  auto& node = unique_table[ite(a, b, False())];
  node.label =
      fmt::format("({} * {})", unique_table[a].label, unique_table[b].label);
  return node.id;
}

BDD_ID Manager::or2(BDD_ID a, BDD_ID b) {
  spdlog::trace(">>>>>>> or2({}, {})", a, b);
  auto& node = unique_table[ite(a, True(), b)];
  node.label =
      fmt::format("({} + {})", unique_table[a].label, unique_table[b].label);
  return node.id;
}

BDD_ID Manager::xor2(BDD_ID a, BDD_ID b) {
  spdlog::trace(">>>>>>> xor2({}, {})", a, b);
  auto& node = unique_table[ite(a, neg(b), b)];
  node.label =
      fmt::format("({} x {})", unique_table[a].label, unique_table[b].label);
  return node.id;
}

BDD_ID Manager::neg(BDD_ID a) {
  spdlog::trace(">>>>>>> neg({})", a);
  auto& node = unique_table[ite(a, False(), True())];
  node.label = fmt::format("!({})", unique_table[a].label);
  return node.id;
}

BDD_ID Manager::nand2(BDD_ID a, BDD_ID b) {
  spdlog::trace(">>>>>>> nand2({}, {})", a, b);
  auto& node = unique_table[neg(and2(a, b))];
  node.label =
      fmt::format("!({} * {})", unique_table[a].label, unique_table[b].label);
  return node.id;
}

BDD_ID Manager::nor2(BDD_ID a, BDD_ID b) {
  spdlog::trace(">>>>>>> nor2({}, {})", a, b);
  auto& node = unique_table[neg(or2(a, b))];
  node.label =
      fmt::format("!({} + {})", unique_table[a].label, unique_table[b].label);
  return node.id;
}

BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b) {
  spdlog::trace(">>>>>>> xnor2({}, {})", a, b);
  auto& node = unique_table[neg(xor2(a, b))];
  node.label =
      fmt::format("!({} x {})", unique_table[a].label, unique_table[b].label);
  return node.id;
}

void Manager::dump() {
  spdlog::info("Unique table size: {}", unique_table.size());
  spdlog::info("Computed table size: {}", computed_table.size());

  for (auto& node : unique_table) {
    spdlog::info(
        "Node: {}"
        "\n  Label: {}"
        "\n  Top: {}"
        "\n  High: {}"
        "\n  Low: {}",
        node.id, node.label, node.top, node.high, node.low);
  }
}

void Manager::visualizeBDD_internal(std::ofstream& file, BDD_ID& root) {
  auto& node = unique_table[root];

  file << fmt::format("n{} [label=\"{}\"]\n", node.id, node.label);

  if (isConstant(root)) return;

  visualizeBDD_internal(file, node.low);
  file << fmt::format("n{} -> n{} [style=dotted]\n", node.id, node.low);

  visualizeBDD_internal(file, node.high);
  file << fmt::format("n{} -> n{} [style=dotted]\n", node.id, node.high);
}

void Manager::visualizeBDD(std::string filepath, BDD_ID& root) {
  std::ofstream file(filepath);

  file << "strict digraph A {\n";
  visualizeBDD_internal(file, root);
  file << "}\n";
}

void Manager::mermaidGraph_internal(std::ofstream& file, BDD_ID& root,
                                    std::set<BDD_ID>& printed_nodes) {
  auto& node = unique_table[root];

  if (printed_nodes.find(node.id) != printed_nodes.end()) return;
  file << fmt::format("n{}[\"{}\"]\n", node.id, node.label);
  printed_nodes.insert(node.id);

  if (isConstant(root)) return;

  mermaidGraph_internal(file, node.low, printed_nodes);
  file << fmt::format("n{} --> n{};\n", node.id, node.low);

  mermaidGraph_internal(file, node.high, printed_nodes);
  file << fmt::format("n{} --> n{};\n", node.id, node.high);
}

void Manager::mermaidGraph(std::string filepath, BDD_ID& root) {
  std::ofstream file(filepath);
  std::set<BDD_ID> printed_nodes;

  file << "graph TD;\n";
  mermaidGraph_internal(file, root, printed_nodes);
}

const Node Manager::getNode(const BDD_ID& id) const { return unique_table[id]; }

std::string Manager::getTopVarName(const BDD_ID& root) {
  auto& node = unique_table[root];
  return unique_table[node.top].label;
}

void Manager::findNodes(const BDD_ID& root, std::set<BDD_ID>& nodes_of_root) {
  auto& node = unique_table[root];

  nodes_of_root.insert(root);

  if (isConstant(root)) return;

  findNodes(node.low, nodes_of_root);
  findNodes(node.high, nodes_of_root);
}

void Manager::findVars(const BDD_ID& root, std::set<BDD_ID>& vars_of_root) {
  auto& node = unique_table[root];

  if (isConstant(root)) return;

  vars_of_root.insert(node.top);

  findVars(node.low, vars_of_root);
  findVars(node.high, vars_of_root);
}

size_t Manager::uniqueTableSize() { return unique_table.size(); }

}  // namespace ClassProject
