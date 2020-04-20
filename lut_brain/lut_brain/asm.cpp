
#include "asm.h"

// Copy Block ASM functions by Daniel Dermont 2043595 //
unsigned int copy_block_ctrl(unsigned int n, unsigned int dataa, unsigned int datab) {

	// Input data
	static int size;
	static float* target;
	static int length;
	static unsigned char* source_array;

	// Variables
	int x, y;
	int start = 1, done, read, write, readdata, writedata, waitrequest = 0;
	void* address;
	int i, j;

	// Output
	int result;


INIT:	if		(n == 0) { size = dataa; target = (float*)datab; return done = 1; }
		else if (n == 1) { length = dataa; source_array = (unsigned char*)datab; return done = 1; }
		else {
			x = dataa; y = datab;
			done = 0; read = 0; write = 0; address = 0; writedata = 0; j = 0;
			result = 0; goto S1;
		}

S1:	if (j < size) { i = 0; goto S2; }
	else { return done = 1; }

S2:	if (i < size) { read = 1; goto S3; }
	else { j += 1; goto S1; }

S3:	if (waitrequest == 0) {
		read = 0; write = 1;
		address = source_array + ((y + j) * length + (x + i));
		goto S4;
	}
	else { goto S3; }

S4:	if (waitrequest == 0) {
		read = 0; write = 0;
		writedata = *(source_array + ((y + j) * length + (x + i))) / 255;
		address = target + (j * size + i);
		target[j * size + i] = writedata;		// Unnecessary in the VHDL
		i += 1;
		goto S2;
	}
	else { goto S4; }
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
		else { data = *(LUT_Address + i); read = 0; address = LUT_Address + i; data = *(LUT_Address + i) + (1 << j); write = 1; goto WRRQ; }

WRRQ:   if (waitrequest == 1) { goto WRRQ; }
		else { *(LUT_Address + i) += (1 << j); write = 0;  j++; current_pos_index++; goto S2; }
}

unsigned int lutForward_ASM_hard(unsigned int n, unsigned int dataa, unsigned int datab) {
	/*
	@Author			: 	Alexis ROSSI
	@Description 	: 	Compute the output value of an LUT
	@Args 			: 	n		: 	Determine the operation to do (address loading or output computation)
										n = 0 : dataa = LUT_array and datab = value
										n = 1 : dataa = LUT_address and datab = n_neuron and LUT_size
						dataa 	: 	Either LUT_array (address of the current layer's LUT) 
									or LUT_Address (addresse of the current LUT) 
						datab 	: 	Either value (address of current layer's LUT output) or 
									n_neuron (the number of neuron in a layer) and LUT_size (size of an LUT)
	@Out 			: 	result	:	This output is necessary for the VHDL even if it has no purpose here
	@Note			: 	When n = 1, the upper 16 bits of data are n_neuron and lower 16 bits are LUT_size
						Called in NNLayer::propagate()
	*/
	
	/* Below are variables not really used in the C++ but necessary for the VHDL*/
	int start = 1;  		// Indicate that the ASM can start
	int done;       		// Indicate the the ASM is finished
	int rd;					// Indicate a reading
	int wr;					// Indicate a writing
	const void *addr;		// Address to write to/read from
	int readdata;			// Read data
	int writedata;			// Write data
	int waitrequest = 0;	// Indicate whether or not a write/read can finish
	
	// Local variables for the input data
	int n_neuron, LUT_size;				
	
	// Local addresses
	static const unsigned char *l_LUT_array;
	static float *l_value;
	static int *l_LUT_Address;
	
	// Variables
	int i, tmp, val_LUT_Address;
	char val_LUT_array;
	
	// Ouptut
	unsigned int result;
	
	INIT	:	if(start) {done = 0; rd = 0; wr = 0; addr = 0; writedata = 0; i = 0;
							result = 0; goto S0;}
				else {goto INIT;}
	
	S0		:	if(n) {l_LUT_Address = (int*)dataa; n_neuron = (int)datab >> 16; LUT_size = (int)datab & 0xFFFF; goto S1;}
			 	else {l_LUT_array = (const unsigned char*)dataa; l_value = (float*)datab;
						LUT_size = 0; n_neuron = 0; goto S1;}	// We already have saved the addresses and we can exit by setting i 
																// to n_neuron which then meets the return condition
				
	S1		:	{rd = 0; wr = 0;}
				if(i < n_neuron) {tmp = LUT_size * i; goto S2;}
				else {done = 1; return result;}
													
	S2 		:	{rd = 1; wr = 0; addr = l_LUT_Address + i;}
				if(waitrequest) {goto S2;}
				else {goto S3;}
					
				// LUT_Address[i] = val_LUT_Address = readdata
	S3		:	{val_LUT_Address = l_LUT_Address[i]; 	// Only needed for the C++ code
				rd = 1; wr = 0; addr = l_LUT_array + tmp + (val_LUT_Address >> 3);}	
				if(waitrequest) {goto S3;}
				else {goto S4;}
				
				// LUT_array[tmp + (val_LUT_Address >> 3)] = val_LUT_array = readdata
	S4		:	{val_LUT_array = l_LUT_array[tmp + (val_LUT_Address >> 3)];		// Only needed for the C++ code
				rd = 0; wr = 1; addr = l_value + i;
				writedata = 1 & (*(l_LUT_array + tmp + (val_LUT_Address >> 3)) >> (val_LUT_Address & 0x7));
				l_value[i] = writedata;}			// Only needed for the C++ code
				if(waitrequest) {goto S4;}
				else {i = i + 1; goto S1;}
}


