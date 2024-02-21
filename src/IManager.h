//
// Created by ludwig on 01.03.16
//
#pragma once

#include <spdlog/spdlog.h>

#include <set>
#include <string>
#include <vector>

#include "Node.h"

namespace ClassProject {

class Node;

class IManager {
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

  virtual void findVars(const Node& root, std::set<Node>& vars_of_root) = 0;

  virtual size_t uniqueTableSize() = 0;

  virtual void visualizeBDD(std::string filepath, const Node& root,
                            bool test_result) = 0;

  virtual const Node getNode(const BDD_ID& id) const = 0;
};

}  // namespace ClassProject
