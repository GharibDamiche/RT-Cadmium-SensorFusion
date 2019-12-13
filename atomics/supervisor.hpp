/**
* Gharib Damiche
*for ARSLab Carleton
*
* Supervisor:
* Supervise fault events from models atomics
*/

#ifndef BOOST_SIMULATION_PDEVS_SUPERVISOR_HPP
#define BOOST_SIMULATION_PDEVS_SUPERVISOR_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>
#include <math.h>
#include <cstddef>
#include <cstdlib>
#include <string>
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

#include "../knowledge_base/knowledge_base.cpp"
//      KnowledgeBase kb();
//      kb.init();
//      if( kb.faultCodeIsPresent('E16') )
//      { then ... }
//      kb.addFaultCode('E75')


using namespace cadmium;
using namespace std;

//Port definition
struct supervisor_defs {
    //Output ports
    struct dataOut : public out_port<float> { };
    //Input ports
    struct A1 : public in_port<bool> { };
    struct B1 : public in_port<bool> { };
    struct C1 : public in_port<bool> { };
    struct D1 : public in_port<bool> { };
};

template<typename TIME>
class Supervisor {
    using defs=supervisor_defs; // putting definitions in context
public:
    KnowledgeBase* kb;

    //Parameters to be overwriten when instantiating the atomic model
    TIME   toggleTime;
    // default constructor
    Supervisor() noexcept{

        kb = new KnowledgeBase;
        kb->init();

        toggleTime  = TIME("00:00:00:040");

        state.idle  = true;
        state.A1    = false;
        state.B1    = false;
        state.C1    = false;
        state.D1    = false;


    }

    // state definition
    struct state_type{
        bool idle;
        bool A1;
        bool B1;
        bool C1;
        bool D1;
    };
    state_type state;

    // ports definition
    using input_ports=std::tuple<typename defs::A1, typename defs::B1, typename defs::C1, typename defs::D1>;
    using output_ports=std::tuple<typename defs::dataOut>;

    // internal transition
    void internal_transition() {
        state.idle = true;
        state.A1 = 0;
        state.B1 = 0;
        state.C1 = 0;
        state.D1 = 0;
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {

        for(const auto &x : get_messages<typename defs::A1>(mbs)){
            state.idle = false;
            state.A1 = x;

            return;
        }

        for(const auto &x : get_messages<typename defs::B1>(mbs)){
            state.idle = false;
            state.B1 = x;

            return;
        }

        for(const auto &x : get_messages<typename defs::C1>(mbs)){
            state.idle = false;
            state.C1 = x;

            return;
        }

        for(const auto &x : get_messages<typename defs::D1>(mbs)){
            state.idle = false;
            state.D1 = x;

            return;
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

        std::string key;
        std::string fault_key;

        //Resolve the key function of priority
        if (state.A1) {
            key = "A1";
            if (kb->fault_code_is_present(key))
            {
                std::cout << "Fault " << key << " is present at frequency: " << kb->get_fault_code(key)->second << std::endl;
                int freq = kb->get_fault_code(key)->second;
                kb->update_fault_code(key, freq+1);
            } else {
                std::cout << "New fault " << key << " is present" << std::endl;
                kb->add_fault_code(key, 1);
            }

            fault_key = key;
            get_messages<typename defs::dataOut>(bags).push_back(1);

        }  if (state.B1) {
            key = "B1";
            if (kb->fault_code_is_present(key))
            {
                std::cout << "Fault " << key << " is present at frequency: " << kb->get_fault_code(key)->second << std::endl;
                int freq = kb->get_fault_code(key)->second;
                kb->update_fault_code(key, freq+1);
            } else {
                std::cout << "New fault " << key << " is present" << std::endl;
                kb->add_fault_code(key, 1);
            }

            fault_key = fault_key + key;
            get_messages<typename defs::dataOut>(bags).push_back(2);

        }  if (state.C1) {
            key = "C1";
            if (kb->fault_code_is_present(key))
            {
                std::cout << "Fault " << key << " is present at frequency: " << kb->get_fault_code(key)->second << std::endl;
                int freq = kb->get_fault_code(key)->second;
                kb->update_fault_code(key, freq+1);
            } else {
                std::cout << "New fault " << key << " is present" << std::endl;
                kb->add_fault_code(key, 1);
            }
            if (fault_key.size() < 4) {
                fault_key = fault_key + key;
                get_messages<typename defs::dataOut>(bags).push_back(3);
            }
        }  if (state.D1) {
            key = "D1";
            if (kb->fault_code_is_present(key))
            {
                std::cout << "Fault " << key << " is present at frequency: " << kb->get_fault_code(key)->second << std::endl;
                int freq = kb->get_fault_code(key)->second;
                kb->update_fault_code(key, freq+1);
            } else {
                std::cout << "New fault " << key << " is present" << std::endl;
                kb->add_fault_code(key, 1);
            }
            if (fault_key.size() < 4) {
                fault_key = fault_key + key;
                get_messages<typename defs::dataOut>(bags).push_back(4);
            }
        }

        // Check and do with the key
        if (kb->fault_code_is_present(fault_key))
        {
            std::cout << "Fault " << fault_key << " is present at frequency: " << kb->get_fault_code(fault_key)->second << std::endl;
            // send trigger to A1 for fault accomodation
            int freq = kb->get_fault_code(fault_key)->second;
            kb->update_fault_code(fault_key, freq+1);
        } else {
            std::cout << "New fault " << fault_key << " is present" << std::endl;
            kb->add_fault_code(fault_key, 1);
        }
        kb->save_faults_code();
        return bags;
    }

    // time_advance function
    TIME time_advance() const {
        if(state.idle){
            return std::numeric_limits<TIME>::infinity();
        } else {
            return toggleTime;
        }
    }

    friend std::ostringstream& operator<<(std::ostringstream& os, const typename Supervisor<TIME>::state_type& i) {
        os << "Output: " << (i.A1 ? 1 : 0);
        return os;
    }

};


#endif // BOOST_SIMULATION_PDEVS_SUPERVISOR_HPP