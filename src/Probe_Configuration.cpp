#include "Probe_Configuration.h"
#include "Probe_State_Machine.h"

void load_probe_configuration(){  
 
    File file = SPIFFS.open("/json_conf.txt", FILE_READ);
 
    if (!file) {

        display.clearDisplay();
        display.setCursor(0,0);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.println("No configuration found");
        display.println("Loading default values");
        display.display();

        configuration_data.calibration_settings.calibration_factor = DEFAULT_CALIBRATION_FACTOR;
        configuration_data.user_settings.vas_freq = DEFAULT_VAS_FREQ; 
        configuration_data.user_settings.target_roc = DEFAULT_TARGET_ROC;        
        configuration_data.user_settings.max_pressure = DEFAULT_MAX_PRESSURE;
        configuration_data.user_settings.unique_id = DEFAULT_UNIQUE_ID;
        configuration_data.user_settings.prev_unique_id = DEFAULT_PREV_UNIQUE_ID;
        configuration_data.calibration_settings.known_weight = DEFAULT_KNOWN_WEIGHT;        
        configuration_data.probe_test_progress.prev_test = DEFAULT_PREV_TEST;
        configuration_data.probe_test_progress.prev_sub = DEFAULT_PREV_SUB;
        return;
    }
    // Parse directly from file
    DeserializationError err = deserializeJson(probe_configuration, file);

    if(err){
        display.clearDisplay();
        display.setCursor(0,0);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.println("Deserialize error");
        display.println(err.c_str());
        display.display();
        return;
    }    
        
    configuration_data.calibration_settings.calibration_factor = probe_configuration["cal_factor"] | DEFAULT_CALIBRATION_FACTOR; 
    configuration_data.user_settings.vas_freq = probe_configuration["vas_freq"] | DEFAULT_VAS_FREQ; 
    configuration_data.user_settings.target_roc = probe_configuration["target_roc"] | DEFAULT_TARGET_ROC;        //10,25,50,75,100    
    configuration_data.user_settings.max_pressure = probe_configuration["max_pressure"] | DEFAULT_MAX_PRESSURE;  //default 1000 kPa (approx 10KG of pressure on 1cm2 surface area)    
    configuration_data.user_settings.unique_id = probe_configuration["unique_id"] | DEFAULT_UNIQUE_ID;
    configuration_data.user_settings.prev_unique_id = probe_configuration["prev_unique_id"] | DEFAULT_PREV_UNIQUE_ID;
    configuration_data.calibration_settings.known_weight = probe_configuration["known_weight"] | DEFAULT_KNOWN_WEIGHT;    
    configuration_data.probe_test_progress.prev_test = probe_configuration["prev_test"] | DEFAULT_PREV_TEST;
    configuration_data.probe_test_progress.prev_sub = probe_configuration["prev_sub"] | DEFAULT_PREV_SUB;

    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    #ifndef PROBE_DEBUG
    display.println("Configuration found");
    display.println("Loading values...");
    #endif
    #ifdef PROBE_DEBUG       
    display.printf("TEST = %i", configuration_data.probe_test_progress.prev_test);
    display.printf("calibration factor = %.2f", configuration_data.calibration_settings.calibration_factor);
    display.printf("vas freq = %i", configuration_data.user_settings.vas_freq);
    display.printf("target roc = %.2f", configuration_data.user_settings.target_roc);    
    display.printf("max pressure = %i", configuration_data.user_settings.max_pressure);    
    #endif
    display.display();
    file.close();
    delay(2000);
}
void save_probe_configuration(){    
    
    File file = SPIFFS.open("/json_conf.txt", FILE_WRITE);
 
    if (!file) {
        display.println("ERROR ON OPEN FILE");
        return;
    }

    probe_configuration["type"] = "settings";
    probe_configuration["cal_factor"] = configuration_data.calibration_settings.calibration_factor;
    probe_configuration["vas_freq"] = configuration_data.user_settings.vas_freq;
    probe_configuration["target_roc"] = configuration_data.user_settings.target_roc;            
    probe_configuration["max_pressure"] = configuration_data.user_settings.max_pressure;
    probe_configuration["unique_id"] = configuration_data.user_settings.unique_id;
    probe_configuration["prev_unique_id"] = configuration_data.user_settings.prev_unique_id;
    probe_configuration["known_weight"] = configuration_data.calibration_settings.known_weight;   
    probe_configuration["prev_test"] = configuration_data.probe_test_progress.prev_test;
    probe_configuration["prev_sub"] = configuration_data.probe_test_progress.prev_sub;
    
    serializeJson(probe_configuration, file);

    //display.clearDisplay();    
    display.fillRect(0,53,display.width(),32,WHITE);
    display.setTextColor(BLACK,WHITE);
    display.setTextSize(1);
        
    display.setCursor(7, 55);
    
    display.println("Configuration saved");    
    display.display();

    file.close();
    delay(2000);
}


