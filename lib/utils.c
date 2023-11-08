#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

char *readline(FILE *stream){
  char *string = NULL;
  int i = 0;

  do{
    string = (char *)realloc(string, ++i * sizeof(char));
    string[i - 1] = fgetc(stream); 
  }while(string[i - 1] != '\r' && string[i - 1] != '\n' && !feof(stream)); 

  string[i - 1] = '\0';

  return string;
}

BMP readHeader(FILE *inputFile){
  BMP header;
  fread(&header.fileSize, sizeof(unsigned int), 1, inputFile);
  fread(&header.reserved, sizeof(int), 1, inputFile);
  fread(&header.dataOffset, sizeof(int), 1, inputFile);
  fread(&header.headerSize, sizeof(int), 1, inputFile);
  fread(&header.width, sizeof(int), 1, inputFile);
  fread(&header.height, sizeof(int), 1, inputFile);
  fread(&header.planes, sizeof(short), 1, inputFile);
  fread(&header.bitCount, sizeof(short), 1, inputFile);
  fread(&header.compression, sizeof(int), 1, inputFile);
  fread(&header.imageSize, sizeof(int), 1, inputFile);
  fread(&header.xPixel, sizeof(int), 1, inputFile);
  fread(&header.yPixel, sizeof(int), 1, inputFile);
  fread(&header.colorsUsed, sizeof(int), 1, inputFile);
  fread(&header.colorsImportant, 1, sizeof(int), inputFile);

  return header;
}

void writeHeader(FILE *outputFile, BMP header){ 
  fwrite("BM", sizeof(char), 2, outputFile);
  fwrite(&header.fileSize, sizeof(unsigned int), 1, outputFile);
  fwrite(&header.reserved, sizeof(unsigned int), 1, outputFile);
  fwrite(&header.dataOffset, sizeof(unsigned int), 1, outputFile);
  fwrite(&header.headerSize, sizeof(unsigned int), 1, outputFile);
  fwrite(&header.width, sizeof(unsigned int), 1, outputFile);
  fwrite(&header.height, sizeof(unsigned int), 1, outputFile);
  fwrite(&header.planes, sizeof(short), 1, outputFile);
  fwrite(&header.bitCount, sizeof(short), 1, outputFile);
  fwrite(&header.compression, sizeof(unsigned int), 1, outputFile);
  fwrite(&header.imageSize, sizeof(unsigned int), 1, outputFile);
  fwrite(&header.xPixel, sizeof(unsigned int), 1, outputFile);
  fwrite(&header.yPixel, sizeof(unsigned int), 1, outputFile);
  fwrite(&header.colorsUsed, sizeof(unsigned int), 1, outputFile);
  fwrite(&header.colorsImportant, 1, sizeof(unsigned int), outputFile);
}

BMP readColorTable(FILE *inputFile, BMP header){ 
  int imgSize = header.height * header.width;
  header.colorTable = (COLORS *)malloc(imgSize * sizeof(COLORS));

  for(int i = 0; i < imgSize; i++){
    fread(&header.colorTable[i].blue, sizeof(unsigned char), 1, inputFile);
    fread(&header.colorTable[i].green, sizeof(unsigned char), 1, inputFile);
    fread(&header.colorTable[i].red, sizeof(unsigned char), 1, inputFile);
    fread(&header.colorTable[i].reserved, sizeof(unsigned char), 1, inputFile);
  }

  return header;
}

void blurImage(BMP header){

    float v = 1.0 / 9.0;
	float kernel[3][3]= {{v, v, v},
						{v, v, v},
						{v, v, v}};

	for(int x = 1; x < header.height - 1; x++) {					
		for(int y = 1; y < header.width - 1; y++) {
			float sumRed = 0.0;
			float sumGreen = 0.0;
			float sumBlue = 0.0;
			for(int i = -1; i <= 1; ++i) {
				for(int j = -1; j <= 1; ++j) {	
					sumRed = sumRed + (float)kernel[i + 1][j + 1] * header.colorTable[(x + i) * header.width + (y + j)].red;
					sumGreen = sumGreen + (float)kernel[i + 1][j + 1] * header.colorTable[(x + i) * header.width + (y + j)].green;
					sumBlue = sumBlue + (float)kernel[i + 1][j + 1] * header.colorTable[(x + i) * header.width + (y + j)].blue;
				}
			}
			header.colorTable[(x) * header.width + (y)].red = sumRed;
			header.colorTable[(x) * header.width + (y)].green = sumGreen;
			header.colorTable[(x) * header.width + (y)].blue = sumBlue;
		}
	}
}

void writeColorTable(BMP header, FILE *outputFile){
    int imgSize = header.height * header.width;
    for(int i = 0; i < imgSize; i++){
      fwrite(&header.colorTable[i].blue, sizeof(unsigned char), 1, outputFile);
      fwrite(&header.colorTable[i].green, sizeof(unsigned char), 1, outputFile);
      fwrite(&header.colorTable[i].red, sizeof(unsigned char), 1, outputFile);
      fwrite(&header.colorTable[i].reserved, sizeof(unsigned char), 1, outputFile);
    }
}

void printImageInfo(BMP header){ 
  printf("Header:\n");
  printf("File size: %d\n", header.imageSize);
  printf("Reserved: %d\n", header.reserved);
  printf("Data offset, in bytes, from header to data: %d\n", header.dataOffset);
  printf("Resolution: %d x %d\n", header.width, header.height);
  printf("Planes: %d\n", header.planes);
  printf("Bit per pixel: %d\n", header.bitCount);
  printf("Image size: %d\n", header.imageSize);
}

