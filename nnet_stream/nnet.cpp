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
 * NAME:        nnet.cpp
 * PROJECT:     nnet_stream
 * Description: HLS implementation of the Convolutional Neural Network
 *******************************************************************************/


#include "headers/weights.h"
#include "headers/defines.h"
#include "headers/activations.h"
#include "C:\Xilinx\Vitis\2020.1\include\multimediaIps\xf_video_mem.hpp"
#include <hls_stream.h>

//#include "ap_fixed.h"

#define EXP_WIDTH	16
#define INT_WIDTH	4

typedef float float24_t;

#define CONV1_BUFFER_SIZE (IMAGE_SIZE * IMAGE_CHANNELS * (CONV1_KERNEL_SIZE -1) + CONV1_KERNEL_SIZE * IMAGE_CHANNELS)

void conv_layer1(hls::stream<float24_t> &out, hls::stream<float24_t> &in,
		float24_t weight[CONV1_KERNEL_SIZE][CONV1_KERNEL_SIZE][CONV1_CHANNELS][CONV1_FILTERS],
		float24_t bias[CONV1_BIAS_SIZE]) {
	int i, j, k, filter;
	float24_t sum, placeholder;
	int row_offset, col_offset, channel_offset;
	//float24_t
	xf::cv::LineBuffer<CONV1_BUFFER_SIZE, 1, float24_t> conv_buff;

	/*
	 * Read the initial buffer
	 * */

	for (i = 0; i < CONV1_BUFFER_SIZE; i++) {
		if (in.empty() == 0) {
			in >> placeholder;
			conv_buff.shift_up(0);
			conv_buff.insert_top(placeholder, 0);
		}
	}

	for (i = 0; i < (IMAGE_SIZE - CONV1_KERNEL_SIZE + 1); i += CONV1_STRIDE)
		conv_layer1_label9: for (j = 0;
				j < (IMAGE_SIZE - CONV1_KERNEL_SIZE + 1); j += CONV1_STRIDE)
				{
			conv_layer1_label2: for (filter = 0; filter < CONV1_FILTERS;
					filter++) {
				sum = 0;
				conv_layer1_label6: for (row_offset = 0;
						row_offset < CONV1_KERNEL_SIZE; row_offset++)
					conv_layer1_label7: for (col_offset = 0;
							col_offset < CONV1_KERNEL_SIZE; col_offset++)
						conv_layer1_label8: for (channel_offset = 0;
								channel_offset < CONV1_CHANNELS;
								channel_offset++) {
							int t1, t2;
							static float24_t val1, val2;
							t1 = row_offset * IMAGE_SIZE * IMAGE_CHANNELS;
							t2 = col_offset * IMAGE_CHANNELS;
							val1 = conv_buff.getval(t1 + t2 + channel_offset,
									0);
							val2 =
									weight[row_offset][col_offset][channel_offset][filter];
							sum += val1 * val2;
						}

				out << relu(sum + bias[filter]);
			}


			if ((j + CONV1_STRIDE < (IMAGE_SIZE - CONV1_KERNEL_SIZE + 1))) {
				conv_layer1_label1: for (int p = 0; p < IMAGE_CHANNELS; p++)
					if (in.empty() == 0) {
						in >> placeholder;
						conv_buff.shift_up(0);
						conv_buff.insert_top(placeholder, 0);
					}
			} else if ((i + CONV1_STRIDE < (IMAGE_SIZE - CONV1_KERNEL_SIZE + 1))
					&& (j + CONV1_STRIDE >= (IMAGE_SIZE - CONV1_KERNEL_SIZE + 1)))
				conv_layer1_label0: for (int p = 0;
						p < CONV1_KERNEL_SIZE * IMAGE_CHANNELS; p++)
					if (in.empty() == 0) {
						in >> placeholder;
						conv_buff.shift_up(0);
						conv_buff.insert_top(placeholder, 0);
					}
		}
}

#define CONV2_BUFFER_SIZE (P1_SIZE * P1_CHANNELS * (CONV2_KERNEL_SIZE -1) + CONV2_KERNEL_SIZE * P1_CHANNELS)

