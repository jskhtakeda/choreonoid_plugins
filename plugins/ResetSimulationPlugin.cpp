// -*- mode: C++; coding: utf-8-unix; -*-

/**
 * @file  ResetSimulation.cpp
 * @brief Reset the simulation at regular intervals
 * @author Tatsuya Ishikawa
 */

#include <cnoid/Plugin>
#include <cnoid/RootItem>
#include <cnoid/SimulatorItem>
#include <cnoid/MessageView>
#include <cnoid/Timer>

using namespace cnoid;

class ResetSimulationPlugin : public Plugin
{
    SimulatorItemPtr simulator_item_;
    Timer reset_timer_;
    double reset_interval_;
public:

    ResetSimulationPlugin() : Plugin("ResetSimulation")
    {
    }

    virtual bool initialize() override
    {
        reset_timer_.sigTimeout().connect([this]() { resetSimulation(); });
        reset_timer_.setInterval(100); // 100ms
        reset_timer_.start();
        reset_interval_ = std::numeric_limits<int>::max();
        return true;
    }

private:

    void resetSimulation()
    {
        if (simulator_item_) {
            if (simulator_item_->simulationTime() > reset_interval_)
                simulator_item_->startSimulation(true);
        } else {
            ItemPtr robot_item = RootItem::instance()->findItem("Robot");
            simulator_item_ = SimulatorItem::findActiveSimulatorItemFor(robot_item);
            if (!simulator_item_) {
                MessageView::mainInstance()->putln("[ResetSimulationPlugin] Simulator item can't be found. Stop reset_timer_");
                reset_timer_.stop();
            }
        }
    }
};

CNOID_IMPLEMENT_PLUGIN_ENTRY(ResetSimulationPlugin)
