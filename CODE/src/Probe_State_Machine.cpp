#include "Probe_State_Machine.h"

void S_home();
void S_probe_settings();
void S_calibration();
void S_raw_capture();
void S_save();

void S_video();
void S_probe_measurement();
void S_vas_measurement();
void S_vas_temp();
void S_cold_pressure();

void S_error_progress_handler();


const S_PROBE_STATE_ELEMENT probe_state_matrix[10] = {
    /*S_HOME*/                      {S_HOME, S_home},
    /*S_SETTINGS*/                  {S_SETTINGS, S_probe_settings},
    /*S_CALIBRATION */              {S_CALIBRATION, S_calibration},	
    /*S_RAW_CAPTURE */              {S_RAW_CAPTURE, S_raw_capture},
    /*S_RESET */                    {S_SAVE, S_save},
    /*S_VIDEO */                    {S_VIDEO, S_video},
    /*S_PROBE_MEASUREMENT */        {S_MEASURE, S_probe_measurement},
    /*S_VAS_MEASUREMENT */          {S_VAS, S_vas_measurement},
    /*S_VAS_TEMP */                 {S_TEMP, S_vas_temp},
    /*S_COLD_PRESSURE */            {S_COLD, S_cold_pressure}        
};

const S_VALID_NEXT_STATE valid_next_state[10][10] = {
    /*S_HOME*/                      {{S_HOME},{S_SETTINGS},{S_CALIBRATION},{S_RAW_CAPTURE},{S_SAVE},{S_VIDEO},{S_HOME},{S_HOME},{S_HOME},{S_HOME}},
    /*S_SETTINGS*/                  {{S_HOME},{S_SETTINGS},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME}},
    /*S_CALIBRATION*/               {{S_HOME},{S_HOME},{S_CALIBRATION},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME}},
    /*S_RAW_CAPTURE*/               {{S_HOME},{S_HOME},{S_HOME},{S_RAW_CAPTURE},{S_SAVE},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME}},
    /*S_SAVE*/                      {{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_SAVE},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME}},
    /*S_VIDEO*/                     {{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_VIDEO},{S_MEASURE},{S_VAS},{S_TEMP},{S_COLD}},
    /*S_MEASURE*/                   {{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_VIDEO},{S_MEASURE},{S_HOME},{S_HOME},{S_HOME}},
    /*S_VAS*/                       {{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_VIDEO},{S_HOME},{S_VAS},{S_HOME},{S_HOME}},
    /*S_TEMP*/                      {{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_SAVE},{S_VIDEO},{S_HOME},{S_HOME},{S_TEMP},{S_HOME}},
    /*S_COLD*/                      {{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_HOME},{S_VIDEO},{S_HOME},{S_HOME},{S_HOME},{S_COLD}}
};

const S_TEST_REQUEST_MATRIX test_request_matrix[9][4] = {
    /*TEST_1 */                     {{SUB_ROUTINE_0, S_VIDEO},{SUB_ROUTINE_1, S_MEASURE},{SUB_ROUTINE_2, S_MEASURE},{SUB_ROUTINE_3, S_MEASURE}},
    /*TEST_2 */                     {{SUB_ROUTINE_0, S_VIDEO},{SUB_ROUTINE_1, S_MEASURE},{SUB_ROUTINE_2, S_MEASURE},{SUB_ROUTINE_3, S_MEASURE}},
    /*TEST_3 */                     {{SUB_ROUTINE_0, S_VIDEO},{SUB_ROUTINE_1, S_VAS},{SUB_ROUTINE_2, S_NA},{SUB_ROUTINE_3, S_NA}},
    /*TEST_4 */                     {{SUB_ROUTINE_0, S_VIDEO},{SUB_ROUTINE_1, S_VAS},{SUB_ROUTINE_2, S_NA},{SUB_ROUTINE_3, S_NA}},
    /*TEST_5 */                     {{SUB_ROUTINE_0, S_VIDEO},{SUB_ROUTINE_1, S_TEMP},{SUB_ROUTINE_2, S_NA},{SUB_ROUTINE_3, S_NA}},
    /*TEST_6 */                     {{SUB_ROUTINE_0, S_VIDEO},{SUB_ROUTINE_1, S_TEMP},{SUB_ROUTINE_2, S_NA},{SUB_ROUTINE_3, S_NA}},
    /*TEST_7 */                     {{SUB_ROUTINE_0, S_VIDEO},{SUB_ROUTINE_1, S_COLD},{SUB_ROUTINE_2, S_COLD},{SUB_ROUTINE_3, S_COLD}},
    /*TEST_8 */                     {{SUB_ROUTINE_0, S_VIDEO},{SUB_ROUTINE_1, S_TEMP},{SUB_ROUTINE_2, S_NA},{SUB_ROUTINE_3, S_NA}},
    /*TEST_FINISH*/                 {{SUB_ROUTINE_0, S_SAVE},{SUB_ROUTINE_1, S_NA},{SUB_ROUTINE_2, S_NA},{SUB_ROUTINE_3, S_NA}}
};

