#include "Ecg_Asist_Function.h"
#include <malloc.h>
#include <math.h>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <exception>

using namespace std;

int NumPoint = 0;
int fs = 0;
int window = 0;
int idxWindow = 0;

double* differential(double* ECG)
{
    double *ECGArrayTemp;
    ECGArrayTemp = new double[NumPoint];

    ECGArrayTemp[0] = ((1.0/8.0))*((0)+(-(2.0*0))+(2.0*ECG[1])+ECG[2]);
    ECGArrayTemp[1] = ((1.0/8.0))*((0)+(-(2.0*ECG[0]))+(2.0*ECG[2])+ECG[3]);
    ECGArrayTemp[NumPoint-2] = ((1.0/8.0))*((-ECG[NumPoint-4])+(-(2.0*ECG[NumPoint-3]))+(2.0*ECG[NumPoint-2])+0);
    ECGArrayTemp[NumPoint-1] = ((1.0/8.0))*((-ECG[NumPoint-3])+(-(2.0*ECG[NumPoint-2])));

    int i;
    for(i = 2; i < NumPoint-2; i++){
        ECGArrayTemp[i] = ((1.0/8.0))*((-ECG[i-2])+(-(2.0*ECG[i-1]))+(2.0*ECG[i+1])+ECG[i+2]);
    }

    return ECGArrayTemp;
}

//2016.01.22 added
double* FPDiff(double *ECG)
{
    double *ECGArrayTemp;
    ECGArrayTemp = new double[NumPoint];

    ECGArrayTemp[0] = ((1.0/8.0))*((2.0*ECG[1])+ECG[2]);
    ECGArrayTemp[1] = ((1.0/8.0))*((-(2.0*ECG[0]))+(2.0*ECG[2])+ECG[3]);
    ECGArrayTemp[NumPoint-1] = ((1.0/8.0))*((-ECG[NumPoint-3])+(-(2.0*ECG[NumPoint-2])));
    ECGArrayTemp[NumPoint-2] = ((1.0/8.0))*((-ECG[NumPoint-4])+(-(2.0*ECG[NumPoint-3]))+(2.0*ECG[NumPoint-1]));

    int i;
    for(i = 2; i < NumPoint-2; i++){
        ECGArrayTemp[i] = ((1.0/8.0))*((-ECG[i-2])+(-(2.0*ECG[i-1]))+(2.0*ECG[i+1])+ECG[i+2]);
    }

    return ECGArrayTemp;

}
//2016.01.22 finished

double* mwintegration(double *ECG)
{
    double* ECGArrayTemp = new double[NumPoint];

    int i,j;
    for(i = 0; i < NumPoint; i++){
        ECGArrayTemp[i] = 0;
        if(i>=window)
        {
            for(j = i; j < window+i; j++){
                ECGArrayTemp[i] = ECGArrayTemp[i] + ECG[j-window];
            }
        }
    }

    for(i = 0; i < NumPoint; i++){
        ECG[i] = ECGArrayTemp[i];
    }

    delete[] ECGArrayTemp;

    return ECG;
}

int* beattimes(double* QRS , int *length)
{
    int j,k=0;
    for (j=0;j<NumPoint;j++)
    {
        if( QRS[j])
        {
            k = k+1;
        }
    }

    int *beat_times;
    beat_times = new int[k];

    k=0;
    for (j=0;j<NumPoint;j++)
    {
        if( QRS[j])  // 2016.01.27 no this is the QRS[j]   // QRS[j] && j%2==1 2016.7.26
        {
            beat_times[k] = QRS[j];     //2016.1.26/ old is beat_times[k] = j;
            k = k+1;
        }
    }
    *length = k;

    return beat_times;
}

int* Modifiedbeattimes(double* QRS , int *length)
{
    int j,k=0;
    for (j=0;j<NumPoint;j++)
    {
        if( QRS[j])
        {
            k = k+1;
        }
    }

    int *beat_times;
    beat_times = new int[k];

    k=0;
    for (j=0;j<NumPoint;j++)
    {
        if( QRS[j])
        {
            beat_times[k] = j;     //2016.1.26/ old is beat_times[k] = j;
            k = k+1;
        }
    }

    *length = k;

    return beat_times;
}

