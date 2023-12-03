//
// Created by Carolina P. Nogueira 2016
//

#include <iostream>
#include <string>

#include "Manager.h"

int main(int argc, char* argv[]) {
  ClassProject::Manager manager;

  auto a = manager.createVar("A");
  auto b = manager.createVar("B");
  auto c = manager.createVar("C");
  auto d = manager.createVar("D");

  auto f = manager.getNode(manager.and2(manager.or2(a, b), manager.and2(c, d)));

  manager.dump();

  manager.visualizeBDD("bdd.dot", f.id, true);

  return 0;
}
