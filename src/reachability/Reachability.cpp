#include "Reachability.h"

#include <fmt/format.h>

namespace ClassProject {

    Reachability::Reachability(unsigned int stateSize, unsigned int inputSize=0) : ReachabilityInterface::ReachabilityInterface(stateSize, inputSize) {
        if (stateSize == 0) 
            throw std::runtime_error(">>> stateSize is zero! <<<");

        for (int i = 0; i < stateSize; i++ ) {
            states.push_back(createVar(fmt::format("s{}", i)));
            next_states.push_back(createVar(fmt::format("s{}'", i)));
            init_state.push_back(false);
        }

        for (int i = 0; i < inputSize; i++ ) {
            inputs.push_back(createVar(fmt::format("i{}", i)));
        }

        transitions = states;
    }

    const std::vector<BDD_ID> &Reachability::getStates() const {
        return states;
    }
    
    const std::vector<BDD_ID> &Reachability::getInputs() const {
        return inputs;
    }

    bool Reachability::isReachable(const std::vector<bool> &stateVector) {

    }

    int Reachability::stateDistance(const std::vector<bool> &stateVector) {
        
    }

    void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) {
        if (transitionFunctions.size() != states.size()) {
            throw std::runtime_error(">>> The number of given transition functions does not match the number of state bits! <<<");
        }

        transitions = transitionFunctions;

        // check if An unknown ID is provided
        // for (int i = 0; i < transitionFunctions.size(); i++) {
        //     if (transitionFunctions[i] is not in unique_table) {
        //         throw std::runtime_error(">>> An unknown ID is provided! <<<");
        //     }
        // }
    }

    void Reachability::setInitState(const std::vector<bool> &stateVector) {
        if (stateVector.size() != states.size()) {
            throw std::runtime_error(">>> StateVector size does not match with number of state bits! <<<");
        }
        init_state = stateVector;
    }

}