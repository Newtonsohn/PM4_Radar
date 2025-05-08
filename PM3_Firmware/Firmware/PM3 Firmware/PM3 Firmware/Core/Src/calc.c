/**
 ****************************************************************************
 * @file
 * @brief Functions for calculating values based on RMS measurements
 *
 * This file contains utility functions for processing RMS measurements
 * and calculating derived values such as distance and angle. These
 * calculations are based on signal analysis and predefined calibration
 * points.
 *
 * Features:
 * - Calculate moving mean for RMS values
 * - Compute RMS for left and right channels
 * - Interpolate distance from RMS values
 * - Determine angle based on distance measurements
 * - Simple current estimation based on RMS calibration
 *
 * Usage:
 * - Designed for use with ADC data buffers
 * - Supports calibration and configuration for specific hardware setups
 *
 *
 * ----------------------------------------------------------------------------
 * @author1  Hanspeter Hochreutener, hhrt@zhaw.ch
 * @author2  Rocheteau Thomas, linortho@students.zhaw.ch
 * @authot3  Roggensinger Lucas, roggeluc@students.zhaw.ch
 * @date	 26.12.2024
 *****************************************************************************/


/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"

#include "main.h"
#include "pushbutton.h"
#include "menu.h"
#include "measuring.h"
#include <string.h>
#include "calc.h"
#include <math.h>
#include "arm_math.h"
#include <stdio.h>



/******************************************************************************
 * Defines
 *****************************************************************************/



/******************************************************************************
 * Variables
 *****************************************************************************/



/******************************************************************************
 * Functions
 *****************************************************************************/


// Function to calculate the moving mean over the RMS values
static float rms_buffer_left[RMS_WINDOW_SIZE] = {0};
static float rms_buffer_right[RMS_WINDOW_SIZE] = {0};
static int rms_buffer_index_left = 0;
static int rms_buffer_index_right = 0;


/**
 ******************************************************************************
 * @brief Calculate the moving mean of a given channel
 *
 * This function computes the moving average of RMS values for a specified
 * channel using a circular buffer. It updates the buffer with the new value,
 * maintains the index for the next update, and returns the current mean.
 *
 * @param new_value The latest RMS value to be added to the buffer.
 * @param channel The channel to update and calculate the moving mean for:
 *                - 0: Left channel
 *                - 1: Right channel
 *
 * @return The moving mean as a float. Returns 0.0f for invalid channel input.
 *
 ******************************************************************************
 */
float calculate_moving_mean(float new_value, int channel) {
    float *buffer;
    int *index;

    // Select the appropriate buffer and index based on the channel
    if (channel == 0) {  // Left channel
        buffer = rms_buffer_left;
        index = &rms_buffer_index_left;
    } else if (channel == 1) {  // Right channel
        buffer = rms_buffer_right;
        index = &rms_buffer_index_right;
    } else {
        return 0.0f;  // Invalid channel
    }

    // Add the new value to the buffer
    buffer[*index] = new_value;

    // Increment the index in a circular manner
    *index = (*index + 1) % RMS_WINDOW_SIZE;

    // Calculate the mean
    float sum = 0;
    for (int i = 0; i < RMS_WINDOW_SIZE; i++) {
        sum += buffer[i];
    }

    return sum / RMS_WINDOW_SIZE;
}

/**
 ******************************************************************************
 * @brief Calculate the RMS values for left and right channels
 *
 * This function computes the Root Mean Square (RMS) values of the left and
 * right channels from the ADC sample buffer. The calculation removes the DC
 * offset and processes squared deviations for accurate results.
 *
 * @param results Pointer to an array where the calculated RMS values will
 *                be stored:
 *                - results[0]: RMS value for the left channel
 *                - results[1]: RMS value for the right channel
 ******************************************************************************
 */
