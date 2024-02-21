/** \file */
//
// Created by tobias on 21.11.16.
//
#pragma once

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "../Manager.h"

class ManagerTest : public ::testing::Test {
 public:
  ClassProject::Manager manager;

 protected:
  void SetUp() override {
    spdlog::trace(
        "Starting test: {}",
        ::testing::UnitTest::GetInstance()->current_test_info()->name());
  }

  void TearDown() override {
    const auto root = manager.getNode(manager.uniqueTableSize() - 1);
    const auto name =
        ::testing::UnitTest::GetInstance()->current_test_info()->name();
    if (HasFailure()) {
      spdlog::error("Unique table size: {}", manager.uniqueTableSize());
      manager.dump();
    }
    manager.visualizeBDD(fmt::format("graphs/{}.mmd", name), root, false);
  }
};

/**
 * @brief Test the creation of variables
 * \dotfile createVar.dot
 */
TEST_F(ManagerTest, createVar) {
  EXPECT_EQ(manager.createVar("A"), 2);
  EXPECT_EQ(manager.createVar("B"), 3);
}

/**
 * @brief Test True constant
 * \dotfile True.dot
 */
TEST_F(ManagerTest, True) { EXPECT_EQ(manager.True(), 1); }

/**
 * @brief Test False constant
 * \dotfile False.dot
 */
TEST_F(ManagerTest, False) { EXPECT_EQ(manager.False(), 0); }

TEST_F(ManagerTest, OrOperatorsEquivalence) {
  const auto a = manager.createVar("A");
  const auto b = manager.createVar("B");
  const auto f = manager.or2(a, b);

  EXPECT_EQ(f, a + b);
  EXPECT_EQ(f, a | b);

  auto c = manager.False();
  c += a;
  c += b;
  EXPECT_EQ(f, c);

  c = manager.False();
  c |= a;
  c |= b;
  EXPECT_EQ(f, c);
}

TEST_F(ManagerTest, AndOperatorsEquivalence) {
  const auto a = manager.createVar("A");
  const auto b = manager.createVar("B");
  const auto f = manager.and2(a, b);

  EXPECT_EQ(f, a * b);
  EXPECT_EQ(f, a & b);

  auto c = manager.True();
  c *= a;
  c *= b;
  EXPECT_EQ(f, c);

  c = manager.True();
  c &= a;
  c &= b;
  EXPECT_EQ(f, c);
}

/**
 * @brief Test that True and False are constants
 * \dotfile isConstant.dot
 */
TEST_F(ManagerTest, isConstant) {
  EXPECT_TRUE(manager.False().isConstant());
  EXPECT_TRUE(manager.True().isConstant());
  EXPECT_FALSE(manager.createVar("A").isConstant());
}

/**
 * @brief Test that created variables are variables
 * \dotfile isVariable.dot
 */
TEST_F(ManagerTest, isVariable) {
  const auto a = manager.createVar("A");
  const auto b = manager.createVar("B");

  EXPECT_TRUE(a.isVariable());
  EXPECT_TRUE(b.isVariable());
  EXPECT_FALSE(manager.True().isVariable());
  EXPECT_FALSE((a + b).isVariable());
}

/**
 * @brief Test the topVar function
 * \dotfile topVar.dot
 */
TEST_F(ManagerTest, topVar) {
  const auto a = manager.createVar("A");
  EXPECT_EQ(manager.False().top(), manager.False());
  EXPECT_EQ(manager.True().top(), manager.True());
  EXPECT_EQ(a.top(), a);
}

/**
 * @brief Test the ite function with true condition
 * \dotfile ite_true.dot
 */
TEST_F(ManagerTest, ite_true) {
  EXPECT_EQ(manager.ite(manager.True(), manager.True(), manager.False()),
            manager.True());
}

/**
 * @brief Test the ite function with false condition
 * \dotfile ite_false.dot
 */
TEST_F(ManagerTest, ite_false) {
  EXPECT_EQ(manager.ite(manager.False(), manager.True(), manager.False()),
            manager.False());
}

/**
 * @brief Test the ite function with same high and low values
 * \dotfile ite_same.dot
 */
TEST_F(ManagerTest, ite_same) {
  const auto a = manager.createVar("A");
  EXPECT_EQ(manager.ite(a, manager.True(), manager.True()), manager.True());
}

/**
 * @brief Test the ite function with true and false values
 * \dotfile ite_true_false.dot
 */
TEST_F(ManagerTest, ite_true_false) {
  const auto a = manager.createVar("A");
  EXPECT_EQ(manager.ite(a, manager.True(), manager.False()), a);
}

/**
 * @brief Test the ite function with an example
 * \dotfile ite_example.dot
 */
