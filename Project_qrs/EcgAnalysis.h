#ifndef ECGANALYSIS_H
#define ECGANALYSIS_H

#include <string>
//#include "ecganalysis_global.h"

using namespace std;
void filter(int,double *,double *,int,double *,double *);
//void Calcu_R_Rate(QString, int *, int , int &, int &, int &, float &, float &);
//void Calcu_R_Rate(QString, int *, int , int &, int &, int &, float &, float &);

class EcgAnalysis
{

public:
    EcgAnalysis(string filepath, string output_dir);
    ~EcgAnalysis();

public:
    double *ECGArrayLead1 = NULL;      // Lead_I  of ECG data
    double *ECGArrayLead2 = NULL;      // Lead_II of ECG data

    double* QRSs = NULL;               // data of QRSs
    double* TWavs = NULL;              // data of TWavs
    double* PWavs = NULL;              // data of PWavs

    double* Ramp = NULL;               // amp of Rwav
    double* Pamp = NULL;               // amp of Pwav
    double* Tamp = NULL;               // amp of Twav

    int* QRSbt = NULL;                 //beatimes of QRSWavs
    int* Pbt = NULL;                   //beatimes of PWavs
    int* Tbt = NULL;                   //beatimes of TWavs

    int QRSWavCnt;              //count of QRSWavs
    int PWavCnt;                //count of PWavs
    int TWavCnt;                //count of TWavs

    double* QRSIntervals = NULL;       //Intervals of RR
    double* PRInterval = NULL;         //Intervals of PR
    double* QTInterval = NULL;         //Intervals of QT

    double* QQIntervals = NULL;        //Intervals of QQ
    double* PPIntervals = NULL;        //Intervals of PP
    double* TTIntervals = NULL;        //Intervals of TT

    string datapath;           //the data path string

    int TP, FP, FN;

};

/*
extern "C" {
    ECGANALYSISSHARED_EXPORT EcgAnalysis* getDllObjet(string data_path);    //create
    ECGANALYSISSHARED_EXPORT void releseDllObject(EcgAnalysis*);             //realease
}
*/
#endif // ECGANALYSIS_H
