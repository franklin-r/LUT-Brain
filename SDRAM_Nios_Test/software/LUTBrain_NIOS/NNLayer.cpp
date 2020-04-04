/*
 * NNLayer.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: jpdavid
 */

#include "NNLayer.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "Image.h"

int my_debug = 0;

NNLayer::NNLayer() {
	// TODO Auto-generated constructor stub
	n_input = 0;
	n_neuron = 0;
	n_input_per_neuron = 0;
	LUT_size = 0;

	LUT_array = 0;
	pos_array = 0;
	value = 0;
}

NNLayer::NNLayer(int new_n_input, int new_n_neuron, int nipn, const unsigned char* LUT_data, const int* pos_data) {
	// TODO Auto-generated constructor stub
	init(new_n_input, new_n_neuron, nipn, LUT_data, pos_data);
}

void NNLayer::init(int new_n_input, int new_n_neuron, int nipn, const unsigned char* LUT_data, const int* pos_data) {
	// TODO Auto-generated constructor stub
	n_input = new_n_input;
	n_neuron = new_n_neuron;
	n_input_per_neuron = nipn;
	LUT_size = ((1<<n_input_per_neuron) + 7)/8;
	LUT_array = LUT_data;
	pos_array = pos_data;
	value = new float[n_neuron];
}

unsigned int simpleRand() {
	unsigned int result = rand();
	result <<= 16;
	result ^=rand();

	return result;
}

void NNLayer::load_values(int new_n_input, int new_n_neuron, int nipn, const unsigned char* LUT_data, const int* pos_data) {
	// TODO Auto-generated constructor stub
	init(new_n_input, new_n_neuron, nipn, LUT_data, pos_data);

	for (int i=0; i<n_neuron; i++) {
		value[i] = -1;
	}
}

NNLayer::~NNLayer() {
	// TODO Auto-generated destructor stub
	delete value;
}

void NNLayer::buildAddress(float* source, const int* current_pos, int* LUT_Address) {
	// Fonction � remplacer
	for (int i = 0; i < n_neuron; i++) {
		for (int j = 0; j < n_input_per_neuron; j++) {
			if (source[*(current_pos++)] != 0) LUT_Address[i] += (1 << j);
		}
	}
}

void NNLayer::lutForward(int* LUT_Address) {
	// Fonction � remplacer
	for (int i = 0; i < n_neuron; i++) {
		value[i] = 1 & (LUT_array[LUT_size * i + (LUT_Address[i] >> 3)] >> (LUT_Address[i] & 0x7));
		if (my_debug) printf("LUT%i[%i] = %f\r\n", i, LUT_Address[i], value[i]);
	}
}


float * NNLayer::propagate(float * source) {
	// TODO Auto-generated constructor stub
	int i;

	if (my_debug) for (i=0; i<n_input; i++) {
		printf("Input %i = %f\r\n", i, source[i]);
	}

	const int * current_pos = pos_array;
	int *LUT_Address = new int[n_neuron] { 0 };

	buildAddress(source, current_pos, LUT_Address);
	lutForward(LUT_Address);

	delete LUT_Address;
	if (my_debug) print();

	return value;
}

void NNLayer::print_activation() {
	printf("---------------\n");
	for (int i=0; i<n_neuron; i++) {
		printf("%i, %i\n", i, (int)value[i]);
	}
}

void NNLayer::print() {
	// TODO Auto-generated constructor stub
	const int * current_pos = pos_array;

	for (int i=0; i<n_neuron; i++) {
		printf("Neuron %i { ", i);
		for (int j=0; j<n_input_per_neuron; j++) {
			if (j!=0) printf(", %i", *(current_pos++));
			else printf("%i", *(current_pos++));
		}
		printf("}, %f, {",value[i]);
		for (int j=LUT_size-1; j>=0; j--) {
			unsigned int hex_value = LUT_array[LUT_size * i + j];
			if (hex_value<16) printf("0");
			printf("%x",hex_value);
		}
		printf("}\r\n");
	}
}