TEST_F(ManagerTest, ite_example) {
  const auto a = manager.createVar("A");
  const auto b = manager.createVar("B");
  const auto c = manager.createVar("C");
  const auto d = manager.createVar("D");

  const auto a_or_b = a + b;
  const auto c_and_d = c * d;
  const auto f = a_or_b * c_and_d;

  EXPECT_EQ(f, 9);
  EXPECT_EQ(f.high(), c_and_d);
  EXPECT_EQ(f.low(), 8);
  EXPECT_EQ(f.top(), a);

  const auto node_8 = manager.getNode(8);
  EXPECT_EQ(node_8.high(), 7);
  EXPECT_EQ(node_8.low(), manager.False());
  EXPECT_EQ(node_8.top(), b);

  EXPECT_EQ(c_and_d.high(), d);
  EXPECT_EQ(c_and_d.low(), manager.False());
  EXPECT_EQ(c_and_d.top(), c);

  EXPECT_EQ(a_or_b.high(), manager.True());
  EXPECT_EQ(a_or_b.low(), b);
  EXPECT_EQ(a_or_b.top(), a);
}

/**
 * @brief Test the and2 function
 * \dotfile and2.dot
 */
TEST_F(ManagerTest, and2) {
  const auto a = manager.createVar("A");
  const auto b = manager.createVar("B");

  const auto f = a * b;

  EXPECT_EQ(f, 4);
  EXPECT_EQ(f.high(), b);
  EXPECT_EQ(f.low(), manager.False());
  EXPECT_EQ(f.top(), a);

  EXPECT_EQ(f.restrict({a}, {true}), b);
  EXPECT_EQ(f.restrict({a}, {false}), manager.False());
}

/**
 * @brief Test the or2 function
 * \dotfile or2.dot
 */
TEST_F(ManagerTest, or2) {
  const auto a = manager.createVar("A");
  const auto b = manager.createVar("B");

  const auto f = a + b;

  EXPECT_EQ(f, 4);
  EXPECT_EQ(f.high(), manager.True());
  EXPECT_EQ(f.low(), b);
  EXPECT_EQ(f.top(), a);

  EXPECT_EQ(f.restrict({a}, {true}), manager.True());
  EXPECT_EQ(f.restrict({a}, {false}), b);
}

/**
 * @brief Test the xor2 function
 * \dotfile xor2.dot
 */
TEST_F(ManagerTest, xor2) {
  const auto a = manager.createVar("A");
  const auto b = manager.createVar("B");

  const auto f = a ^ b;

  EXPECT_EQ(f, 5);
  EXPECT_EQ(f.high(), (!b));
  EXPECT_EQ(f.low(), b);
  EXPECT_EQ(f.top(), a);

  EXPECT_EQ(f.restrict({a}, {true}), !b);
  EXPECT_EQ(f.restrict({a}, {false}), b);
}

/**
 * @brief Test the neg function
 * \dotfile neg.dot
 */
TEST_F(ManagerTest, neg) {
  const auto a = manager.createVar("A");

  const auto f = !a;

  EXPECT_EQ(f, 3);
  EXPECT_EQ(f.high(), manager.False());
  EXPECT_EQ(f.low(), manager.True());
  EXPECT_EQ(f.top(), a);

  EXPECT_EQ(f.restrict({a}, {true}), manager.False());
}

/**
 * @brief Test the nand2 function
 * \dotfile nand2.dot
 */
TEST_F(ManagerTest, nand2) {
  const auto a = manager.createVar("A");
  const auto b = manager.createVar("B");

  const auto f = a.nand(b);

  EXPECT_EQ(f, 5);
  EXPECT_EQ(f.high(), !b);
  EXPECT_EQ(f.low(), manager.True());
  EXPECT_EQ(f.top(), a);

  EXPECT_EQ(f.restrict({a}, {true}), !b);
  EXPECT_EQ(f.restrict({a}, {false}), manager.True());
}

/**
 * @brief Test the nor2 function
 * \dotfile nor2.dot
 */
TEST_F(ManagerTest, nor2) {
  const auto a = manager.createVar("A");
  const auto b = manager.createVar("B");

  const auto f = a.nor(b);

  EXPECT_EQ(f, 5);
  EXPECT_EQ(f.high(), manager.False());
  EXPECT_EQ(f.low(), !b);
  EXPECT_EQ(f.top(), a);

  EXPECT_EQ(f.restrict({a}, {true}), manager.False());
  EXPECT_EQ(f.restrict({a}, {false}), !b);
}

/**
 * @brief Test the xnor2 function
 * \dotfile xnor2.dot
 */
TEST_F(ManagerTest, xnor2) {
  const auto a = manager.createVar("A");
  const auto b = manager.createVar("B");

  const auto f = a.xnor(b);

  EXPECT_EQ(f, 5);
  EXPECT_EQ(f.high(), b);
  EXPECT_EQ(f.low(), !b);
  EXPECT_EQ(f.top(), a);

  EXPECT_EQ(f.restrict({a}, {true}), b);
  EXPECT_EQ(f.restrict({a}, {false}), !b);
}