const String link_strings[8] = {"measure.html","measure.html","vas.html","vas.html","summation.html","summation.html","coldpress.html","summation.html"};
const String video_strings[8] = {"Pain Threshold","Pain Threshold","VAS","VAS","Temporal Summation","Temporal Summation","Cold Presssure","Temporal Summation"};
const String ld_select_strings[8] = {"Local","Distal","Local","Distal","Local","Distal","",""};
const String check_mark[2] = {"\"fa fa-check-circle\"","\"fa fa-times-circle\""};
const char *test_1_files[3] = {
    "/measure_1_sub_1.csv",
    "/measure_1_sub_2.csv",
    "/measure_1_sub_3.csv"
    };
const char *test_2_files[3] = {
    "/measure_2_sub_1.csv",
    "/measure_2_sub_2.csv",
    "/measure_2_sub_3.csv"
    };
const char *files[8] = {
    "/test_1_sub_1.csv",    
    "/test_2_sub_1.csv",    
    "/test_3_sub_1.csv",
    "/test_4_sub_1.csv",
    "/test_5_sub_1.csv",
    "/test_6_sub_1.csv",
    "/test_7_sub_1.csv",
    "/test_8_sub_1.csv"
    };

volatile SS_PROBE_SUPER_STATE probe_super_state = SS_CONFIG;
volatile S_PROBE_STATE probe_state = S_HOME;
volatile E_PROBE_EVENT probe_event = E_no_event;
volatile S_PROBE_STATE_pointer probe_state_pointer = S_home;

TEST_PROGRESS test_flags = {
    .test = TEST_1,
    .sub = SUB_ROUTINE_1,
    .raw_max_on = 0,
    .raw_roc_on = 0,
    .adjust = 0,
    .resume = 0,
    .once = 0
};

void write_test_progress(){    
    display.printf("TEST %i SUB %i", test_flags.test + 1, test_flags.sub);
}

void S_home(){  
    
    switch(probe_event){
        case E_no_event:

            if(test_flags.once == 0){         
                
                display.clearDisplay();
                display.fillRect(0,0,display.width(),10,WHITE);
                display.setTextColor(BLACK,WHITE);
                display.setTextSize(1);
                    
                display.setCursor(25, 1);
                display.println("Pressure Probe");                
                display.setTextColor(WHITE);
                display.setCursor(0, 14);
                display.println("");
                display.println("SERVER IP:");
                display.println("");
                display.println(WiFi.localIP());
                
                display.display();
                test_flags.once = 1;
            }
            break;
        case E_start:
            if(test_flags.once == 0){
                uint32_t rid = esp_random();
                
                configuration_data.user_settings.prev_unique_id = configuration_data.user_settings.unique_id;
                configuration_data.user_settings.unique_id = rid;
                configuration_data.probe_test_progress.prev_test = TEST_1;
                configuration_data.probe_test_progress.prev_sub = SUB_ROUTINE_0;

                //create JSON document with space for 3 field
                const size_t capacity = JSON_OBJECT_SIZE(3);
                DynamicJsonDocument doc(capacity);
                //insert the field value
                doc["type"] = "id";
                doc["id"] = configuration_data.user_settings.unique_id;
                doc["prev_id"] = configuration_data.user_settings.prev_unique_id;
                String json_id;

                serializeJson(doc, json_id); 
                webSocket.broadcastTXT(json_id);

                save_probe_configuration();

                probe_event = E_no_event;
            }            
            break;
        case E_reset:
            if(test_flags.once == 0){
                WiFi.disconnect(true, true);
                delay(500);
                test_flags.once = 1;
                probe_event = E_no_event;
                ESP.restart();                
            }
            break;
        case E_ws_connected:
            if(test_flags.once == 0){
                probe_event = E_no_event;
                //create JSON document with space for 3 field
                const size_t capacity = JSON_OBJECT_SIZE(3);
                DynamicJsonDocument doc(capacity);
                //insert the field value
                doc["type"] = "id";
                doc["id"] = configuration_data.user_settings.unique_id;
                doc["prev_id"] = configuration_data.user_settings.prev_unique_id;
                String json_id;

                serializeJson(doc, json_id); 
                webSocket.broadcastTXT(json_id);
                delay(100);
                test_flags.once = 1;
            }                                
            break;
        case E_rx_id:
            if(test_flags.once == 0){
                configuration_data.user_settings.unique_id = configuration_data.trx_data.socket_rx_data.user_settings.unique_id;
                test_flags.once = 1;
                probe_event = E_no_event;                
            }                
            break;
        default:
            break;
    }
    return;    
}

