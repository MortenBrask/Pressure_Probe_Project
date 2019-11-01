#ifndef PROBE_STATE_MACHINE_H_
#define PROBE_STATE_MACHINE_H_

#include "main.h"

typedef enum {
    SS_CONFIG,
    SS_TEST
}SS_PROBE_SUPER_STATE;

typedef enum {
    S_HOME,
    S_SETTINGS,    
    S_CALIBRATION,    
    S_RAW_CAPTURE,
    S_SAVE,
    S_VIDEO,
    S_MEASURE,
    S_VAS,
    S_TEMP,
    S_COLD,
    S_NA        
}S_PROBE_STATE;

typedef enum {
    E_req_home,
    E_req_settings,
    E_req_calibration,
    E_req_raw,
    E_req_save,
    E_req_video,
    E_req_measure,
    E_req_vas,
    E_req_temp,
    E_req_cold,
    E_no_event,
    E_ws_connected,
    E_start,
    E_stop,
    E_set,    
    E_reset,
    E_plus,
    E_minus,
    E_raw_max_on,
    E_raw_roc_on,
    E_rx_id,
    E_rx_vas,
    E_rx_temp,
    E_rx_calibration_factor,
    E_rx_weight,
    E_rx_settings,    
    E_tx_measure,
    E_tx_sub,
    E_error
}E_PROBE_EVENT;

typedef enum {
    socket_RX,
    socket_TX
}SOCKET_TRX_TYPE;

typedef enum {
    START,
    STOP,
    SET,
    RESET,    
    DATA_PROBE_SETTINGS,
    DATA_TARGET_ROC,
    DATA_UNIQUE_ID,
    DATA_VAS_FREQ,    
    DATA_MAX_PRESSURE,   
    DATA_PROBE_MEASUREMENT,
    DATA_VAS_MEASUREMENT,
    DATA_CALIBRATION_SETTINGS,
    DATA_CALIBRATION_WEIGHT,
    DATA_CALIBRATION_FACTOR    
}SOCKET_MSG_TYPE;

typedef struct {
    float measurement;
    //int vas;    
}MEASUREMENT;

typedef struct {
    float target_roc;                //target rate of change value    
    uint32_t unique_id;
    uint32_t prev_unique_id;    
    int vas_freq;                    //frequency interval to store VAS data                       
    int max_pressure;
}PROBE_USER_SETTINGS;

typedef struct {
    float known_weight;
    float calibration_factor;        //a factor obtained by calibrating, default 500 
}PROBE_CALIBRATION_SETTINGS;

typedef struct {
    int current_test;
    int current_sub;
    int prev_test;
    int prev_sub;
}PROBE_TEST_PROGRESS;

typedef struct { 
    SOCKET_MSG_TYPE msg_type;
    PROBE_USER_SETTINGS user_settings;
    PROBE_CALIBRATION_SETTINGS calibration_data;
    MEASUREMENT measurement_data;            
}SOCKET_TRX_DATA;

typedef struct {
    SOCKET_TRX_TYPE socket_trx_type;        
    SOCKET_TRX_DATA socket_rx_data;
    SOCKET_TRX_DATA socket_tx_data;
}PROBE_TRX;

typedef struct {
    PROBE_USER_SETTINGS user_settings;    
    PROBE_CALIBRATION_SETTINGS calibration_settings;
    PROBE_TEST_PROGRESS probe_test_progress;
    PROBE_TRX trx_data;
}PROBE_DATA;

typedef enum {
    TEST_1,
    TEST_2,
    TEST_3,
    TEST_4,
    TEST_5,
    TEST_6,
    TEST_7,
    TEST_8,
    TEST_FINISH
}TEST_ROUTINE;

typedef enum {
    SUB_ROUTINE_0,
    SUB_ROUTINE_1,
    SUB_ROUTINE_2,
    SUB_ROUTINE_3    
}TEST_SUB;

typedef struct {
    TEST_ROUTINE test;
    TEST_SUB sub;
    int raw_max_on;
    int raw_roc_on;
    int adjust;
    int resume;
    int once;
}TEST_PROGRESS;

typedef void (*S_PROBE_STATE_pointer)();

typedef struct {    
    S_PROBE_STATE next_state;
}S_VALID_NEXT_STATE;

typedef struct {
    TEST_SUB sub_routine;
    S_PROBE_STATE probe_test_state;
}S_TEST_REQUEST_MATRIX;

typedef struct {
    S_PROBE_STATE next_state;
    S_PROBE_STATE_pointer state_pointer;
}S_PROBE_STATE_ELEMENT;

extern const S_PROBE_STATE_ELEMENT probe_state_matrix[10];
extern const S_VALID_NEXT_STATE valid_next_state[10][10];
extern const S_TEST_REQUEST_MATRIX test_request_matrix[9][4];
extern const String link_strings[8];
extern const String video_strings[8];
extern const String ld_select_strings[8];
extern const String check_mark[2];
extern const char *test_1_files[3];
extern const char *test_2_files[3];
extern const char *files[8];

extern TEST_PROGRESS test_flags;

extern volatile SS_PROBE_SUPER_STATE probe_super_state;
extern volatile S_PROBE_STATE probe_state;
extern volatile E_PROBE_EVENT probe_event;

extern volatile S_PROBE_STATE_pointer probe_state_pointer;

extern void load_probe_configuration();
extern void save_probe_configuration();

extern void init_calibration();
extern void calibrate_hx711();
extern void fine_tune_hx711();

extern void initialize_variables();

extern void probe_measurement_init();
extern void probe_measurement();
extern void probe_measurement_finish();

extern void probe_vas_measurement_init();
extern void probe_vas_measurement();
extern void probe_vas_measurement_finish();

extern void probe_cold_pressure_init();
extern void probe_cold_pressure_measurement();
extern void probe_cold_pressure_finish();

extern void S_home();
extern void S_probe_settings();
extern void S_calibration();
extern void S_video();
extern void S_probe_measurement();
extern void S_vas_measurement();
extern void S_raw_capture();
extern void S_reset();

extern PROBE_DATA configuration_data;                   //the basis for the probe to work...

#endif