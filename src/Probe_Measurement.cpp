#include "Probe_Measurement.h"
#include "Probe_State_Machine.h"
#include "Probe_Measurement_Common.h"

Circular_Gauge gauge(0, 1000);

File test_measurement;

int raw_average = 1;

float average_roc_result[MAX_ROC_SMOOTH];                       // the readings from the analog input

int actual_readings = 0;

int readIndex = 0;                                              // the index of the current reading
float roc_total = 0;                                            // the running total
float roc_average = 0;                          
float roc_total_measurement_average = 0;                        // the actual average of rate of change
int roc_total_measurement_count = 0;
float calculated_target_roc_pr_reading = 0;                     //the calculated target rate of change between each dataline in the average roc array 

float current_reading = 0;
float previuos_reading = 0;
float next_reading = 0;

unsigned long start_time = 0;
unsigned long time_now = 0;
unsigned long time_previous = 0;
unsigned long save_time_previous = 0;
unsigned long dt = 0;                                           // dt in milliseconds

void initialize_variables(){

    actual_readings = 0;    

    readIndex = 0;                                              // the index of the current reading
    roc_total = 0;                                              // the running total
    roc_average = 0;                                            // the average of rate of change
    roc_total_measurement_average = 0;                          // the average of rate of change
    roc_total_measurement_count = 0;
    calculated_target_roc_pr_reading = 0.00;

    previuos_reading = 0;
    current_reading = 0;
    next_reading = 0;    
    time_now = 0;
    time_previous = 0;
    save_time_previous = 0;    

    calculated_target_roc_pr_reading = configuration_data.user_settings.target_roc / measurement_rate_hz ;

    next_reading = configuration_data.user_settings.target_roc;

    for (int thisReading = 0; thisReading < MAX_ROC_SMOOTH; thisReading++) {
        average_roc_result[thisReading] = 0;
    }
}

