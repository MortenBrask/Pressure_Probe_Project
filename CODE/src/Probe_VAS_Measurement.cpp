#include "Probe_VAS_Measurement.h"
#include "Probe_State_Machine.h"
#include "Probe_Measurement_Common.h"

void probe_vas_measurement_init(){   

    start_time = 0;
    time_now = 0;

    switch(test_flags.test + 1){
        case 3:
            switch(test_flags.sub){
                case 1:
                    test_measurement = SPIFFS.open("/test_3_sub_1.csv", FILE_WRITE);

                    if (!test_measurement) {
                        display.println("ERROR ON OPEN FILE");
                        Serial.println("ERROR ON OPEN FILE");
                        return;
                    }                    
                    break;                
                default:
                    break;
            }
            break;
        case 4:
            switch(test_flags.sub){
                case 1:
                    test_measurement = SPIFFS.open("/test_4_sub_1.csv", FILE_WRITE);

                    if (!test_measurement) {
                        display.println("ERROR ON OPEN FILE");
                        Serial.println("ERROR ON OPEN FILE");
                        return;
                    }                    
                    break;                 
                default:
                    break;
            }
            break;
        case 5:
            switch(test_flags.sub){
                case 1:
                    test_measurement = SPIFFS.open("/test_5_sub_1.csv", FILE_WRITE);

                    if (!test_measurement) {
                        display.println("ERROR ON OPEN FILE");
                        Serial.println("ERROR ON OPEN FILE");
                        return;
                    }                    
                    break;
                default:
                    break;               
            }
            break;
        case 6:
            switch(test_flags.sub){
                case 1:
                    test_measurement = SPIFFS.open("/test_6_sub_1.csv", FILE_WRITE);

                    if (!test_measurement) {
                        display.println("ERROR ON OPEN FILE");
                        Serial.println("ERROR ON OPEN FILE");
                        return;
                    }                    
                    break;
                default:
                    break;               
            }
            break;
        case 7:
            switch(test_flags.sub){
                case 1:
                    test_measurement = SPIFFS.open("/test_7_sub_1.csv", FILE_WRITE);

                    if (!test_measurement) {
                        display.println("ERROR ON OPEN FILE");
                        Serial.println("ERROR ON OPEN FILE");
                        return;
                    }                    
                    break;                
                default:
                    break;               
            }
            break;
        case 8:
        switch(test_flags.sub){
                case 1:
                    test_measurement = SPIFFS.open("/test_8_sub_1.csv", FILE_WRITE);

                    if (!test_measurement) {
                        display.println("ERROR ON OPEN FILE");
                        Serial.println("ERROR ON OPEN FILE");
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
    test_measurement.println("VAS_FREQ");    
    test_measurement.print(configuration_data.user_settings.unique_id);
    test_measurement.print(",");
    test_measurement.printf("%d.%d",test_flags.test + 1, test_flags.sub);
    test_measurement.print(",");
    test_measurement.println(configuration_data.user_settings.vas_freq);
    test_measurement.print("VAS");
    test_measurement.print(",");
    test_measurement.println("ms");       
    test_measurement.print("0");
    test_measurement.print(",");
    test_measurement.print("0");
    test_measurement.print(",");
    test_measurement.println("START");

}

void probe_vas_measurement_finish(){
    time_now = millis();
    test_measurement.print(configuration_data.trx_data.socket_rx_data.measurement_data.measurement);
    test_measurement.print(",");
    test_measurement.print(time_now - start_time);
    test_measurement.print(",");
    test_measurement.println("STOP");
    test_measurement.close();
}

void probe_vas_measurement(){
    time_now = millis();
    test_measurement.print(configuration_data.trx_data.socket_rx_data.measurement_data.measurement);
    test_measurement.print(",");
    test_measurement.println(time_now - start_time);
}

