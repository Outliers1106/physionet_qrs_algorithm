#include <stdio.h>
#include "qrs_func_assembly.h"
#include <stdlib.h>

//输入格式
//wqrs -r 100 -f 0:0 -t 10:0 -s 0
//rdann -a wqrs -r 100

extern "C" __declspec(dllexport) int* wqrs_func(int argc1,char** argv1,int argc2,char** argv2){
	wqrs(argc1,argv1);
	printf("\n finish wqrs");
	int* r_inds=rdann(argc2,argv2);
	printf("\n finish rdann");
	return r_inds;
}

extern "C" __declspec(dllexport) int* sqrs_func(int argc1, char** argv1, int argc2, char** argv2) {
	sqrs(argc1, argv1);
	printf("\n finish sqrs");
	int* r_inds = rdann(argc2, argv2);
	printf("\n finish rdann");
	return r_inds;
}

extern "C" __declspec(dllexport) int* sqrs125_func(int argc1, char** argv1, int argc2, char** argv2) {
	sqrs125(argc1, argv1);
	printf("\n finish sqrs125");
	int* r_inds = rdann(argc2, argv2);
	printf("\n finish rdann");
	return r_inds;
}

extern "C" __declspec(dllexport) void EcgAnalysis_func(char* filepathChars,int sampfrom,int sampto,bool flag) {
	//这里的filepath用ecg数据名即可
	string filepath = filepathChars;
	string output_dir = "mit-bih-arrhythmia-database-1.0.0";
	EcgAnalysis(filepath,output_dir,sampfrom,sampto);
	if(!flag)
		system("get_hrv.sh -R mit-bih-arrhythmia-database-1.0.0/RRIntervals.txt");
}

/*
__declspec(dllexport) int wqrs_func(int a,int b) {
	
	printf("\n %d", argc1);
	printf("\n argv1: %s", argv1[0]);
	wqrs(argc1, argv1);
	printf("\n finish wqrs");
	int* r_inds=rdann(argc2,argv2);
	printf("\n finish rdann");
	return r_inds;
	
	return a + b;
}
*/
/*
extern "C" __declspec(dllexport) int wqrs_func(int params) {
	printf("%d", params);
	char* argv1[] = {"wqrs","-r","100","-f","0:0","-t","5:9.4","-s","0"};
	char* argv2[] = { "rdann","-a","wqrs","-r","100" };
	//wqrs(9, argv1);
	printf("\n finish wqrs");
	//rdann(5, argv2);
	printf("\n finish rdann");
	return 1000;
}
*/
/*
int main(){
	char* wqrs_argv[]={"wqrs","-r","100"};
	char* rdann_argv[]={"rdann", "-a", "wqrs", "-r", "100"};
	int* r_inds;
	r_inds=wqrs_func(3,wqrs_argv,5,rdann_argv);
	int i=1;
	int num=r_inds[0];
	for(i=1;i<=num;i++){
		printf("%d\n",r_inds[i]);
	}
	return 0;
}
*/