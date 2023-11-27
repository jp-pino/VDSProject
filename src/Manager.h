// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014
#pragma once

#include <string>
#include <vector>

#include "ManagerInterface.h"

namespace ClassProject {
    struct Node {
        BDD_ID id;
        BDD_ID high;
        BDD_ID low;
        BDD_ID top_var;
        std::string label;

        Node(BDD_ID id, std::string label) : id(id), label(label) {};
    };

    class Manager : public ManagerInterface {
        private:
            std::vector<Node> nodes;

        public:
            BDD_ID createVar(const std::string& label) override;

    };
}