void S_probe_settings(){
    
    switch(probe_event){
        case E_no_event:
            if(test_flags.once == 0){
                
                display.clearDisplay();
                display.fillRect(0,0,display.width(),12,WHITE);
                display.setTextColor(BLACK,WHITE);
                display.setTextSize(1);                    
                display.setCursor(25, 2);
                display.println("Pressure Probe");                
                display.setTextColor(WHITE);
                display.setCursor(0, 14);               
                display.println("");
                display.println("Settings");
                display.println("");
                display.println("SERVER IP:");
                display.println("");
                display.println(WiFi.localIP());
                
                display.display();
                delay(10);
                test_flags.once = 1;
            }            
            break;
        case E_ws_connected:
            if(test_flags.once == 0){
                //create JSON document with space for 2 field
                const size_t capacity = JSON_OBJECT_SIZE(4);
                DynamicJsonDocument doc(capacity);
                //insert the field value
                doc["type"] = "settings";
                doc["target_roc"] = configuration_data.user_settings.target_roc;
                doc["max_pressure"] = configuration_data.user_settings.max_pressure;
                doc["vas_freq"] = configuration_data.user_settings.vas_freq;
                String json_settings;

                serializeJson(doc, json_settings); 
                webSocket.broadcastTXT(json_settings);
                test_flags.once = 1;
            }
        case E_rx_settings:
            if(test_flags.once == 0){
                save_probe_configuration();
                test_flags.once = 1;
            }           
            break;        
        default:            
            break;
    } 
}

void S_video(){

    switch(probe_event){
        case E_no_event:

            if(test_flags.once == 0){
                
                display.clearDisplay();
                display.fillRect(0,0,display.width(),12,WHITE);
                display.setTextColor(BLACK,WHITE);
                display.setTextSize(1);                    
                display.setCursor(25, 2);
                display.println("Pressure Probe");
                display.setTextColor(WHITE);
                display.setCursor(0, 14);
                write_test_progress();
                display.println("");
                display.println("Video instruction");
                display.println("");
                display.println("SERVER IP:");
                display.println("");
                display.println(WiFi.localIP());
                display.display();
                delay(10);
                save_probe_configuration();
                test_flags.once = 1;
            }
            break;
        default:
            break;
    }
    return;    
}

void S_probe_measurement(){    
    switch(probe_event){
        case E_no_event:
            if(test_flags.once == 0){
                                
                display.clearDisplay();
                display.fillRect(0,0,display.width(),12,WHITE);
                display.setTextColor(BLACK,WHITE);
                display.setTextSize(1);
                    
                display.setCursor(25, 2);
                display.println("Pressure Probe");
                
                display.setTextColor(WHITE);
                display.setCursor(0, 14);
                
                write_test_progress();
                display.println("");
                display.println("Measurement");
                display.println("");
                display.println("SERVER IP:");
                display.println("");
                display.println(WiFi.localIP());
                
                display.display();
                
                delay(10);

                test_flags.once = 1;                
            }
            break;
        case E_start:
            if(test_flags.once == 0){
                
                Serial.println("Measurement init");
                probe_measurement_init();
                test_flags.once = 1;
            }
                        
            probe_measurement();
            break;
        case E_stop:
            if(test_flags.once == 0){
                if(test_flags.sub < SUB_ROUTINE_3){
                    test_flags.sub = static_cast<TEST_SUB>(static_cast<int>(test_flags.sub) + 1);
                }
                probe_measurement_finish();
                test_flags.once = 1;
            }
            break;
        case E_reset:
            if(test_flags.once == 0){
                test_flags.sub = static_cast<TEST_SUB>(static_cast<int>(test_flags.sub) - 1);
                test_flags.once = 1;
            }
            break;
        default:
            break;
    } 
    return;
}

