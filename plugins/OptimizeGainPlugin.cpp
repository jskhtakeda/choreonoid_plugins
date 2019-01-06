// -*- mode: C++; coding: utf-8-unix; -*-

/**
 * @file  OptimizeGainPlugin.cpp
 * @brief Reset the simulation at regular intervals and optimize gains with nlopt
 * @author Hiroki Takeda
 */

#include <string>

#include <cnoid/Plugin>
#include <cnoid/SimulatorItem>
#include <cnoid/MessageView>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "ResetSimulationPlugin.h"

using namespace boost::interprocess;
using namespace cnoid;

class OptimizeGainPlugin : public Plugin
{
    SimulatorItemPtr simulator_item_;
    shared_memory_object shm_gain{};
    shared_memory_object shm_eval{};
    const char* GAIN_SHM = "Gain";
    const char* EVAL_SHM = "Eval";

  public:
    OptimizeGainPlugin() : Plugin("OptimizeGain")
    {
        require("Body");
    }

    bool initialize() override
    {
        shm_gain.remove(GAIN_SHM);
        shm_eval.remove(EVAL_SHM);
        shm_gain = shared_memory_object{create_only, GAIN_SHM, read_write};
        shm_gain.truncate(1024);
        shm_eval = shared_memory_object{create_only, EVAL_SHM, read_write};
        shm_eval.truncate(1024);

        ResetSimulationPlugin* reset_simulation = findResetSimulation();
        if (reset_simulation) {
            reset_simulation->sigResetSimulation().connect([this]() { this->evalNLOPT(); });
        }

        return true;
    }

    bool finalize() override
    {
        shm_gain.remove(GAIN_SHM);
        shm_eval.remove(EVAL_SHM);
        return true;
    }

  private:

    void evalNLOPT()
    {
        mapped_region region_gain{shm_gain, read_write};
        double *gain = static_cast<double*>(region_gain.get_address());
        for (int i = 0; i < 4; i++)
            gain[i] = 0.01 * i;

        mapped_region region_eval{shm_eval, read_only};
        double *eval = static_cast<double*>(region_eval.get_address());
        for (int i = 0; i < 1; i++)
            MessageView::mainInstance()->putln(std::string("eval: ") + std::to_string(eval[i]));
    }
};

CNOID_IMPLEMENT_PLUGIN_ENTRY(OptimizeGainPlugin)
