// File Name: arrayPtr.c
// Author: kanon
// Created Time: 2020年02月24日 星期一 14时39分46秒

#include <stdio.h>
#include <unistd.h>

int main(int argc,char **argv) {

   char *array[10] = {NULL};
   array[0] = "wang";
   array[1] = "gu";
   for(int i=0;i<2;i++)
       printf("%s\n",array[i]);



    return 0 ;
}
