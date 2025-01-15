#ifndef LIKELIHOOD_H
#define LIKELIHOOD_H

#include "BinInputs.hpp"
#include "Log.hpp"

#include <map>

class Likelihood {

public:
    Likelihood(BinInputs* bin_inputs, std::map<int, double> N_gen_Bp, std::map<int, double> N_gen_Bm);
    double operator()(const double* x);
    void SetDebug(bool debug){ m_debug = debug; }

private:
    std::map<int, double> m_N_gen_Bp;
    std::map<int, double> m_N_gen_Bm;
    BinInputs* m_bin_inputs;
    bool m_debug = false;
    Log m_log;

};

#endif //  Likelihood_H
