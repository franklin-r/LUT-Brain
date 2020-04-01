/*
 * Image.cpp
 *
 *  Created on: Jul 15, 2013
 *      Author: jpdavid
 */

#include "Image.h"

int start = 1;  // Not really used, just an artefact to move to hardware ASM
int done;       // Not really used, just an artefact to move to hardware ASM

Image::Image() {
	// TODO Auto-generated constructor stub
	source_array = 0;
	length = 0;
	height = 0;
}

Image::Image(int new_length, int new_height, bool isInput) {
	// TODO Auto-generated constructor stub
	source_array = 0;
	init(new_length, new_height, isInput);
}

void Image::init(int new_length, int new_height, bool isInput) {
	// TODO Auto-generated constructor stub
	length = new_length;
	height = new_height;
	if (source_array != 0) delete[] source_array;
	source_array = (isInput) ? (new unsigned char[height*length] BINARY_IMAGE) : (new unsigned char[height*length]);
}

unsigned char * Image::source_pixel(int x, int y) {
	// TODO Auto-generated constructor stub
	return source_array + (y*length+x);
}

void Image::copy_block(int x, int y, int size, float* target) {
	// TODO Auto-generated constructor stub
	for (int j = 0; j < size; j++) {
		for (int i = 0; i < size; i++) {
			target[j * size + i] = (*source_pixel(x + i, y + j)) / 255.0;
		}
	}
}


// Copy Block ASM functions by Daniel Dermont 2043595 //
void Image::copy_block_hard(int in_x, int in_y, int input_size, float* input_target) {
	int x; int y; int size; float* target;
	int address, data, read, write, waitrequest = 0;
	int i; int j;
	float* MAR;


	INIT:		if (start == 0) { goto INIT; }
			else { done = 0; address = 0; data = 0; read = 0; write = 0; target = input_target; size = input_size; x = in_x; y = in_y; i = 0; j = 0; goto J1; }

	J1:		if (j < size) { goto I1; }
			else { done = 1; }

	I1:		if (i < size) { MAR = target + (j * size + i); goto I2; }
			else { j++; goto J1; }

	I2:		if (1) {address = source_array + ((y+j)*length+(x+i)), read = 1; goto RREQ;}
	
	I3:		if (1) { address = MAR; data = (*(source_array + ((y+j)*length+(x+i))) / 255.0);  write = 1; goto WREQ; }

	RREQ:		if (waitrequest == 1) {goto RREQ;}
			else {/* Read pos in target */ read = 0; goto I3;}

	WREQ:		if(waitrequest == 1) {goto WREQ;}
			else {/* Write to new Image */ write = 0; i++; goto I1;}
}

// Copy Block ASM functions by Daniel Dermont 2043595 //
/* void Image::copy_block_hard_opt(int in_x, int in_y, int input_size, float* input_target) {
	int x; int y; int size; float* target;
	int i; int j;
	float* MAR;

	INIT:	if (start == 0) { goto INIT; }
			else { done = 0; target = input_target; size = input_size; x = in_x; y = in_y; i = 0; j = 0; MAR = target; goto J1; }

	J1:		if (j < size) { goto I1; }
			else { done = 1; }

// CANT DO A READ AND A WRITE IN THE SAME STATE
	I1:		if (i < size) { *MAR = (*source_pixel(x + i, y + j)) / 255.0; MAR = target + (j * size + i); i++; goto I1; }
			else { j++; goto J1; }
}
*/
/*******************************************************
 * Application d'un reseau de neuronnes a cette image.
 *
 * Applique toutes les sous-images size x size de cette
 * image a l'entree de ce reseau de neuronnes.
 *
 *******************************************************/
Image * Image::apply_NN(NN * network, int size, int pos) {
	//float source[size*size];
	float * source = new float[size*size];
	Image * result = new Image(length-size+1,height-size+1, false);

	printf("Processing line ");
	for (int y=0; y<=height-size; y++) {
		printf("%i,",y);
		for (int x=0; x<=length-size; x++) {
			/* Appliquer le reseau sur un sous-bloc de l'image */
			copy_block(x, y, size, source);
			network->propagate(source);

			/* Stocker les bons/meilleurs matchs */
			unsigned char pixel;
			pixel = 255*(network->layer[network->n_layer-1].value[pos]);
			*(result->source_pixel(x,y)) = pixel;
		}
	}

	delete source;

	printf("\r\n");
	return result;
}

/**********************************************************
 * Sauvegarde de l'image dans un fichier csv.
 *
 **********************************************************/

void Image::printToFile(int x, int y, std::string file_name) {
	
	std::ofstream outputFile;
	outputFile.open(file_name + ".csv");

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < length; i++) {
			outputFile << (unsigned int)(*source_pixel(i, j)) << ", ";
		}
		outputFile << "\n";
	}
	
	outputFile.close();
}

/**********************************************************
 * Affiche l'image a l'ecran a la position x,y.
 *
 **********************************************************/
void Image::print() {
	printf("\n");
	for(int i=0; i<length; i++) {
		for(int j=0; j<height; j++) {
			int value = (int) *source_pixel(i,j);
			printf("%i,",value);
		}
		printf("\n");
	}
}

Image::~Image() {
	if (source_array != 0) delete[] source_array;
}

