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
#include <hls_stream.h>
#include <algorithm>
#include <iostream>
#include <fstream>

//#include "ap_fixed.h"

#define EXP_WIDTH	16
#define INT_WIDTH	4
#define num_img 	9
typedef float float24_t;


#define eps 0.1

void nnet(hls::stream<float24_t> &fc3_out, hls::stream<float24_t> &img_in);

int main()
{
	hls::stream<float24_t> FC3_out("test_FC3_out");
	hls::stream<float24_t> img_in("image in");

	float24_t fc_layer3_out[FC3_ACT_SIZE * num_img];
	float fc_layer3_ref[FC3_ACT_SIZE];

	int i,j,k;
	int correct_values, total_values;

	// open and read image
	float buffer;
	FILE* images = fopen("C:/School/comp4601/Project/COMP4601_CNN_HLS/nnet_stream/ref/img.txt","r");
	if(images == NULL)
	{
		printf("Couldn't open images.txt");
		exit(1);
	}

	while(fscanf(images,"%f",&buffer) != EOF)
	{
		img_in << buffer;
	}
	// finish reading image

	//read correct result
	int ans[num_img];
	FILE* answers = fopen("C:/School/comp4601/Project/COMP4601_CNN_HLS/nnet_stream/ref/test.out","r");
	if(answers == NULL)
	{
		printf("Couldn't open ans.txt");
		exit(1);
	}
	for(int i = 0; i < num_img; i++){
		fscanf(answers,"%d",&ans[i]);
	}

	//finish reading correct results

	nnet(FC3_out, img_in);

	// put outputs into array
	for(i = 0; i < FC3_ACT_SIZE*num_img; i++) {
		FC3_out>>fc_layer3_out[i];
	}


	printf("\n\n\n\n");

	printf("Check results\n");


	float24_t outMax[num_img] = {0.0};
	int outMaxIndex[num_img] = {0};
	for(int k = 0; k<num_img; k++){
		for(int i = 0;i < 10; i++) {
		  if(outMax[k] < fc_layer3_out[i]){
			  outMax[k] = fc_layer3_out[i];
			  outMaxIndex[k] = i;
		  }
		}
	}
	bool flag = true;
	for(int i = 0; i<num_img; i++){
		if(outMaxIndex[i] == ans[i]){
			printf("test %d passed\n", i);
			continue;
		}else{
			printf("the %d output is incorrect\n", i);
			flag == false;
		}
	}
	if(flag == true){
		printf("all tests pass");
	}


	return 0;



}
