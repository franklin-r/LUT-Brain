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
	int Start = 1;  		// Indicate that the ASM can start
	int Done;       		// Indicate the the ASM is finished
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
	
	INIT	:	if(Start == 0) {goto INIT;}
				else {Done = 0; rd = 0; wr = 0; addr = 0; writedata = 0; i = 0;
						result = 0; goto S0;}
	
	S0		:	if(n == 0) {l_LUT_array = (const unsigned char*)dataa; l_value = (float*)datab;
							LUT_size = 0; n_neuron = 0; goto S1;}	// We already have saved the addresses and we can exit by setting i 
																	// to n_neuron which then meets the return condition
			 	else {l_LUT_Address = (int*)dataa; n_neuron = (int)datab >> 16; LUT_size = (int)datab & 0xFFFF; goto S1;}
				
	S1		:	if(i < n_neuron) {tmp = LUT_size * i; goto S2;}
				else {Done = 1; return result;}
													
	S2 		:	{rd = 1; wr = 0; addr = l_LUT_Address + i;}
				if(waitrequest == 0) {goto S3;}
				else {goto S2;}
					
				// LUT_Address[i] = val_LUT_Address = readdata
	S3		:	{val_LUT_Address = l_LUT_Address[i]; 	// Only needed for the C++ code
				rd = 1; wr = 0; addr = l_LUT_array + tmp + (val_LUT_Address >> 3);}	
				if(waitrequest == 0) {goto S4;}
				else {goto S3;}
				
				// LUT_array[tmp + (val_LUT_Address >> 3)] = val_LUT_array = readdata
	S4		:	{val_LUT_array = l_LUT_array[tmp + (val_LUT_Address >> 3)];		// Only needed for the C++ code
				rd = 0; wr = 1; addr = l_value + i;
				writedata = 1 & (*(l_LUT_array + tmp + (val_LUT_Address >> 3)) >> (val_LUT_Address & 0x7));
				l_value[i] = writedata;}			// Only needed for the C++ code
				if(waitrequest == 0) {i = i + 1; goto S1;}
				else {goto S4;}
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
	int Start = 1;  		// Indicate that the ASM can start
	int Done;       		// Indicate the the ASM is finished
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
	
	INIT	:	if(Start == 0) {goto INIT;}
				else {Done = 0; rd = 0; wr = 0; addr = 0; writedata = 0; i = 0;
						T0 = (n == 0); result = 0; goto S0;}
	
	S0		:	if(T0) {l_LUT_array = (const unsigned char*)dataa; l_value = (float*)datab; T1 = 0;		// We want to exit if n = 0
							LUT_size = 0; n_neuron = 0; goto S1;}	// We already have saved the addresses and we can exit by setting i 
																	// to n_neuron which then meets the return condition
			 	else {l_LUT_Address = (int*)dataa; n_neuron = (int)datab >> 16; LUT_size = (int)datab & 0xFFFF; 
						T1 = 1; goto S1;}		// Necessarily true at the beginning
				
	S1		:	if(T1) {tmp = LUT_size * i; goto S2;}
				else {Done = 1; return result;}
													
	S2 		:	{rd = 1; wr = 0; addr = l_LUT_Address + i;}
				if(waitrequest == 0) {goto S3;}
				else {goto S2;}
					
				// LUT_Address[i] = val_LUT_Address = readdata
	S3		:	{val_LUT_Address = l_LUT_Address[i]; 	// Only needed for the C++ code
				rd = 1; wr = 0; addr = l_LUT_array + tmp + (val_LUT_Address >> 3);}	
				if(waitrequest == 0) {goto S4;}
				else {goto S3;}
				
				// LUT_array[tmp + (val_LUT_Address >> 3)] = val_LUT_array = readdata
	S4		:	{val_LUT_array = l_LUT_array[tmp + (val_LUT_Address >> 3)];		// Only needed for the C++ code
				rd = 0; wr = 1; addr = l_value + i;
				writedata = 1 & (*(l_LUT_array + tmp + (val_LUT_Address >> 3)) >> (val_LUT_Address & 0x7));
				l_value[i] = writedata;}			// Only needed for the C++ code
				if(waitrequest == 0) {T1 = (i + 1 < n_neuron); i = i + 1; goto S1;}
				else {goto S4;}
}