double InitialMax(double* ECG)
{
    //  initMax = max(W(1, 3*sfreq:6*sfreq));
    double* ECG_Array = new double[((6-3)*fs+1)];
    int j;
    for(j=0;j<=((6-3)*fs);j++)
    {
        ECG_Array[j] = ECG[3*fs+j];
    }

    delete[] ECG_Array;
    return max(ECG_Array,3*fs+1);
}

//2016.01.22 added
double ModifiedInitialMax(double *ECG, int i)
{
    try {
        double *ECG_Array = new double[(2*fs+1)];

        int j;

        for(j=0;j<=2*fs;j++)
        {
            ECG_Array[j] = ECG[j+(2*i+1)*fs];
        }

        double result = max(ECG_Array,2*fs+1);
        delete[] ECG_Array;

        return result;
    }catch (exception& e)
    {
        printf("Exception: %s", e.what());
        return 0;
    }
}
//2016.01.22 finished

double sum(double* ECG,int length)
{
    int j;
    double sum = 0;
    for(j=0;j<length;j++)
    {
        sum += ECG[j];
    }
    return sum;
}

double max(double* ECG , int length)
{
    double max = ECG[0];
    int i;
    for(i=0;i<length;i++)
    {
        if(max<ECG[i])
            max = ECG[i];
    }
    return max;
}

double min(double* ECG, int length)
{
    double min = ECG[0];
    int i;
    for(i=0;i<length;i++)
    {
        if(min>ECG[i])
            min = ECG[i];
    }
    return min;
}

int Max_Pos(double* ECG,int length)
{
    double max = ECG[0];
    int pos = 0;
    int i;
    for(i=0;i<length;i++)
    {
        if(max<ECG[i])
        {
            max = ECG[i];
            pos = i;
        }
    }

    return pos+1;
}

double* _abs(double* ECG,int i)
{
    double *ECG_Array;
    ECG_Array = new double[(idxWindow+1)];

    int j;
    for(j=0;j<=idxWindow;j++)
    {
        ECG_Array[j] = fabs(ECG[j+i-idxWindow/2]);
    }

    return ECG_Array;
}

void update_array(double* threshArray , double* ECG , int i)
{

    int j;
    for(j=0;j<6;j++)
    {
        threshArray[j] = threshArray[j+1];
    }

    double* ECG_Array = new double[(2*idxWindow+1)];

    for(j=0;j<=2*idxWindow;j++)
    {
        ECG_Array[j] = ECG[j+i-idxWindow];
    }

    threshArray[6] = max(ECG_Array,2*idxWindow+1);

    delete[] ECG_Array;
}