void rms(uint32_t *results) {
    uint32_t sum_left = 0;
    uint32_t sum_right = 0;
    uint16_t adc_offset = 2048;  // DC offset in ADC units (1.65V for a 3.3V, 12-bit ADC)

    for (uint32_t i = 0; i < ADC_NUMS; i += 2) {
        // Remove offset from raw ADC values and square the result
        int32_t adjusted_left = (int32_t)ADC_samples[i] - adc_offset;
        int32_t adjusted_right = (int32_t)ADC_samples[i + 1] - adc_offset;

        sum_left += adjusted_left * adjusted_left;  // Sum of squared deviations
        sum_right += adjusted_right * adjusted_right;
    }

    // Compute RMS (in ADC units)
    results[0] = sqrt(sum_left / (ADC_NUMS / 2));
    results[1] = sqrt(sum_right / (ADC_NUMS / 2));
}



/**
 ******************************************************************************
 * @brief Calculate the current from the RMS value
 *
 * This function estimates the current in Amperes based on the input RMS value,
 * using linear interpolation between two calibration points: the RMS values
 * corresponding to 0 Amps and 5 Amps.
 *
 * @param rms The RMS value to be converted to current.
 * @return The calculated current in Amperes. Returns:
 *         - 0.0 if the RMS value is less than or equal to the calibration
 *           point for 0 Amps.
 *         - 5.0 if the RMS value is greater than or equal to the calibration
 *           point for 5 Amps.
 ******************************************************************************
 */
float calculate_current(float rms) {
    // Define calibration points: RMS at 0 Amps and RMS at 5 Amps
    const float rms_0_amps = 250;   // RMS value for 0 Amps
    const float rms_5_amps = 520;  // RMS value for 5 Amps

    // Ensure the RMS value is within the valid range
    if (rms <= rms_0_amps) {
        return 0.0; // Below or equal to 0 Amps
    } else if (rms >= rms_5_amps) {
        return 5.0; // Above or equal to 5 Amps
    }

    // Linearly interpolate the current
    return 5.0 * (rms - rms_0_amps) / (rms_5_amps - rms_0_amps);
}


/**
 ******************************************************************************
 * @brief Calculate the distance from the RMS value
 *
 * This function estimates the distance in centimeters based on the input RMS
 * value, using linear interpolation between two calibration points: the RMS
 * values corresponding to 0 cm and 10 cm.
 *
 * @param rms The RMS value to be converted to distance.
 * @return The calculated distance in centimeters. Returns:
 *         - 0.0 if the RMS value is greater than the calibration point for 0 cm.
 *         - 10.0 if the RMS value is less than the calibration point for 10 cm.
 ******************************************************************************
 */
float calculate_distance(float rms) {
    // Define calibration points: {RMS at 0 cm, RMS at 10 cm}
    const float rms_0_cm = 950;  // RMS value at 0 cm
    const float rms_10_cm = 400; // RMS value at 10 cm

    // Ensure the RMS value is within the valid range
    if (rms > rms_0_cm) {
        return 0.0; // Closer than 0 cm
    } else if (rms < rms_10_cm) {
        return 10.0; // Farther than 10 cm
    }

    // Linearly interpolate the distance
    return 10.0 * (rms_0_cm - rms) / (rms_0_cm - rms_10_cm);
}


/**
 ******************************************************************************
 * @brief Calculate the angle between two distances
 *
 * This function computes the angle in degrees based on the difference between
 * two distances (left and right) and a fixed baseline distance between copper pads.
 *
 * @param distance_left The measured distance to the left pad in centimeters.
 * @param distance_right The measured distance to the right pad in centimeters.
 * @return The calculated angle in degrees.
 ******************************************************************************
 */
float calculate_angle(float distance_left, float distance_right) {
    const float BASELINE = 5.0;  // Fixed distance between copper pads in cm

    // Calculate the angle in radians
    float angle_radians = atan((distance_left - distance_right) / BASELINE);

    // Convert to degrees
    float angle_degrees = angle_radians * (180.0 / M_PI);

    return angle_degrees;
}
