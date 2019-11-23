//INCLUDES---------------------------------------------------------------------------------
#include "main.h" 
//INCLUDES END-----------------------------------------------------------------------------

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

HX711 Probe;

AsyncWebServer wifi_config_server(80);
AsyncWebServer server(80);

WebSocketsServer webSocket = WebSocketsServer(81);

DNSServer dns;

Ticker ticker;
Ticker TimeTicker;

const size_t capacity = JSON_OBJECT_SIZE(10) + 130; //size obtained from https://arduinojson.org/v6/assistant/
DynamicJsonDocument probe_configuration(capacity);

PROBE_DATA configuration_data{};

int restart = 0;

void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

//callback notifying us of the need to save config, thers still exist a bug, which does not allow closing and opening a new webserver on the same port without power reset.

void saveConfigCallback () {  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("Wifi config saved!");
  display.println("");
  display.println("Please Restart device...");  
  display.display();
  restart = 1;

}

void configModeCallback (AsyncWiFiManager *myWiFiManager) {
  ticker.attach(0.2, tick);

  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  display.clearDisplay();
  display.fillRect(0,0,display.width(),12,WHITE);
  display.setTextColor(BLACK,WHITE);
  display.setTextSize(1);    
  display.setCursor(25, 2);
  display.println("Configure WiFi");  
  display.setTextColor(WHITE);
  display.setCursor(0, 14);
  display.println("");
  display.println("Connect to SSID");
  display.println(myWiFiManager->getConfigPortalSSID());  
  display.println("with password:");
  display.println("12345678");
  display.println(WiFi.softAPIP());  
  display.display(); 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.6 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);  
  //Initialize OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  //Display LOGO
  display.drawBitmap((SCREEN_WIDTH - mimanex_WIDHT) / 2, (SCREEN_HEIGHT - mimanex_HEIGHT) / 2,
      Mimanex_LOGO, mimanex_WIDHT, mimanex_HEIGHT, 1);
  display.display();
  delay(2000);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  } 
  //load configuration data
  load_probe_configuration();

#ifdef PROBE_DEBUG
  Serial.println(configuration_data.calibration_settings.calibration_factor);
#endif

  //Initialize load cell 
  Probe.begin(HX711_DATA, HX711_CLK, 64);
  Probe.set_scale(configuration_data.calibration_settings.calibration_factor);
  Probe.tare();
  
  //Start WiFi manager
  AsyncWiFiManager wifiManager(&wifi_config_server,&dns);  
  //set callback when no known WiFi could connect 
  wifiManager.setAPCallback(configModeCallback);
  //set callback for saving configuration
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  //check if a known wifi exist, and try connect, else start AP with "PressureProbe", "12345678"
  if (!wifiManager.autoConnect("PressureProbe", "12345678")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    ESP.restart();
    delay(1000);
  }

  if(restart == 1){
    while(1){};
    ESP.restart();//"soft restart is not enough for the new webserver to function after wifi configuration has been made...."
    delay(5000);
  }

  //initialize the webserver
  probe_server_init();
  //start server
  server.begin();
  //initialize websocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);  

  //display server is started
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("SERVER STARTED");  
  display.println("IP address:");
  display.println(WiFi.localIP());
  display.display(); 
  
  //stop LED blinking, as we are now connected and ready
  ticker.detach();
  
  //keep LED on
  digitalWrite(BUILTIN_LED, LOW);   
}

void loop() {
  //the probe state pointer points to the valid state, one of the functions in 'Probe_State_Machine.cpp'    
  (*probe_state_pointer)();
  //need a loop for websocket as it is synchronous, the websocket included in 'ESPAsyncWebServer' proved to be unreliable...
  webSocket.loop();
  delay(5);  
}