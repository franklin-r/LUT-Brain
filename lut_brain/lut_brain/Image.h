/*
 * Image.h
 *
 *  Created on: Jul 15, 2013
 *      Author: jpdavid
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include "NN.h"
//#include <io.h>
#include "stdlib.h"
#include "stdio.h"
#include "data.h"
#include <iostream>
#include <fstream>

void copy_block_optimise_mod(int x, int y, int size, float* target);

class Image {
public:
	Image();
	Image(int new_length, int new_height, bool isInput);
	void init(int new_length, int new_height, bool isInput);

	unsigned char * source_pixel(int x, int y);
	void copy_block(int x, int y, int size, float * target);
	void copy_block_optimise(int x, int y, int size, float* target);
	Image ** apply_NN(NN * network, int size);
	void printToFile(int x, int y, std::string file_name);
	void print();
	~Image();

	int height;
	int length;
	unsigned char *source_array;
};

#endif /* IMAGE_H_ */
