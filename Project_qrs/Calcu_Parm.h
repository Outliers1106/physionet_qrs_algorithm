#ifndef CALCU_PARM_H
#define CALCU_PARM_H
// calcu six Intervals RR-PR-QT-QQ-PP-TT


//calcu RRIntervals
double* RRInterval( const int QRS_times[], const int RwaveCnt, const int Fs );

//calcu PRIntervals
double* PRIntervals( const int* QRS_times, const int* PWav_times, const int RwaveCnt, const int PwaveCnt, const int Fs );

//calcu QTIntervals
double* QTIntervals( const int* QRS_times, const int* TWav_times, const int RwaveCnt, const int TwaveCnt, const int Fs );

//calcu QQIntervals
double* QQInterval( const int QRS_times[], const int RwaveCnt, const int Fs );

//calcu PPIntervals
double* PPInterval( const int PWav_times[], const int PwaveCnt, const int Fs );

//calcu TTIntervals
double* TTInterval( const int TWav_times[], const int TwaveCnt, const int Fs );


#endif // CALCU_PARM_H
