//
// Created by ludwig on 22.11.16.
//

#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include "Tests.h"

int main(int argc, char* argv[]) {
  spdlog::cfg::load_env_levels();

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
