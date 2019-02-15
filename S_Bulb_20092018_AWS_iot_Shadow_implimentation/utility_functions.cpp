#include "global.h"
#include "eeprom_functions.h"
#include "utility_functions.h"
#include "wifi_connectivity.h"
#include "http_webserver.h"
#include "ntp_time.h"
#include "async_ping.h"
#include "aws_iot.h"
#include "websocket_server.h"

extern bool led_status;
extern char encoded_uid[25];
extern char clientid_array[25];
extern bool in_easy_connect_mode;
extern bool is_udp_n_webserver_running;
extern WiFiUDP Udp;
extern unsigned int localUdpPort;
extern ESP8266WebServer http_server;//(80);
extern WebSocketsServer *webSocket;
extern const char *ssid;//"test";  //Soft-AP SSID
extern const char *password;//"password";//Soft-AP Password
extern bool is_connected_to_internet;

void initialize(void)
{
WiFi.mode(WIFI_OFF);
delay(1000);
Serial.begin(115200);
//clear_signature();//only for testing;
//analogWrite(LED,0);
Serial.print("Free Heap at startup = ");
Serial.println(ESP.getFreeHeap());
led_status=0;
//setup_wifi_eventhandlers();
//setup_userid();
//setup_clientid();
}

bool check_if_first_boot(void)
{
  bool result=false;
  EEPROM.begin(512);
  if(EEPROM.read(EEPROM_SIGNATURE_BYTE_1)=='h'&&EEPROM.read(EEPROM_SIGNATURE_BYTE_2)=='s')result=false;
  else result=true;
  EEPROM.end();
  return result;
}

void handle_first_boot(void)
{
  Serial.println("Very First boot!!");
  clear_eeprom();
  write_signature();
  write_device_name(DEVICE_NAME);
}

void stop_running_services(void)
{
 if(is_udp_n_webserver_running)
  {
    Udp.stop();
    http_server.stop();
    aws_disconnect();
    is_udp_n_webserver_running=false;
    is_connected_to_internet=false;
    Serial.println("UDP and WEBSERVER Stopped and Disconnected with AWS IOT");
  }  
}

void start_services(void)
{
  send_ping(HTTP_TEST_URL);//Start HTTP test
  Udp.begin(localUdpPort);
  http_server.begin();
  is_udp_n_webserver_running=true;
  if(is_connected_to_internet)aws_connect();
  Serial.println("Started UDP SERVER and WEBSERVER");
}

