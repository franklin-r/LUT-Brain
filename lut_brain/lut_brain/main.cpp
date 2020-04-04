/********************************************************************
 * main.cpp
 *
 * Routine principale pour le projet ELE8307 A20.
 *
 * v1.2 - Application avec une machine pre-entrainee pour la
 * 			reconnaissance de chiffres écrits* (avec une image
 * 			72x156 pixels contenant 10 characteres 28x238).
 *
 *		- Ajout d'un #define pour activer/desactiver les printf dans
 *			la routine Image::apply_NN();
 *
 * Author : (V)-F)-I)
 *
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <ctime>

#include "NN.h"
#include "Image.h"
#include "data.h"

int start = 1;  // Not really used, just an artefact to move to hardware ASM
int done;       // Not really used, just an artefact to move to hardware ASM

/*
    La fonction suivante "exemple_ASM" a ete ecrite par JP David. 
    Elle est appelee dans le main.
    C'est un exemple illustratif d'une fonction qui retourne la somme des valeurs d'un tableau
    C'est la version purement logicielle
*/
int exemple_ASM(unsigned char* table, int length) {
    int result = 0;
    for (int i = 0; i < length; i++) {
        result += table[i];
    }
    return result;
}

/*
    La fonction suivante "exemple_ASM_hard" a ete ecrite par JP David. 
    Elle est appelee dans le main
    C'est une premiere version ASM pas optimisee
    ATTENTION comme cela reste du C/C++, toutes les operations se font de maniere sequentielles, ce qui ne sera pas le cas en VHDL
*/
int exemple_ASM_hard(unsigned char* input_table, int input_length) {    // pas optimise
    unsigned char* table; int length;
    int i, result;
    unsigned char* MAR;

    INIT:   if (start == 0) { goto INIT; }
            else { done = 0; table = input_table; length = input_length; result = 0;  i = 0;  goto S1; }

    S1:     if (i < length) { MAR = table + i; goto S2; }
            else { done = 1; return result; }

    S2:     if (1) { result = result + *MAR; i++; goto S1; }

}

/*
    La fonction suivante "exemple_ASM_hard_optimise" a ete ecrite par JP David. 
    Elle est appelee dans le main
    C'est une version un peu plus optimisee de l'ASM
    ATTENTION comme cela reste du C/C++, toutes les operations se font de maniere sequentielles, ce qui ne sera pas le cas en VHDL
 */
int exemple_ASM_hard_optimise(unsigned char* input_table, int input_length) {    // optimise
    unsigned char* table; int length;
    int i, result;
    unsigned char* MAR;

    INIT:   if (start == 0) { goto INIT; }
            else { done = 0; table = input_table; length = input_length; result = 0;  i = 0; MAR = table; goto S1; }

    S1:     if (i < length) { result = result + *MAR; MAR = table + i + 1; i++; goto S1; }
            else { done = 1; return result; }

 }


/*********************************************************************
 * main
 *********************************************************************/
int main(int argc, char **argv)
{
	printf("Entering ELE8307 Fall 2018 Project main() \r\n");

    unsigned char my_ASM_example[5] = { 1, 2, 3, 4, 5 };
    int result = exemple_ASM(my_ASM_example, 5);
    int result_hard = exemple_ASM_hard(my_ASM_example, 5);
    int result_hard_optimise = exemple_ASM_hard_optimise(my_ASM_example, 5);

    printf("Soft result : %i, hard result : %i, hard result optimise : %i\n", result, result_hard, result_hard_optimise);

    /* Initialisation et application a une image de la machine neuronale (poids aleatoires) */

    printf("> Exemple 1: NN Aleatoire \r\n");
    NN network(9);

	// First Cluster
    network.layer[0].load_values(MATRIX_SIZE*MATRIX_SIZE, 36*LAYER_SIZE, 6, LUT_data0, pos_data0);
    network.layer[1].load_values(36*LAYER_SIZE, 6*LAYER_SIZE, 6, LUT_data1, pos_data1);
    network.layer[2].load_values(6*LAYER_SIZE,LAYER_SIZE, 6, LUT_data2, pos_data2);

	// Second Cluster
    network.layer[3].load_values(LAYER_SIZE, 36*LAYER_SIZE, 6, LUT_data3, pos_data3);
    network.layer[4].load_values(36*LAYER_SIZE, 6*LAYER_SIZE, 6, LUT_data4, pos_data4);
	network.layer[5].load_values(6*LAYER_SIZE, LAYER_SIZE, 6, LUT_data5, pos_data5);

	// Third Cluster
	network.layer[6].load_values(LAYER_SIZE, 36*OUTPUT_SIZE, 6, LUT_data6, pos_data6);
	network.layer[7].load_values(36*OUTPUT_SIZE, 6*OUTPUT_SIZE, 6, LUT_data7, pos_data7);
	network.layer[8].load_values(6*OUTPUT_SIZE, OUTPUT_SIZE, 6, LUT_data8, pos_data8);
/*
    network.layer[0].print();
    network.layer[1].print();
    network.layer[2].print();
    network.layer[3].print();
    network.layer[4].print();
	network.layer[5].print();
	network.layer[6].print();
	network.layer[7].print();
	network.layer[8].print();
    */
	Image my_image(IMAGE_LENGTH, IMAGE_HEIGHT, true);
	my_image.printToFile(0, 0, "output_data/input_image");
	printf("Start processing ...\r\n");

	/*
	for (int i=0;i<10;i++) {
		clock_t time1 = clock();
		Image * result_image = my_image.apply_NN(&network, MATRIX_SIZE, i);
		clock_t time2 = clock();
		result_image->printToFile(0, 0, ("output_data/result_" + std::to_string(i)));
		delete result_image;
		printf("done in %d ms\r\n",(int)(time2-time1));
	}
	*/
	
	clock_t time1 = clock();
	Image ** result_images = my_image.apply_NN(&network, MATRIX_SIZE);
	clock_t time2 = clock();
	printf("done in %d ms\r\n",(int)(time2-time1));
	
	for (int i = 0;i < 10; i++) {
		result_images[i]->printToFile(0, 0, ("output_data/result_" + std::to_string(i)));
		delete result_images[i];
	}
	delete result_images;
	

	return 0;
}


