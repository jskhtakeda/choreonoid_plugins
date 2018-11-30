// -*- mode: C++; coding: utf-8-unix; -*-

#ifndef __CONTROLLERBASE_H__
#define __CONTROLLERBASE_H__

#include <cnoid/SimpleController>
#include <functional>
#include <utility>
#include <queue>

namespace cnoid {

using controlLawPair = std::pair<std::function<void>, unsigned>;

class ControlLaw
{
public:
    ControlLaw();
    ~ControlLaw();
    void pushControlLaw(std::function<void> controlFunc, const unsigned control_period)
    {
        law_queue.push(std::make_pair(controlFunc, control_period));
    };
    void clearControlLaw() { std::queue<controlLawPair>().swap(law_queue); };
    void execControl();

private:
    ControlLaw(const Controllaw& org);
    std::queue<controlLawPair> law_queue;
};

class ControllerBase : public SimpleController
{
public:
    virtual bool initialize(SimpleControllerIO* io) override;
    virtual bool start() override;
    virtual bool control() override;
protected:
    std::vector<size_t> actuation_joints_idx_;
    std::vector<cnoid::LinkPtr> joints_;
    std::vector<double> q_ref_;
    std::vector<double> q_prev_;
    std::vector<double> q_ref_prev_;
    std::vector<double> p_gain_;
    std::vector<double> d_gain_;
    std::vector<double> u_limit_;
    double dt_;

    virtual void parseOptionString(SimpleControllerIO* io);
    void PDControl();
};

}
#endif // __PDCONTROLLER_H__