//2016.1.23
void Modifiedupdate_array(double *threshArray, double *ECG, int count,int i)
{
    int j;
    for(j=0;j<count-1;j++)
    {
        threshArray[j] = threshArray[j+1];
    }

    double* ECG_Array = new double[(idxWindow+1)];

    for(j=0;j<=idxWindow;j++)
    {
        ECG_Array[j] = ECG[j+i-idxWindow/2];
    }

    threshArray[count-1] = max(ECG_Array,idxWindow+1);

    delete[] ECG_Array;
}
//2016.1.23finished
/*
double* QRSdetection(double* ECG)
{
    // Differential function
    double *_ECGs;
    _ECGs = differential(ECG);


    //Squaring step
    int j;
    for(j=0;j<NumPoint;j++)
    {
        _ECGs[j] = _ECGs[j] * _ECGs[j];
    }

    //Moving window interation
    _ECGs = mwintegration(_ECGs);

    //Decision rules
    double Tminus = -(0.1 * fs);
    double threshFrac = 0.5;
    double initMax = InitialMax(_ECGs);


    double *threshArray;
    threshArray = (double*) malloc(7*sizeof(double));
    for(j=0;j<7;j++)
    {
        threshArray[j] = initMax;
    }

    double threshAverage = threshFrac*(initMax*7)/7;

    double *QRSs;
    QRSs = (double*) malloc(NumPoint*sizeof(double));

    double *QRSloc;

    int k;
    for(k=0;k<NumPoint;k++)
    {
        QRSs[k] = 0;
    }

    int i = 0;
    double Max;
    int Pos;

    while ( i < NumPoint )
    {
        i = i+1;
        if ( (i>idxWindow) && (i< (NumPoint - idxWindow)) )
        {
            if ( ( _ECGs[i] > threshAverage ) && ( (i - Tminus) > (0.4 * fs) ) )       //0.4 old is 0.2
            {
                QRSloc = _abs(ECG,i);   
                Max = max(QRSloc,idxWindow+1);
                Pos = Max_Pos(QRSloc,idxWindow+1);

                QRSs[i-idxWindow/2+1+Pos] = ECG[i-idxWindow/2+1+Pos]; // old is _ECGs is wrong
                Tminus = i;
                update_array(threshArray, _ECGs, i);
                threshAverage = threshFrac * (sum(threshArray,7) - (min(threshArray,7) + max(threshArray,7))) / 5;
            }
        }
    }

    SFREE(threshArray);

    return QRSs;
}
*/
//2016.1.22  added modified
void ModifiedQRSdetection(double *ECG,double *RpeakIndex)      //double* ModifiedQRSdetection(double *ECG)
{
    // Differential function
    double *_ECGs;
    _ECGs = FPDiff(ECG);

    int j;
    //Squaring step
    for(j=0;j<NumPoint;j++)
    {
        _ECGs[j] = _ECGs[j] * _ECGs[j];
    }

    //Moving window interation
    _ECGs = mwintegration(_ECGs);

    //Decision rules
    double Tminus = -(0.1 * fs);
    double threshFrac = 0.4;

    double initMax;
    double threshAverage;


    double threshArray[7];
    for(j=0;j<7;j++)
    {
        threshArray[j] = 0;
    }

    for(j=0;j<7;j++)
    {
        initMax = ModifiedInitialMax(_ECGs,j);
        threshArray[j] = initMax;
    }

    threshAverage = threshFrac*(sum(threshArray,7) - min(threshArray,7) - max(threshArray,7))/5;

    //Modified secondary threshold.
    double last5interval[5];

    for(j=0;j<5;j++)
    {
        last5interval[j] = 0.75*fs;
    }

    double secondary_threshold;

    double last5interval_average;
    last5interval_average = sum(last5interval,5)/5;

//    double *RpeakIndex;
//    RpeakIndex = (double*) malloc(NumPoint*sizeof(double));

    double *QRSloc;

    int k;
    for(k=0;k<NumPoint;k++)
    {
        RpeakIndex[k] = 0;
    }

    int i=1;
    int Rpeakcnt =1;
    int real_loc;
    int iter=0;
    double Max;
    int Pos;

    while ( i < NumPoint )
    {
        if ( (i>idxWindow) && (i< (NumPoint - idxWindow)) )
        {
            if ( ( _ECGs[i] > threshAverage ) && ( (i - Tminus) > (0.2 * fs) ) )
            {
                QRSloc = _abs(ECG,i);
                Max = max(QRSloc,idxWindow+1);
                Pos = Max_Pos(QRSloc,idxWindow+1);
                delete[] QRSloc;

                real_loc = i-0.5*idxWindow+Pos-1;
                if((real_loc - Tminus) >(0.2*fs))
                {
                    RpeakIndex[Rpeakcnt]=real_loc;

                    Tminus = i;
                    Modifiedupdate_array(threshArray, _ECGs,7, i);
                    threshAverage = threshFrac * (sum(threshArray,7) - min(threshArray,7) - max(threshArray,7)) / 5;

                    if(Rpeakcnt >1)
                    {
                        for(j=0;j<4;j++)
                        {
                            last5interval[j] = last5interval[j+1];
                        }
                        last5interval[4] = max(0.2*fs,RpeakIndex[Rpeakcnt] - RpeakIndex[Rpeakcnt-1]);
                        last5interval_average = sum(last5interval,5)/5;
                    }

                    Rpeakcnt = Rpeakcnt + 1;
                    i = ceil( Tminus + fs * 0.2 );
                }

            }
            else if((Rpeakcnt > 1) && ( (i - Tminus) > 1.6 * last5interval_average ))  //*
            {
                secondary_threshold = threshAverage * 0.3;
                iter = RpeakIndex[Rpeakcnt - 1];
                 while( iter < i )
                 {
                     if((_ECGs[iter] > secondary_threshold )&& (( iter - Tminus ) > 0.2 * fs) )
                     {
                         QRSloc = _abs(ECG,iter);
                         Max = max(QRSloc,idxWindow+1);
                         Pos = Max_Pos(QRSloc,idxWindow+1);
                         delete[] QRSloc;

                         real_loc = iter-0.5*idxWindow+Pos-1;

                         if((real_loc - Tminus) >(0.2*fs))   //*
                         {
                             RpeakIndex[Rpeakcnt]=real_loc;

                             Tminus = iter;
                             Modifiedupdate_array(threshArray, _ECGs,7, iter);
                             threshAverage = threshFrac * (sum(threshArray,7) - min(threshArray,7) - max(threshArray,7)) / 5;

                             if(Rpeakcnt >1)
                             {
                                 for(j=0;j<4;j++)
                                 {
                                     last5interval[j] = last5interval[j+1];
                                 }
                                 last5interval[4] = RpeakIndex[Rpeakcnt] - RpeakIndex[Rpeakcnt-1];
                                 last5interval_average = sum(last5interval,5)/5;
                             }

                             Rpeakcnt = Rpeakcnt + 1;
                             break;
                         }
                         else
                         {
                             Tminus = Tminus + 1.6 * last5interval_average - 0.2 * fs;
                         }

                         i = ceil( Tminus + 0.2 * fs );

                     }
                     iter = iter + 1;
                 }
            }

        }

        i =i+1;
    }


    delete[] _ECGs;

//    return RpeakIndex;
}
//2016.1.22  fininshed

