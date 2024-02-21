#pragma once

#include <gtest/gtest.h>

#include "Reachability.h"

using namespace ClassProject;

struct ReachabilityTest : testing::Test {
  std::unique_ptr<ClassProject::IReachability> fsm =
      std::make_unique<ClassProject::Reachability>(2, 2);

  std::vector<Node> inputVars = fsm->getInputs();
  std::vector<Node> stateVars = fsm->getStates();
  std::vector<Node> transitionFunctions;

  void TearDown() override {
    if (HasFailure()) {
      fsm->dump();
    }
  }
};

TEST_F(ReachabilityTest, HowTo_Example) {
  auto s0 = stateVars.at(0);
  auto s1 = stateVars.at(1);

  transitionFunctions.push_back(!s0);  // s0' = not(s0)
  transitionFunctions.push_back(!s1);  // s1' = not(s1)
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

  // Counter up to 3: 00, 01, 10 and reset to 00
  transitionFunctions.push_back(i0 * fsm->ite(s1, fsm->False(), (!s0)));
  transitionFunctions.push_back(i0 * s0 * !s1);

  fsm->setTransitionFunctions(transitionFunctions);
  fsm->setInitState({false, false});

  ASSERT_TRUE(fsm->isReachable({false, false}));
  ASSERT_TRUE(fsm->isReachable({false, true}));
  ASSERT_TRUE(fsm->isReachable({true, false}));
  ASSERT_FALSE(fsm->isReachable({true, true}));
}

TEST_F(ReachabilityTest, StateDistanceTest) {
  auto s0 = stateVars.at(0);
  auto s1 = stateVars.at(1);

  // FSM describes a counter
  transitionFunctions.push_back(!s0);
  transitionFunctions.push_back(s0 ^ s1);

  fsm->setTransitionFunctions(transitionFunctions);
  fsm->setInitState({false, false});

  ASSERT_EQ(fsm->stateDistance({false, false}), 0);
  ASSERT_EQ(fsm->stateDistance({true, false}), 1);
  ASSERT_EQ(fsm->stateDistance({false, true}), 2);
  ASSERT_EQ(fsm->stateDistance({true, true}), 3);
}

TEST_F(ReachabilityTest, StateDistanceTestWithInput) {
  auto s0 = stateVars.at(0);
  auto s1 = stateVars.at(1);
  auto i0 = inputVars.at(0);

  // Counter up to 3: 00, 01, 10 and reset to 00
  transitionFunctions.push_back(i0 * fsm->ite(s1, fsm->False(), (!s0)));
  transitionFunctions.push_back(i0 * s0 * !s1);

  fsm->setTransitionFunctions(transitionFunctions);
  fsm->setInitState({false, false});

  ASSERT_EQ(fsm->stateDistance({false, false}), 0);
  ASSERT_EQ(fsm->stateDistance({true, false}), 1);
  ASSERT_EQ(fsm->stateDistance({false, true}), 2);
  ASSERT_EQ(fsm->stateDistance({true, true}), -1);
}

TEST_F(ReachabilityTest, ExceptionsTest) {
  auto s0 = stateVars.at(0);
  auto s1 = stateVars.at(1);

  transitionFunctions.push_back(!s0);
  transitionFunctions.push_back(!s0);
  transitionFunctions.push_back(s0 ^ s1);

  EXPECT_THROW(fsm->setTransitionFunctions(transitionFunctions),
               std::runtime_error);
  EXPECT_THROW(fsm->setInitState({false, false, true}), std::runtime_error);
  EXPECT_THROW(fsm->isReachable({true, true, true}), std::runtime_error);
  EXPECT_THROW(fsm->stateDistance({true, true, true}), std::runtime_error);
}

struct ReachabilityTest3States : testing::Test {
  std::unique_ptr<ClassProject::IReachability> fsm =
      std::make_unique<ClassProject::Reachability>(3);

  std::vector<Node> inputVars = fsm->getInputs();
  std::vector<Node> stateVars = fsm->getStates();
  std::vector<Node> transitionFunctions;

  void TearDown() override {
    if (HasFailure()) {
      fsm->dump();
    }
  }
};

