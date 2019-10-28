#ifndef RDANN_H
#define RDANN_H
typedef struct node{
	int index;
	int data;
	struct node* next;
}node;
#ifdef __cplusplus
extern "C"
{
#endif
	int* rdann(int argc, char** argv);
	char* prog_name_rdann(char* s);
	void help_rdann();
#ifdef __cplusplus
}
#endif
#endif