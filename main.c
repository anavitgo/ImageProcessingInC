#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "utils.h"



int main(){


    FILE *inputFile = NULL, *outputFile = NULL;
    char *filePath = NULL;
    BMP header;

    filePath = readline(stdin);
    double start = omp_get_wtime();
    inputFile = fopen(filePath, "rb");

    if(inputFile == NULL){
        printf("File does not exist.\n");
        exit(1);
    }

    if(strcmp(strrchr(filePath, '.'),".bmp") != 0){
        printf("Wrong BMP file format.\n");
        exit(1);
    }

    header.signature = (char *)malloc(2 * sizeof(char));
    fread(header.signature , sizeof(char), 2, inputFile);
    if(strstr(header.signature, "BM") == NULL){ 
        printf("Current file is not a BMP file.\n");
        exit(1);
    }

    header = readHeader(inputFile);
    printImageInfo(header);
    outputFile = fopen("out.bmp", "wb");
    header = readColorTable(inputFile, header);
    #ifdef USE_OMP
        parallelBlurImage(header);
    #else
        sequentialBlurImage(header);
    #endif
    writeHeader(outputFile, header);
    writeColorTable(header, outputFile);


    fclose(inputFile);
    fclose(outputFile);
    free(filePath);

    double end = omp_get_wtime();
	printf("TIME TAKEN: %lfs\n", end - start);
    return 0;
}