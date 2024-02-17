//
// Created by Carolina P. Nogueira 2016
//

#include <iostream>
#include <string>

#include "Manager.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  ClassProject::Manager manager;

  auto a = manager.createVar("A");
  auto b = manager.createVar("B");
  auto c = manager.createVar("C");
  auto d = manager.createVar("D");

  auto f = (a + b) * (c * d);

  manager.dump();

  manager.visualizeBDD("bdd.dot", f, true);

  return 0;
}