TEST_F(ReachabilityTest3States, StateDistance) {
  auto s0 = stateVars.at(0);
  auto s1 = stateVars.at(1);
  auto s2 = stateVars.at(2);

  transitionFunctions.push_back(!s0);  // s0'
  // s1 = s0 ? !s1 : s1
  transitionFunctions.push_back(s0 ^ s1);  // s1'
  // s2 = s1 & s0 ? !s2 : s2
  transitionFunctions.push_back((s1 * s0) ^ s2);  // s2'
  fsm->setTransitionFunctions(transitionFunctions);

  fsm->setInitState({false, false, false});

  ASSERT_EQ(fsm->stateDistance({false, false, false}), 0);
  ASSERT_EQ(fsm->stateDistance({true, false, false}), 1);
  ASSERT_EQ(fsm->stateDistance({false, true, false}), 2);
  ASSERT_EQ(fsm->stateDistance({true, true, false}), 3);
  ASSERT_EQ(fsm->stateDistance({false, false, true}), 4);
  ASSERT_EQ(fsm->stateDistance({true, false, true}), 5);
  ASSERT_EQ(fsm->stateDistance({false, true, true}), 6);
  ASSERT_EQ(fsm->stateDistance({true, true, true}), 7);
}

TEST(Group06_Test, DefaultStates) {
  std::unique_ptr<ClassProject::IReachability> fsm1 =
      std::make_unique<ClassProject::Reachability>(1);
  ASSERT_TRUE(fsm1->isReachable({false}));
  ASSERT_FALSE(fsm1->isReachable({true}));
}

