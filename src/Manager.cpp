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

    bool Manager::isConstant(BDD_ID f) {
        return nodes[f].high == NULL && nodes[f].low == NULL && nodes[f].top_var == NULL;
        // return nodes[f].id == nodes[f].high && nodes[f].id == nodes[f].low && nodes[f].id == nodes[f].top_var;
    }

    bool Manager::isVariable(BDD_ID x) {
        return !isConstant(x);
    }
}
