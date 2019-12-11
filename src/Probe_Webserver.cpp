#include "Probe_Webserver.h"

//data_handler is taking care of parsing the json formatted messages received from the webpage
//it saves and sets the events and data 
void data_handler(uint8_t * payload){
    
    Serial.println("Data received");
    StaticJsonDocument<500> JSONDoc;                        //Memory pool
    auto error = deserializeJson(JSONDoc, payload);
    
    if (error) {                                            //Check for errors in parsing
        Serial.println("Parsing failed");
        return; 
    }
    
    const char * jstype = JSONDoc["type"];                  //Get sensor type value        

    Serial.println("type received =");
    Serial.println(jstype);

    if(!strcmp(jstype,"Start")){      
    #ifdef PROBE_DEBUG
        Serial.println("start received");
    #endif
        probe_event = E_start;
        test_flags.once = 0;
    }
    else if(!strcmp(jstype,"Stop")){
    #ifdef PROBE_DEBUG
        Serial.println("stop received");
    #endif
        probe_event = E_stop;
        test_flags.once = 0;
    }
    else if(!strcmp(jstype,"Set")){
    #ifdef PROBE_DEBUG
        Serial.println("set received");
    #endif
        probe_event = E_set;
        test_flags.once = 0;
    }
    else if(!strcmp(jstype,"Reset")){
    #ifdef PROBE_DEBUG
        Serial.println("reset received");
    #endif
        probe_event = E_reset;
        test_flags.once = 0;
    }
    else if(!strcmp(jstype,"Plus")){
    #ifdef PROBE_DEBUG
        Serial.println("plus received");
    #endif
        probe_event = E_plus;
        test_flags.once = 0;
    }
    else if(!strcmp(jstype,"Minus")){
    #ifdef PROBE_DEBUG
        Serial.println("minus received");
    #endif
        probe_event = E_minus;
        test_flags.once = 0;
    }
    else if(!strcmp(jstype,"Raw_max_on")){
    #ifdef PROBE_DEBUG
        Serial.println("raw max received");
    #endif
        test_flags.raw_max_on = 1;        
    }
    else if(!strcmp(jstype,"Raw_roc_on")){
    #ifdef PROBE_DEBUG
        Serial.println("raw roc received");
    #endif
        probe_event = E_raw_roc_on;
        test_flags.raw_roc_on = 1; 
    }
    else if(!strcmp(jstype,"Raw_max_off")){
    #ifdef PROBE_DEBUG
        Serial.println("raw max received");
    #endif
        test_flags.raw_max_on = 0;        
    }
    else if(!strcmp(jstype,"Raw_roc_off")){
    #ifdef PROBE_DEBUG
        Serial.println("raw roc received");
    #endif
        test_flags.raw_roc_on = 0; 
    }
    else if(!strcmp(jstype,"VAS")){
        int jsvas = JSONDoc["vas"];
        configuration_data.trx_data.socket_rx_data.msg_type = DATA_VAS_MEASUREMENT;
        configuration_data.trx_data.socket_trx_type = socket_RX;
        configuration_data.trx_data.socket_rx_data.measurement_data.measurement = jsvas;
    #ifdef PROBE_DEBUG
        Serial.println("VAS data received");
    #endif
        probe_event = E_rx_vas;
        test_flags.once = 0;
    }
    else if(!strcmp(jstype,"Temp")){
        int jstemp = JSONDoc["temp"];
        configuration_data.trx_data.socket_rx_data.msg_type = DATA_VAS_MEASUREMENT;
        configuration_data.trx_data.socket_trx_type = socket_RX;
        configuration_data.trx_data.socket_rx_data.measurement_data.measurement = jstemp;
    #ifdef PROBE_DEBUG
        Serial.println("Temp data received");
    #endif
        probe_event = E_rx_temp;
        test_flags.once = 0;
    }
    else if(!strcmp(jstype,"factor")){
        int jscal_factor = JSONDoc["factor"];
        configuration_data.trx_data.socket_rx_data.msg_type = DATA_CALIBRATION_FACTOR;
        configuration_data.trx_data.socket_trx_type = socket_RX;
        configuration_data.trx_data.socket_rx_data.calibration_data.calibration_factor = jscal_factor;
    #ifdef PROBE_DEBUG
        Serial.println("calibration factor data received");
    #endif
        probe_event = E_rx_calibration_factor;
        test_flags.once = 0;
    }
    else if(!strcmp(jstype,"weight")){
        int jscal_weight = JSONDoc["weight"];
        configuration_data.trx_data.socket_rx_data.msg_type = DATA_CALIBRATION_WEIGHT;
        configuration_data.trx_data.socket_trx_type = socket_RX;
        configuration_data.trx_data.socket_rx_data.calibration_data.known_weight = jscal_weight;
    #ifdef PROBE_DEBUG
        Serial.println("calibration factor data received");
    #endif
        probe_event = E_rx_weight;
        test_flags.once = 0;
    }
    else if(!strcmp(jstype,"ID")){
        const char * jsid = JSONDoc["id"];
        configuration_data.trx_data.socket_rx_data.msg_type = DATA_UNIQUE_ID;
        configuration_data.trx_data.socket_trx_type = socket_RX;
        configuration_data.trx_data.socket_rx_data.user_settings.unique_id = (int)jsid;
    #ifdef PROBE_DEBUG
        Serial.println("data received =");
        Serial.println(jsid);
    #endif
        probe_event = E_rx_id;
        test_flags.once = 0;        
    }
    else if(!strcmp(jstype,"Settings")){        
        float jsroc = JSONDoc["roc"];
        int jsmaxpress = JSONDoc["max_press"];
        int jsfreq = JSONDoc["vas_freq"];
        configuration_data.user_settings.target_roc = jsroc;
        configuration_data.user_settings.max_pressure = jsmaxpress;
        configuration_data.user_settings.vas_freq = jsfreq;
    #ifdef PROBE_DEBUG
        Serial.println("data received =");
        Serial.println(jsroc);
        Serial.println(jsmaxpress);
        Serial.println(jsfreq);
    #endif
        probe_event = E_rx_settings;
        test_flags.once = 0;
        
    }
    
}
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                #ifdef PROBE_DEBUG
                IPAddress ip = webSocket.remoteIP(num);                
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                #endif

                probe_event = E_ws_connected;
                test_flags.once = 0;
            }
                break;
        case WStype_TEXT:
            {
                #ifdef PROBE_DEBUG
                Serial.printf("[%u] get Text: %s\n", num, payload);
                #endif 

                data_handler(payload);
            }                        
            break;
        case WStype_BIN:

            #ifdef PROBE_DEBUG
            Serial.printf("[%u] get binary length: %u\n", num, length);
            #endif            
            break;
		case WStype_ERROR:                              //the webpage handles reconnect on error...			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
        default:
			break;
    }    
}