void S_calibration(){
    
    switch(probe_event){
        case E_no_event:            
            if(test_flags.once == 0){
                                
                display.clearDisplay();
                display.fillRect(0,0,display.width(),9,WHITE);
                display.setTextColor(BLACK,WHITE);
                display.setTextSize(1);
                    
                display.setCursor(25, 1);
                display.println("Pressure Probe");
                
                display.setTextColor(WHITE);
                
                display.setCursor(35, 11);
                display.println("Calibration");
                
                display.setCursor(0, 48);
                display.println("SERVER IP:");
                display.println(WiFi.localIP());
                
                display.display();
                delay(10);

                test_flags.once = 1;                
            }
            break;
        case E_ws_connected:
            if(test_flags.once == 0){
                //create JSON document with space for 2 field
                const size_t capacity = JSON_OBJECT_SIZE(2);
                DynamicJsonDocument doc(capacity);
                //insert the field value
                doc["type"] = "factor";
                doc["factor"] = configuration_data.calibration_settings.calibration_factor;
                String json_factor;

                serializeJson(doc, json_factor); 
                webSocket.broadcastTXT(json_factor);

                DynamicJsonDocument doc_2(capacity);
                //insert the field value
                doc_2["type"] = "weight";
                doc_2["weight"] = configuration_data.calibration_settings.known_weight;
                String json_weight;

                serializeJson(doc_2, json_weight); 
                webSocket.broadcastTXT(json_weight);
                test_flags.once = 1;
            }
            break;
        case E_start:
            if(test_flags.once == 0){
                
                Serial.println("Calibration init");
                init_calibration();
                test_flags.once = 1;
            }           
            break;
        case E_set:
            if(test_flags.once == 0){
                
                Serial.println("Calibration init");
                calibrate_hx711();
                test_flags.once = 1;
            }                        
            fine_tune_hx711();
            break;
        case E_plus:
            if(test_flags.once == 0){
                test_flags.adjust = 1;
                probe_event = E_set;
                test_flags.once = 1;
            }            
            break;
        case E_minus:
            if(test_flags.once == 0){
                test_flags.adjust = 2;
                probe_event = E_set;
                test_flags.once = 1;
            } 
            break;
        case E_rx_weight:
            if(test_flags.once == 0){
                configuration_data.calibration_settings.known_weight = configuration_data.trx_data.socket_rx_data.calibration_data.known_weight;
                probe_event = E_set;
                test_flags.once = 1;
            }
            break;
        case E_rx_calibration_factor:
            if(test_flags.once == 0){
                configuration_data.calibration_settings.calibration_factor = configuration_data.trx_data.socket_rx_data.calibration_data.calibration_factor;
                probe_event = E_set;
                test_flags.once = 1;
            }
            break;
        case E_stop:
            if(test_flags.once == 0){
                save_probe_configuration();                
                test_flags.once = 1;
            }
            break;
        case E_reset:
            if(test_flags.once == 0){
                probe_event = E_no_event;
                test_flags.once = 0;
            }
            break;
        default:            
            break;
    }
    return;
}

void S_vas_measurement(){
    
    switch(probe_event){
        case E_no_event:
            if(test_flags.once == 0){
                                
                display.clearDisplay();
                display.fillRect(0,0,display.width(),12,WHITE);
                display.setTextColor(BLACK,WHITE);
                display.setTextSize(1);
                    
                display.setCursor(25, 2);
                display.println("Pressure Probe");
                
                display.setTextColor(WHITE);
                display.setCursor(0, 14);
                write_test_progress();
                display.println("");
                display.println("VAS");
                display.println("");
                display.println("SERVER IP:");
                display.println("");
                display.println(WiFi.localIP());
                
                display.display();
                delay(10);

                test_flags.once = 1;                
            }
            break;
        case E_start:
            if(test_flags.once == 0){
                
                Serial.println("VAS init");
                probe_vas_measurement_init();
                test_flags.once = 1;
            }
            break;
        case E_stop:
            if(test_flags.once == 0){
                if(test_flags.sub < SUB_ROUTINE_2){
                    test_flags.sub = static_cast<TEST_SUB>(static_cast<int>(test_flags.sub) + 1);
                }
    #ifdef PROBE_DEBUG
                Serial.println("VAS stop finish");
    #endif
                
                test_flags.once = 1;
            }
            break;
        case E_rx_vas:
            if(test_flags.once == 0){
                probe_vas_measurement_finish();
    #ifdef PROBE_DEBUG
                Serial.println("VAS rx finish");
    #endif
                test_flags.once = 1;
                probe_event = E_no_event;
            }
            break;
        case E_reset:
            if(test_flags.once == 0){
                test_flags.sub = static_cast<TEST_SUB>(static_cast<int>(test_flags.sub) - 1);
                test_flags.once = 1;
            }
            break;
        default:
            break;
    } 
    return;
}