void conv_layer2(hls::stream<float24_t> &out, hls::stream<float24_t> &in,
		float24_t weight[CONV2_KERNEL_SIZE][CONV2_KERNEL_SIZE][CONV2_CHANNELS][CONV2_FILTERS],
		float24_t bias[CONV2_BIAS_SIZE]) {
	int i, j, k, filter;
	float24_t sum, placeholder;
	int row_offset, col_offset, channel_offset;
	xf::cv::LineBuffer<CONV2_BUFFER_SIZE, 1, float24_t> conv_buff;

	/*
	 * Read the initial buffer
	 * */

	for (i = 0; i < CONV2_BUFFER_SIZE; i++) {
		if (in.empty() == 0) {
			in >> placeholder;
			conv_buff.shift_up(0);
			conv_buff.insert_top(placeholder, 0);
		}
	}

	for (i = 0; i < (P1_SIZE - CONV2_KERNEL_SIZE + 1); i += CONV2_STRIDE)
		conv_layer2_label5: for (j = 0; j < (P1_SIZE - CONV2_KERNEL_SIZE + 1);
				j += CONV2_STRIDE)
				{
			conv_layer2_label12: for (filter = 0; filter < CONV2_FILTERS;
					filter++) {
				sum = 0;
				conv_layer2_label13: for (row_offset = 0;
						row_offset < CONV2_KERNEL_SIZE; row_offset++)
					conv_layer2_label10: for (col_offset = 0;
							col_offset < CONV2_KERNEL_SIZE; col_offset++)
						conv_layer2_label11: for (channel_offset = 0;
								channel_offset < CONV2_CHANNELS;
								channel_offset++) {
							int t1, t2;
							t1 = row_offset * P1_SIZE * P1_CHANNELS;
							t2 = col_offset * P1_CHANNELS;
							sum +=
									conv_buff.getval(t1 + t2 + channel_offset,
											0)
											* weight[row_offset][col_offset][channel_offset][filter];
						}
				out << relu(sum + bias[filter]);
			}


			if ((j + CONV2_STRIDE < (P1_SIZE - CONV2_KERNEL_SIZE + 1))) {
				conv_layer2_label3: for (int p = 0; p < P1_CHANNELS; p++)
					if (in.empty() == 0) {
						in >> placeholder;
						conv_buff.shift_up(0);
						conv_buff.insert_top(placeholder, 0);
					}
			} else if ((i + CONV2_STRIDE < (P1_SIZE - CONV2_KERNEL_SIZE + 1))
					&& (j + CONV2_STRIDE >= (P1_SIZE - CONV2_KERNEL_SIZE + 1)))
				conv_layer2_label4: for (int p = 0;
						p < CONV2_KERNEL_SIZE * P1_CHANNELS; p++)
					if (in.empty() == 0) {
						in >> placeholder;
						conv_buff.shift_up(0);
						conv_buff.insert_top(placeholder, 0);
					}
		}
}

#define POOL1_BUFFER_SIZE (P1_SIZE*P1_CHANNELS)

void pool_layer1(hls::stream<float24_t>& out, hls::stream<float24_t>& in) {

	int i, j, k, l, m;
	float24_t read;
	xf::cv::LineBuffer<POOL1_BUFFER_SIZE, 1, float24_t> pool_buff;

	for (i = 0; i < P1_SIZE; i++)
		pool_layer1_label6: for (l = 0; l < P1_KERNEL_SIZE; l++) {
			pool_layer1_label14: for (j = 0; j < P1_SIZE; j++)
				pool_layer1_label15: for (m = 0; m < P1_KERNEL_SIZE; m++)
					pool_layer1_label19: pool_layer1_label18: for (k = 0;
							k < P1_CHANNELS; k++) {
						in >> read;
						if (l == 0 && m == 0)
							pool_buff.val[j * P1_CHANNELS + k][0] = read;

						else
							pool_buff.val[j * P1_CHANNELS + k][0] =
									pool_buff.val[j * P1_CHANNELS + k][0]
											> read ?
											pool_buff.val[j * P1_CHANNELS + k][0] :
											read;

						if (l == (P1_KERNEL_SIZE - 1)
								&& m == (P1_KERNEL_SIZE - 1))
							out << pool_buff.val[j * P1_CHANNELS + k][0];
					}
			pool_layer1_label20: for (int skip = P1_SIZE * P1_STRIDE;
					skip < A1_SIZE; skip++)
				pool_layer1_label16: for (int channel = 0;
						channel < P1_CHANNELS; channel++)
					in >> read;
		}
	pool_layer1_label7: for (int skip_row = P1_SIZE * P1_STRIDE;
			skip_row < A1_SIZE; skip_row++)
		pool_layer1_label33: for (int skip_col = 0; skip_col < A1_SIZE;
				skip_col++)
			pool_layer1_label35: for (int skip_channel = 0;
					skip_channel < A1_CHANNELS; skip_channel++)
				in >> read;
}