//template_processor inserts the correct html in the webpage being served, should it contain keywords that matches one of the listed below
String template_processor(const String& var)
{
    //Here is to be inserted the correct links for the instruction videos to show. insert the correct URL based on test progress (test_flags.test) like the default example
    if(var == "SOURCEVIDEO"){
        switch (test_flags.test){
            case TEST_1:
                return String("https://www.youtube.com/embed/mLy7_ZMYRao");      //URL to an instruction video to TEST_1
            case TEST_2:
                return String("https://www.youtube.com/embed/mLy7_ZMYRao");
            case TEST_3:
                return String("https://www.youtube.com/embed/FXQow21r86Y");
            case TEST_4:
                return String("https://www.youtube.com/embed/FXQow21r86Y");
            case TEST_5:
                return String("https://www.youtube.com/embed/N-ks1kCI6Pw");
            case TEST_6:
                return String("https://www.youtube.com/embed/N-ks1kCI6Pw");
            case TEST_7:
                return String("https://www.youtube.com/embed/_tjogGNOC5Q");
            case TEST_8:
                return String("https://www.youtube.com/embed/FXQow21r86Y");
            default:
                return String("https://www.youtube.com/embed/NpEaa2P7qZI");
        }         
    }
    else if(var == "NEXT_LINK"){
        return link_strings[test_flags.test];
    }
    else if(var == "VIDEO_TEXT"){
        return video_strings[test_flags.test];
    }
    else if(var == "LD_SELECT"){
        return ld_select_strings[test_flags.test];
    }
    else if(var == "TEMP_NEXT"){
        if(test_flags.test == TEST_8){
            return "download.html";
        }
        else{
            return "video.html";
        }
    }
    else if(var == "CHECK1"){
        return (configuration_data.probe_test_progress.prev_test > TEST_1) ? check_mark[0] : check_mark[1]; 
    }
    else if(var == "CHECK2"){
        return (configuration_data.probe_test_progress.prev_test > TEST_2) ? check_mark[0] : check_mark[1]; 
    }
    else if(var == "CHECK3"){
        return (configuration_data.probe_test_progress.prev_test > TEST_3) ? check_mark[0] : check_mark[1]; 
    }
    else if(var == "CHECK4"){
        return (configuration_data.probe_test_progress.prev_test > TEST_4) ? check_mark[0] : check_mark[1]; 
    }
    else if(var == "CHECK5"){
        return (configuration_data.probe_test_progress.prev_test > TEST_5) ? check_mark[0] : check_mark[1]; 
    }
    else if(var == "CHECK6"){
        return (configuration_data.probe_test_progress.prev_test > TEST_6) ? check_mark[0] : check_mark[1]; 
    }
    else if(var == "CHECK7"){
        return (configuration_data.probe_test_progress.prev_test > TEST_7) ? check_mark[0] : check_mark[1]; 
    }
    else if(var == "CHECK8"){
        return (configuration_data.probe_test_progress.prev_test > TEST_8) ? check_mark[0] : check_mark[1]; 
    }
        
    return String();
}

