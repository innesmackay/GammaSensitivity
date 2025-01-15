#include "Likelihood.hpp"

#include "TMath.h"

Likelihood::Likelihood(BinInputs* bin_inputs, std::map<int, double> N_gen_Bp, std::map<int, double> N_gen_Bm){
    m_bin_inputs = bin_inputs;
    m_N_gen_Bp = N_gen_Bp;
    m_N_gen_Bm = N_gen_Bm;
    m_log = Log("Likelihood");
}


double Likelihood::operator()(const double* par){

    // Setup fit parameters
    double n = m_bin_inputs->GetNbins();
    std::map<int, double> Yi_map_Bp;
    std::map<int, double> Yi_map_Bm;
    double tot_Yi_Bp = 0;
    double tot_Yi_Bm = 0;
    for (int i=-1*n; i<n+1; i++){
        if (i==0) continue;
        double Ki = m_bin_inputs->GetKi(i);
        double Kmi = m_bin_inputs->GetKi(-1*i);
        double ci = m_bin_inputs->GetCi(i);
        double si = m_bin_inputs->GetSi(i);
        double kappa = m_bin_inputs->GetCoherenceFactor();

        Yi_map_Bp[i] = Kmi + (par[2]*par[2] + par[4]*par[4])*Ki + 2*kappa*TMath::Sqrt(Ki*Kmi)*(ci*par[2] - si*par[4]);
        tot_Yi_Bp += Yi_map_Bp[i];
        Yi_map_Bm[i] = Ki + (par[3]*par[3] + par[5]*par[5])*Kmi + 2*kappa*TMath::Sqrt(Ki*Kmi)*(ci*par[3] + si*par[5]);
        tot_Yi_Bm += Yi_map_Bm[i];

    }

    double logL = 0;
    for (int i=-1*n; i<n+1; i++){
        if (i==0) continue;
        double N_fit_Bp = par[0] * Yi_map_Bp[i] / tot_Yi_Bp;
        double N_fit_Bm = par[1] * Yi_map_Bm[i] / tot_Yi_Bm;
        logL += m_N_gen_Bp[i] * TMath::Log(N_fit_Bp) - N_fit_Bp;
        logL += m_N_gen_Bm[i] * TMath::Log(N_fit_Bm) - N_fit_Bm;

        //double bp_pull = (m_N_gen_Bp[i] - N_fit_Bp)/TMath::Sqrt(m_N_gen_Bp[i]);
        //double bm_pull = (m_N_gen_Bm[i] - N_fit_Bm)/TMath::Sqrt(m_N_gen_Bm[i]);
        //logL -= TMath::Power(bp_pull, 2);
        //logL -= TMath::Power(bm_pull, 2);

    }

    return -1*logL;
}
