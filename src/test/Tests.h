//
// Created by tobias on 21.11.16.
//
#pragma once

#include <gtest/gtest.h>
#include "../Manager.h"

TEST(ManagerTest, createVar) {
    Manager manager;

    EXPECT_EQ(manager.createVar("False"), 0);
    EXPECT_EQ(manager.createVar("True"), 1);
}