const String evaluate_request(){

    if(probe_event <= 10){

        S_VALID_NEXT_STATE new_state = valid_next_state[probe_state][probe_event];    

        if(new_state.next_state != probe_state){             

            if(probe_super_state == SS_CONFIG){

                if(probe_state == S_HOME && new_state.next_state == S_VIDEO){
                    probe_super_state = SS_TEST;
                    if(test_flags.resume == 0){
                        configuration_data.user_settings.prev_unique_id = configuration_data.user_settings.unique_id;//starting a new test elimantes ability to download previous testdata. thus the prev_id is equel to the new id
                        test_flags.test = TEST_1;
                        test_flags.sub = SUB_ROUTINE_0;
                    }
                    else{
                        test_flags.test = static_cast<TEST_ROUTINE>(static_cast<int>(configuration_data.probe_test_progress.prev_test));
                        test_flags.sub = SUB_ROUTINE_0;
                        test_flags.resume = 0;
                    }                    
                }
                probe_state = new_state.next_state;
                probe_event = E_no_event;        
                test_flags.once = 0;
                probe_state_pointer = probe_state_matrix[probe_state].state_pointer;
            }
            else{

                S_TEST_REQUEST_MATRIX check_test = test_request_matrix[test_flags.test][test_flags.sub];

                if(check_test.probe_test_state != new_state.next_state){
                    probe_state = S_HOME;
                    probe_event = E_no_event;
                    test_flags.once = 0;
                    probe_state_pointer = probe_state_matrix[probe_state].state_pointer;
                }

                if(new_state.next_state == S_HOME){
                    probe_super_state = SS_CONFIG;
                    configuration_data.probe_test_progress.prev_test = test_flags.test;
                    configuration_data.probe_test_progress.prev_sub = test_flags.sub;
                }
                else{           
                    if(test_flags.test < TEST_FINISH){            
                        if(test_flags.sub == SUB_ROUTINE_3 || check_test.probe_test_state == S_NA){
                            configuration_data.probe_test_progress.prev_sub = SUB_ROUTINE_0;
                            configuration_data.probe_test_progress.prev_test = static_cast<TEST_ROUTINE>(static_cast<int>(test_flags.test) + 1);
                            
                            test_flags.test  = static_cast<TEST_ROUTINE>(static_cast<int>(test_flags.test) + 1);
                            test_flags.sub = SUB_ROUTINE_0;                            
                        }
                        else if(test_flags.sub < SUB_ROUTINE_3){
                            configuration_data.probe_test_progress.prev_sub = test_flags.sub;
                            configuration_data.probe_test_progress.prev_test = test_flags.test;
                            
                            test_flags.sub = static_cast<TEST_SUB>(static_cast<int>(test_flags.sub) + 1);                     
                        }                        
                    }
                    if(test_flags.test == TEST_FINISH){
                        if(check_test.probe_test_state == S_SAVE){
                            configuration_data.probe_test_progress.prev_sub = test_flags.sub;
                            configuration_data.probe_test_progress.prev_test = test_flags.test;                            

                            test_flags.test  = TEST_1;
                            test_flags.sub = SUB_ROUTINE_0;
                            probe_super_state = SS_CONFIG;                            
                        }                        
                    }
                }                
                probe_state = new_state.next_state;
                probe_event = E_no_event;
                test_flags.once = 0;
                
                probe_state_pointer = probe_state_matrix[probe_state].state_pointer;
            }
        }
        else {
            test_flags.once = 0;
            probe_event = E_no_event;
        }
        switch(new_state.next_state){
        case S_HOME:
            return "/index.html";
        break;
        case S_SETTINGS:
            return "/settings.html";
        break;
        case S_CALIBRATION:
            return "/calibration.html";
        break;
        case S_RAW_CAPTURE:
            return "/raw-data.html";
        break;
        case S_SAVE:
            return "/download.html";
        break;
        case S_VIDEO:
            return "/video.html";
        break;
        case S_MEASURE:
            return "/measure.html";
        break;
        case S_VAS:
            return "/vas.html";
        break;
        case S_TEMP:
            return "/summation.html";
        break;
        case S_COLD:
            return "/coldpress.html";
        break;
        case S_NA:
        break;
    }
    }
    else{
        //event is not a http request
    }        
return "";    
}

