#ifndef COCO_CORE_SN_HPP_
#define COCO_CORE_SN_HPP_

#include <vector>
#include <unordered_map>
#include <string>



// Data structure for light curve (one filter per object)
struct LCData {
	std::vector<double> completeMJD_;
    std::vector<double> completeFlux_;
    std::vector<double> completeFluxErr_;

	std::string name_;
    std::string filter_;
    std::vector<double> mjd_;
    std::vector<double> t_;
    std::vector<double> flux_;
    std::vector<double> fluxErr_;

    double mjdMin_;
	double mjdMax_;
	double normalization_;
};



// Data structure for specta
struct SpecData {
    std::vector<double> rawWav_;
    std::vector<double> rawFlux_;

    std::vector<double> wav_;
    std::vector<double> flux_;
    std::vector<double> fluxCorrected_;
};



// Data structure for one epoch of data contains a slice though a light curve
// and a spectrum if available
struct SNEpoch {
    double MJD_;
    std::vector<double> flux_;
    std::vector<double> fluxErr_;
    std::vector<double> filter_;

    SpecData spec_;
};



// Supernova(SN) class prividing the main data object in the code.
// The class can be initialised multiple ways depending on the usage:
// 1)  SN() - Initialises an empty data structure, used when light curves and
//     spectra are results of simulations and not loaded from text files.
// 2)  SN(string) - Initialises SN class with either a light curve file or
//     list of spectra
// 3)  SN(string,string) - Initialises with both the light curve and list of spectra
class SN {
private:
    // Raw input data for a light curve file
    std::vector<double> _rawMJD;
    std::vector<double> _rawFlux;
    std::vector<double> _rawFluxErr;
    std::vector<std::string> _rawFilter;

public:
    // Data file names
    std::string spectraFileList_;
    std::string lcFile_;

    // SN properties
    double z_;
    std::string name_;
    std::vector<std::string> filterList_;

    // Hash tables mapping filter names onto light curves and MJDs onto spectra
    std::unordered_map<std::string,LCData> LC_;
    std::unordered_map<double,SpecData> spec_;
    std::unordered_map<double,SNEpoch> epoch_;

    // Constructors
    SN();
    SN(std::string);
    SN(std::string,std::string);

    // Data loading routines
    void loadLC(std::string);
    void restoreCompleteLC();
};


#endif  // COCO_CORE_SN_HPP_
