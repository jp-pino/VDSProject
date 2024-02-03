#ifndef VDSPROJECT_REACHABILITY_TESTS_H
#define VDSPROJECT_REACHABILITY_TESTS_H

#include <gtest/gtest.h>

#include "Reachability.h"

using namespace ClassProject;

struct ReachabilityTest : testing::Test {
  std::unique_ptr<ClassProject::ReachabilityInterface> fsm =
      std::make_unique<ClassProject::Reachability>(2, 2);

  std::vector<BDD_ID> inputVars = fsm->getInputs();
  std::vector<BDD_ID> stateVars = fsm->getStates();
  std::vector<BDD_ID> transitionFunctions;

  void TearDown() override {
    if (HasFailure()) {
      fsm->dump();
    }
  }
};

TEST_F(ReachabilityTest, HowTo_Example) {
  auto s0 = stateVars.at(0);
  auto s1 = stateVars.at(1);

  transitionFunctions.push_back(fsm->neg(s0));  // s0' = not(s0)
  transitionFunctions.push_back(fsm->neg(s1));  // s1' = not(s1)
  fsm->setTransitionFunctions(transitionFunctions);

  fsm->setInitState({false, false});

  ASSERT_TRUE(fsm->isReachable({false, false}));
  ASSERT_FALSE(fsm->isReachable({false, true}));
  ASSERT_FALSE(fsm->isReachable({true, false}));
  ASSERT_TRUE(fsm->isReachable({true, true}));
}

TEST_F(ReachabilityTest, InputsTest) {
  auto s0 = stateVars.at(0);
  auto s1 = stateVars.at(1);
  auto i0 = inputVars.at(0);
  auto i1 = inputVars.at(1);

  // s0' = not(s0) & i0
  transitionFunctions.push_back(fsm->and2(fsm->neg(s0), i0));
  // s1' = not(s1) & i0 & i1
  transitionFunctions.push_back(fsm->and2(fsm->and2(fsm->neg(s1), i0), i1));

  fsm->setTransitionFunctions(transitionFunctions);
  fsm->setInitState({false, false});

  ASSERT_TRUE(fsm->isReachable({false, false}));
  ASSERT_FALSE(fsm->isReachable({false, true}));
  ASSERT_TRUE(fsm->isReachable({true, false}));
  ASSERT_TRUE(fsm->isReachable({true, true}));
}

TEST_F(ReachabilityTest, StateDistanceTest) {
  auto s0 = stateVars.at(0);
  auto s1 = stateVars.at(1);

  // FSM describes a counter
  transitionFunctions.push_back(fsm->neg(s0));
  transitionFunctions.push_back(fsm->ite(s0, fsm->neg(s1), s1));

  fsm->setTransitionFunctions(transitionFunctions);
  fsm->setInitState({false, false});

  ASSERT_EQ(fsm->stateDistance({false, false}), 1);
  ASSERT_EQ(fsm->stateDistance({true, false}), 2);
  ASSERT_EQ(fsm->stateDistance({false, true}), 3);
  ASSERT_EQ(fsm->stateDistance({true, true}), 4);
}

TEST_F(ReachabilityTest, ExceptionsTest) {
  auto s0 = stateVars.at(0);
  auto s1 = stateVars.at(1);

  // FSM describes a counter
  transitionFunctions.push_back(fsm->neg(s0));
  transitionFunctions.push_back(fsm->neg(s0));
  transitionFunctions.push_back(fsm->ite(s0, fsm->neg(s1), s1));

  EXPECT_THROW(fsm->setTransitionFunctions(transitionFunctions),
               std::runtime_error);
  EXPECT_THROW(fsm->setInitState({false, false, true}), std::runtime_error);
  EXPECT_THROW(fsm->isReachable({true, true, true}), std::runtime_error);
  EXPECT_THROW(fsm->stateDistance({true, true, true}), std::runtime_error);
}

#endif
