#include "Probe_Calibration.h"
#include "Probe_State_Machine.h"

float units;

void init_calibration(){  

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Initializing... ");  
  display.display();
  
  Probe.set_scale();
  Probe.tare();  

  delay(2500);
}
  
void calibrate_hx711(){

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Place known weight ");
    display.println("to Calibrate");
    display.display();
    delay(5000);

    display.println("Calibration started");
    display.display();
    units = Probe.get_units(20);

    if(configuration_data.calibration_settings.known_weight != 0){
    configuration_data.calibration_settings.calibration_factor = units / configuration_data.calibration_settings.known_weight;
    }

    const size_t capacity = JSON_OBJECT_SIZE(2);
    DynamicJsonDocument doc(capacity);

    doc["type"] = "factor";
    doc["factor"] = configuration_data.calibration_settings.calibration_factor;

    String json_factor;

    serializeJson(doc, json_factor);

    #ifdef PROBE_DEBUG
    Serial.println("data Sent");
    Serial.println(json_factor);
    #endif

    webSocket.broadcastTXT(json_factor);
}

void fine_tune_hx711(){     

    Probe.set_scale(configuration_data.calibration_settings.calibration_factor);

    Serial.println(configuration_data.calibration_settings.calibration_factor);  

    units = Probe.get_units(40);

    if (units < 0){
    units = 0.00;
    }    
    
    if(test_flags.adjust != 0){
        const size_t capacity = JSON_OBJECT_SIZE(2);
        DynamicJsonDocument doc(capacity);

        if(test_flags.adjust == 1 ){
            configuration_data.calibration_settings.calibration_factor += 1.0;
            test_flags.adjust = 0;
            doc["type"] = "factor";
            doc["factor"] = configuration_data.calibration_settings.calibration_factor;
        }      
        else if(test_flags.adjust == 2 ){
            configuration_data.calibration_settings.calibration_factor -= 1.0;
            test_flags.adjust = 0;
            doc["type"] = "factor";
            doc["factor"] = configuration_data.calibration_settings.calibration_factor;
        }
        String json_factor;

        serializeJson(doc, json_factor);

        #ifdef PROBE_DEBUG
        Serial.println("data Sent");
        Serial.println(json_factor);
        #endif

        webSocket.broadcastTXT(json_factor);
    }
    const size_t capacity = JSON_OBJECT_SIZE(2);
    DynamicJsonDocument doc(capacity);
    doc["type"] = "measure";
    doc["measure"] = units;

    String json_cal;

    serializeJson(doc, json_cal);

    #ifdef PROBE_DEBUG
    Serial.println("data Sent");
    Serial.println(json_cal);
    #endif

    webSocket.broadcastTXT(json_cal);     
}