//
// Created by ludwig on 01.03.16
//
#pragma once

#include <set>
#include <string>

namespace ClassProject {

typedef size_t BDD_ID;

class ManagerInterface {
 public:
  virtual BDD_ID createVar(const std::string& label) = 0;

  virtual const BDD_ID& True() = 0;

  virtual const BDD_ID& False() = 0;

  virtual bool isConstant(BDD_ID f) = 0;

  virtual bool isVariable(BDD_ID x) = 0;

  virtual BDD_ID topVar(BDD_ID f) = 0;

  virtual BDD_ID ite(BDD_ID i, BDD_ID t, BDD_ID e) = 0;

  virtual BDD_ID coFactorTrue(BDD_ID f, BDD_ID x) = 0;

  virtual BDD_ID coFactorFalse(BDD_ID f, BDD_ID x) = 0;

  virtual BDD_ID coFactorTrue(BDD_ID f) = 0;

  virtual BDD_ID coFactorFalse(BDD_ID f) = 0;

  virtual BDD_ID and2(BDD_ID a, BDD_ID b) = 0;

  virtual BDD_ID or2(BDD_ID a, BDD_ID b) = 0;

  virtual BDD_ID xor2(BDD_ID a, BDD_ID b) = 0;

  virtual BDD_ID neg(BDD_ID a) = 0;

  virtual BDD_ID nand2(BDD_ID a, BDD_ID b) = 0;

  virtual BDD_ID nor2(BDD_ID a, BDD_ID b) = 0;

  virtual BDD_ID xnor2(BDD_ID a, BDD_ID b) = 0;

  virtual std::string getTopVarName(const BDD_ID& root) = 0;

  virtual void findNodes(const BDD_ID& root,
                         std::set<BDD_ID>& nodes_of_root) = 0;

  virtual void findVars(const BDD_ID& root, std::set<BDD_ID>& vars_of_root) = 0;

  virtual size_t uniqueTableSize() = 0;
  virtual size_t ucache_hits() = 0;
  virtual size_t pcache_hits() = 0;

  virtual void visualizeBDD(std::string filepath, BDD_ID& root,
                            bool test_result) = 0;
};
}  // namespace ClassProject
