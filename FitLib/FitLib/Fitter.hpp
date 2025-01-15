#ifndef FITTER_H
#define FITTER_H

#include "Settings.hpp"
#include "BinInputs.hpp"
#include "Log.hpp"

#include "Minuit2/Minuit2Minimizer.h"

#include <map>
#include <vector>

class Fitter {

public:

    // Initialisation
    Fitter(Settings s, BinInputs* bin_inputs, std::map<int, double> N_gen_Bp, std::map<int, double> N_gen_Bm);
    ~Fitter();
    void RunFit();
    Int_t SetupParameters();

    // Other
    double GetNtot(std::string bmode){
        if (bmode == "Bp") return m_minuit->X()[ 0 ];
        else if (bmode == "Bm") return m_minuit->X()[ 1 ];
        else { m_log.error("Couldn't find B mode"); return -100; }
    }
    double GetObs(std::string obs){
        if (obs == "xp") return m_minuit->X()[ 2 ];
        else if (obs == "xm") return m_minuit->X()[ 3 ];
        else if (obs == "yp") return m_minuit->X()[ 4 ];
        else if (obs == "ym") return m_minuit->X()[ 5 ];
        else{ m_log.error("Couldn't find observable"); return -100; }

    }
    double GetNtotError(std::string bmode){
        if (bmode == "Bp") return m_minuit->Errors()[ 0 ];
        else if (bmode == "Bm") return m_minuit->Errors()[ 1 ];
        else { m_log.error("Couldn't find B mode"); return -100; }
    }
    double GetObsError(std::string obs){
        if (obs == "xp") return m_minuit->Errors()[ 2 ];
        else if (obs == "xm") return m_minuit->Errors()[ 3 ];
        else if (obs == "yp") return m_minuit->Errors()[ 4 ];
        else if (obs == "ym") return m_minuit->Errors()[ 5 ];
        else{ m_log.error("Couldn't find observable"); return -100; }

    }
    double GetMinChiSquared(){ return m_minuit->MinValue(); }
    void SetDebug(bool debug){ m_debug = debug; }

private:
    Log m_log;
    Settings m_settings;
    BinInputs* m_bin_inputs;
    std::map<int, double> m_N_gen_Bp;
    std::map<int, double> m_N_gen_Bm;
    ROOT::Minuit2::Minuit2Minimizer* m_minuit = new ROOT::Minuit2::Minuit2Minimizer();
    bool m_debug = false;
};

#endif //  Fitter_H