void probe_measurement_init(){    

    initialize_variables();
    if(probe_state == S_RAW_CAPTURE){
        test_measurement = SPIFFS.open("/raw_capture_measurement.csv", FILE_WRITE);

        if (!test_measurement) {
            display.println("ERROR ON OPEN FILE");
            Serial.println("ERROR ON OPEN FILE");
            return;
        }
        start_time = millis();

        test_measurement.println("RAW_CAPTURE");                
        test_measurement.print("TARGET_ROC");
        test_measurement.print(",");
        test_measurement.println("MAX_Kpa");        
        test_measurement.print(configuration_data.user_settings.target_roc);
        test_measurement.print(",");
        test_measurement.println(configuration_data.user_settings.max_pressure);
        test_measurement.print("Kpa");
        test_measurement.print(",");
        test_measurement.print("ms");
        test_measurement.print(",");
        test_measurement.println("ROC pr sec");
        test_measurement.print("0");
        test_measurement.print(",");
        test_measurement.print("0");
        test_measurement.println("");
    }
    else {
        switch(test_flags.test + 1){
            case 1:
                switch(test_flags.sub){
                    case 1:
                        test_measurement = SPIFFS.open("/measure_1_sub_1.csv", FILE_WRITE);

                        if (!test_measurement) {
                            display.println("ERROR ON OPEN FILE");
                            Serial.println("ERROR ON OPEN FILE");
                            return;
                        }                    
                        break;
                    case 2:
                        test_measurement = SPIFFS.open("/measure_1_sub_2.csv", FILE_WRITE);

                        if (!test_measurement) {
                            display.println("ERROR ON OPEN FILE");
                            return;
                        }
                        break;
                    case 3:
                        test_measurement = SPIFFS.open("/measure_1_sub_3.csv", FILE_WRITE);

                        if (!test_measurement) {
                            display.println("ERROR ON OPEN FILE");
                            return;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case 2:
                switch(test_flags.sub){
                    case 1:
                        test_measurement = SPIFFS.open("/measure_2_sub_1.csv", FILE_WRITE);

                        if (!test_measurement) {
                            display.println("ERROR ON OPEN FILE");
                            Serial.println("ERROR ON OPEN FILE");
                            return;
                        }                    
                        break;
                    case 2:
                        test_measurement = SPIFFS.open("/measure_2_sub_2.csv", FILE_WRITE);

                        if (!test_measurement) {
                            display.println("ERROR ON OPEN FILE");
                            return;
                        }
                        break;
                    case 3:
                        test_measurement = SPIFFS.open("/measure_2_sub_3.csv", FILE_WRITE);

                        if (!test_measurement) {
                            display.println("ERROR ON OPEN FILE");
                            return;
                        }
                        break; 
                    default:
                        break;
                }
                break;            
            default:
                break;
        }
        start_time = millis();

        test_measurement.print("USER_ID");
        test_measurement.print(",");
        test_measurement.print("TEST");
        test_measurement.print(",");
        test_measurement.print("TARGET_ROC");
        test_measurement.print(",");
        test_measurement.println("MAX_Kpa");
        test_measurement.print(configuration_data.user_settings.unique_id);
        test_measurement.print(",");
        test_measurement.printf("%d.%d",test_flags.test + 1, test_flags.sub);
        test_measurement.print(",");
        test_measurement.print(configuration_data.user_settings.target_roc);
        test_measurement.print(",");
        test_measurement.println(configuration_data.user_settings.max_pressure);
        test_measurement.print("Kpa");
        test_measurement.print(",");
        test_measurement.print("ms");
        test_measurement.print(",");
        test_measurement.println("ROC pr sec");
        test_measurement.print("0");
        test_measurement.print(",");
        test_measurement.print("0");
        test_measurement.println("");
    }    
    gauge.begin();            
}

void probe_measurement_finish(){           

    if(test_flags.once == 0){
        display.clearDisplay();

        display.drawBitmap((SCREEN_WIDTH - exmark_WIDHT) / 2, (SCREEN_HEIGHT - exmark_HEIGHT) / 2,
        exmark_bitmap, exmark_WIDHT, exmark_HEIGHT, 1);
        display.setCursor(0,57);
        display.printf("TEST %i SUB %i", test_flags.test + 1, test_flags.sub);
        display.display();
        //delay(100);

        roc_total_measurement_average = roc_total_measurement_average / roc_total_measurement_count;
        
        test_measurement.print(current_reading);
        test_measurement.print(",");
        test_measurement.print(time_now - start_time);
        test_measurement.print(",");
        test_measurement.println(roc_total);
        test_measurement.print("");
        test_measurement.print(",");
        test_measurement.print("ROC average =");
        test_measurement.print(",");
        test_measurement.println(roc_total_measurement_average);
        test_measurement.close();
        
        if(test_flags.test == TEST_1 && test_flags.sub == SUB_ROUTINE_3){
            File test_1 = SPIFFS.open("/test_1_sub_1.csv", FILE_WRITE);

            if (!test_1) {
                display.println("ERROR ON OPEN FILE");
                return;
            }
            for (int i = 0; i < 3; i++){
                File test_1_measure = SPIFFS.open(test_1_files[i], FILE_READ);
                
                while(test_1_measure.available()){
                    char c = test_1_measure.read();
                    if(c == '\n'){
                        //data.println();
                    }
                    else{
                        test_1.print(c);
                    } 
                }                
                test_1_measure.close();
            }
        }

        if(test_flags.test == TEST_2 && test_flags.sub == SUB_ROUTINE_3){
            File test_2 = SPIFFS.open("/test_2_sub_1.csv", FILE_WRITE);

            if (!test_2) {
                display.println("ERROR ON OPEN FILE");
                return;
            }
            for (int i = 0; i < 3; i++){
                File test_2_measure = SPIFFS.open(test_2_files[i], FILE_READ);
                
                while(test_2_measure.available()){
                    char c = test_2_measure.read();
                    if(c == '\n'){
                        //data.println();
                    }
                    else{
                        test_2.print(c);
                    } 
                }
                test_2_measure.close();
            }
        }
        test_flags.once = 1;
    }
}

void probe_measurement(){    

    //read from the load cell
    float sensorValue = Probe.get_units(raw_average);
    //plus one to keep track of measurements pr second
    actual_readings++;
    // done to obtain Kpa from Kg/cm2
    current_reading = sensorValue * 98.06; 

    if(current_reading <= 0.0){
        // done to prevent calculation errors with negative values
        current_reading = 0.0; 
    }
    //get the time for the current reading
    time_now = millis();        

    if(probe_state != S_RAW_CAPTURE || (probe_state == S_RAW_CAPTURE && test_flags.raw_max_on == 1)){
        //sends JSON stop to client and sets E_stop event
        if(current_reading >= configuration_data.user_settings.max_pressure){  

            //create JSON document with space for one field
            const size_t capacity = JSON_OBJECT_SIZE(1);
            DynamicJsonDocument doc(capacity);
            //insert the field value
            doc["type"] = "stop";

            String json_stop;

            serializeJson(doc, json_stop);        
            
            Serial.println("data Sent");
            Serial.println(json_stop);
            probe_event = E_stop;
            test_flags.once = 0;
            webSocket.broadcastTXT(json_stop);        
        }
    }    

    // subtract the last reading:
    roc_total = roc_total - average_roc_result[readIndex];
    // read from the sensor:
    average_roc_result[readIndex] = current_reading - previuos_reading;        
    // add the reading to the total:
    roc_total = roc_total + average_roc_result[readIndex];
    // advance to the next position in the array:
    readIndex = readIndex + 1;       
    // calculate the average:        
    roc_average = roc_total / actual_readings;
    //if index exceeds array size, set index to zero does not occure as the index is reset every second 
    if(readIndex >= MAX_ROC_SMOOTH){
        readIndex = 0;
    }        
    //do not start measuring if below threshold
    if(current_reading > start_threshold){
        //set next reading in order to give correct visual feedback 
        next_reading += calculated_target_roc_pr_reading;
    }
    if(probe_state != S_RAW_CAPTURE || (probe_state == S_RAW_CAPTURE && test_flags.raw_roc_on == 1)){
        //adjust the gauge with the new values 
        gauge.set_min_max(0, 2 * next_reading);
    }
    else {
        //raw_data max 10kg == approx 1000 kPa cm^2  
        gauge.set_min_max(0, 1000);
    }    
              
    //if a second has past, and the pressure is above threshold
    if (time_now - save_time_previous  >= interval && current_reading > start_threshold){  
        //print reading
        Serial.println(actual_readings);
        //divide the target ROC with the number of actual readings taken the last second
        calculated_target_roc_pr_reading = configuration_data.user_settings.target_roc / actual_readings ;
        //reset readings counter
        actual_readings = 0;
        //add to the ROC total
        roc_total_measurement_average += roc_total;
        //plus one to the saved measurements counter
        roc_total_measurement_count++;
        
        save_time_previous = time_now;                    

        readIndex = 0;

        test_measurement.print(current_reading);
        test_measurement.print(",");
        test_measurement.print(time_now - start_time);
        test_measurement.print(",");
        test_measurement.println(roc_total);

        if(probe_state == S_RAW_CAPTURE){
            //create JSON document with space for one field
            const size_t capacity2 = JSON_OBJECT_SIZE(4);
            DynamicJsonDocument doc(capacity2);
            //insert the field value
            doc["type"] = "raw";
            doc["measure"] = current_reading;
            doc["milli"] = time_now - start_time;
            doc["roc"] = roc_total;

            String json_measure;

            serializeJson(doc, json_measure);        
            
            Serial.println("data Sent");
            Serial.println(json_measure);
            //probe_event = E_stop;
            //test_flags.once = 0;
            webSocket.broadcastTXT(json_measure);
            //delay(100);
        }               
    }    
    
    time_previous = time_now;
    previuos_reading = current_reading;
    //draw the gauge needle with the current value   
    gauge.drawGaugeData(current_reading, current_reading);         
}