#define POOL2_BUFFER_SIZE (P2_SIZE*P2_CHANNELS)

void pool_layer2(hls::stream<float24_t>& out, hls::stream<float24_t>& in) {

	int i, j, k, l, m;
	float24_t read;
	xf::cv::LineBuffer<POOL2_BUFFER_SIZE, 1, float24_t> pool_buff;

	for (i = 0; i < P2_SIZE; i++)
		pool_layer2_label28: for (l = 0; l < P2_KERNEL_SIZE; l++) {
			pool_layer2_label0: for (j = 0; j < P2_SIZE; j++)
				pool_layer2_label26: for (m = 0; m < P2_KERNEL_SIZE; m++)
					pool_layer2_label36: for (k = 0; k < P2_CHANNELS; k++) {
						in >> read;
						if (l == 0 && m == 0)
							pool_buff.val[j * P2_CHANNELS + k][0] = read;

						else
							pool_buff.val[j * P2_CHANNELS + k][0] =
									pool_buff.val[j * P2_CHANNELS + k][0]
											> read ?
											pool_buff.val[j * P2_CHANNELS + k][0] :
											read;

						if (l == (P2_KERNEL_SIZE - 1)
								&& m == (P2_KERNEL_SIZE - 1))
							out << pool_buff.val[j * P2_CHANNELS + k][0];
					}
			pool_layer2_label38: for (int skip = P2_SIZE * P2_STRIDE;
					skip < A2_SIZE; skip++)
				pool_layer2_label37: for (int channel = 0;
						channel < P2_CHANNELS; channel++)
					in >> read;
		}
	pool_layer2_label9: for (int skip_row = P2_SIZE * P2_STRIDE;
			skip_row < A2_SIZE; skip_row++)
		pool_layer2_label16: for (int skip_col = 0; skip_col < A2_SIZE;
				skip_col++)
			pool_layer2_label39: for (int skip_channel = 0;
					skip_channel < A2_CHANNELS; skip_channel++)
				in >> read;
}

void fc_layer1(hls::stream<float24_t> &out, hls::stream<float24_t> &in,
		float24_t weight[FC1_WEIGHTS_H][FC1_WEIGHTS_W],
		float24_t bias[FC1_BIAS_SIZE]) {
	float24_t read;
	float24_t output[FC1_ACT_SIZE] = { 0 };

	in >> read;
	for (int i = 0; i < FC1_WEIGHTS_W; i++)
		output[i] = weight[0][i] * read;

	fc_layer1_label12: for (int j = 1; j < FC1_WEIGHTS_H; j++) {
		in >> read;
		fc_layer1_label40: for (int i = 0; i < FC1_WEIGHTS_W; i++) {
			output[i] += weight[j][i] * read;
		}
	}
	fc_layer1_label15: for (int i = 0; i < FC1_WEIGHTS_W; i++)
		out << relu(output[i] + bias[i]);

}

void fc_layer2(hls::stream<float24_t> &out, hls::stream<float24_t> &in,
		float24_t weight[FC2_WEIGHTS_H][FC2_WEIGHTS_W],
		float24_t bias[FC2_BIAS_SIZE]) {
	float24_t read;
	float24_t output[FC2_ACT_SIZE] = { 0 };

	in >> read;
	for (int i = 0; i < FC2_WEIGHTS_W; i++)
		output[i] = weight[0][i] * read;

	fc_layer2_label13: for (int j = 1; j < FC2_WEIGHTS_H; j++) {
		in >> read;
		fc_layer2_label41: for (int i = 0; i < FC2_WEIGHTS_W; i++) {
			output[i] += weight[j][i] * read;
		}
	}
	fc_layer2_label11: for (int i = 0; i < FC2_WEIGHTS_W; i++)
		out << relu(output[i] + bias[i]);

}

