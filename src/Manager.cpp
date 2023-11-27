#include "Manager.h"

namespace ClassProject {
    BDD_ID Manager::createVar(const std::string& label) {
        nodes.emplace_back(nodes.size(), label);
        return nodes.back().id;
    }

    const BDD_ID& Manager::True() { 
        return nodes[1].id; 
    }
    
    const BDD_ID& Manager::False() { 
        return nodes[0].id; 
    }
}
