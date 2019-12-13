/**
* By: Ben Earle and Kyle Bjornson
* ARSLab - Carleton University
*
* Analog Input:
* Model to interface with a analog Input pin for Embedded Cadmium.
*/
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

#include <cadmium/modeling/coupling.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/modeling/dynamic_atomic.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/tuple_to_ostream.hpp>
#include <cadmium/logger/common_loggers.hpp>

#include <NDTime.hpp>
#include <cadmium/io/iestream.hpp>


#include <cadmium/real_time/arm_mbed/io/digitalInput.hpp>
#include <cadmium/real_time/arm_mbed/io/digitalOutput.hpp>
#include <cadmium/real_time/arm_mbed/io/analogOutput.hpp>

#include "../atomics/supervisor.hpp"

#include "../atomics/fault_generator_A.hpp"
#include "../atomics/fault_generator_B.hpp"
#include "../atomics/fault_generator_C.hpp"
#include "../atomics/fault_generator_D.hpp"

#include "../bloom-filter/bloom_filter.hpp"


#ifdef RT_ARM_MBED
#include "../mbed.h"

#else
// When simulating the model it will use these files as IO in place of the pins specified.
const char* BUTTON1 = "./inputs/FAKE_INPUT.txt";
const char* LOGS    = "./outputs/LOGS_Out.txt";
#endif

using namespace std;

using hclock=chrono::high_resolution_clock;
using TIME = NDTime;


int main(int argc, char ** argv) {

#ifdef RT_ARM_MBED
    //Logging is done over cout in RT_ARM_MBED
      struct oss_sink_provider{
        static std::ostream& sink(){
          return cout;
        }
      };
#else
    // all simulation timing and I/O streams are ommited when running embedded
    auto start = hclock::now(); //to measure simulation execution time

    static std::ofstream out_data("SensorFusion.txt");
    struct oss_sink_provider{
        static std::ostream& sink(){
            return out_data;
        }
    };
#endif

    /*************** Loggers *******************/
    using info=cadmium::logger::logger<cadmium::logger::logger_info, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using debug=cadmium::logger::logger<cadmium::logger::logger_debug, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using state=cadmium::logger::logger<cadmium::logger::logger_state, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using log_messages=cadmium::logger::logger<cadmium::logger::logger_messages, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using routing=cadmium::logger::logger<cadmium::logger::logger_message_routing, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using global_time=cadmium::logger::logger<cadmium::logger::logger_global_time, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using local_time=cadmium::logger::logger<cadmium::logger::logger_local_time, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using log_all=cadmium::logger::multilogger<info, debug, state, log_messages, routing, global_time, local_time>;

    using logger_top=cadmium::logger::multilogger<log_messages, global_time>;


    /*******************************************/
    /********************************************/
    /*********** APPLICATION GENERATOR **********/
    /********************************************/
    using AtomicModelPtr=std::shared_ptr<cadmium::dynamic::modeling::model>;
    using CoupledModelPtr=std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>>;


    /********************************************/
    /********** Initiating Atomics *******************/
    /********************************************/

    AtomicModelPtr supervisor = cadmium::dynamic::translate::make_dynamic_atomic_model<Supervisor, TIME>("supervisor");

    AtomicModelPtr faultGenA = cadmium::dynamic::translate::make_dynamic_atomic_model<FaultGeneratorA, TIME>("faultGenA");
    AtomicModelPtr faultGenB = cadmium::dynamic::translate::make_dynamic_atomic_model<FaultGeneratorB, TIME>("faultGenB");
    AtomicModelPtr faultGenC = cadmium::dynamic::translate::make_dynamic_atomic_model<FaultGeneratorC, TIME>("faultGenC");
    AtomicModelPtr faultGenD = cadmium::dynamic::translate::make_dynamic_atomic_model<FaultGeneratorD, TIME>("faultGenD");

    /********************************************/
    /********** DigitalInput1 *******************/
    /********************************************/

    //AtomicModelPtr digitalInput1 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("digitalInput1", BUTTON1);

    /********************************************/
    /********* LogOutput *******************/
    /********************************************/

    AtomicModelPtr logOutput = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogOutput, TIME>("logOutput", LOGS);


    /************************/
    /*******TOP MODEL********/
    /************************/

    cadmium::dynamic::modeling::Ports iports_TOP = {};

    cadmium::dynamic::modeling::Ports oports_TOP = {};

    cadmium::dynamic::modeling::Models submodels_TOP =  {supervisor, faultGenA, faultGenB, faultGenC, faultGenD, logOutput};

    cadmium::dynamic::modeling::EICs eics_TOP = {};

    cadmium::dynamic::modeling::EOCs eocs_TOP = {};

    cadmium::dynamic::modeling::ICs ics_TOP = {

            cadmium::dynamic::translate::make_IC<faultGeneratorA_defs::dataOut, supervisor_defs::A1>("faultGenA","supervisor"),
            cadmium::dynamic::translate::make_IC<faultGeneratorB_defs::dataOut, supervisor_defs::B1>("faultGenB","supervisor"),
            cadmium::dynamic::translate::make_IC<faultGeneratorC_defs::dataOut, supervisor_defs::C1>("faultGenC","supervisor"),
            cadmium::dynamic::translate::make_IC<faultGeneratorD_defs::dataOut, supervisor_defs::D1>("faultGenD","supervisor"),

            cadmium::dynamic::translate::make_IC<supervisor_defs::dataOut, analogOutput_defs::in>("supervisor","logOutput"),
    };

    CoupledModelPtr TOP = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
            "TOP",
                    submodels_TOP,
                    iports_TOP,
                    oports_TOP,
                    eics_TOP,
                    eocs_TOP,
                    ics_TOP
    );

    ///****************////
    cadmium::dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(NDTime("00:00:10:000"));
#ifndef RT_ARM_MBED
    return 0;
#endif
}