double* TWavdetection(double* ECG,double *QRSs)
{

    double* TRegions = new double[NumPoint];

    int k;
    for(k=0;k<NumPoint;k++)
    {
        TRegions[k] = ECG[k];
    }

    double* TFlags = new double[NumPoint];

    for(k=0;k<NumPoint;k++)
    {
        TFlags[k] = 0;
    }

    double *TWavs;
    TWavs = new double[NumPoint];

    for(k=0;k<NumPoint;k++)
    {
        TWavs[k] = 0;
    }

    // detect QRS regions
//    double *QRSs;
//    QRSs = ModifiedQRSdetection(ECG);


    int *beat_times;
    int length = 0;
    beat_times = beattimes(QRSs,&length);
    int i;

    for(k=0;k<length;k++)
    {
        for(i=0;i<3*idxWindow/2;i++)
        {
            if ((beat_times[k]+(3*idxWindow)+i) < NumPoint)
            {
                TFlags[beat_times[k]+(3*idxWindow)+i+1] = 1;
            }
        }
    }

    // differential function

    double *_ECGs;
    _ECGs = differential(ECG);

    //remove QRS regions

    for (i=0;i<NumPoint;i++)
    {
        if (TFlags[i] == 0)
        {
            _ECGs[i] = 0;
            TRegions[i] = 0;

        }
    }

    //Squaring step
    int j;
    for(j=0;j<NumPoint;j++)
    {
        _ECGs[j] = _ECGs[j] * _ECGs[j];
    }

    //Moving window interation
    _ECGs = mwintegration(_ECGs);

    //decision rules
    double Tminus = -(0.1 * fs);
    double threshFrac = 0.3;
    double initMax = InitialMax(_ECGs);

    double threshArray[7];

    for(j=0;j<7;j++)
    {
        threshArray[j] = initMax;
    }

    double threshAverage = threshFrac*(initMax*7)/7;

    double *TWavloc;

    i = 0;
    double Max;
    int Pos;
    for(i=0;i<NumPoint;i++)
    {
        if ( (i>beat_times[0]) && (i< (NumPoint - idxWindow)) )
        {
            if ( ( _ECGs[i] > threshAverage ) && ( (i - Tminus) > (0.6 * fs) ) )
            {
                TWavloc = _abs(TRegions,i+1);
                Max = max(TWavloc,idxWindow+1);
                Pos = Max_Pos(TWavloc,idxWindow+1);
                delete[] TWavloc;

                TWavs[i-idxWindow/2+1+Pos] = TRegions[i-idxWindow/2+1+Pos];
                Tminus = i;
                update_array(threshArray, _ECGs, i);
                threshAverage = threshFrac * (sum(threshArray,7) - (min(threshArray,7) + max(threshArray,7))) / 5;
            }
        }
    }

    delete[] _ECGs;
    delete[] beat_times;
    delete[] TRegions;
    delete[] TFlags;

    return TWavs;
}

