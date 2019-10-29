#include "EcgAnalysis.h"
#include <list>
#include <time.h>
#include <fstream>
#include <string>
#include "wfdb.h"
#include "stringutils.h"
#include "Calcu_Parm.h"
#include "Ecg_Asist_Function.h"

using namespace std;
EcgAnalysis::EcgAnalysis(string filepath, string output_dir)
{
   //--------------------------------Reading----------------------------------//
    printf("\nEcg signals reading begin....\n");
    int nsig;

    char* filepathChars = stringToChars(filepath);
	if ((nsig = isigopen(filepathChars, NULL, 0)) < 1) return;
	WFDB_Siginfo* siginfo = (WFDB_Siginfo*)malloc(sizeof(WFDB_Siginfo)*nsig);
	WFDB_Sample* sample = (WFDB_Sample*)malloc(sizeof(WFDB_Sample)*nsig);

    // init global variables
    fs = (int)sampfreq(filepathChars);
    window = fs/10;
    idxWindow = window;

    if(isigopen(filepathChars, siginfo, nsig) < nsig){
        printf("Error when open signal file: %s\n", filepath.c_str());
        delete[] filepathChars;
        return;
    }
	printf("\n isigopne done \n");
    delete[] filepathChars;

    NumPoint = siginfo[0].nsamp;
	delete[] siginfo;
    // if the record is too short
    // just return
    if(NumPoint < fs * minDuration){
        printf("The record is too short.\n");
        return;
    }


    ECGArrayLead1 = new double[NumPoint];
    ECGArrayLead2 = new double[NumPoint];

    for(int i= 0; i<NumPoint; i++){
        if(getvec(sample)<0){
            break;
        }

        if(i >= 0){
            ECGArrayLead1[i] = (double)sample[0];
        }
        //printf("%d %lf",i, ECGArrayLead1[i]);
    }

   //--------------------------------Denoising----------------------------------//
   printf("Ecg signals denosing begin....\n");
   double time_Start_Denoising = (double)clock();
   int NP = NumPoint;
   int i;

   double* z = new double[NP];

   double a[13]= {1.0,-2.0,1.0,0,0,0,0,0,0,0,0,0,0};
   double b[13]= {1.0,0,0,0,0,0,-2.0,0,0,0,0,0,1.0};

   double d[33]= {1.0,-1.0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   double c[33]= {-1.0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32.0,-32.0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1.0};

   filter(13,a,b,NP,ECGArrayLead1,z);

   for(i=0;i<NP;i++)
   {
      z[i] = z[i]/24.0;
   }
   filter(33,d,c,NP,z,ECGArrayLead1);

    for(i=0;i<NP;i++)
    {
       ECGArrayLead1[i] = ECGArrayLead1[i]/24.0;
    }

    for(i=0;i<NP-21;i++)
    {
       ECGArrayLead1[i] = ECGArrayLead1[i+21];
    }
    for(i=NP-21;i<NP;i++)
    {
       ECGArrayLead1[i] = 0;
    }

    double time_End_Denoising = (double)clock();

    delete[] z;

	printf("Ecg signals denosing finished!\n");

	printf("Denosing cost time：%lfs\n", (time_End_Denoising - time_Start_Denoising)/1000.0);

//--------------------------------Detecting----------------------------------//

     ofstream out;
     int pos;

     printf("Ecg signals detecting begin....\n");
     double time_Start_Detecting = (double)clock();

     //2016.1.25

     QRSs = new double[NumPoint];
     printf("Before QRS detection....\n");
     ModifiedQRSdetection(ECGArrayLead1,QRSs);
//     QRSs = ModifiedQRSdetection(ECGArrayLead1);

     printf("Rwave detecting finished!\n");

     /***************************************************/

     //TWavdetection
     TWavs = TWavdetection(ECGArrayLead1,QRSs);
     printf("Twave detecting finished!\n");

     //PWavdetection
     PWavs = PWavdetection(ECGArrayLead1,QRSs);
     printf("Pwave detecting finished!\n");

    /***************Intervals*********************/


     QRSWavCnt = 0;
     PWavCnt = 0;
     TWavCnt = 0;

     QRSbt = beattimes( QRSs, &QRSWavCnt );
     Pbt = Modifiedbeattimes( PWavs, &PWavCnt );//beattimes
     Tbt = Modifiedbeattimes( TWavs, &TWavCnt );//beattimes

     printf("QRSWavCnt = %d\n", QRSWavCnt);
     printf("PWavCnt = %d\n", PWavCnt);
     printf("TWavCnt = %d\n", TWavCnt);

    if(QRSbt == NULL) {
        printf("QRSbt is NULL\n");
        return;
    }

    if (QRSWavCnt > 0){
        QRSIntervals = RRInterval( QRSbt, QRSWavCnt, fs);
        QQIntervals = QQInterval(QRSbt,QRSWavCnt,fs);
        Ramp = new double[QRSWavCnt];
    }

    if (PWavCnt > 0){
        PPIntervals = PPInterval(Pbt,PWavCnt,fs);
        Pamp = new double[PWavCnt];
    }

    if (TWavCnt > 0){
        TTIntervals = TTInterval(Tbt,TWavCnt,fs);
        Tamp = new double[TWavCnt];
    }

    if (QRSWavCnt > 0 && PWavCnt > 0){
        PRInterval = PRIntervals( QRSbt, Pbt, QRSWavCnt, PWavCnt, fs );
    }

    if (QRSWavCnt > 0 && TWavCnt > 0){
        QTInterval = QTIntervals( QRSbt, Tbt, QRSWavCnt, TWavCnt, fs );
    }
    printf("Interval calculating finished!\n");

    // write analysis results to files
    printf("Writing analysis results begins...\n");
    const char* ramp = stringToChars(output_dir + "/Ramp.txt");
    printf("Ramp path: %s\n", ramp);
    out.open(ramp, std::ofstream::out);
    delete[] ramp;

    for(i = 0; i < QRSWavCnt; i++){
        pos = QRSbt[i];
        Ramp[i] = ECGArrayLead1[pos];
        out << i << " " <<Ramp[i] << " "  << "  Pos:" << pos <<"\n";
    }
    out.clear();
    out.close();

    const char* pamp = stringToChars(output_dir + "/Pamp.txt");
    out.open(pamp, std::ofstream::out);
    delete[] pamp;

    for(i = 0; i < PWavCnt; i++){
        pos = Pbt[i];
        Pamp[i] = ECGArrayLead1[pos];
        out << i << " " <<Pamp[i] << " " << "  Pos:" << pos <<"\n";
    }
    out.clear();
    out.close();

    const char* tamp = stringToChars(output_dir + "/Tamp.txt");
    out.open(tamp, std::ofstream::out);
    delete[] tamp;

    for(i = 0; i < TWavCnt; i++){
        pos = Tbt[i];
        Tamp[i] = ECGArrayLead1[pos];
        out << i << " " <<Tamp[i] << " " << "  Pos:" << pos <<"\n";
    }
    out.clear();
    out.close();

    const char* rrintervals = stringToChars(output_dir + "/RRIntervals.txt");
    out.open(rrintervals, std::ofstream::out);
    delete[] rrintervals;
     double time = 0;
     for(i = 0; i < QRSWavCnt-1; i++){
         time += QRSIntervals[i];
         out << time << " " <<QRSIntervals[i] << " " << "N" << "\n";
     }
     out.clear();
     out.close();

    const char* printervals = stringToChars(output_dir + "/PRIntervals.txt");
    out.open(printervals, std::ofstream::out);
    delete[] printervals;
     for(i = 0; i < PWavCnt; i++){
         out << i << " " <<PRInterval[i] << " " << "\n";
     }
     out.clear();
     out.close();

     const char* qtintervals = stringToChars(output_dir + "/QTIntervals.txt");
     out.open(qtintervals, std::ofstream::out);
    delete[] qtintervals;
     for(i = 0; i < TWavCnt; i++){
         out << i << " " <<QTInterval[i] << " " << "\n";
     }
     out.clear();
     out.close();

     const char* qqintervals = stringToChars(output_dir + "/QQIntervals.txt");
     out.open(qqintervals, std::ofstream::out);
    delete[] qqintervals;
     for(i = 0; i < QRSWavCnt-1; i++){
         out << i << " " <<QQIntervals[i] << " " << "\n";
     }
     out.clear();
     out.close();

     const char* ppintervals = stringToChars(output_dir + "/PPIntervals.txt");
     out.open(ppintervals, std::ofstream::out);
    delete[] ppintervals;
     for(i = 0; i < PWavCnt-1; i++){
         out << i << " " <<PPIntervals[i] << " " << "\n";
     }
     out.clear();
     out.close();

     const char* ttintervals = stringToChars(output_dir + "/TTIntervals.txt");
     out.open(ttintervals, std::ofstream::out);
    delete[] ttintervals;
     for(i = 0; i < TWavCnt-1; i++){
         out << i << " " <<TTIntervals[i] << " " << "\n";
     }
     out.clear();
     out.close();

     const char* qrsbt = stringToChars(output_dir + "/QRSbt.txt");
     out.open(qrsbt, std::ofstream::out);
    delete[] qrsbt;
     for(i = 0; i < QRSWavCnt; i++){
         out << i << " " <<QRSbt[i] << " " << "\n";
     }
     out.clear();
     out.close();

 //2015.8.20


 //intervals
     double RRIntervals = sum(QRSIntervals,QRSWavCnt-1)/(QRSWavCnt-1);


     double PRIntervals = sum(PRInterval,PWavCnt)/(PWavCnt);


     double QTIntervals = sum(QTInterval,TWavCnt)/(TWavCnt);


 //rate
     double qqIntervals = sum(QQIntervals,QRSWavCnt-1)/(QRSWavCnt-1);


     double ppIntervals = sum(PPIntervals,PWavCnt)/(PWavCnt);


     double ttIntervals = sum(TTIntervals,TWavCnt)/(TWavCnt);
 //    ui->label_TT->setText(QString::number(60 / ttIntervals,'f',0)+"次/分");

     double ramp1 = sum(Ramp,QRSWavCnt)/(QRSWavCnt);
     double pamp1 = sum(Pamp,PWavCnt)/(PWavCnt);
     double tamp1 = sum(Tamp,TWavCnt)/(TWavCnt);



     const char* signals = stringToChars(output_dir + "/Signals.txt");
     out.open(signals, std::ofstream::out);

       out  << "RRIntervals: " <<RRIntervals << " s" << "\n";
       out  << "PRIntervals: " <<PRIntervals << " s" << "\n";
       out  << "QTIntervals: " <<QTIntervals << " s" << "\n";
       out  << "QQIntervals: " <<qqIntervals << " s" << "\n";
       out  << "PPIntervals: " <<ppIntervals << " s" << "\n";
       out  << "TTIntervals: " <<ttIntervals << " s" << "\n";
       out  << "Ramp:        " <<ramp1<< " mV" << "\n";
       out  << "Pamp:        " <<pamp1<< " mV" << "\n";
       out  << "Tamp:        " <<tamp1<< " mV" << "\n";

       out.clear();
       out.close();

     double time_End_Detecting = (double)clock();

     printf("Ecg signals detecting finished!\n");

     printf("Detecting cost time：%lfs\n", (time_End_Detecting - time_Start_Detecting)/1000.0);

	 
}

void filter(int ord, double *a, double *b, int np, double *x, double *y)
{

    int i,j;
    y[0]=b[0]*x[0];

    for (i=1;i<ord;i++)
    {
        y[i]=0.0;
        for (j=0;j<=i;j++)
        {
            y[i]=y[i]+b[j]*x[i-j];
        }
        for (j=1;j<=i;j++)
        {
            y[i]=y[i]-a[j]*y[i-j];
        }
    }
/* end of initial part */
    for (i=ord;i<np;i++)
    {
        y[i]=0.0;
        for (j=0;j<ord;j++)
        {
            y[i]=y[i]+b[j]*x[i-j];
        }
        for (j=1;j<ord;j++)
        {
            y[i]=y[i]-a[j]*y[i-j];
        }

    }

}

EcgAnalysis::~EcgAnalysis(){
	
    delete[] ECGArrayLead1;
    delete[] ECGArrayLead2;

    printf("~EcgAnalysis #1\n");

    delete[] QRSs;
    delete[] TWavs;
    delete[] PWavs;
    printf("~EcgAnalysis #2\n");
    delete[] Ramp;
    delete[] Pamp;
    delete[] Tamp;
    printf("~EcgAnalysis #3\n");
    delete[] QRSbt;
    delete[] Pbt;                   //beatimes of PWavs
    delete[] Tbt;                   //beatimes of TWavs
    printf("~EcgAnalysis #4\n");
    delete[] QRSIntervals;       //Intervals of RR
    delete[] PRInterval;         //Intervals of PR
    delete[] QTInterval;         //Intervals of QT
    printf("~EcgAnalysis #5\n");
    delete[] QQIntervals;        //Intervals of QQ
    delete[] PPIntervals;        //Intervals of PP
    delete[] TTIntervals;        //Intervals of TT
	
}


