#include "Fitter.hpp"
#include "Likelihood.hpp"

#include "Math/Functor.h"
#include "Math/Functions.h"
#include "Minuit2/FCNBase.h"

Fitter::Fitter(Settings s, BinInputs* bin_inputs, std::map<int, double> N_gen_Bp, std::map<int, double> N_gen_Bm){
    m_settings = s;
    m_bin_inputs = bin_inputs;
    m_N_gen_Bp = N_gen_Bp;
    m_N_gen_Bm = N_gen_Bm;
    m_log = Log("Fitter");
}

Fitter::~Fitter(){}

void Fitter::RunFit(){

    m_log.info("Initialisng the likelihood fit");

    // Other details
    m_minuit->SetPrintLevel(3);
    m_minuit->SetStrategy(2);
    m_minuit->SetErrorDef(0.5);

    // Setup parameters
    Int_t nPars = this->SetupParameters();

    // Set the likelihood function
    Likelihood* L = new Likelihood(m_bin_inputs, m_N_gen_Bp, m_N_gen_Bm);
    L->SetDebug(m_debug);
    ROOT::Math::Functor fcn(*L, nPars);
    m_minuit->SetFunction(fcn);

    m_log.success("Minimising ...");
    m_minuit->Minimize();

    m_log.success("Running HESSE ...");
    m_minuit->Hesse();

    //std::cout << "Running MINOS" << std::endl;
    //double xp_lo; double xp_hi;
    //m_minuit->GetMinosError(2, xp_lo, xp_hi);
    //std::cout << " MINOS " << xp_lo << " " << xp_hi << std::endl;

    return;
}

Int_t Fitter::SetupParameters(){
    m_log.info("Initialisng the input parameters");
    m_minuit->SetVariable(0, "Total_Bp_yield", m_bin_inputs->GetIntegratedYield("Bp"), 0.02);
    m_minuit->SetVariableLimits(0, 0.85*m_bin_inputs->GetIntegratedYield("Bp"), 1.15*m_bin_inputs->GetIntegratedYield("Bp"));
    m_minuit->SetVariable(1, "Total_Bm_yield", m_bin_inputs->GetIntegratedYield("Bm"), 0.02);
    m_minuit->SetVariableLimits(1, 0.85*m_bin_inputs->GetIntegratedYield("Bm"), 1.15*m_bin_inputs->GetIntegratedYield("Bm"));
    m_minuit->SetVariable(2, "x+", m_bin_inputs->GetObs("xp"), 0.00001);
    m_minuit->SetVariableLimits(2, -1, 1);
    m_minuit->SetVariable(3, "x-", m_bin_inputs->GetObs("xm"), 0.00001);
    m_minuit->SetVariableLimits(3, -1, 1);
    m_minuit->SetVariable(4, "y+", m_bin_inputs->GetObs("yp"), 0.00001);
    m_minuit->SetVariableLimits(4, -1, 1);
    m_minuit->SetVariable(5, "y-", m_bin_inputs->GetObs("ym"), 0.00001);
    m_minuit->SetVariableLimits(5, -1, 1);
    return 6;
}