double* PWavdetection(double* ECG, double *QRSs)
{

    double* PRegions = new double[NumPoint];

    int k;
    for(k=0;k<NumPoint;k++)
    {
        PRegions[k] = ECG[k];
    }

    double* PFlags = new double[NumPoint];

    for(k=0;k<NumPoint;k++)
    {
        PFlags[k] = 0;
    }

    double *PWavs;
    PWavs = new double[NumPoint];

    for(k=0;k<NumPoint;k++)
    {
        PWavs[k] = 0;
    }

    // detect QRS regions
//    double *QRSs;
//    QRSs = ModifiedQRSdetection(ECG);

    int *beat_times;
    int length = 0;
    beat_times = beattimes(QRSs,&length);
    int i;

    for(k=1;k<length;k++)  //old is 0
    {
        for(i=0;i<3*idxWindow/2;i++)
        {
            int Ppos = beat_times[k]-(5*idxWindow/2)+i+1;
            PFlags[Ppos] = 1;  //error
        }
    }

    // differential function

    double *_ECGs;
    _ECGs = differential(ECG);

    //remove QRS regions

    for (i=0;i<NumPoint;i++)
    {
        if (PFlags[i] == 0)
        {
            _ECGs[i] = 0;
            PRegions[i] = 0;

        }
    }

    //Squaring step
    int j;
    for(j=0;j<NumPoint;j++)
    {
        _ECGs[j] = _ECGs[j] * _ECGs[j];
    }

    //Moving window interation
    _ECGs = mwintegration(_ECGs);

    //decision rules
    double Tminus = -(0.1 * fs);
    double threshFrac = 0.1;
    double initMax = InitialMax(_ECGs);

    double threshArray[7];

    for(j=0;j<7;j++)
    {
        threshArray[j] = initMax;
    }

    double threshAverage = threshFrac*(initMax*7)/7;

    double *PWavloc;

    i = 0;
    double Max ;
    int Pos;

    for(i=0;i<NumPoint;i++)
    {
        if ( (i>idxWindow) && (i< (NumPoint - idxWindow)) )
        {
            if ( ( _ECGs[i] > threshAverage ) && ( (i - Tminus) > (0.6 * fs) ) )
            {
                PWavloc = _abs(PRegions,i);
                Max = max(PWavloc,idxWindow+1);
                Pos = Max_Pos(PWavloc,idxWindow+1);
                delete[] PWavloc;

                PWavs[i-idxWindow/2+ Pos] = PRegions[i-idxWindow/2+Pos];
                Tminus = i;
                update_array(threshArray, _ECGs, i);
                threshAverage = threshFrac * (sum(threshArray,7) - (min(threshArray,7) + max(threshArray,7))) / 5;
            }
        }
    }

    delete[] beat_times;
    delete[] _ECGs;
    delete[] PRegions;
    delete[] PFlags;

    return PWavs;
}
