/** \file */
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
    auto root = manager.getNode(manager.uniqueTableSize() - 1).id;
    auto name = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    if (HasFailure()) {
      manager.dump();
      manager.mermaidGraph(fmt::format("graphs/{}.mmd.err", name), root);
    } else {
      manager.mermaidGraph(fmt::format("graphs/{}.mmd", name), root);
    }
    manager.visualizeBDD(fmt::format("graphs/{}.dot", name), root,
                         !HasFailure());
  }
};

/**
 * @fn TEST_F(ManagerTest, createVar)
 * @brief Test the creation of variables
 * \dotfile createVar.dot
 */
TEST_F(ManagerTest, createVar) {
  EXPECT_EQ(manager.createVar("A"), 2);
  EXPECT_EQ(manager.createVar("B"), 3);
}

/**
 * @fn TEST_F(ManagerTest, True)
 * @brief Test True constant
 * \dotfile True.dot
 */
TEST_F(ManagerTest, True) { EXPECT_EQ(manager.True(), 1); }

/**
 * @fn TEST_F(ManagerTest, False)
 * @brief Test False constant
 * \dotfile False.dot
 */
TEST_F(ManagerTest, False) { EXPECT_EQ(manager.False(), 0); }

/**
 * @fn TEST_F(ManagerTest, isConstant)
 * @brief Test that True and False are constants
 * \dotfile isConstant.dot
 */
TEST_F(ManagerTest, isConstant) {
  EXPECT_TRUE(manager.isConstant(0));
  EXPECT_TRUE(manager.isConstant(1));
}

/**
 * @fn TEST_F(ManagerTest, isVariable)
 * @brief Test that created variables are variables
 * \dotfile isVariable.dot
 */
TEST_F(ManagerTest, isVariable) {
  EXPECT_EQ(manager.createVar("A"), 2);
  EXPECT_EQ(manager.createVar("B"), 3);

  EXPECT_TRUE(manager.isVariable(2));
  EXPECT_TRUE(manager.isVariable(3));
}

/**
 * @fn TEST_F(ManagerTest, topVar)
 * @brief Test the topVar function
 * \dotfile topVar.dot
 */
TEST_F(ManagerTest, topVar) {
  EXPECT_EQ(manager.topVar(manager.False()), manager.False());
  EXPECT_EQ(manager.topVar(manager.True()), manager.True());
}

/**
 * @fn TEST_F(ManagerTest, ite_true)
 * @brief Test the ite function with true condition
 */
TEST_F(ManagerTest, ite_true) {
  EXPECT_EQ(manager.ite(manager.True(), manager.True(), manager.False()),
            manager.True());
}

/**
 * @fn TEST_F(ManagerTest, ite_false)
 * @brief Test the ite function with false condition
 */
TEST_F(ManagerTest, ite_false) {
  EXPECT_EQ(manager.ite(manager.False(), manager.True(), manager.False()),
            manager.False());
}

/**
 * @fn TEST_F(ManagerTest, ite_same)
 * @brief Test the ite function with same high and low values
 */
TEST_F(ManagerTest, ite_same) {
  auto a = manager.createVar("A");
  EXPECT_EQ(manager.ite(a, manager.True(), manager.True()), manager.True());
}

/**
 * @fn TEST_F(ManagerTest, ite_true_false)
 * @brief Test the ite function with true and false values
 */
TEST_F(ManagerTest, ite_true_false) {
  auto a = manager.createVar("A");
  EXPECT_EQ(manager.ite(a, manager.True(), manager.False()), a);
}

/**
 * @fn TEST_F(ManagerTest, ite_example)
 * @brief Test the ite function with an example
 * \dotfile ite_example.dot
 */
TEST_F(ManagerTest, ite_example) {
  auto a = manager.getNode(manager.createVar("A"));
  auto b = manager.getNode(manager.createVar("B"));
  auto c = manager.getNode(manager.createVar("C"));
  auto d = manager.getNode(manager.createVar("D"));

  auto a_or_b = manager.getNode(manager.or2(a.id, b.id));
  auto c_and_d = manager.getNode(manager.and2(c.id, d.id));
  auto f = manager.getNode(manager.and2(a_or_b.id, c_and_d.id));

  EXPECT_EQ(f.id, 9);
  EXPECT_EQ(f.high, c_and_d.id);
  EXPECT_EQ(f.low, 8);
  EXPECT_EQ(f.top, a.id);

  auto node_8 = manager.getNode(8);
  EXPECT_EQ(node_8.high, 7);
  EXPECT_EQ(node_8.low, manager.False());
  EXPECT_EQ(node_8.top, b.id);

  EXPECT_EQ(c_and_d.high, d.id);
  EXPECT_EQ(c_and_d.low, manager.False());
  EXPECT_EQ(c_and_d.top, c.id);

  EXPECT_EQ(a_or_b.high, manager.True());
  EXPECT_EQ(a_or_b.low, b.id);
  EXPECT_EQ(a_or_b.top, a.id);
}

