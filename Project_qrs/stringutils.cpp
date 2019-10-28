//
// Created by liuyang on 2016/8/30.
//
#include "stringutils.h"
#include <string>

using namespace std;

char* stringToChars(string s){
    char* c;
    const int len = s.length();
    c =new char[len+1];
    strcpy(c,s.c_str());
    return c;
}

char* constcharsToChars(const char* cc){
    char* c = new char[strlen(cc)+1];
    strcpy(c,cc);
    return c;
}
