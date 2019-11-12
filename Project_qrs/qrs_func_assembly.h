#ifndef QRS_FUNC_ASSEMBLY
#define QRS_FUNC_ASSEMBLY
/*
extern "C" {
#include "wqrs.h"
#include "rdann.h"
}
//extern "C" __declspec(dllexport) int wqrs_func(int params);
extern "C" __declspec(dllexport) int* wqrs_func(int argc1,char** argv1,int argc2,char** argv2);
*/
#include "wqrs.h"
#include "rdann.h"
#include "sqrs.h"
#include "sqrs125.h"
#include "EcgAnalysis.h"
extern "C" __declspec(dllexport) int* wqrs_func(int argc1, char** argv1, int argc2, char** argv2);
extern "C" __declspec(dllexport) int* sqrs_func(int argc1, char** argv1, int argc2, char** argv2);
extern "C" __declspec(dllexport) int* sqrs125_func(int argc1, char** argv1, int argc2, char** argv2);
extern "C" __declspec(dllexport) void EcgAnalysis_func(char* filepathChars,int sampfrom,int sampto,bool flag);
#endif