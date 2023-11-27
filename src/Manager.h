// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014
#pragma once

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
  std::vector<Node> nodes;

 public:
  /**
   * Constructor
   * Creates the constant nodes True and False
   */
  Manager();
  BDD_ID createVar(const std::string& label) override;

  const BDD_ID& True() override;
  const BDD_ID& False() override;

  bool isConstant(BDD_ID f) override;
  bool isVariable(BDD_ID x) override;

  BDD_ID topVar(BDD_ID f) override;

  BDD_ID ite(BDD_ID i, BDD_ID t, BDD_ID e) override;
  BDD_ID coFactorTrue(BDD_ID f, BDD_ID x) override;
  BDD_ID coFactorFalse(BDD_ID f, BDD_ID x) override;

  BDD_ID coFactorTrue(BDD_ID f) override;
  BDD_ID coFactorFalse(BDD_ID f) override;

  // Not implemented yet

  BDD_ID and2(BDD_ID a, BDD_ID b) override { return 0; }

  BDD_ID or2(BDD_ID a, BDD_ID b) override { return 0; }

  BDD_ID xor2(BDD_ID a, BDD_ID b) override { return 0; }

  BDD_ID neg(BDD_ID a) override { return 0; }

  BDD_ID nand2(BDD_ID a, BDD_ID b) override { return 0; }

  BDD_ID nor2(BDD_ID a, BDD_ID b) override { return 0; }

  BDD_ID xnor2(BDD_ID a, BDD_ID b) override { return 0; }

  std::string getTopVarName(const BDD_ID& root) override { return ""; }

  void findNodes(const BDD_ID& root, std::set<BDD_ID>& nodes_of_root) override {
  }

  void findVars(const BDD_ID& root, std::set<BDD_ID>& vars_of_root) override {}

  size_t uniqueTableSize() override { return 0; }

  void visualizeBDD(std::string filepath, BDD_ID& root) override {}
};
}  // namespace ClassProject
