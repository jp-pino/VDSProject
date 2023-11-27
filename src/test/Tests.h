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
