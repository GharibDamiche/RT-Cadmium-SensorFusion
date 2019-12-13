/**
* Gharib Damiche
*for ARSLab Carleton
 *
* FaultGeneratorD:
* Simple model to generate randomly 2 states variable error on Output
*/

#ifndef BOOST_SIMULATION_PDEVS_FAULT_GENERATOR_D_HPP
#define BOOST_SIMULATION_PDEVS_FAULT_GENERATOR_D_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>
#include <math.h>
#include <assert.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>

using namespace cadmium;
using namespace std;

//Port definition
struct faultGeneratorD_defs {
    struct dataOut : public out_port<bool> { };
    struct in : public in_port<bool> { };
};

template<typename TIME>
class FaultGeneratorD {
    using defs=faultGeneratorD_defs; // putting definitions in context
public:
    //Parameters to be overwriten when instantiating the atomic model
    TIME   toggleTime;
    // default constructor
    FaultGeneratorD() noexcept{
        toggleTime  = TIME("00:00:00:270");
        state.emitOn = false;
    }

    // state definition
    struct state_type{
        bool emitOn;
    };
    state_type state;

    // ports definition
    using input_ports=std::tuple<typename defs::in>;
    using output_ports=std::tuple<typename defs::dataOut>;

    // internal transition
    void internal_transition() {
        state.emitOn=!state.emitOn;
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        for(const auto &x : get_messages<typename defs::in>(mbs)){
            // if(x.value == 0)
            //   state.fastToggle = !state.fastToggle;
            //state.fastToggle = (x == 0);
        }
    }
    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), std::move(mbs));
    }

    // output function
    typename make_message_bags<output_ports>::type output() const {
        typename make_message_bags<output_ports>::type bags;
        bool out = 1;
        get_messages<typename defs::dataOut>(bags).push_back(out);

        return bags;
    }

    // time_advance function
    TIME time_advance() const {
        return toggleTime;
    }

    friend std::ostringstream& operator<<(std::ostringstream& os, const typename FaultGeneratorD<TIME>::state_type& i) {
        os << "D1: " << (i.emitOn ? 1 : 0);
        return os;
    }
};


#endif // BOOST_SIMULATION_PDEVS_FAULT_GENERATOR_D_HPP