void S_vas_temp(){
    
    switch(probe_event){
        case E_no_event:            

            if(test_flags.once == 0){
                                
                display.clearDisplay();
                display.fillRect(0,0,display.width(),12,WHITE);
                display.setTextColor(BLACK,WHITE);
                display.setTextSize(1);
                    
                display.setCursor(25, 2);
                display.println("Pressure Probe");
                
                display.setTextColor(WHITE);
                display.setCursor(0, 14);
                
                write_test_progress();
                display.println("");
                display.println("VAS Summation");
                display.println("");
                display.println("SERVER IP:");
                display.println("");
                display.println(WiFi.localIP());
                
                display.display();
                delay(10);

                test_flags.once = 1;                
            }
            break;
        case E_ws_connected:
            if(test_flags.once == 0){
                //create JSON document with space for 2 field
                const size_t capacity = JSON_OBJECT_SIZE(2);
                DynamicJsonDocument doc(capacity);
                //insert the field value
                doc["type"] = "vas_freq";
                doc["vas_freq"] = configuration_data.user_settings.vas_freq;
                String json_freq;

                serializeJson(doc, json_freq); 
                webSocket.broadcastTXT(json_freq);
                test_flags.once = 1;
            }
            break;
        case E_start:
            if(test_flags.once == 0){
                
                Serial.println("VAS init");
                probe_vas_measurement_init();
                test_flags.once = 1;
            }            
            break;
        case E_stop:
            if(test_flags.once == 0){
                if(test_flags.sub < SUB_ROUTINE_2){
                    test_flags.sub = static_cast<TEST_SUB>(static_cast<int>(test_flags.sub) + 1);
                }
                Serial.println("VAS finish");
                
                probe_vas_measurement_finish();
                test_flags.once = 1;
            }
            break;
        case E_rx_vas:
            if(test_flags.once == 0){
                probe_vas_measurement();
                test_flags.once = 1;
            }
            break;
        case E_reset:
            if(test_flags.once == 0){
                test_flags.sub = static_cast<TEST_SUB>(static_cast<int>(test_flags.sub) - 1);
                test_flags.once = 1;
            }
            break;
        default:            
            break;
    } 
    return;
}

void S_raw_capture(){
    
    switch(probe_event){
        case E_no_event:
            
            if(test_flags.once == 0){
                                
                display.clearDisplay();
                display.fillRect(0,0,display.width(),12,WHITE);
                display.setTextColor(BLACK,WHITE);
                display.setTextSize(1);
                    
                display.setCursor(25, 2);
                display.println("Pressure Probe");
                
                display.setTextColor(WHITE);
                display.setCursor(0, 14);
                
                display.println("");
                display.println("RAW DATA CAPTURE");
                display.println("");
                display.println("SERVER IP:");
                display.println("");
                display.println(WiFi.localIP());
                
                display.display();
                delay(10);

                test_flags.once = 1;                
            }
            break;
        case E_start:
            if(test_flags.once == 0){
                
                Serial.println("Raw capture init");
                probe_measurement_init();
                test_flags.once = 1;
            }                        
            probe_measurement();
            break;
        case E_stop:
            if(test_flags.once == 0){
                                
                probe_measurement_finish();
                test_flags.once = 1;
            }
            break;
        default:            
            break;
    } 
    return;
}

