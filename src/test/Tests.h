//
// Created by tobias on 21.11.16.
//
#pragma once

#include <gtest/gtest.h>

#include "../Manager.h"

class ManagerTest : public ::testing::Test {
 public:
  ClassProject::Manager manager;

 protected:
  void SetUp() override {
    // Code to run before each test case
  }

  void TearDown() override {
    // Code to run after each test case
  }
};

TEST_F(ManagerTest, createVar) {
  EXPECT_EQ(manager.createVar("A"), 2);
  EXPECT_EQ(manager.createVar("B"), 3);
}

TEST_F(ManagerTest, True) { EXPECT_EQ(manager.True(), 1); }

TEST_F(ManagerTest, False) { EXPECT_EQ(manager.False(), 0); }

TEST_F(ManagerTest, isConstant) {
  EXPECT_TRUE(manager.isConstant(0));
  EXPECT_TRUE(manager.isConstant(1));
}

TEST_F(ManagerTest, isVariable) {
  EXPECT_EQ(manager.createVar("A"), 2);
  EXPECT_EQ(manager.createVar("B"), 3);

  EXPECT_TRUE(manager.isVariable(2));
  EXPECT_TRUE(manager.isVariable(3));
}

TEST_F(ManagerTest, topVar) {
  EXPECT_EQ(manager.topVar(manager.False()), manager.False());
  EXPECT_EQ(manager.topVar(manager.True()), manager.True());
}

TEST_F(ManagerTest, ite_true) {
  EXPECT_EQ(manager.ite(manager.True(), manager.True(), manager.False()),
            manager.True());
}

TEST_F(ManagerTest, ite_false) {
  EXPECT_EQ(manager.ite(manager.False(), manager.True(), manager.False()),
            manager.False());
}

TEST_F(ManagerTest, ite_same) {
  auto a = manager.createVar("A");
  EXPECT_EQ(manager.ite(a, manager.True(), manager.True()), manager.True());
}

TEST_F(ManagerTest, ite_true_false) {
  auto a = manager.createVar("A");
  EXPECT_EQ(manager.ite(a, manager.True(), manager.False()), a);
}

TEST_F(ManagerTest, ite_example) {
  auto a = manager.createVar("A");
  auto b = manager.createVar("B");
  auto c = manager.createVar("C");
  auto d = manager.createVar("D");

  auto f = manager.getNode(manager.and2(manager.or2(a, b), manager.and2(c, d)));

  manager.dump();

  EXPECT_EQ(f.id, 9);
  EXPECT_EQ(f.high, 7);
  EXPECT_EQ(f.low, 8);
  EXPECT_EQ(f.top, 2);

  auto node_8 = manager.getNode(8);
  EXPECT_EQ(node_8.high, 7);
  EXPECT_EQ(node_8.low, 0);
  EXPECT_EQ(node_8.top, 3);

  auto node_7 = manager.getNode(7);
  EXPECT_EQ(node_7.high, 5);
  EXPECT_EQ(node_7.low, 0);
  EXPECT_EQ(node_7.top, 4);

  auto node_6 = manager.getNode(6);
  EXPECT_EQ(node_6.high, 1);
  EXPECT_EQ(node_6.low, 3);
  EXPECT_EQ(node_6.top, 2);
}
