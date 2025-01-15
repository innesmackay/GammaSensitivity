#ifndef BININPUTS_H
#define BININPUTS_H

#include "Log.hpp"
#include "Settings.hpp"

#include <ostream>
#include <map>

class BinInputs {

public:
    BinInputs(Settings set);
    ~BinInputs();
    void LoadAll();
    void LoadKi();
    void LoadCiSi();
    void CalculateCiSi();
    void CalculateCPObservables();
    void LoadIntegratedYields();
    void LoadPredictedYields();
    void LoadCoherenceFactor();

    int GetNbins(){ return m_nbins; }
    double GetCi(int bin){ return m_ci[bin]; }
    double GetSi(int bin){ return m_si[bin]; }
    double GetKi(int bin){ return m_Ki[bin]; }
    double GetCoherenceFactor(){ return m_kappa; }
    double GetObs(std::string obs){ return m_cp_obs[obs]; }
    double GetIntegratedYield(std::string bmode){ return m_int_N[bmode]; }
    double GetPredictedNi(std::string bmode, int bin){
        if (bmode == "Bp") return m_Ni_Bp[bin];
        else if (bmode == "Bm") return m_Ni_Bm[bin];
        else{ m_log.error("Couldn't find B mode"); return 0; }
    }
    void SetDebug(bool debug){ m_debug = debug; }

private:
    bool m_debug = false;
    Log m_log;
    int m_nbins;
    Settings m_settings;
    std::map<int, double> m_Ki;
    std::map<int, double> m_ci;
    std::map<int, double> m_si;
    double m_kappa;
    std::map<std::string, double> m_cp_obs;
    std::map<std::string, double> m_int_N;
    std::map<int, double> m_Ni_Bp;
    std::map<int, double> m_Ni_Bm;
};

#endif //  ObservedYields_H
