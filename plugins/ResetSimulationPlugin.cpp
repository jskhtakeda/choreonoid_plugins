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
#include <cnoid/ToolBar>
#include <cnoid/SpinBox>

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
        reset_timer_.sigTimeout().connect([this]() { this->resetSimulation(); });
        reset_timer_.setInterval(100); // 100ms
        reset_interval_ = 3.0;

        std::unique_ptr<ToolBar> bar = std::make_unique<ToolBar>("ResetSimulation");
        bar->setVisibleByDefault(true);

        ToolButton* button = bar->addToggleButton("ResetSimulation");
        button->sigToggled().connect([this](bool checked) {
                if (checked) this->reset_timer_.start();
                else this->reset_timer_.stop();
            });

        std::unique_ptr<SpinBox> timeSpin = std::make_unique<SpinBox>();
        timeSpin->setMaximum(1000000);
        timeSpin->setValue(reset_interval_);
        timeSpin->sigValueChanged().connect([this](const int value) { this->reset_interval_ = value; });
        bar->addWidget(timeSpin.release());

        addToolBar(bar.release());
        return true;
    }

private:

    void resetSimulation()
    {
        if (simulator_item_) {
            if (simulator_item_->simulationTime() > reset_interval_)
                simulator_item_->startSimulation(true);
        } else {
            simulator_item_ = dynamic_cast<SimulatorItem*>(RootItem::instance()->findItem("AISTSimulator"));
            if (!simulator_item_) {
                MessageView::mainInstance()->putln("[ResetSimulationPlugin] Simulator item can't be found. Stop reset_timer_");
                reset_timer_.stop();
            }
        }
    }
};

CNOID_IMPLEMENT_PLUGIN_ENTRY(ResetSimulationPlugin)
