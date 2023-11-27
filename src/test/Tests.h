//
// Created by tobias on 21.11.16.
//

#ifndef VDSPROJECT_TESTS_H
#define VDSPROJECT_TESTS_H

#include <gtest/gtest.h>
#include "../Manager.h"

TEST(ManagerTest, createVar) {
    Manager manager;

    EXPECT_EQ(manager.createVar("False"), 0);
    EXPECT_EQ(manager.createVar("True"), 1);
}

#endif
