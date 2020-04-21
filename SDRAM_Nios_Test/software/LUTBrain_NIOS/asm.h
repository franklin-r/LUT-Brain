#ifndef ASM_H_
#define ASM_H_

#include "system.h"

int buildAddress_hard(int n_neuron, int n_input_per_neuron, int source, int LUT_Address, int current_pos);
int lutForward_hard(int LUT_array, int value, int LUT_Address, int n_neuron, int LUT_size);
int copyBlock_hard(int size, int source, int length, int source_array, int x, int y);


#endif /* ASM_H_ */
