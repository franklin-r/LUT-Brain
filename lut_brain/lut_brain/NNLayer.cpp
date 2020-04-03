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
int Start = 1;  		// Not really used, just an artefact to move to hardware ASM
int Done;       		// Not really used, just an artefact to move to hardware ASM
int rd;					// Not really used, just an artefact to move to hardware ASM
int wr;					// Not really used, just an artefact to move to hardware ASM
const void *addr;		// Not really used, just an artefact to move to hardware ASM
int readdata;			// Not really used, just an artefact to move to hardware ASM
int writedata;			// Not really used, just an artefact to move to hardware ASM
int waitrequest = 0;	// Not really used, just an artefact to move to hardware ASM

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
	// Fonction à remplacer
	for (int i = 0; i < n_neuron; i++) {
		for (int j = 0; j < n_input_per_neuron; j++) {
			if (source[*(current_pos++)] != 0) LUT_Address[i] += (1 << j);
		}
	}
}

void NNLayer::lutForward(int* LUT_Address) {
	// Fonction à remplacer
	for (int i = 0; i < n_neuron; i++) {
		value[i] = 1 & (LUT_array[LUT_size * i + (LUT_Address[i] >> 3)] >> (LUT_Address[i] & 0x7));
		if (my_debug) printf("LUT%i[%i] = %f\r\n", i, LUT_Address[i], value[i]);
	}
}


void NNLayer::lutForward_ASM_hard(int *LUT_Address, int data) {
	/*
	@Author			: 	Alexis ROSSI
	@Description 	: 	Compute the output value of an LUT
	@Args 			: 	LUT_Address 	: Address of an LUT
						data 			: Number of neuron in a layer and size of an LUT
	@Out 			: 	Void
	@Note			: 	Upper 16 bits of data are n_neuron and lower 16 bits are LUT_size
						Called in NNLayer::propagate()
	*/
	
	// Local variables for the input data
	int n_neuron, LUT_size;
	
	// Variables
	int i, tmp, val_LUT_Address;
	char val_LUT_array;
	
	// Ouptut
	int result;
	
	
	INIT 	:	if(Start == 0) {goto INIT;}
				else {Done = 0; rd = 0; wr = 0; addr = 0; writedata = 0; i = 0;
						n_neuron = data >> 16; LUT_size = data & 0xFFFF; result = 0; goto S1;}
						
	S1		:	if(i < n_neuron) {tmp = LUT_size * i; goto S2;}
				else {Done = 1; return;}
													
	S2 		:	if(waitrequest == 0) {rd = 1; wr = 0; addr = LUT_Address + i; goto S3;}
				else {goto S2;}
					
				// LUT_Address[i] = val_LUT_Address = readdata
	S3		:	if(waitrequest == 0) {val_LUT_Address = LUT_Address[i]; 	// Only needed for the C++ code
										rd = 1; wr = 0; addr = LUT_array + tmp + (val_LUT_Address >> 3); 
										goto S4;}	
				else {goto S3;}
				
				// LUT_array[i] = val_LUT_array = readdata	
	S4		:	if(waitrequest == 0) {val_LUT_array = LUT_array[i];			// Only needed for the C++ code
										rd = 0; wr = 1; addr = value + i;
										writedata = 1 & (*(LUT_array + i) >> (val_LUT_Address & 0x7));
										value[i] = writedata;				// Only needed for the C++ code
										i = i + 1; goto S1;}
				else {goto S4;}
}


void NNLayer::lutForward_ASM_hard_opti(int *LUT_Address, int data) {
	/*
	@Author			: 	Alexis ROSSI
	@Description 	: 	Compute the output value of an LUT
	@Args 			: 	LUT_Address 	: Address of an LUT
						data 			: Number of neuron in a layer and size of an LUT
	@Out 			: 	Void
	@Note			: 	Upper 16 bits of data are n_neuron and lower 16 bits are LUT_size
						Called in NNLayer::propagate()
						The modification with the "non-optimized" version is the pre-computation of the
						condition i + 1 < n_neuron
	*/
	
	// Local variables for addresses
	int *l_LUT_Address; unsigned char *l_LUT_array; float *l_value; 
	
	// Local variables for the input data
	int n_neuron, LUT_size;
	
	// Variables
	int i, tmp, val_LUT_Address;
	char val_LUT_array;
	
	// Test condition
	int T1;
	
	// Ouptut
	int result;
	
	
	INIT 	:	if(Start == 0) {goto INIT;}
				else {Done = 0; rd = 0; wr = 0; addr = 0; writedata = 0; i = 0;
						n_neuron = data >> 16; LUT_size = data & 0xFFFF; T1 = 1; result = 0; goto S1;}
						
	S1		:	if(T1) {tmp = LUT_size * i; goto S2;}
				else {Done = 1; return;}
													
	S2 		:	if(waitrequest == 0) {rd = 1; wr = 0; addr = LUT_Address + i; goto S3;}
				else {goto S2;}
					
				// LUT_Address[i] = val_LUT_Address = readdata
	S3		:	if(waitrequest == 0) {val_LUT_Address = LUT_Address[i]; 	// Only needed for the C++ code
										rd = 1; wr = 0; addr = LUT_array + tmp + (val_LUT_Address >> 3); 
										goto S4;}	
				else {goto S3;}
				
				// LUT_array[i] = val_LUT_array = readdata	
	S4		:	if(waitrequest == 0) {val_LUT_array = LUT_array[i];			// Only needed for the C++ code
										rd = 0; wr = 1; addr = value + i;
										writedata = 1 & (*(LUT_array + i) >> (val_LUT_Address & 0x7));
										value[i] = writedata;				// Only needed for the C++ code
										T1 = (i + 1 < n_neuron); i = i + 1; goto S1;}
				else {goto S4;}
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
	//lutForward(LUT_Address);
	//lutForward_ASM_hard(LUT_Address, (n_neuron << 16) | (LUT_size & 0xFFFF));
	lutForward_ASM_hard_opti(LUT_Address, (n_neuron << 16) | (LUT_size & 0xFFFF));

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

