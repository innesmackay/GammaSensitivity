#include "BinInputs.hpp"
#include "TMath.h"

#include <iostream>

BinInputs::BinInputs(Settings set){
    m_log = Log("BinInputs");
    m_settings = set;
    m_nbins = set.getD("n_bins");
}

BinInputs::~BinInputs(){}

void BinInputs::LoadAll(){
    m_log.info("Loading all bin inputs");
    LoadKi();
    if (m_settings.getB("use_cisi")){ LoadCiSi(); }
    else{ CalculateCiSi(); }
    LoadCoherenceFactor();
    CalculateCPObservables();
    LoadIntegratedYields();
    LoadPredictedYields();
    m_log.success("All inputs loaded");
    return;
}

void BinInputs::LoadKi(){
    m_log.info("Loading the Ki inputs");
    Settings Ki_inputs(m_settings.get("Ki_inputs"));
    Ki_inputs.read();
    for (int i=-1*m_nbins; i<m_nbins+1; i++){
        if (i==0) continue;
        m_Ki[i] = Ki_inputs.getD( "K_" + std::to_string(i) );
        if (m_debug){ m_log.debug("K_" + std::to_string(i) + ": " + std::to_string(m_Ki[i])); }
    }
    return;
}


void BinInputs::LoadCiSi(){
    m_log.info("Loading the ci and si inputs from file");
    Settings ci_si_inputs(m_settings.get("sp_inputs"));
    ci_si_inputs.read();
    for (int i=1; i<m_nbins+1; i++){
        m_ci[i] = ci_si_inputs.getD("c_" + std::to_string(i));
        m_si[i] = ci_si_inputs.getD("s_" + std::to_string(i));
        m_ci[-1*i] = m_ci[i];
        m_si[-1*i] = -1*m_si[i];
        if (m_debug){
            m_log.debug("c_" + std::to_string(i) + ": " + std::to_string(m_ci[i]));
            m_log.debug("s_" + std::to_string(i) + ": " + std::to_string(m_si[i]));
        }
    }
    return;
}


void BinInputs::CalculateCiSi(){
    m_log.info("Calculating the ci and si inputs from coherence and deltaD");
    Settings coherence_inputs(m_settings.get("coherence_inputs"));
    coherence_inputs.read();
    Settings deltaD_inputs(m_settings.get("deltaD_inputs"));
    deltaD_inputs.read();
    for (int i=1; i<m_nbins+1; i++){
        m_ci[i] = coherence_inputs.getD("kappa_" + std::to_string(i)) * TMath::Cos(TMath::DegToRad()*deltaD_inputs.getD("delta_" + std::to_string(i)));
        m_si[i] = coherence_inputs.getD("kappa_" + std::to_string(i)) * TMath::Sin(TMath::DegToRad()*deltaD_inputs.getD("delta_" + std::to_string(i)));
        m_ci[-1*i] = m_ci[i];
        m_si[-1*i] = -1*m_si[i];
    }
    return;
}


void BinInputs::CalculateCPObservables(){
    m_log.info("Calculating CP observables");
    double g = TMath::DegToRad()*m_settings.getD("input_gamma");
    double dB = TMath::DegToRad()*m_settings.getD("input_deltaB");
    double rB = m_settings.getD("input_rB");
    m_cp_obs["xp"] = rB * TMath::Cos(g + dB);
    m_cp_obs["xm"] = rB * TMath::Cos(dB - g);
    m_cp_obs["yp"] = rB * TMath::Sin(g + dB);
    m_cp_obs["ym"] = rB * TMath::Sin(dB - g);
    if (m_debug){
        m_log.debug("Input x+: " + std::to_string(m_cp_obs["xp"]));
        m_log.debug("Input x-: " + std::to_string(m_cp_obs["xm"]));
        m_log.debug("Input y+: " + std::to_string(m_cp_obs["yp"]));
        m_log.debug("Input y-: " + std::to_string(m_cp_obs["ym"]));
    }
    return;
}


void BinInputs::LoadIntegratedYields(){
    m_log.info("Loading integrated yields");
    m_int_N["Bp"] = m_settings.getD("N_tot_Bp");
    m_int_N["Bm"] = m_settings.getD("N_tot_Bm");
    return;
}


void BinInputs::LoadCoherenceFactor(){
    m_kappa = m_settings.getD("input_coherence");
    if (m_debug) m_log.debug("Input coherence factor: " + std::to_string(m_kappa));
    return;
}


void BinInputs::LoadPredictedYields(){
    m_log.info("Loading the predicted yields ...");
    double rB = m_settings.getD("input_rB");
    std::map<int, double> unnormalised_Yi_Bp;
    std::map<int, double> unnormalised_Yi_Bm;
    double tot_Yi_Bp = 0; double tot_Yi_Bm = 0;
    for (int i=-1*m_nbins; i<m_nbins+1; i++){
        if (i==0){ continue; }
        unnormalised_Yi_Bp[i] = m_Ki[-1*i] + rB*rB*m_Ki[i] + 2*m_kappa*TMath::Sqrt(m_Ki[-1*i]*m_Ki[i])*(m_ci[i]*m_cp_obs["xp"] - m_si[i]*m_cp_obs["yp"]);
        tot_Yi_Bp += unnormalised_Yi_Bp[i];
        unnormalised_Yi_Bm[i] = m_Ki[i] + rB*rB*m_Ki[-1*i] + 2*m_kappa*TMath::Sqrt(m_Ki[-1*i]*m_Ki[i])*(m_ci[i]*m_cp_obs["xm"] + m_si[i]*m_cp_obs["ym"]);
        tot_Yi_Bm += unnormalised_Yi_Bm[i];
    }
    
    for (int i=-1*m_nbins; i<m_nbins+1; i++){
        if (i==0){ continue; }
        m_Ni_Bp[i] = (m_int_N["Bp"]*unnormalised_Yi_Bp[i]) / tot_Yi_Bp;
        m_Ni_Bm[i] = (m_int_N["Bm"]*unnormalised_Yi_Bm[i]) / tot_Yi_Bm;
        if (m_debug){
            m_log.debug("N(B+)_" + std::to_string(i) + ": " + std::to_string(m_Ni_Bp[i]));
            m_log.debug("N(B-)_" + std::to_string(i) + ": " + std::to_string(m_Ni_Bm[i]));
        }
    }
    return;
}