void setup_device_name_n_connect_to_preconfigured_ap(void)
{
  char temp_arr1[50];
  char temp_arr2[50];
  String param1="";
  String param2="";
    in_easy_connect_mode=false;//Normal light control Operational mode
//    delete(webSocket);//websocket is required only in setup mode but commented because websocket will be used for intensity control (because it gives real time control) 
//    Serial.println("Websocket deleted!!!");
    int wifi_status;
    Serial.print("Setting WiFi Device Name to ...."); 
    Serial.println(get_device_name());
    WiFi.hostname(get_device_name());
    Serial.println("Preconfigured AP found.");
    Serial.println("Trying to connect with preconfigured AP");
    WiFi.mode(WIFI_STA);
    Serial.println("Configuring module in STA mode");
    Serial.print("Setting WiFi Device Name to ...."); 
    Serial.println(get_device_name());
//    WiFi.hostname(get_device_name());
    get_device_name().toCharArray(temp_arr1,50);
    if(wifi_station_set_hostname(temp_arr1))Serial.println("DEVICE NAME SET.............SUCCESSFULLY");
    else Serial.println("DEVICE NAME SET.............FAIL");
    delay(250);
    param1=get_eeprom_ssid();
    param2=get_eeprom_password();
    param1.toCharArray(temp_arr1,50);
    param2.toCharArray(temp_arr2,50);
    WiFi.begin(temp_arr1,temp_arr2);//Initiate New connection with requested AP
    Serial.printf("Trying to connect with SSID = %s and Password = %s\n\r",temp_arr1,temp_arr2);
    wifi_status=WiFi.status();
    while(wifi_status==WL_DISCONNECTED)
          {
            wifi_status=WiFi.status();//wait for connection result
            Serial.print(".");
            delay(250);
          }
          switch(wifi_status)
          {
            case WL_CONNECTED://connection successfull
              WiFi.setAutoConnect(true);//Configure module to automatically connect on power on to the last used access point
              if(!WiFi.getAutoConnect())WiFi.setAutoReconnect(true);//module will attempt to reconnect to an access point in case it is disconnected
              Serial.println("Connection Successfull !!!!");
              Serial.print("IP address: ");
              Serial.println(WiFi.localIP());
            break;        
            case WL_NO_SSID_AVAIL://either SSID is wrong or not reachable
              Serial.println("Unable to connect");
              Serial.println("Reason : Either SSID is wrong or not reachable");
            break;        
            case WL_CONNECT_FAILED://password is incorrect
              Serial.println("Unable to connect");
              Serial.println("Reason : Incorrect Password");
            break;        
            case WL_IDLE_STATUS://wifi module in changing state
              Serial.println("Unable to connect");
              Serial.println("Reason : WiFi Module is busy");
            break;        
            case WL_DISCONNECTED://module is not configured in station mode
              Serial.println("Unable to connect");
              Serial.println("Reason : Unknown");
            break;        
          }
/*********Setup handler for different http requests***********************************************************/
//    http_server.on("/Ping",handlePing);//Web server for get and post request handling
//    http_server.on("/on",handleon);
//    http_server.on("/off",handleoff);
//    http_server.on("/int",handleint);
    setup_http_request_handleres();
/*********NTP Setup*****************************************************************************************/
//    setSyncProvider(getNtpTime);
//    setSyncInterval(NTP_TIME_SYNC_INTERVAL);
    init_ntp();
/*********Ping Call backs**********************************************************************************/
//    ping.on(true,ping_reply);
//    ping.on(false,ping_complete);
    init_ping();
/*********AWS Iot init**************************************************************************************/
    aws_iot_set_credentials();//setup aws credentials
/***********************************************************************************************************/    
/*****************Start Sending UDP Multicast Packets so that App can listen and get connected****************/             
    /***Listen for broadcast packets***/
    if(wifi_status==WL_CONNECTED)//start UDP Server,Web server and MQTT Client only if module is connected to AP
    {
    send_ping(HTTP_TEST_URL);//start HTTP test;   
    Serial.println("Connected To Network");
    Udp.begin(localUdpPort);//UDP used for device discovery and connection management.
    Serial.printf("Now listening at IP %s, UDP port %d\n\r", WiFi.localIP().toString().c_str(), localUdpPort);
    http_server.begin();
    is_udp_n_webserver_running=true;
    }               
}

void handle_easy_setup_mode(void)
{
    in_easy_connect_mode=true;
    Serial.println("No Preconfigured AP Found.");
    Serial.println("Making this device as AP for configuration.");
    WiFi.mode(WIFI_AP);
    delay(1000);  
    Serial.print("Configuring access point...");
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    Serial.println("Starting WebSocket Server at port 81");
    webSocket->begin();
    webSocket->onEvent(webSocketEvent);  
}
/*********Encode User ID***********************************************************************************/
void setup_userid(void)
{
  Serial.print("Current User ID :");
  Serial.println(get_user_id());
  Serial.print("Encoded User ID :");
  encode_user_id(get_user_id()).toCharArray(encoded_uid,25);
  Serial.println(encoded_uid);        
}
/*********Setup client ID*********************************************************************************/
void setup_clientid(void)
{
Serial.print("Chip ID :");
Serial.println(ESP.getChipId(),HEX);
Serial.print("Flash Chip ID :");
Serial.println(ESP.getFlashChipId(),HEX);
String clientid_string="SMAY_";
sprintf(clientid_array,"%x",ESP.getChipId());
clientid_string+=String(clientid_array);
clientid_string+="_";
sprintf(clientid_array,"%x",ESP.getFlashChipId());
clientid_string+=String(clientid_array);
clientid_string.toCharArray(clientid_array,25);
Serial.print("Client ID :");
Serial.println(clientid_array);
}

