#include "Settings.hpp"
#include "BinInputs.hpp"
#include "Fitter.hpp"
#include "TRandom.h"

int main(int argc , char* argv[]){

    Log log("main");

    // Fit config
    std::string settings_file = argv[1];
    Settings set(settings_file);
    set.read();

    // Load bin inputs
    BinInputs* bin_inputs = new BinInputs(set);
    bin_inputs->SetDebug(set.getB("debug"));
    bin_inputs->LoadAll();
    gRandom->SetSeed(set.getD("random_seed"));

    // Generate and run fit
    std::ofstream outfile; outfile.open(set.get("outfile"));
    for (int i=0; i<set.getI("n_toys"); i++){
        log.debug("Running toy " + std::to_string(i));
        log.info("Generating yields ...");
        std::map<int, double> N_gen_Bp;
        std::map<int, double> N_gen_Bm;
        for (int j=-1*set.getI("n_bins"); j<set.getI("n_bins")+1; j++){
            if (j==0) continue;
            N_gen_Bp[j] = gRandom->Poisson(bin_inputs->GetPredictedNi("Bp", j));
            N_gen_Bm[j] = gRandom->Poisson(bin_inputs->GetPredictedNi("Bm", j));
            if (set.getB("debug")){
                log.debug("Generated N(B+)_" + std::to_string(j) + ": " + std::to_string(N_gen_Bp[j]));
                log.debug("Generated N(B-)_" + std::to_string(j) + ": " + std::to_string(N_gen_Bm[j]));
            }
        }
        log.success("Yields generated!");

        Fitter* fit = new Fitter(set, bin_inputs, N_gen_Bp, N_gen_Bm);
        fit->SetDebug(set.getB("debug"));
        fit->RunFit();
        outfile << fit->GetObs("xp") << " " << fit->GetObsError("xp") << " ";
        outfile << fit->GetObs("xm") << " " << fit->GetObsError("xm") << " ";
        outfile << fit->GetObs("yp") << " " << fit->GetObsError("yp") << " ";
        outfile << fit->GetObs("ym") << " " << fit->GetObsError("ym") << std::endl;
    }
    outfile.close();

}
