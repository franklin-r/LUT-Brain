/*
 * NNLayer.h
 *
 *  Created on: Jul 11, 2013
 *      Author: jpdavid
 */

#ifndef NNLAYER_H_
#define NNLAYER_H_

class NNLayer {
public:
	NNLayer();
	NNLayer(int new_n_input, int new_n_neuron, int nipn, const unsigned char* LUT_data, const int* pos_data);
	void init(int new_n_input, int new_n_neuron, int nipn, const unsigned char* LUT_data, const int* pos_data);
	void load_values(int new_n_input, int new_n_neuron, int nipn, const unsigned char* LUT_data, const int* pos_data);
	void buildAddress(float* source, const int* current_pos, int* LUT_Address);
	void buildAddress_hard(float* source, const int* current_pos, int* LUT_Address);
	void buildAddress_hard_optimise(float* source, const int* current_pos, int* LUT_Address);
	void lutForward(int* LUT_Address);
	void lutForward_ASM_hard(int *LUT_Address, int data);
	void lutForward_ASM_hard2(const unsigned char *dataa, float *datab, char n);		// Overload
	void lutForward_ASM_hard2(int *dataa, int datab, char n);							// Overload
	void lutForward_ASM_hard_opti(int *LUT_Address, int data) ;
	void lutForward_ASM_hard_opti2(const unsigned char *dataa, float *datab, char n);	// Overload
	void lutForward_ASM_hard_opti2(int *dataa, int datab, char n);						// Overload
	virtual ~NNLayer();	

	int n_input;
	int n_neuron;
	int n_input_per_neuron;
	int LUT_size;

	const unsigned char *LUT_array;
	const int *pos_array;
	float *value;

	float * propagate(float * source);

	void print_activation();
	void print();

};

#endif /* NNLAYER_H_ */
