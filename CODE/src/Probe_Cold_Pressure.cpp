#include "Probe_Cold_Pressure.h"
#include "Probe_State_Machine.h"
#include "Probe_Measurement_Common.h"


void probe_cold_pressure_init(){   

    initialize_variables();    
        
    test_measurement = SPIFFS.open("/test_7_sub_1.csv", FILE_WRITE);

    if (!test_measurement) {
        display.println("ERROR ON OPEN FILE");
        Serial.println("ERROR ON OPEN FILE");
        return;
    }

    start_time = millis();

    test_measurement.print("USER_ID");
    test_measurement.print(",");
    test_measurement.print("TEST");
    test_measurement.print(",");
    test_measurement.print("VAS_FREQ"); 
    test_measurement.print(",");
    test_measurement.print("MAX_PRESSURE");
    test_measurement.print(",");
    test_measurement.println("TARGET_ROC");
    //-----------------------------------------------------------------------   
    test_measurement.print(configuration_data.user_settings.unique_id);
    test_measurement.print(",");
    test_measurement.printf("%d.%d",test_flags.test + 1, test_flags.sub);
    test_measurement.print(",");
    test_measurement.print(configuration_data.user_settings.vas_freq);
    test_measurement.print(",");
    test_measurement.print(configuration_data.user_settings.max_pressure);
    test_measurement.print(",");
    test_measurement.println(configuration_data.user_settings.target_roc);
    //-------------------------------------------------------------------------
    test_measurement.print("VAS");
    test_measurement.print(",");
    test_measurement.print("Kpa");
    test_measurement.print(",");
    test_measurement.print("ms");
    test_measurement.print(",");
    test_measurement.println("ROC pr sec");
    //------------------------------------------------------------------------       
    test_measurement.print("0");
    test_measurement.print(",");
    test_measurement.print("0");
    test_measurement.print(",");
    test_measurement.print("0");
    test_measurement.print(",");
    test_measurement.print("0");
    test_measurement.print(",");
    test_measurement.println("START");

    gauge.begin();
}

void probe_cold_pressure_finish(){           

    if(test_flags.once == 0){
        display.clearDisplay();

        display.drawBitmap((SCREEN_WIDTH - exmark_WIDHT) / 2, (SCREEN_HEIGHT - exmark_HEIGHT) / 2,
        exmark_bitmap, exmark_WIDHT, exmark_HEIGHT, 1);
        display.setCursor(0,57);
        display.printf("TEST %i SUB %i", test_flags.test + 1, test_flags.sub);
        display.display();

        roc_total_measurement_average = roc_total_measurement_average / roc_total_measurement_count;
        
        test_measurement.print(configuration_data.trx_data.socket_rx_data.measurement_data.measurement);
        test_measurement.print(",");
        test_measurement.print(current_reading);
        test_measurement.print(",");
        test_measurement.print(time_now - start_time);
        test_measurement.print(",");
        test_measurement.println(roc_total);
        test_measurement.print("");
        test_measurement.print(",");
        test_measurement.print("");
        test_measurement.print(",");
        test_measurement.print("ROC average =");
        test_measurement.print(",");
        test_measurement.println(roc_total_measurement_average);
        test_measurement.close();
        
        test_flags.once = 1;
    }
}

void probe_cold_pressure_measurement(){    
    
    //get the time for the current reading
    time_now = millis();

    if(test_flags.sub == SUB_ROUTINE_2 || test_flags.sub == SUB_ROUTINE_3){
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
        //adjust the gauge with the new values 
        gauge.set_min_max(0, 2 * next_reading);
    }
    //if a second has past, and the pressure is above threshold
    if (((time_now - save_time_previous  >= interval) && (current_reading > start_threshold)) || ((time_now - save_time_previous  >= interval) && (test_flags.sub == SUB_ROUTINE_1))){  
        if(test_flags.sub == SUB_ROUTINE_2 || test_flags.sub == SUB_ROUTINE_3){
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

            readIndex = 0;
        }
        
        save_time_previous = time_now;

        if(test_flags.sub == SUB_ROUTINE_1){
            //time_now = millis();
            test_measurement.print(configuration_data.trx_data.socket_rx_data.measurement_data.measurement);
            test_measurement.print(",");
            test_measurement.print("");
            test_measurement.print(",");
            test_measurement.print(time_now - start_time);
            test_measurement.print(",");
            test_measurement.println("");
        }
        else if(test_flags.sub == SUB_ROUTINE_2){
            test_measurement.print(configuration_data.trx_data.socket_rx_data.measurement_data.measurement);
            test_measurement.print(",");                
            test_measurement.print(current_reading);
            test_measurement.print(",");
            test_measurement.print(time_now - start_time);
            test_measurement.print(",");
            test_measurement.println(roc_total);
        }
        else if(test_flags.sub == SUB_ROUTINE_3){
            test_measurement.print("VAS STOP");
            test_measurement.print(",");                
            test_measurement.print(current_reading);
            test_measurement.print(",");
            test_measurement.print(time_now - start_time);
            test_measurement.print(",");
            test_measurement.println(roc_total);
        }                               
    }    
    
    time_previous = time_now;
        
    //draw the gauge needle with the current value
    if(test_flags.sub != SUB_ROUTINE_1){
        previuos_reading = current_reading;
        gauge.drawGaugeData(current_reading, current_reading);
    }   
             
}