void S_cold_pressure(){
    
    switch(probe_event){
        case E_no_event:
            
            if(test_flags.once == 0){
                                
                display.clearDisplay();
                display.fillRect(0,0,display.width(),12,WHITE);
                display.setTextColor(BLACK,WHITE);
                display.setTextSize(1);
                    
                display.setCursor(25, 2);
                display.println("Pressure Probe");
                
                display.setTextColor(WHITE);
                display.setCursor(0, 14);
                
                write_test_progress();
                display.println("");
                display.println("Cold Pressure");
                display.println("");
                display.println("SERVER IP:");
                display.println("");
                display.println(WiFi.localIP());
                
                display.display();
                delay(10);

                test_flags.once = 1;                
            }
            break;
        case E_ws_connected:
            if(test_flags.once == 0){
                //create JSON document with space for 2 field
                const size_t capacity = JSON_OBJECT_SIZE(2);
                DynamicJsonDocument doc(capacity);
                //insert the field value
                doc["type"] = "vas_freq";
                doc["vas_freq"] = configuration_data.user_settings.vas_freq;
                String json_freq;

                serializeJson(doc, json_freq); 
                webSocket.broadcastTXT(json_freq);
                test_flags.once = 1;
            }
            break;
        case E_start:
            if(test_flags.once == 0){
                
                Serial.println("VAS init");
                probe_cold_pressure_init();                
                test_flags.once = 1;
            }            
                        
            probe_cold_pressure_measurement();
            break;        
        case E_stop:
            if(test_flags.once == 0){                
                               
                probe_cold_pressure_finish();
                
                if(test_flags.sub == SUB_ROUTINE_2){
                    test_flags.sub = static_cast<TEST_SUB>(static_cast<int>(test_flags.sub) + 1);
                }
                test_flags.once = 1;
            }
            break;
        case E_set:
            if(test_flags.once == 0){
                test_flags.sub = static_cast<TEST_SUB>(static_cast<int>(test_flags.sub) + 1);
                probe_event = E_start;
                test_flags.once = 1;
            }
            break;        
        case E_reset:
            if(test_flags.once == 0){
                test_flags.sub = static_cast<TEST_SUB>(static_cast<int>(test_flags.sub) + 2);
                probe_event = E_start;
                test_flags.once = 1;
            }
            break;
        case E_rx_vas:
            if(test_flags.once == 0){
                test_flags.once = 1;
                probe_event = E_start;
            }
            
            break;
        default:            
            break;
    }    
    return;
}

void S_save(){
    
    switch(probe_event){
        case E_no_event:
            
            if(test_flags.once == 0){
                                
                display.clearDisplay();
                display.fillRect(0,0,display.width(),12,WHITE);
                display.setTextColor(BLACK,WHITE);
                display.setTextSize(1);
                    
                display.setCursor(25, 2);
                display.println("Pressure Probe");
                
                display.setTextColor(WHITE);
                display.setCursor(0, 14);
                
                display.println("");
                display.println("DOWNLOAD DATA");
                display.println("");
                display.println("SERVER IP:");
                display.println("");
                display.println(WiFi.localIP());
                
                display.display();
                delay(10);

                test_flags.once = 1;                
            }
            break;
        case E_start:
            if(test_flags.once == 0){
                //append all files from test protocol starting from TEST1 up until configuration_data.probe_test_progress.prev_test
                File data = SPIFFS.open("/full_test_protocol.csv", FILE_WRITE);

                if (!data) {
                    display.println("ERROR ON OPEN data.csv FILE");
                    Serial.println("ERROR ON OPEN FILE");
                    return;
                }
                File test_data; 

                if(configuration_data.probe_test_progress.prev_test == TEST_8){
                    
                    for(int i = 0; i <= configuration_data.probe_test_progress.prev_test; i++){

                        test_data = SPIFFS.open(files[i], FILE_READ);

                        if (!test_data) {
                            display.println("ERROR ON OPEN test_data.csv FILE");
                            Serial.println("ERROR ON OPEN FILE");
                            return;
                        }
                        while(test_data.available()){
                            char c = test_data.read();
                            if(c == '\n'){                                
                            }
                            else{
                                data.print(c);
                            }                            
                        }
                        test_data.close();                    
                    }
                }
                else {
                    for(int i = 0; i < configuration_data.probe_test_progress.prev_test; i++){

                        test_data = SPIFFS.open(files[i], FILE_READ);

                        if (!test_data) {
                            display.println("ERROR ON OPEN test_data.csv FILE");
                            Serial.println("ERROR ON OPEN FILE");
                            return;
                        }

                        while(test_data.available()){
                            char c = test_data.read();
                            if(c == '\n'){                                
                            }
                            else{
                                data.print(c);
                            }                            
                        }
                        test_data.close();                    
                    }
                }
                                
                data.close();

                //create JSON document with space for one field
                const size_t capacity = JSON_OBJECT_SIZE(1);
                DynamicJsonDocument doc(capacity);
                //insert the field value
                doc["type"] = "set"; //send to notify file is ready to download               

                String json_set;

                serializeJson(doc, json_set);        
                
                Serial.println("data Sent");
                Serial.println(json_set);
                
                webSocket.broadcastTXT(json_set);
                

                test_flags.once = 1;
            }
            
            break;        
        default:            
            break;
    } 
    return;
}