unsigned int lutForward_ASM_hard_opti(unsigned int n, unsigned int dataa, unsigned int datab) {
	/*
	@Author			: 	Alexis ROSSI
	@Description 	: 	Compute the output value of an LUT
	@Args 			: 	n		: 	Determine the operation to do (address loading or output computation)
										n = 0 : dataa = LUT_array and datab = value
										n = 1 : dataa = LUT_address and datab = n_neuron and LUT_size
						dataa 	: 	Either LUT_array (address of the current layer's LUT) 
									or LUT_Address (addresse of the current LUT) 
						datab 	: 	Either value (address of current layer's LUT output) or 
									n_neuron (the number of neuron in a layer) and LUT_size (size of an LUT)
	@Out 			: 	result	:	This output is necessary for the VHDL even if it has no purpose here
	@Note			: 	When n = 1, the upper 16 bits of data are n_neuron and lower 16 bits are LUT_size
						Called in NNLayer::propagate()
	*/
	
	/* Below are variables not really used in the C++ but necessary for the VHDL*/
	int start = 1;  		// Indicate that the ASM can start
	int done;       		// Indicate the the ASM is finished
	int rd;					// Indicate a reading
	int wr;					// Indicate a writing
	const void *addr;		// Address to write to/read from
	int readdata;			// Read data
	int writedata;			// Write data
	int waitrequest = 0;	// Indicate whether or not a write/read can finish
	
	// Local variables for the input data
	int n_neuron, LUT_size;				
	
	// Local addresses
	static const unsigned char *l_LUT_array;
	static float *l_value;
	static int *l_LUT_Address;
	
	// Variables
	int i, tmp, val_LUT_Address;
	char val_LUT_array;
	
	// Ouptut
	unsigned int result;
	
	// Test condition
	int T0, T1;
	
	INIT	:	if(start) {done = 0; rd = 0; wr = 0; addr = 0; writedata = 0; i = 0;
							T0 = (n == 0); result = 0; goto S0;}
				else {goto INIT;}
	
	S0		:	if(T0) {l_LUT_array = (const unsigned char*)dataa; l_value = (float*)datab; T1 = 0;		// We want to exit if n = 0
							LUT_size = 0; n_neuron = 0; goto S1;}	// We already have saved the addresses and we can exit by setting i 
																	// to n_neuron which then meets the return condition
			 	else {l_LUT_Address = (int*)dataa; n_neuron = (int)datab >> 16; LUT_size = (int)datab & 0xFFFF; 
						T1 = 1; goto S1;}		// Necessarily true at the beginning
				
	S1		:	{rd = 0; wr = 0;}
				if(T1) {tmp = LUT_size * i; goto S2;}
				else {done = 1; return result;}
													
	S2 		:	{rd = 1; wr = 0; addr = l_LUT_Address + i;}
				if(waitrequest) {goto S2;}
				else {goto S3;}
					
				// LUT_Address[i] = val_LUT_Address = readdata
	S3		:	{val_LUT_Address = l_LUT_Address[i]; 	// Only needed for the C++ code
				rd = 1; wr = 0; addr = l_LUT_array + tmp + (val_LUT_Address >> 3);}	
				if(waitrequest) {goto S3;}
				else {goto S4;}
				
				// LUT_array[tmp + (val_LUT_Address >> 3)] = val_LUT_array = readdata
	S4		:	{val_LUT_array = l_LUT_array[tmp + (val_LUT_Address >> 3)];		// Only needed for the C++ code
				rd = 0; wr = 1; addr = l_value + i;
				writedata = 1 & (*(l_LUT_array + tmp + (val_LUT_Address >> 3)) >> (val_LUT_Address & 0x7));
				l_value[i] = writedata;}			// Only needed for the C++ code
				if(waitrequest) {goto S4;}
				else {T1 = (i + 1 < n_neuron); i = i + 1; goto S1;}
}