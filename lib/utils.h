#ifndef UTILS_H
#define UTILS_H

typedef struct{ 
  unsigned char blue, green, red;
  unsigned char reserved;
} COLORS;

typedef struct bmp{
  char *signature;
  int fileSize, reserved, dataOffset;
  int headerSize, width, height;
  short planes, bitCount;
  int compression, imageSize, xPixel, yPixel;
  int colorsUsed, colorsImportant;
  COLORS *colorTable;
}BMP;

char *readline(FILE *stream);
BMP readHeader(FILE *inputFile);
void writeHeader(FILE *outputFile, BMP header);
BMP readColorTable(FILE *inputFile, BMP header);
void sequentialBlurImage(BMP header);
void parallelBlurImage(BMP header);
void writeColorTable(BMP header, FILE *outputFile);
void printImageInfo(BMP header);

#endif