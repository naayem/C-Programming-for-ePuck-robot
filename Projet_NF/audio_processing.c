#include "ch.h"
#include "hal.h"
#include <main.h>
#include <chprintf.h>
#include <audio/microphone.h>
#include <audio_processing.h>
#include <communications.h>
#include <fft.h>
#include <arm_math.h>
#include <game_management.h>

#define MIN_VALUE_THRESHOLD	100000

#define MIN_FREQ			26	//we don't analyze before this index to not use resources for nothing
#define FREQ_PONG_INIT		28	//sol diese, la
#define FREQ_ALPHABET		30	//la diese
#define FREQ_LETTER_M		32	//si
#define FREQ_LETTER_O 		34  //do
#define FREQ_LETTER_N 		36  //do diese
#define FREQ_LETTER_D	 	38  //re
#define FREQ_LETTER_A	 	41  //re diese
#define FREQ_END_GAME		43	//mi
#define FREQ_ELIOT			45	//fa
#define MAX_FREQ			47	//we don't analyze after this index to not use resources for nothing

#define FREQ_END_GAME_L				(FREQ_END_GAME-1)//2984Hz
#define FREQ_END_GAME_H				(FREQ_END_GAME+1)//3028Hz
#define FREQ_PONG_INIT_L			(FREQ_PONG_INIT-1)
#define FREQ_PONG_INIT_H			(FREQ_PONG_INIT+1)
#define FREQ_ALPHABET_L				(FREQ_ALPHABET-1)
#define FREQ_ALPHABET_H				(FREQ_ALPHABET+1)
#define FREQ_LETTER_M_L				(FREQ_LETTER_M-1)
#define FREQ_LETTER_M_H				(FREQ_LETTER_M+1)
#define FREQ_LETTER_O_L				(FREQ_LETTER_O-1)
#define FREQ_LETTER_O_H				(FREQ_LETTER_O+1)
#define FREQ_LETTER_N_L				(FREQ_LETTER_N-1)
#define FREQ_LETTER_N_H				(FREQ_LETTER_N+1)
#define FREQ_LETTER_D_L				(FREQ_LETTER_D-1)
#define FREQ_LETTER_D_H				(FREQ_LETTER_D+1)
#define FREQ_LETTER_A_L				(FREQ_LETTER_A-1)
#define FREQ_LETTER_A_H				(FREQ_LETTER_A+1)
#define FREQ_ELIOT_L				(FREQ_ELIOT-1)
#define FREQ_ELIOT_H				(FREQ_ELIOT+1)

//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
static float micLeft_cmplx_input[2 * FFT_SIZE];
//Arrays containing the computed magnitude of the complex numbers
static float micLeft_output[FFT_SIZE];
//containing the maximum norm index to be able to verify the sound a second time
static int16_t max_norm_index = -1;
//contains the new letter to be written, to pass it on to game management
static lettre letter_state_new = AUCUN;
//use to make sure we are in the alphabet mode
static bool mode_alphabet_on = 0;

