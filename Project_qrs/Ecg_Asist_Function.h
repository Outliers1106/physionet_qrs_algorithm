#ifndef ECG_ASIST_FUNCTION_H
#define ECG_ASIST_FUNCTION_H


//#define fs 360            //sample fs is 360Hz
//#define window 36        //moving window size is 100//2016.07.25
//#define idxWindow 36      //cell is 0.1*fs
#define minDuration 15
extern int fs;
extern int window;
extern int idxWindow;

//#define NumPoint 650000   //count of data is 650000
extern int NumPoint;

double* differential(double* ECG);

double* FPDiff(double* ECG);

double  ModifiedInitialMax(double* ECG,int i);

void Modifiedupdate_array(double* threshArray , double* ECG , int count, int i);

void ModifiedQRSdetection(double* ECG, double *RpeakIndex);

int* Modifiedbeattimes(double* QRS , int *length);

double* mwintegration(double* ECG);

int* beattimes(double* QRS , int *length);

double  InitialMax(double* ECG);

double* QRSdetection(double* ECG);

double* TWavdetection(double* ECG, double *QRSs);

double* PWavdetection(double* ECG, double *QRSs);

double sum(double* ECG, int length);

double max(double* ECG , int length);

double min(double* ECG, int length);

double* _abs(double* ECG,int i);

int Max_Pos(double* ECG,int length);

void update_array(double* threshArray , double* ECG , int i);


#endif // ECG_ASIST_FUNCTION_H
