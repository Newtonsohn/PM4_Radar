#ifndef CALC_H
#define CALC_H
//calc.h
#include <stdint.h>
#define BUFFER_SIZE 100  // Number of samples to fill
#define RMS_WINDOW_SIZE 10  // Number of RMS values to average for moving mean


float calculate_moving_mean(float new_value, int channel);
// Funktion zur Distanzberechnung
float calc_distance(uint16_t amplitude);
float calculate_distance(float rms);
void rms(uint32_t *results);
float calculate_angle(float distance_left, float distance_right);
float calculate_current(float rms);
float calculate_rms(uint16_t* samples, uint32_t num_samples);
//Filter
void init_bandpass_filter(void);
void process_signal(float32_t *input, float32_t *output, uint32_t block_size);
void filter_channels(float32_t *adc_samples, float32_t *channel1, float32_t *channel2);

#endif // CALC_H
