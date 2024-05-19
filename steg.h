#ifndef stego_h 
#define stego_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

typedef struct {
    char *msg;
    size_t len;
} Message;

char *readText(const char *file_path);
void writeText(const char *filename, const char *content);
void readPng(char *file_name, png_structp png, png_infop info, png_bytep **row_pointers);
void writePng(char *file_name, png_structp png, png_infop info,png_bytep *row_pointers);
void encodeMsg(png_bytep *byte, char *message, size_t message_length, size_t width, size_t heigth);
Message decodeMsg(png_bytep *byte, char *message, size_t width, size_t height);
		
#endif