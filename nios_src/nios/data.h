#ifndef __DATA_H
#define __DATA_H

#define IMAGE_LENGTH 72
#define IMAGE_HEIGHT 156

#define MATRIX_SIZE 28
#define LAYER_SIZE  256
#define OUTPUT_SIZE 10

#define LUT_SIZE 6
#define LUT_BYTES 8

extern unsigned char BINARY_IMAGE[IMAGE_HEIGHT * IMAGE_LENGTH];

extern const unsigned char LUT_data0[36 * LAYER_SIZE * LUT_BYTES];
extern const int pos_data0[36 * LAYER_SIZE * LUT_SIZE];

extern const unsigned char LUT_data1[6 * LAYER_SIZE * LUT_BYTES];
extern const int pos_data1[6 * LAYER_SIZE * LUT_SIZE];

extern const unsigned char LUT_data2[1 * LAYER_SIZE * LUT_BYTES];
extern const int pos_data2[1 * LAYER_SIZE * LUT_SIZE];

extern const unsigned char LUT_data3[36 * LAYER_SIZE * LUT_BYTES];
extern const int pos_data3[36 * LAYER_SIZE * LUT_SIZE];

extern const unsigned char LUT_data4[6 * LAYER_SIZE * LUT_BYTES];
extern const int pos_data4[6 * LAYER_SIZE * LUT_SIZE];

extern const unsigned char LUT_data5[1 * LAYER_SIZE * LUT_BYTES];
extern const int pos_data5[1 * LAYER_SIZE * LUT_SIZE];

extern const unsigned char LUT_data6[36 * OUTPUT_SIZE * LUT_BYTES];
extern const int pos_data6[36 * OUTPUT_SIZE * LUT_SIZE];

extern const unsigned char LUT_data7[6 * OUTPUT_SIZE * LUT_BYTES];
extern const int pos_data7[6 * OUTPUT_SIZE * LUT_SIZE];

extern const unsigned char LUT_data8[1 * OUTPUT_SIZE * LUT_BYTES];
extern const int pos_data8[1 * OUTPUT_SIZE * LUT_SIZE];

#endif