void fc_layer3(hls::stream<float24_t> &out, hls::stream<float24_t> &in,
		float24_t weight[FC3_WEIGHTS_H][FC3_WEIGHTS_W],
		float24_t bias[FC3_BIAS_SIZE]) {
	float24_t read;
	float24_t output[FC3_ACT_SIZE] = { 0 };

	in >> read;
	for (int i = 0; i < FC3_WEIGHTS_W; i++)
		output[i] = weight[0][i] * read;

	fc_layer3_label10: for (int j = 1; j < FC3_WEIGHTS_H; j++) {
		in >> read;
		fc_layer3_label42: for (int i = 0; i < FC3_WEIGHTS_W; i++) {
			output[i] += weight[j][i] * read;
		}
	}
	fc_layer3_label14: for (int i = 0; i < FC3_WEIGHTS_W; i++)
		out << relu(output[i] + bias[i]);

}

void nnet(hls::stream<float24_t> &fc3_out) {
	hls::stream<float24_t> conv1_out("conv1_out");
	hls::stream<float24_t> conv2_out("conv2_out");
	hls::stream<float24_t> pool1_out("pool1_out");
	hls::stream<float24_t> pool2_out("pool2_out");
	hls::stream<float24_t> fc1_out("fc1_out");
	hls::stream<float24_t> fc2_out("fc2_out");
	hls::stream<float24_t> image_in("image_in");

	for (int i = 0; i < IMAGE_SIZE * IMAGE_SIZE * IMAGE_CHANNELS; i++)
		image_in << image[i];

	float24_t conv_layer1_bias[8] = { 0.382973, -0.516139, 0.479715, -0.405078, -0.535074, 0.055494, 0.004570, 0.520299};
	float24_t conv_layer1_weights[4][4][1][8] = { 0.127658, -0.172046, 0.159905, -0.135026, -0.178358, 0.018498, 0.001523, 0.173433, 0.069220, 0.174779, -0.154158, 0.144203, 0.165248, 0.085908, -0.038042, 0.030844, 0.144833, 0.039220, 0.112758, 0.193106, -0.178630, -0.067105, -0.113628, -0.073338, -0.188103, -0.176678, -0.070577, -0.003858, -0.157367, -0.063701, 0.009085, -0.079303, 0.155854, 0.123176, 0.203832, 0.065081, -0.152176, -0.126078, 0.065033, 0.106744, 0.103528, -0.034280, 0.183086, 0.159394, 0.170360, -0.095104, -0.132978, -0.185423, 0.199216, -0.188278, -0.110762, -0.202652, 0.041087, -0.101372, -0.017761, -0.118263, -0.149484, 0.044968, 0.082382, 0.028359, -0.098785, -0.165341, 0.189333, -0.117591, 0.181434, -0.184019, 0.158436, -0.108840, -0.022896, -0.150452, 0.173823, -0.063423, 0.199529, 0.141018, 0.109755, -0.087301, -0.012236, 0.138308, 0.113364, -0.058664, 0.102356, -0.045425, -0.057010, -0.202180, -0.132807, 0.201115, -0.100012, -0.069055, 0.072125, -0.174592, -0.180866, 0.188446, -0.013017, 0.059861, 0.187681, 0.145165, -0.141143, -0.016704, 0.105438, -0.099990, 0.130062, -0.089290, 0.166861, 0.073417, 0.034483, 0.130122, 0.119021, -0.019411, -0.139878, -0.096747, 0.084889, -0.092464, -0.087128, 0.133429, 0.065400, -0.111677, -0.015204, 0.103509, -0.031014, -0.099950, 0.181790, -0.102737, 0.063195, 0.091496, 0.123855, 0.070997, -0.008070, 0.151769};

	conv_layer1(conv1_out, image_in, conv_layer1_weights, conv_layer1_bias);
	pool_layer1(pool1_out, conv1_out);

	conv_layer2(conv2_out, pool1_out, conv_layer2_weights, conv_layer2_bias);
	pool_layer2(pool2_out, conv2_out);

	fc_layer1(fc1_out, pool2_out, fc_layer1_weights, fc_layer1_bias);
	fc_layer2(fc2_out, fc1_out, fc_layer2_weights, fc_layer2_bias);
	fc_layer3(fc3_out, fc2_out, fc_layer3_weights, fc_layer3_bias);
}
