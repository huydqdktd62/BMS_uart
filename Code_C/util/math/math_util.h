/*
 * match_util.h
 *
 *  Created on: Apr 18, 2022
 *      Author: quangnd
 */

#ifndef UTIL_MATH_MATH_UTIL_H_
#define UTIL_MATH_MATH_UTIL_H_
#include "stdint.h"

float get_min_f(float value1, float value2);
float get_max_f(float value1, float value2);
float absolute_f(float x);
float square_root_f(float x);
float cube_root_f(float x);
float diff_f(float x, float y);
float exponent_f(float x);
float sign_f(float value);
int32_t binary_search_f(float data, const float * const array, int32_t max_size);
uint16_t binary_search_int(const uint16_t data, const uint16_t *array,
                const uint16_t max_size);

#endif /* UTIL_MATH_MATH_UTIL_H_ */
