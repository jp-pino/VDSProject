#include "Manager.h"

namespace ClassProject {
    BDD_ID Manager::createVar(const std::string& label) {
        nodes.emplace_back(nodes.size(), label);
        return nodes.back().id;
    }
}
