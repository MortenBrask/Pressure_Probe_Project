#ifndef PROBE_MEASUREMENT_COMMON_H_
#define PROBE_MEASUREMENT_COMMON_H_

#include "main.h"

#define MAX_ROC_SMOOTH 80
#define interval 1000
#define measurement_rate_hz 80.00
#define start_threshold 2.5

extern File test_measurement;                           //FILE to write the data  

extern int raw_average;                                 //raw average from HX711

extern float average_roc_result[];                      // the readings from the analog input

extern int actual_readings;                             //because of different load under wifi opertation etc, an actual measurements count is needed to get precise result 
extern int readIndex;                                   // the index of the current reading
extern float roc_total;                                 // the running total
extern float roc_average;                               //rate of change average
extern float roc_total_measurement_average;             // the average of rate of change
extern int roc_total_measurement_count;                 //counter from which the average is calculated.
extern float calculated_target_roc_pr_reading;          //the calculated target rate of change between each dataline in the average roc array 

extern float current_reading;                           //current HX711 reading
extern float previuos_reading;                          //previuos HX711 reading
extern float next_reading;                              //calculated next reding in order to comply with the set ROC(rate of change)

extern unsigned long start_time;
extern unsigned long time_now;
extern unsigned long time_previous;
extern unsigned long save_time_previous;

extern unsigned long dt;                                // dt in milliseconds

extern void initialize_variables();

#endif