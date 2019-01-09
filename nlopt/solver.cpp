#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include "unistd.h"

#include <nlopt.hpp>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace boost::interprocess;

nlopt::opt solver;

shared_memory_object shm_start{};
shared_memory_object shm_gain{};
shared_memory_object shm_eval{};

const char* START_SHM = "Start";
const char* GAIN_SHM = "Gain";
const char* EVAL_SHM = "Eval";

const int GAIN_NUM = 4;

void myinitialize()
{
    // TODO: 本当はinitialize()の初めに書くのでなく
    // ctrl+Cされたときにremoveが呼ばれるとかにするべき
    shm_start.remove(START_SHM);
    shm_gain.remove(GAIN_SHM);
    shm_eval.remove(EVAL_SHM);
    
    shm_start = shared_memory_object{create_only, START_SHM, read_write};
    shm_gain = shared_memory_object{create_only, GAIN_SHM, read_write};
    shm_eval = shared_memory_object{create_only, EVAL_SHM, read_write};

    shm_start.truncate(1024);
    shm_gain.truncate(1024);
    shm_eval.truncate(1024);
}

double myfunc(const std::vector<double> &x, std::vector<double> &grad, void *data)
{
    // shm setting
    mapped_region region_start{shm_start, read_write};
    mapped_region region_gain{shm_gain, read_write};
    mapped_region region_eval{shm_eval, read_write};
    // shm cast
    bool *start = static_cast<bool*>(region_start.get_address());
    double *gain = static_cast<double*>(region_gain.get_address());
    double *eval = static_cast<double*>(region_eval.get_address());
    // set gains
    for (int i=0; i<GAIN_NUM; i++) { gain[i] = x[i]; }
    std::cout << "[nlopt/solver.cpp] nlopt wants to simulate with this gains: "
              << x[0] << ", " << x[1] << ", " << x[2] << ", " << x[3] << std::endl;
    // start simulation
    *start = true;
    // wait for finishing simulation
    while (!(*start)) { usleep(10000); } // [us]
    // when simulator is finished, read evaluation value and return it
    return *eval;
}

void mysolve()
{
    // generate solver
    solver = nlopt::opt(nlopt::GN_ISRES, 4);
    // set lower bound for solver
    std::vector<double> lb(GAIN_NUM);
    lb[0] = -100; lb[1] = -100; lb[2] = -100; lb[3] = -100;
    solver.set_lower_bounds(lb);
    // set upper bound for solver
    std::vector<double> ub(GAIN_NUM);
    ub[0] = 100; ub[1] = 100; ub[2] = 100; ub[3] = 100;
    solver.set_upper_bounds(ub);
    // set objective func for solver
    solver.set_min_objective(myfunc, NULL);
    // set x tolerance for solver
    solver.set_xtol_rel(1e-4);
    // define i/o variables for solver
    std::vector<double> x(GAIN_NUM);
    x[0] = 0.0; x[1] = 0.0; x[2] = 0.0; x[3] = 0.0;
    double minf;

    try{
        // set i/o variables to solver and start optimizing
        nlopt::result result = solver.optimize(x, minf);
        std::cout << "found minimum at f(" << x[0] << ","
                  << x[1] << "," << x[2] << "," << x[3] << ") = "
                  << std::setprecision(10) << minf << std::endl;
    }
    catch(std::exception &e) {
        std::cout << "nlopt failed: " << e.what() << std::endl;
    }
}

int main()
{
    myinitialize();
    mysolve();
}