/**
 * @fn TEST_F(ManagerTest, and2)
 * @brief Test the and2 function
 * \dotfile and2.dot
 */
TEST_F(ManagerTest, and2) {
  auto a = manager.getNode(manager.createVar("A"));
  auto b = manager.getNode(manager.createVar("B"));

  auto f = manager.getNode(manager.and2(a.id, b.id));

  EXPECT_EQ(f.id, 4);
  EXPECT_EQ(f.high, b.id);
  EXPECT_EQ(f.low, manager.False());
  EXPECT_EQ(f.top, a.id);
}

/**
 * @fn TEST_F(ManagerTest, or2)
 * @brief Test the or2 function
 * \dotfile or2.dot
 */
TEST_F(ManagerTest, or2) {
  auto a = manager.getNode(manager.createVar("A"));
  auto b = manager.getNode(manager.createVar("B"));

  auto f = manager.getNode(manager.or2(a.id, b.id));

  EXPECT_EQ(f.id, 4);
  EXPECT_EQ(f.high, manager.True());
  EXPECT_EQ(f.low, b.id);
  EXPECT_EQ(f.top, a.id);
}

/**
 * @fn TEST_F(ManagerTest, xor2)
 * @brief Test the xor2 function
 * \dotfile xor2.dot
 */
TEST_F(ManagerTest, xor2) {
  auto a = manager.getNode(manager.createVar("A"));
  auto b = manager.getNode(manager.createVar("B"));

  auto f = manager.getNode(manager.xor2(a.id, b.id));
  auto not_b = manager.getNode(4);

  EXPECT_EQ(f.id, 5);
  EXPECT_EQ(f.high, not_b.id);
  EXPECT_EQ(f.low, b.id);
  EXPECT_EQ(f.top, a.id);
}

/**
 * @fn TEST_F(ManagerTest, neg)
 * @brief Test the neg function
 * \dotfile neg.dot
 */
TEST_F(ManagerTest, neg) {
  auto a = manager.getNode(manager.createVar("A"));

  auto f = manager.getNode(manager.neg(a.id));

  EXPECT_EQ(f.id, 3);
  EXPECT_EQ(f.high, manager.False());
  EXPECT_EQ(f.low, manager.True());
  EXPECT_EQ(f.top, a.id);
}

/**
 * @fn TEST_F(ManagerTest, nand2)
 * @brief Test the nand2 function
 * \dotfile nand2.dot
 */
TEST_F(ManagerTest, nand2) {
  auto a = manager.getNode(manager.createVar("A"));
  auto b = manager.getNode(manager.createVar("B"));

  auto f = manager.getNode(manager.nand2(a.id, b.id));
  auto a_and_b = manager.getNode(4);
  auto not_b = manager.getNode(5);

  EXPECT_EQ(f.id, 6);
  EXPECT_EQ(f.high, not_b.id);
  EXPECT_EQ(f.low, manager.True());
  EXPECT_EQ(f.top, a.id);

  EXPECT_EQ(a_and_b.id, 4);
  EXPECT_EQ(a_and_b.high, b.id);
  EXPECT_EQ(a_and_b.low, manager.False());
  EXPECT_EQ(a_and_b.top, a.id);
}

/**
 * @fn TEST_F(ManagerTest, nor2)
 * @brief Test the nor2 function
 * \dotfile nor2.dot
 */
TEST_F(ManagerTest, nor2) {
  auto a = manager.getNode(manager.createVar("A"));
  auto b = manager.getNode(manager.createVar("B"));

  auto f = manager.getNode(manager.nor2(a.id, b.id));
  auto a_or_b = manager.getNode(4);
  auto not_b = manager.getNode(5);

  EXPECT_EQ(f.id, 6);
  EXPECT_EQ(f.high, manager.False());
  EXPECT_EQ(f.low, not_b.id);
  EXPECT_EQ(f.top, a.id);

  EXPECT_EQ(a_or_b.id, 4);
  EXPECT_EQ(a_or_b.high, manager.True());
  EXPECT_EQ(a_or_b.low, b.id);
  EXPECT_EQ(a_or_b.top, a.id);
}
