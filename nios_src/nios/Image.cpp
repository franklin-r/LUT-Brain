/*
 * Image.cpp
 *
 *  Created on: Jul 15, 2013
 *      Author: jpdavid
 */

#include "Image.h"

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
	if ((source_array != 0) && (source_array != BINARY_IMAGE)) delete[] source_array;
	source_array = (isInput) ? (BINARY_IMAGE) : (new unsigned char[height*length]);
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

/*******************************************************
 * Application d'un reseau de neurones a cette image.
 *
 * Applique toutes les sous-images size x size de cette
 * image a l'entree de ce reseau de neuronnes.
 *
 *******************************************************/
Image ** Image::apply_NN_opt(NN * network, int size) {
	//float source[size*size];
	float * source = new float[size*size];
	//Image * result = new Image(length-size+1,height-size+1, false);
	Image **tab_result = (Image**)malloc(network->layer[network->n_layer-1].n_neuron * sizeof(Image*));

	for(int i = 0; i < network->layer[network->n_layer-1].n_neuron; i++) {
		tab_result[i] = new Image(length-size+1, height-size+1, false);
	}
	printf("Processing line ");
	for (int y=0; y<=height-size; y++) {
		printf("%i,",y);
		for (int x=0; x<=length-size; x++) {
			/* Appliquer le reseau sur un sous-bloc de l'image */

			ALT_CI_COPYBLOCK_0(0,size,(int)source);
			ALT_CI_COPYBLOCK_0(1,length,(int)source_array);
			ALT_CI_COPYBLOCK_0(2,x,y);

			//copy_block(x, y, size, source);

			network->propagate(source);

			/* Stocker les bons/meilleurs matchs */
			/*
			unsigned char pixel;
			pixel = 255*(network->layer[network->n_layer-1].value[pos]);
			*(result->source_pixel(x,y)) = pixel;
			*/
			unsigned char pixel;
			for(int k = 0; k < network->layer[network->n_layer-1].n_neuron; k++) {
				if (network->layer[network->n_layer-1].value[k] == 1) pixel = 1;
				else 												  pixel = 0;
				*(tab_result[k]->source_pixel(x,y)) = pixel;
			}
		}
	}

	delete source;

	printf("\r\n");
	return tab_result;
}

/**********************************************************
 * Sauvegarde de l'image dans un fichier csv.
 *
 **********************************************************/
void Image::printToFile(int x, int y, const char * file_name) {
/*
	char buffer[256]; snprintf(buffer, 256, "%s.csv", file_name);
	FILE *myFile = fopen(buffer, "wt");

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < length; i++) {
			fprintf(myFile, "%i, ", (unsigned int)(*source_pixel(i, j)));
		}
		fprintf(myFile, "\n");
	}
	fclose(myFile);
*/
	printf("printToFile removed for FPGA implementation\n");
}

/**********************************************************
 * Affiche l'image a l'ecran a la position x,y.
 *
 **********************************************************/
void Image::printToScreen(int x, int y, VGA *pVGA) {
	for(int i=0; i<length; i++) {
		for(int j=0; j<height; j++) {
			pVGA->Set_Pixel_Color(x+i,y+j,*source_pixel(i,j));
		}
	}
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
	if ((source_array != 0) && (source_array != BINARY_IMAGE)) delete[] source_array;
}
