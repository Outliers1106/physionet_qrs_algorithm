/*******************************
Copyright:
Author:         潘佳斌 刘阳
Date:           2016-08-29
Description:    通过QRS、P、T的位置索引，计算RR、QT、PR间期
Include:        double* BeatTimes( const double ECGArray[],int& RwaveCnt, const int Cnt )
                double* RRInterval( const double QRS_times[], const int RwaveCnt, const int fs )
                double* QTIntervals( const double* QRS_times, const double* TWav_times,
                const int RwaveCnt, const int TwaveCnt, const int fs )
*******************************/

#include "Calcu_Parm.h"
#include <fstream>
using namespace std;

/******************************
Function:       RRInterval
Description:    计算RR间期
Calls:          NULL
Input:          QRS_times[]     R峰位置的索引序列
                RwaveCnt        R峰个数
                fs              采样频率
Output:         intervals.txt   RR间期时间序列
Return:         RR间期时间序列
Other:
******************************/
double* RRInterval( const int QRS_times[], const int RwaveCnt, const int Fs )
{
    if( RwaveCnt <= 0 || QRS_times ==  NULL )
    {
        printf("QRS_times is NULL.\n");
    }
    double* RRIntervals = new double[RwaveCnt - 1];
    for( int i = 0; i < RwaveCnt - 1; ++i )
    {
        RRIntervals[i] = (double)( QRS_times[i + 1] - QRS_times[ i ] ) / Fs;

    }
/******Test Begin******/
//    ofstream out;
//    out.open( "RRintervals.txt" );
//    for( int i = 0; i < RwaveCnt - 1; i++ )
//        out << i << ' ' << RRIntervals[i] << "\n";

//    out.close();
//    //cout << "RwaveCnt = " << RwaveCnt << endl;
/******Test Over******/
    return RRIntervals;
}

/* *****************************
Function:       PRIntervals
Description:    PR间期
Calls:          NULL
Input:          QRS_times       R峰时间序索引列
                PWav_times      P波时间索引序列
                RwaveCnt        R波个数
                PwaveCnt        P波个数
                fs              采样频率
Output:
Return:         PR间期时间序列数组指针
Other:
***************************** */
double* PRIntervals( const int* QRS_times, const int* PWav_times, const int RwaveCnt, const int PwaveCnt, const int Fs )
{
    int len = RwaveCnt < PwaveCnt? RwaveCnt: PwaveCnt;
    if( len <= 0 )
    {
//        cerr << "P_times is NULL";
        printf("P_times is NULL.\n");
        //exit(1);
    }

    double* PRIntervals = new double[len];

    int i = 0;
    int j = 0;

    while( j < len )
    {
        if( QRS_times[i] > PWav_times[j] )
        {
            PRIntervals[j] = (double)( QRS_times[i] - PWav_times[j] ) / Fs;
            j++;
        }
        else
            i++;
    }


//    ofstream out;
//    out.open( "PRintervals.txt" );
//    for( int i = 0; i < len; i++ )
//        out << i << ' ' <<  PRIntervals[i] << "\n";

//    out.close();
    return PRIntervals;
}
/******************************
Function:       QRIntervals
Description:    QR间期计算
Calls:          NULL
Input:          QRS_times       R峰位置索引序列
                TWav_times      T峰位置索引序列
                RwaveCnt        R峰个数
                TwaveCnt        T波个数
                FS              采样频率
Output:
Return:         QT间期时间序列数组指针
Other:
******************************/
double* QTIntervals( const int* QRS_times, const int* TWav_times, const int RwaveCnt, const int TwaveCnt, const int Fs )
{
    int len = RwaveCnt < TwaveCnt? RwaveCnt: TwaveCnt;
    if( len <= 0 )
    {
//        cerr << "T_times is NULL";
        printf("T_times is NULL.\n");
        //exit(1);
    }

    double* QTIntervals = new double[len];

    int i = 0;
    int j = 0;

    while( j < len )
    {
        if( TWav_times[i] > QRS_times[j]  )
        {
            QTIntervals[j] = (double)( TWav_times[i] - QRS_times[j] ) / Fs;
            j++;
        }
        else
            i++;
    }

//    ofstream out;
//    out.open( "QTintervals.txt" );
//    for( int i = 0; i < len ; i++ )
//        out << i << ' ' << QTIntervals[i] << "\n";

//    out.close();
    return QTIntervals;
}

//2015.8.20
double* QQInterval(const int QRS_times[], const int RwaveCnt, const int Fs )
{
    if( RwaveCnt <= 0 || QRS_times ==  NULL )
    {
        printf("QRS_times is NULL.\n");
    }
    double* QQIntervals = new double[RwaveCnt - 1];
    for( int i = 0; i < RwaveCnt - 1; ++i )
    {
        QQIntervals[i] = (double)( QRS_times[i + 1] - QRS_times[ i ] ) / Fs;
    }

    return QQIntervals;
}

double* PPInterval( const int PWav_times[], const int PwaveCnt, const int Fs )
{
    if( PwaveCnt <= 0 || PWav_times ==  NULL )
    {
        printf("PWav_times is NULL.\n");
    }
    double* PPIntervals = new double[PwaveCnt - 1];
    for( int i = 0; i < PwaveCnt - 1; ++i )
    {
        PPIntervals[i] = (double)( PWav_times[i + 1] - PWav_times[ i ] ) / Fs;
    }

    return PPIntervals;
}

double* TTInterval( const int TWav_times[], const int TwaveCnt, const int Fs )
{
    if( TwaveCnt <= 0 || TWav_times ==  NULL )
    {
        printf("TWav_times is NULL.\n");
    }
    double* TTIntervals = new double[TwaveCnt - 1];
    for( int i = 0; i < TwaveCnt - 1; ++i )
    {
        TTIntervals[i] = (double)( TWav_times[i + 1] - TWav_times[ i ] ) / Fs;
    }

    return TTIntervals;
}