void probe_server_init(void){

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        probe_event = E_req_home;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false);
        response->addHeader("Cache-Control","no-store");
        request->send(response);               
    });   

    server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
        probe_event = E_req_home;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false);
        response->addHeader("Cache-Control","no-store");
        request->send(response);              
    });

    server.on("/home.html", HTTP_GET, [](AsyncWebServerRequest *request){
        probe_event = E_req_home;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false);
        response->addHeader("Cache-Control","no-store");
        request->send(response);               
    }); 

    server.on("/video.html", HTTP_GET, [](AsyncWebServerRequest *request){ 
        probe_event = E_req_video;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false, template_processor);
        response->addHeader("Cache-Control","no-store");
        request->send(response);                   
    });    

    server.on("/measure.html", HTTP_GET, [](AsyncWebServerRequest *request){
        probe_event = E_req_measure;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false, template_processor);
        response->addHeader("Cache-Control","no-store");
        request->send(response);               
    });

    server.on("/vas.html", HTTP_GET, [](AsyncWebServerRequest *request){
        probe_event = E_req_vas;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false, template_processor);
        response->addHeader("Cache-Control","no-store");
        request->send(response);               
    });

    server.on("/summation.html", HTTP_GET, [](AsyncWebServerRequest *request){
        probe_event = E_req_temp;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false, template_processor);
        response->addHeader("Cache-Control","no-store");
        request->send(response);               
    });

    server.on("/coldpress.html", HTTP_GET, [](AsyncWebServerRequest *request){
        probe_event = E_req_cold;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false, template_processor);
        response->addHeader("Cache-Control","no-store");
        request->send(response);               
    });

    server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request){
        probe_event = E_req_settings;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false);
        response->addHeader("Cache-Control","no-store");
        request->send(response);        
    });

    server.on("/calibration.html", HTTP_GET, [](AsyncWebServerRequest *request){
        probe_event = E_req_calibration;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false);
        response->addHeader("Cache-Control","no-store");
        request->send(response);        
    });

    server.on("/raw-data.html", HTTP_GET, [](AsyncWebServerRequest *request){
        probe_event = E_req_raw;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false);
        response->addHeader("Cache-Control","no-store");
        request->send(response);        
    });

    server.on("/download.html", HTTP_GET, [](AsyncWebServerRequest *request){
        probe_event = E_req_save;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false, template_processor);
        response->addHeader("Cache-Control","no-store");
        request->send(response);        
    });

    server.on("/resume.html", HTTP_GET, [](AsyncWebServerRequest *request){
        
        if(configuration_data.probe_test_progress.prev_test == TEST_FINISH){
            probe_event = E_req_save;
        }
        else {
            probe_event = E_req_video;
        }
        
        test_flags.resume = 1;
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, evaluate_request(), String(), false, template_processor);
        response->addHeader("Cache-Control","no-store");
        request->send(response);        
    });

    server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.js", String(), false);        
    });

    server.on("/settings.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/settings.js", String(), false);        
    });

    server.on("/raw-data.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/raw-data.js", String(), false);        
    });

    server.on("/download.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/download.js", String(), false);        
    });

    server.on("/video.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/video.js", String(), false);        
    });

    server.on("/measurement.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/measurement.js", String(), false);        
    });

    server.on("/summation.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/summation.js", String(), false);        
    });

    server.on("/coldpress.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/coldpress.js", String(), false);        
    });

    server.on("/calibration.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/calibration.js", String(), false);        
    });

    server.on("/websocket.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/websocket.js", String(), false);
    });

    server.on("/prevent-back-button.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/prevent-back-button.js", String(), false);
    });

    server.on("/vas.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/vas.js", String(), false);
    });

    server.on("/css/app.css", HTTP_GET, [](AsyncWebServerRequest *request){        
        request->send(SPIFFS, "/css/app.css", String(), false);               
    });

    server.on("/sounds/click.mp3", HTTP_GET, [](AsyncWebServerRequest *request){        
        request->send(SPIFFS, "/sounds/click.mp3", String(), false);               
    });

    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){        
        request->send(SPIFFS, "/Mimanex_logo_favi.bmp", String(), false);               
    });

    server.on("/downloadtest.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/full_test_protocol.csv", "text/csv", true);
    });

    server.on("/downloadraw.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/raw_capture_measurement.csv", "text/csv", true);
    });

    //--------------------------INDIVIUAL DOWNLOAD URL's-------------------------------------------//
    //All the following URL's can be removed or kept. they make it possible to 
    //retreive the individual test data on the go by entering the URL's listed here
    
    server.on("/download1.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test_1_sub_1_measurement.csv", "text/csv", true);
    });

    server.on("/download2.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test_1_sub_2_measurement.csv", "text/csv", true);
    });

    server.on("/download3.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test_1_sub_3_measurement.csv", "text/csv", true);
    });

    server.on("/download4.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test_2_sub_1_measurement.csv", "text/csv", true);
    });

    server.on("/download5.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test_2_sub_2_measurement.csv", "text/csv", true);
    });

    server.on("/download6.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test_2_sub_3_measurement.csv", "text/csv", true);
    });

    server.on("/download7.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test_3_sub_1_measurement.csv", "text/csv", true);
    });
    
    server.on("/download8.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test_4_sub_1_vas.csv", "text/csv", true);
    });

    server.on("/download9.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test_5_sub_1_temp.csv", "text/csv", true);
    });

    server.on("/download10.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test_6_sub_1_temp.csv", "text/csv", true);
    });

    server.on("/download11.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test_7_sub_1_cold.csv", "text/csv", true);
    });

    server.on("/download12.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test_8_sub_1_temp.csv", "text/csv", true);
    });    
}