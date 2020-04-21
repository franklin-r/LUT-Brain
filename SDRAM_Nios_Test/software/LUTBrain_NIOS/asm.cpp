#include "asm.h"

int buildAddress_hard(int n_neuron, int n_input_per_neuron, int source, int LUT_Address, int current_pos){
	ALT_CI_BUILDADDRESS_0(0,n_neuron,n_input_per_neuron);
	ALT_CI_BUILDADDRESS_0(1,source,LUT_Address);
	ALT_CI_BUILDADDRESS_0(2,current_pos,0);
	return 0;
}


int lutForward_hard(int LUT_array, int value, int LUT_Address, int n_neuron, int LUT_size){
	ALT_CI_LUTFORWARD_0(0,LUT_array, value);
	ALT_CI_LUTFORWARD_0(1,LUT_Address,(n_neuron << 16) | (LUT_size & 0xFFFF));
	return 0;
}


int copyBlock_hard(int size, int source, int length, int source_array, int x, int y){
	ALT_CI_COPYBLOCK_0(0,size,source);
	ALT_CI_COPYBLOCK_0(1,length,source_array);
	ALT_CI_COPYBLOCK_0(2,x,y);
	return 0;
}