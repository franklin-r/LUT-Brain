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

void NNLayer::buildAddress_hard(float* source, const int* current_pos, int* LUT_Address) {
	/*
	@Author			: 	Louis-Normand ANG HOULE
	@Description 	: 	Build addresses equivalent to neuron interconnects
	@Args 			: 	float*     source
						const int* current_pos
						int*       LUT_Address
	@Out 			: 	void
	Note			: 	
	*/

	//Control signals
	int start = 1;
	int done;
	int data, read, write, waitrequest = 0;
	void* address;

	//Local variables
	int i, j;
	int current_pos_index = 0;

//Initialization
INIT:	if (start == 0) { goto INIT; }                
		else { write = 0; read = 0; done = 0; i = 0; goto S1; }

//Check if all neurons are done
S1:		if (i < n_neuron) {j = 0; goto S2; }
		else { done = 1; return; }

// Check if all neuron inputs are connected; Read data in current_pos
S2:		if (j < n_input_per_neuron) { address = (void*)(current_pos + current_pos_index); read = 1; goto RDRQ1; }
		else { i++; goto S1; }

//Read data in source + *current_pos
S3:		if (1) {address = source + *(current_pos + current_pos_index); read = 1; goto RDRQ2; }

//Check if input needs to be connected.
S4:		if (source[current_pos[current_pos_index]] != 0) { address = LUT_Address + i; read = 1; goto RDRQ3; }
		else { j++; current_pos_index++; goto S2; }

//Read data in LUT_Address + i
S5:		if (1) { address = LUT_Address + i; data = *(LUT_Address + i) + (1 << j); write = 1; goto WRRQ; }

// Wait for memory
/*Read *current_pos from data*/
RDRQ1:  if (waitrequest == 1) { goto RDRQ1; }
		else { data = current_pos[current_pos_index];  read = 0; goto S3; }

/*Read *(source + *current_pos) from data*/
RDRQ2:  if (waitrequest == 1) { goto RDRQ2; }
        else { data = source[current_pos[current_pos_index]]; read = 0; goto S4; }

/*Read *(LUT_Address + i) from data*/
RDRQ3:  if (waitrequest == 1) { goto RDRQ3; }
		else { data = *(LUT_Address + i); read = 0; goto S5; }

WRRQ:   if (waitrequest == 1) { goto WRRQ; }
        else { *(LUT_Address + i) += (1 << j); write = 0;  j++; current_pos_index++; goto S2; }
}

void NNLayer::buildAddress_hard_optimise(float* source, const int* current_pos, int* LUT_Address) {
	/*
	@Author			: 	Louis-Normand ANG HOULE
	@Description 	: 	Build addresses equivalent to neuron interconnects
	@Args 			: 	float*     source
						const int* current_pos
						int*       LUT_Address
	@Out 			: 	void
	Note			:
	*/

	//Control signals
	int start = 1;
	int done;
	int data, read, write, waitrequest = 0;
	void* address;

	//Local variables
	int i, j;
	int current_pos_index = 0;

	//Initialization
INIT:	if (start == 0) { goto INIT; }
		else { write = 0; read = 0; done = 0; i = 0; goto S1; }

//Check if all neurons are done
S1:		if (i < n_neuron) { j = 0; goto S2; }
		else { done = 1; return; }

// Check if all neuron inputs are connected; Read data in current_pos
S2:		if (j < n_input_per_neuron) { address = (void*)(current_pos + current_pos_index); read = 1; goto RDRQ1; }
		else { i++; goto S1; }

//Check if input needs to be connected.
S4:		if (source[current_pos[current_pos_index]] != 0) { address = LUT_Address + i; read = 1; goto RDRQ3; }
		else { j++; current_pos_index++; goto S2; }

// Wait for memory
/*Read *current_pos from data*/ //Read data in source + *current_pos
RDRQ1:  if (waitrequest == 1) { goto RDRQ1; }
		else { data = current_pos[current_pos_index]; address = source + *(current_pos + current_pos_index); goto RDRQ2; }

/*Read *(source + *current_pos) from data*/
RDRQ2:  if (waitrequest == 1) { goto RDRQ2; }
		else { data = source[current_pos[current_pos_index]]; read = 0; goto S4; }

/*Read *(LUT_Address + i) from data*/
RDRQ3:  if (waitrequest == 1) { goto RDRQ3; }
		else { data = *(LUT_Address + i); read = 0; address = LUT_Address + i; data = *(LUT_Address + i) + (1 << j); write = 1; goto WRRQ;}

WRRQ:   if (waitrequest == 1) { goto WRRQ; }
		else { *(LUT_Address + i) += (1 << j); write = 0;  j++; current_pos_index++; goto S2; }

}

