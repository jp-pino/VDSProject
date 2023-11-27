//
// Created by tobias on 21.11.16.
//
#pragma once

#include <gtest/gtest.h>
#include "../Manager.h"

TEST(ManagerTest, createVar) {
  ClassProject::Manager manager;

  EXPECT_EQ(manager.createVar("A"), 2);
  EXPECT_EQ(manager.createVar("B"), 3);
}

TEST(ManagerTest, True) {
  ClassProject::Manager manager;

  EXPECT_EQ(manager.True(), 1);
}

TEST(ManagerTest, False) {
  ClassProject::Manager manager;

  EXPECT_EQ(manager.False(), 0);
}

TEST(ManagerTest, isConstant) {
  ClassProject::Manager manager;

  EXPECT_TRUE(manager.isConstant(0));
  EXPECT_TRUE(manager.isConstant(1));
}

TEST(ManagerTest, isVariable) {
  ClassProject::Manager manager;

  EXPECT_EQ(manager.createVar("A"), 2);
  EXPECT_EQ(manager.createVar("B"), 3);

  EXPECT_TRUE(manager.isVariable(2));
  EXPECT_TRUE(manager.isVariable(3));
}

TEST(ManagerTest, topVar) {
  ClassProject::Manager manager;

  EXPECT_EQ(manager.topVar(manager.False()), manager.False());
  EXPECT_EQ(manager.topVar(manager.True()), manager.True());
}
