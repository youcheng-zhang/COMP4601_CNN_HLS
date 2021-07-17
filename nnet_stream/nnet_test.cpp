/******************************************************************************
 * (C) Copyright 2020 AMIQ Consulting
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * NAME:        nnet_test.cpp
 * PROJECT:     nnet_stream
 * Description: Basic test that sends an image to the Convolutional Neural Network
 *              and checks that the output matches the python reference
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "headers/defines.h"
#include <hls_video.h>
#include <algorithm>

//#include "ap_fixed.h"

#define EXP_WIDTH	16
#define INT_WIDTH	4

typedef float float24_t;


#define eps 0.1

void nnet(hls::stream<float24_t> &fc3_out);

int main()
{
	int passCount = 0;
	hls::stream<float24_t> FC3_out("test_FC3_out");

	float24_t fc_layer3_out[FC3_ACT_SIZE];
	float fc_layer3_ref[FC3_ACT_SIZE];
	float24_t placeholder;

	int i,j,k;
	int correct_values, total_values;

	nnet(FC3_out);

	for(i = 0; i < FC3_ACT_SIZE; i++) {
		FC3_out>>fc_layer3_out[i];
	}

	FILE* fc_layer3_content = fopen("C:/Users/xqsxl/Desktop/comp4601/CNN-using-HLS-master/nnet_stream/ref/fc_layer3_py.out","r");
	if(fc_layer3_content == NULL)
	{
		printf("Couldn't open ref/fc_layer3_py.out");
	    exit(1);
	}

	for(i = 0; i < FC3_ACT_SIZE; i++)
	{
		fscanf(fc_layer3_content,"%f",&fc_layer3_ref[i]);
	}


	correct_values = 0;
	total_values = 0;

	printf("\n\n\n\n");

	printf("Checking FC Layer 3 ...\n");

	float24_t refMax = fc_layer3_ref[0];
	int refMaxIndex = 0;
	for(int i = 1;i < 11; i++) {
	  /* We are comparing largest variable with every element
	   * of array. If there is an element which is greater than
	   * largest variable value then we are copying that variable
	   * to largest, this way we have the largest element copied
	   * to the variable named "largest" at the end of the loop
	   *
	   */
	  if(refMax < fc_layer3_ref[i])
		  refMax = fc_layer3_ref[i];
	  	  refMaxIndex = i;
	}

	float24_t outMax = fc_layer3_out[0];
	int outMaxIndex = 0;
	for(int i = 1;i < 11; i++) {
	  /* We are comparing largest variable with every element
	   * of array. If there is an element which is greater than
	   * largest variable value then we are copying that variable
	   * to largest, this way we have the largest element copied
	   * to the variable named "largest" at the end of the loop
	   *
	   */
	  if(outMax < fc_layer3_out[i])
		  outMax = fc_layer3_out[i];
		  outMaxIndex = i;
	}

	if (outMaxIndex == refMaxIndex){
		printf("test 1 pass\n");
		passCount ++;
	}
	else{
		printf("test 1 fail\n");
	}


	printf("%d out of 1 test passed", passCount);

	return 0;



}