/*
*	Simple function used to detect the highest value in a buffer
*	and to execute a motor command depending on it
*/
void sound_remote(float* data){
	float max_norm = MIN_VALUE_THRESHOLD;
	int16_t max_norm_index_verify = -1;

	//search for the highest peak
	if (max_norm_index==-1){
		for(uint16_t i = MIN_FREQ ; i <= MAX_FREQ ; i++){
			if(data[i] > max_norm){
				max_norm = data[i];
				max_norm_index = i;
			}
		}
		return;
	}
	//double verification
	for(uint16_t j = MIN_FREQ ; j <= MAX_FREQ ; j++){
		if(data[j] > max_norm){
			max_norm = data[j];
			max_norm_index_verify = j;
		}
	}
	if (max_norm_index != max_norm_index_verify){
		max_norm_index = -1;
		return;
	}

	if(max_norm_index >= FREQ_END_GAME_L && max_norm_index <= FREQ_END_GAME_H){
		max_norm_index = -1;
		mode_alphabet_on = 0;
		state_compare(ENDGAME);
	}
	else if(max_norm_index >= FREQ_PONG_INIT_L && max_norm_index <= FREQ_PONG_INIT_H){
		max_norm_index = -1;
		state_compare(PONG_INIT);
	}
	else if(max_norm_index >= FREQ_ALPHABET_L && max_norm_index <= FREQ_ALPHABET_H){
		max_norm_index = -1;
		mode_alphabet_on = 1;
		letter_state_new = AUCUN;
		state_compare(ALPHABET);
	}
	else if(max_norm_index >= FREQ_LETTER_M_L && max_norm_index <= FREQ_LETTER_M_H){
		max_norm_index = -1;
		if (mode_alphabet_on && letter_state_new==AUCUN){
			letter_state_new = LETTRE_M;
		}
	}
	else if(max_norm_index >= FREQ_LETTER_O_L && max_norm_index <= FREQ_LETTER_O_H){
		max_norm_index = -1;
		if (mode_alphabet_on && letter_state_new==AUCUN){
			letter_state_new = LETTRE_O;
		}
	}
	else if(max_norm_index >= FREQ_LETTER_N_L && max_norm_index <= FREQ_LETTER_N_H){
		max_norm_index = -1;
		if (mode_alphabet_on && letter_state_new==AUCUN){
			letter_state_new = LETTRE_N;
		}
	}
	else if(max_norm_index >= FREQ_LETTER_D_L && max_norm_index <= FREQ_LETTER_D_H){
		max_norm_index = -1;
		if (mode_alphabet_on && letter_state_new==AUCUN){
			letter_state_new = LETTRE_D;
		}
	}
	else if(max_norm_index >= FREQ_LETTER_A_L && max_norm_index <= FREQ_LETTER_A_H){
		max_norm_index = -1;
		if (mode_alphabet_on && letter_state_new==AUCUN){
			letter_state_new = LETTRE_A;
		}
	}
	else if(max_norm_index >= FREQ_ELIOT_L && max_norm_index <= FREQ_ELIOT_H){
		max_norm_index = -1;
		if (mode_alphabet_on && letter_state_new==AUCUN){
			letter_state_new = ELIOT;
		}
	}
	else {
		max_norm_index = -1;
		letter_state_new = AUCUN;
	}
}

/*
*	Callback called when the demodulation of the four microphones is done.
*	We get 160 samples per mic every 10ms (16kHz)
*	
*	params :
*	int16_t *data			Buffer containing 4 times 160 samples. the samples are sorted by micro
*							so we have [micRight1, micLeft1, micBack1, micFront1, micRight2, etc...]
*	uint16_t num_samples	Tells how many data we get in total (should always be 640)
*/
void processAudioData(int16_t *data, uint16_t num_samples){
	/*
	*
	*	We get 160 samples per mic every 10ms
	*	So we fill the samples buffers to reach
	*	1024 samples, then we compute the FFTs.
	*
	*/
	static uint16_t nb_samples = 0;

	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4){
		//construct an array of complex numbers. Put 0 to the imaginary part
		micLeft_cmplx_input[nb_samples] = (float)data[i + MIC_LEFT];

		nb_samples++;

		micLeft_cmplx_input[nb_samples] = 0;

		nb_samples++;

		//stop when buffer is full
		if(nb_samples >= (2 * FFT_SIZE)){
			break;
		}
	}

	if(nb_samples >= (2 * FFT_SIZE)){
		/*	FFT proccessing
		*
		*	This FFT function stores the results in the input buffer given.
		*	This is an "In Place" function. 
		*/
		doFFT_optimized(FFT_SIZE, micLeft_cmplx_input);

		/*	Magnitude processing
		*
		*	Computes the magnitude of the complex numbers and
		*	stores them in a buffer of FFT_SIZE because it only contains
		*	real numbers.
		*
		*/
		arm_cmplx_mag_f32(micLeft_cmplx_input, micLeft_output, FFT_SIZE);

		nb_samples = 0;
		sound_remote(micLeft_output);
	}
}

float* get_audio_buffer_ptr(BUFFER_NAME_t name){
	if(name == LEFT_CMPLX_INPUT){
		return micLeft_cmplx_input;
	}
	else{
		return NULL;
	}
}

lettre get_letter_state(void){
	lettre letter_state_management = letter_state_new;
	letter_state_new = AUCUN;
	return letter_state_management;
}

_Bool letter_ready (void){
	if (letter_state_new != AUCUN){
		mode_alphabet_on = 0;
		return 1;
	}else return 0;
}

void next_letter (void){
	mode_alphabet_on = 1;
	letter_state_new = AUCUN;
}