unsigned int buildAddress_ctrl(unsigned int n, unsigned int dataa, unsigned int datab) {
    /*
	@Author			: 	Louis-Normand ANG HOULE
	@Description 	: 	Build addresses equivalent to neuron interconnects
	@Args 			:   n     
	                    dataa 
						datab

	@Out 			: 	unsigned int return
	Note			:
	*/

	//Input data / Registers
	static int n_neuron;             //
	static int n_input_per_neuron;   //
	static float* source;            //
	static int* LUT_Address;         //
	static const int* current_pos;   //

	//Output signal
	int done;
	int result;

	//Control signals
	int read, write, waitrequest = 0;
	int data;
	void* address;

	//Local variables
	int i, j;
	int current_pos_index = 0;

//Initialization/Write parameters
INIT:	if		(n == 0) { n_neuron = dataa; n_input_per_neuron = datab; done = 1; return result = 0; }
		else if (n == 1) { source = (float*)dataa; LUT_Address = (int*)datab; done = 1; return result = 0; }
		else if (n == 2) { current_pos = (const int*)dataa;  done = 0;  write = 0; read = 0; i = 0; goto S1; }
		else { return result = 1; }

//Check if all neurons are done
S1:		if (i < n_neuron) { j = 0; goto S2; }
		else { done = 1; return result = 0; }

// Check if all neuron inputs are connected; Read data in current_pos
S2:		if (j < n_input_per_neuron) { address = (void*)(current_pos + current_pos_index); read = 1; goto RDRQ1; }
		else { i++; goto S1; }

//Check if input needs to be connected.
S4:		if (source[current_pos[current_pos_index]] != 0) { address = LUT_Address + i; read = 1; goto RDRQ3; }
		else { j++; current_pos_index++; goto S2; }

// Wait for memory
/*Read *current_pos from data*/ //Read data in source + *current_pos
RDRQ1:  if (waitrequest == 1) { goto RDRQ1; }
		else { data = current_pos[current_pos_index]; address = source + *(current_pos + current_pos_index); goto RDRQ2; }

/*Read *(source + *current_pos) from data*/
RDRQ2:  if (waitrequest == 1) { goto RDRQ2; }
		else { data = source[current_pos[current_pos_index]]; read = 0; goto S4; }

/*Read *(LUT_Address + i) from data*/
RDRQ3:  if (waitrequest == 1) { goto RDRQ3; }
		else { data = *(LUT_Address + i); read = 0; address = LUT_Address + i; data = *(LUT_Address + i) + (1 << j); write = 1; goto WRRQ;}

WRRQ:   if (waitrequest == 1) { goto WRRQ; }
		else { *(LUT_Address + i) += (1 << j); write = 0;  j++; current_pos_index++; goto S2; }
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
				
				// LUT_array[tmp + (val_LUT_Address >> 3)] = val_LUT_array = readdata	
	S4		:	if(waitrequest == 0) {val_LUT_array = LUT_array[tmp + (val_LUT_Address >> 3)];		// Only needed for the C++ code
										rd = 0; wr = 1; addr = value + i;
										writedata = 1 & (*(LUT_array + tmp + (val_LUT_Address >> 3)) >> (val_LUT_Address & 0x7));
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
						The bug correction was to change T1 = (i < n_neuron) to T1 = (i + 1 < n_neuron)
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
				
				// LUT_array[tmp + (val_LUT_Address >> 3)] = val_LUT_array = readdata	
	S4		:	if(waitrequest == 0) {val_LUT_array = LUT_array[tmp + (val_LUT_Address >> 3)];	// Only needed for the C++ code
										rd = 0; wr = 1; addr = value + i;
										writedata = 1 & (*(LUT_array + tmp + (val_LUT_Address >> 3)) >> (val_LUT_Address & 0x7));
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

	// Test section
	//buildAddress(source, current_pos, LUT_Address);                                 // Code original
	//buildAddress_hard(source, current_pos, LUT_Address);                            // Test ASM hard
	//buildAddress_hard_optimise(source, current_pos, LUT_Address);                     // Test ASM hard optimisé

	buildAddress_ctrl(0, n_neuron, n_input_per_neuron);
	buildAddress_ctrl(1, (int)source, (int) LUT_Address);
	buildAddress_ctrl(2, (int)current_pos, 0);

	//lutForward(LUT_Address);                                                          // Code original
	//lutForward_ASM_hard(LUT_Address, (n_neuron << 16) | (LUT_size & 0xFFFF));         // Test ASM hard  
	lutForward_ASM_hard_opti(LUT_Address, (n_neuron << 16) | (LUT_size & 0xFFFF));  // Test ASM hard optimisé


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