TEST(Group06_Test, threeStateTwoInputDistanceExample) { /* NOLINT */

  std::unique_ptr<ClassProject::Reachability> threestateDistance =
      std::make_unique<ClassProject::Reachability>(3, 2);
  std::vector<Node> stateVars7 = threestateDistance->getStates();
  std::vector<Node> transitionFunctions;

  auto s0 = stateVars7.at(0);
  auto s1 = stateVars7.at(1);
  auto s2 = stateVars7.at(2);
  auto inputs = threestateDistance->getInputs();
  auto i0 = inputs.at(0);
  auto i1 = inputs.at(1);

  /**     s2  s1  s0
   *  A:  0   0   0
   *  B:  0   0   1
   *  C:  0   1   0
   *  D:  0   1   1
   *  E:  1   0   0
   *  F:  1   0   1
   *  G:  1   1   0
   *  H:  1   1   1
   */
  auto A = !s0 * !s1 * !s2;
  auto B = s0 * !s1 * !s2;
  auto C = !s0 * s1 * !s2;
  auto D = s0 * s1 * !s2;
  auto E = !s0 * !s1 * s2;
  auto F = s0 * !s1 * s2;
  auto G = !s0 * s1 * s2;
  auto H = s0 * s1 * s2;

  /**         i1  i0
   * inp0     0   0
   * inp1     0   1
   * inp2     1   0
   * inp3     1   1
   *
   */
  auto inp0 = !i0 * !i1;
  auto inp1 = i0 * !i1;
  auto inp2 = !i0 * i1;
  auto inp3 = i0 * i1;

  // s0' = not(s1)*not(s0)*not(i) + !s1*s0*!i + s1*!s0*!i
  auto AtoB = A * inp3;
  auto AtoE = A * inp2;
  auto AtoH = A * inp1;
  auto BtoB = B * inp0;
  auto BtoG = B * inp3;
  auto BtoD = B * inp2;
  auto BtoC = B * inp1;

  auto s0trans = AtoB + AtoH + BtoB + BtoD + E + D + G + C + H;
  auto s1trans = BtoG + AtoH + F + BtoD + BtoC + D + C + G + H;
  auto s2trans = AtoE + AtoH + BtoG + E + F + G + H;

  transitionFunctions.push_back(s0trans);
  transitionFunctions.push_back(s1trans);
  transitionFunctions.push_back(s2trans);
  threestateDistance->setTransitionFunctions(transitionFunctions);

  /**     s2  s1  s0
   *  A:  0   0   0
   *  B:  0   0   1
   *  C:  0   1   0
   *  D:  0   1   1
   *  E:  1   0   0
   *  F:  1   0   1
   *  G:  1   1   0
   *  H:  1   1   1
   */

  ASSERT_TRUE(threestateDistance->isReachable({false, false, false}));  //! A
  ASSERT_TRUE(threestateDistance->isReachable({true, false, false}));   //! B
  ASSERT_TRUE(threestateDistance->isReachable({false, true, false}));   //! C
  ASSERT_TRUE(threestateDistance->isReachable({true, true, false}));    //! D
  ASSERT_TRUE(threestateDistance->isReachable({false, false, true}));   //! E
  ASSERT_TRUE(threestateDistance->isReachable({true, false, true}));    //! F
  ASSERT_TRUE(threestateDistance->isReachable({false, true, true}));    //! G
  ASSERT_TRUE(threestateDistance->isReachable({true, true, true}));     //! H

  ASSERT_EQ(threestateDistance->stateDistance({false, false, false}), 0);  //! A
  ASSERT_EQ(threestateDistance->stateDistance({true, false, false}), 1);   //! B
  ASSERT_EQ(threestateDistance->stateDistance({false, true, false}), 2);   //! C
  ASSERT_EQ(threestateDistance->stateDistance({true, true, false}), 2);    //! D
  ASSERT_EQ(threestateDistance->stateDistance({false, false, true}), 1);   //! E
  ASSERT_EQ(threestateDistance->stateDistance({true, false, true}), 2);    //! F
  ASSERT_EQ(threestateDistance->stateDistance({false, true, true}), 2);    //! G
  ASSERT_EQ(threestateDistance->stateDistance({true, true, true}), 1);     //! H

  threestateDistance->setInitState({true, false, false});                //! B
  ASSERT_FALSE(threestateDistance->isReachable({false, false, false}));  //! A
  ASSERT_TRUE(threestateDistance->isReachable({true, false, false}));    //! B
  ASSERT_TRUE(threestateDistance->isReachable({false, true, false}));    //! C
  ASSERT_TRUE(threestateDistance->isReachable({true, true, false}));     //! D
  ASSERT_FALSE(threestateDistance->isReachable({false, false, true}));   //! E
  ASSERT_FALSE(threestateDistance->isReachable({true, false, true}));    //! F
  ASSERT_TRUE(threestateDistance->isReachable({false, true, true}));     //! G
  ASSERT_TRUE(threestateDistance->isReachable({true, true, true}));      //! H

  ASSERT_EQ(threestateDistance->stateDistance({false, false, false}),
            -1);                                                           //! A
  ASSERT_EQ(threestateDistance->stateDistance({true, false, false}), 0);   //! B
  ASSERT_EQ(threestateDistance->stateDistance({false, true, false}), 1);   //! C
  ASSERT_EQ(threestateDistance->stateDistance({true, true, false}), 1);    //! D
  ASSERT_EQ(threestateDistance->stateDistance({false, false, true}), -1);  //! E
  ASSERT_EQ(threestateDistance->stateDistance({true, false, true}), -1);   //! F
  ASSERT_EQ(threestateDistance->stateDistance({false, true, true}), 1);    //! G
  ASSERT_EQ(threestateDistance->stateDistance({true, true, true}), 2);     //! H

  threestateDistance->setInitState({false, false, true});                //! E
  ASSERT_FALSE(threestateDistance->isReachable({false, false, false}));  //! A
  ASSERT_FALSE(threestateDistance->isReachable({true, false, false}));   //! B
  ASSERT_FALSE(threestateDistance->isReachable({false, true, false}));   //! C
  ASSERT_FALSE(threestateDistance->isReachable({true, true, false}));    //! D
  ASSERT_TRUE(threestateDistance->isReachable({false, false, true}));    //! E
  ASSERT_TRUE(threestateDistance->isReachable({true, false, true}));     //! F
  ASSERT_TRUE(threestateDistance->isReachable({false, true, true}));     //! G
  ASSERT_TRUE(threestateDistance->isReachable({true, true, true}));      //! H

  ASSERT_EQ(threestateDistance->stateDistance({false, false, false}),
            -1);                                                           //! A
  ASSERT_EQ(threestateDistance->stateDistance({true, false, false}), -1);  //! B
  ASSERT_EQ(threestateDistance->stateDistance({false, true, false}), -1);  //! C
  ASSERT_EQ(threestateDistance->stateDistance({true, true, false}), -1);   //! D
  ASSERT_EQ(threestateDistance->stateDistance({false, false, true}), 0);   //! E
  ASSERT_EQ(threestateDistance->stateDistance({true, false, true}), 1);    //! F
  ASSERT_EQ(threestateDistance->stateDistance({false, true, true}), 2);    //! G
  ASSERT_EQ(threestateDistance->stateDistance({true, true, true}), 3);     //! H
}
