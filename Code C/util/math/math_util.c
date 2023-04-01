/*
 * match_util.c
 *
 *  Created on: Apr 18, 2022
 *      Author: quangnd
 */

#include <math_util.h>

float get_min_f(float value1, float value2) {
        float min;
        min = (value1 < value2) ? value1 : value2;
        return min;
}

float get_max_f(float value1, float value2) {
        float max;
        max = (value1 > value2) ? value1 : value2;
        return max;
}

int32_t binary_search_f(float data, const float * const array, int32_t max_size) {

        int32_t lower_bound = 0;
        int32_t upper_bound = max_size - 1;
        int32_t mid_point = -1;
        int32_t index = -1;

        while (lower_bound <= upper_bound) {
                // compute the mid point
                // midPoint = (lowerBound + upperBound) / 2;
        	/*
        		* If the decimal value of the number provided is less than
        		* or equal to 0.5 then the value returned will be the integer
        		* smaller than the number provided in the argument.
        	*/
                mid_point =(int32_t)(lower_bound + (upper_bound - lower_bound) / 2);
                // data found
                if (array[mid_point] == data) {
                        index = mid_point;
                        break;
                }
                else {
                        // if data is larger
                        if (array[mid_point] > data) {
                                // data is in upper half
                                lower_bound = (int32_t)(mid_point + 1u);
                        }
                        // data is smaller
                        else {
                                // data is in lower half
                                upper_bound =(int32_t) (mid_point - 1u);
                        }
                }
        }
        if (lower_bound > upper_bound) {
                index = (int32_t)lower_bound;
        }
        return (uint32_t)index;

}

uint16_t binary_search_int(const uint16_t data, const uint16_t *array,
                const uint16_t max_size) {

        int16_t lower_bound = 0;
        int16_t upper_bound = max_size - 1;
        int16_t mid_point = -1;
        int16_t index = -1;

        while (lower_bound <= upper_bound) {
                // compute the mid point
                // midPoint = (lowerBound + upperBound) / 2;
        	/*
        		* If the decimal value of the number provided is less than
        		* or equal to 0.5 then the value returned will be the integer
        		* smaller than the number provided in the argument.
        	*/
                mid_point =(int16_t)(lower_bound + (upper_bound - lower_bound) / 2);
                // data found
                if (array[mid_point] == data) {
                        index = mid_point;
                        break;
                }
                else {
                        // if data is larger
                        if (array[mid_point] > data) {
                                // data is in upper half
                                lower_bound = (int16_t)(mid_point + 1u);
                        }
                        // data is smaller
                        else {
                                // data is in lower half
                                upper_bound =(int16_t) (mid_point - 1u);
                        }
                }
        }
        if (lower_bound > upper_bound) {
                index = (int16_t)lower_bound;
        }
        return (uint16_t)index;
}


float absolute_f(float x) {
        if (x < 0)
                x = -x;
        return x;
}

float square_root_f(float x) {
        float guess = 1;

        while (absolute_f((guess * guess) / x - 1) >= 0.0001)
                guess = ((x / guess) + guess) / 2;

        return guess;
}

float exponent_f(float x) {
        return (1 + x + x * x / 2);             // Taylor approximate
}

float sign_f(float value) {

        if(value <0.0f) return (-1.0f);
        if(value >0.0f) return (1.0f);
        return 0.0f;
}

float diff_f(float x, float y) {
        if (x > (y * y * y))
                return (x - (y * y * y));
        else
                return ((y * y * y) - x);
}

float cube_root_f(float x) {
        float start = 0, end = x;
        float mid = (start + end) / 2;
        float error = diff_f(x, mid);
        if (x > 0) {
                while (error > 0.001) {
                        // If mid*mid*mid is greater than x set
                        // end = mid
                        if ((mid * mid * mid) > x) {
                                end = mid;
                        }
                        // If mid*mid*mid is less than x set
                        // start = mid
                        else {
                                start = mid;
                        }
                        mid = (start + end) / 2;
                        error = diff_f(x, mid);
                }
                // If error is less than e then mid is
                // our answer so return mid
                if (error <= 0.001) {
                        return mid;
                }
        }
        else {
                while (error > 0.001) {
                        // If mid*mid*mid is greater than x set  end = mid
                        if ((mid * mid * mid) < x) {
                                end = mid;
                        }
                        // If mid*mid*mid is less than x set start = mid
                        else {
                                start = mid;
                        }
                        mid = (start + end) / 2;
                        error = diff_f(x, mid);
                }
                // If error is less than e then mid is our answer so return mid
                if (error <= 0.001) {
                        return mid;
                }
        }
        return mid;
}

