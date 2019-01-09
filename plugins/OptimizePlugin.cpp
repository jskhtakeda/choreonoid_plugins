// -*- mode: C++; coding: utf-8-unix; -*-

/**
 * @file  OptimizePlugin.cpp
 * @brief Reset the simulation at regular intervals and optimize gains with nlopt
 * @author Hiroki Takeda
 */

#include <cnoid/Plugin>
#include <cnoid/RootItem>
#include <cnoid/SimulatorItem>
#include <cnoid/MessageView>
#include <cnoid/Timer>
#include <cnoid/SimulationBar>
#include <cnoid/ToolBar>
#include <cnoid/SpinBox>

#include "../util/util.h"


#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <string>

using namespace boost::interprocess;
using namespace cnoid;

class OptimizePlugin : public Plugin
{
    SimulatorItemPtr simulator_item_;
    Timer timer_;
    double interval_;
    shared_memory_object shm_start{};
    const char* START_SHM = "Start";

  public:
    OptimizePlugin() : Plugin("Optimize")
    {
        require("Body");
    }

    virtual bool initialize() override
    {
        shm_start = shared_memory_object{open_or_create, START_SHM, read_write};
        shm_start.truncate(1024);

        timer_.sigTimeout().connect([this]() { this->myloopfunc(); });
        timer_.setInterval(100); // 100ms
        timer_.start();
        interval_ = 3.0;
        
        return true;
    }

    virtual bool finalize() override
    {
    }

  private:
    void myloopfunc()
    {
        mapped_region region_start{shm_start, read_write};
        int *start = static_cast<int*>(region_start.get_address());
        if (simulator_item_) {
            if (*start == 0) {
                simulator_item_->startSimulation(true);
                *start = 1;
            } else if (*start  == 1) {
                if (simulator_item_->simulationTime() > interval_)
                    *start = 2;
            }
        } else {
            simulator_item_ = findSimulatorItem("AISTSimulator");
            if (!simulator_item_) {
                MessageView::mainInstance()->putln("[OptimizePlugin] Simulator item can't be found. Stop timer_");
                // timer_.stop();
            }
        }
    }

};

CNOID_IMPLEMENT_PLUGIN_ENTRY(OptimizePlugin)
