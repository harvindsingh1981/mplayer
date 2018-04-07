extern "C" {
#include "user_interface.h"
}
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <WebSocketsServer.h>
#include <StringSplitter.h>
#include <ESP8266WebServer.h>
/********************MQTT Settings********************************************************************/
#define MQTT_BROKER_IP                  "192.168.43.225"
#define MQTT_BROKER_USERNAME            "harvind"
#define MQTT_BROKER_PASSWORD            "singh"
#define MQTT_PORT                       1883
#define MQTT_BROKER_RECONNECT_INTERVAL  5000
#include <ESP8266MQTTClient.h>//Mqtt client Library - Modification made in ESP8266MQTTClient.h and ESP8266MQTTClient.cpp to make ".onDisconnect" work
MQTTClient mqtt_client;
String mqtt_broker_uri="";
/*****************************************************************************************************/
const char *ssid = "test";
const char *password = "password";
//String ret_val;
bool ws_conn_status[5]={false,false,false,false,false};
unsigned long ws_last_ping_time[5]={0,0,0,0,0};
#define DEVICE_NAME                   "Harvind_Singh"
#define EEPROM_SSID_ADDR              0         //Max 32 characters excluding '\0' Start@0,  END@31,  '\0'@32
#define EEPROM_PASSWORD_ADDR          40        //Max 20 charscters excluding '\0' Start@40, END@59,  '\0'@60
#define EEPROM_DEVICE_NAME_ADDR       61        //Max 32 characters excluding '\0' Start@61, END@92 , '\0'@93 
#define EEPROM_IF_PRE_CONFIGURED_AP   509       //'y' for yes and 'n' for no
#define EEPROM_SIGNATURE_BYTE_1       510       //'h'
#define EEPROM_SIGNATURE_BYTE_2       511       //'s'

#define RED_LED     13
#define GREEN_LED   12
#define BLUE_LED    15
#define LED         12
bool led_status=0;
int led_intensity=0;
#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...)
#endif
/***********WiFi Event Handlers*******************************/
WiFiEventHandler ap_stationConnectedHandler;
WiFiEventHandler ap_stationDisconnectedHandler;
WiFiEventHandler sta_ConnectedHandler;
WiFiEventHandler sta_DisconnectedHandler;
WiFiEventHandler sta_AuthModeChanged;
WiFiEventHandler sta_GotIP;
WiFiEventHandler sta_DHCPTimeout;
/*************************************************************/
//#define DEBUG_WEBSOCKETS(...) Serial.printf( __VA_ARGS__ )

/***************UDP Config***********************/
WiFiUDP Udp;
ESP8266WebServer server(80);
WebSocketsServer *webSocket = new WebSocketsServer(81);

unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char replyPacekt[100];//= "Received......ok";  // a reply string to send back
//char replyPacket[] ="CONNECT_TO;Infra-6A-3;bhjekiuheocyjro";
bool in_easy_connect_mode=false;
bool is_udp_connected=false;
bool is_udp_n_webserver_running=false;

void setup() 
{
char temp_arr1[50];
char temp_arr2[50];
String param1="";
String param2="";
mqtt_broker_uri+="mqtt://";
mqtt_broker_uri+=MQTT_BROKER_USERNAME;//"harvind";
mqtt_broker_uri+=":";
mqtt_broker_uri+=MQTT_BROKER_PASSWORD;//"singh";
mqtt_broker_uri+="@";
mqtt_broker_uri+=MQTT_BROKER_IP;//"192.168.43.225";
mqtt_broker_uri+=":";
mqtt_broker_uri+=MQTT_PORT;//"1883";
mqtt_broker_uri+="#";
mqtt_broker_uri+="ESP_";
mqtt_broker_uri+=String(ESP.getChipId());

WiFi.mode(WIFI_OFF);
delay(1000);
Serial.begin(115200);
//clear_signature();//only for testing;
//pinMode(RED_LED, OUTPUT);
//pinMode(GREEN_LED, OUTPUT);
//pinMode(BLUE_LED, OUTPUT);
//pinMode(LED, OUTPUT);
//digitalWrite(LED,0);
analogWrite(LED,0);
led_status=0;
//while(1)
//{
//  digitalWrite(RED_LED,1);
//  digitalWrite(GREEN_LED,1);
//  digitalWrite(BLUE_LED,1);
//  delay(500);
//  digitalWrite(RED_LED,0);
//  digitalWrite(GREEN_LED,0);
//  digitalWrite(BLUE_LED,0);
//  delay(500);
//}
setup_wifi_eventhandlers();
if(check_if_first_boot())
  {
    Serial.println("Very First boot!!");
    clear_eeprom();
    write_signature();
    write_device_name(DEVICE_NAME);
//    Serial.print("Device name is .....");
//    Serial.println(get_device_name());
//    WiFi.hostname(get_device_name());
  }
else
  {
    Serial.println("Valid Signature detected....");
  }
if(check_if_pre_configured_ap())
  {
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
    server.on("/Ping",handlePing);//Web server for get and post request handling
    server.on("/on",handleon);
    server.on("/off",handleoff);
    server.on("/int",handleint);
    mqtt_client.onData(mqtt_ondata);
    mqtt_client.onConnect(mqtt_onconnect);
    mqtt_client.onSubscribe(mqtt_onsubscribe);
    mqtt_client.onPublish(mqtt_onpublish);
    mqtt_client.onDisconnect(mqtt_ondisconnect);//Modification made in ESP8266MQTTClient.h and ESP8266MQTTClient.cpp to make ".onDisconnect" work
/*****************Start Sending UDP Multicast Packets so that App can listen and get connected****************/             
    /***Listen for broadcast packets***/
    if(wifi_status==WL_CONNECTED)//start UDP and Web server only if module is connected to AP
    {
    Udp.begin(localUdpPort);//UDP used for device discovery and connection management.
    Serial.printf("Now listening at IP %s, UDP port %d\n\r", WiFi.localIP().toString().c_str(), localUdpPort);
    server.begin();
    mqtt_client.begin(mqtt_broker_uri/*"mqtt://harvind:singh@192.168.43.225:1883#ESP_"*/);
    is_udp_n_webserver_running=true;
    }             
  }
else
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
}
void handleon(void)
{
//  digitalWrite(LED,1);
  analogWrite(LED,255);
  server.send(200,"text/plain","1");
  led_status=1;
}
void handleoff(void)
{
//  digitalWrite(LED,0);
  analogWrite(LED,0);
  server.send(200,"text/plain","0");
  led_status=0;  
}

void handlePing(void)
{
  String reply;
  char str[]="Pong";
  if(led_status)reply=String(str)+';'+String(wifi_station_get_hostname())+';'+'1'+';'+String(led_intensity);
  else reply=String(str)+';'+String(wifi_station_get_hostname())+';'+'0'+';'+String(led_intensity);
  Serial.println("Received Ping.......");
//  server.send(200,"text/plain","Pong");
  server.send(200,"text/plain",reply);
  is_udp_connected=true;
}
void handleint(void)
{
  String message = "";
  int intensity;
  if (server.arg("Intensity")== ""){     //Parameter not found
//  message = "Intensity Argument not found";
  }
  else
  {     //Parameter found
//  message = "Intensity Argument = ";
//  message += server.arg("Intensity");     //Gets the value of the query parameter
//    message=server.arg("Intensity");
//    intensity=message.toInt();
//    analogWrite(LED,intensity);
    message="ok";
    intensity=server.arg("Intensity").toInt();
    Serial.print("Setting light Intensity to ....");
    Serial.println(intensity);
    analogWrite(LED,intensity);
    led_intensity=intensity;
  }
  server.send(200, "text/plain", message); //Returns the HTTP response
}

bool connect_to_prestored_ap(void)
{
    int timeout=0;
    char temp_arr1[50];
    char temp_arr2[50];
    String param1="";
    String param2="";    
    int wifi_status;
    WiFi.disconnect(true);
    param1=get_eeprom_ssid();
    param2=get_eeprom_password();
    param1.toCharArray(temp_arr1,50);
    param2.toCharArray(temp_arr2,50);
    WiFi.begin(temp_arr1,temp_arr2);//Initiate New connection with requested AP
    Serial.printf("Trying to connect with SSID = %s and Password = %s\n\r",temp_arr1,temp_arr2);
    wifi_status=WiFi.status();
    while(wifi_status==WL_DISCONNECTED && timeout<=80)
          {
            wifi_status=WiFi.status();//wait for connection result
            Serial.print(".");
            delay(250);
            timeout++;
          }
          switch(wifi_status)
          {
            case WL_CONNECTED://connection successfull
              WiFi.setAutoConnect(true);//Configure module to automatically connect on power on to the last used access point
              if(!WiFi.getAutoConnect())WiFi.setAutoReconnect(true);//module will attempt to reconnect to an access point in case it is disconnected
              Serial.println("Connection Successfull !!!!");
              Serial.print("IP address: ");
              Serial.println(WiFi.localIP());
              return true;
            break;        
            case WL_NO_SSID_AVAIL://either SSID is wrong or not reachable
              Serial.println("Unable to connect");
              Serial.println("Reason : Either SSID is wrong or not reachable");
              return false;
            break;        
            case WL_CONNECT_FAILED://password is incorrect
              Serial.println("Unable to connect");
              Serial.println("Reason : Incorrect Password");
              return false;
            break;        
            case WL_IDLE_STATUS://wifi module in changing state
              Serial.println("Unable to connect");
              Serial.println("Reason : WiFi Module is busy");
              return false;
            break;        
            case WL_DISCONNECTED://module is not configured in station mode
              Serial.println("Unable to connect");
              Serial.println("Reason : Unknown");
              return false;
            break;        
          }
}
/********************************MQTT CALLBACKS***********************************************/
void mqtt_ondata(String topic, String data, bool cont)
{
    Serial.printf("Data received, topic: %s, data: %s\r\n", topic.c_str(), data.c_str());
    mqtt_client.unSubscribe("/qos0");
}

void mqtt_onconnect()
{
    Serial.println("Connected to MQTT Broker");
    Serial.print("Subscribing topic:Power , Sub_id :");
    Serial.println(mqtt_client.subscribe("Power", 0));
    Serial.print("Subscribing topic:Intensity , Sub_id :");
    Serial.println(mqtt_client.subscribe("Intensity", 0));
}

void mqtt_onsubscribe(int sub_id)
{
  Serial.printf("Subscribe topic id: %d ok\r\n", sub_id);
}

void mqtt_onpublish(int pub_id)
{
  Serial.printf("Publish topic id: %d ok\r\n", pub_id);  
}

void mqtt_ondisconnect(void)
{
  Serial.println("Disconnected from MQTT broker");
}
/*********************************************************************************************/

void loop() 
{
  if(in_easy_connect_mode)
  {
  webSocket->loop();
  update_alive_status();
  }
  else
  {
    int wifi_status=WiFi.status(); 
    if(wifi_status==WL_CONNECTED)
    {
        String temp_string;
  /*********Handle MQTT requests*********************/    
      mqtt_client.handle();
  /**************************************************/     
  /*********Handle UDP requests**********************/    
        int packetSize = Udp.parsePacket();
        if (packetSize)
        {
          // receive incoming UDP packets
          Serial.printf("Received %d bytes from %s, port %d\n\r", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
          int len = Udp.read(incomingPacket, 255);
          if (len > 0)
          {
            incomingPacket[len] = 0;//Append '\0' at the end of string
          }
          Serial.printf("UDP packet contents: %s\n\r", incomingPacket);
          process_udp_request(Udp.remoteIP(), Udp.remotePort(),incomingPacket);    
        }
  /*********Handle HTTP requests**********************/    
      server.handleClient();    //Handling of incoming requests
  /**************************************************/     
    }
  /*****Connection Lost with AP.....Reconnect********/    
    else //Module is not connected to AP
    {
      if(is_udp_n_webserver_running)
      {
        Udp.stop();
        server.stop();
        is_udp_n_webserver_running=false;
        Serial.println("UDP and WEBSERVER Stopped");
      }
      Serial.println("Unable to connect......Trying again");
      if(connect_to_prestored_ap())
      {
        Udp.begin(localUdpPort);
        server.begin();
        is_udp_n_webserver_running=true;
        Serial.println("Started UDP and WEBSERVER");
        mqtt_client.begin(mqtt_broker_uri);
      }
    }
  /**************************************************/         
  }
}

void process_udp_request(IPAddress ip,int port,char* command)
{
  String reply;
  char temp_arr[100];
  //delay(250);
  if(String(command).equals("CONNECT_REQ"))
  {
    if(!is_udp_connected)//if connection is made previously then do not respond to multicast messages simply reply "OK" in else condition
    {
    if(led_status)reply=String(command)+/*';'+"CONNECTED" +*/ ';' + WiFi.localIP().toString() + ';' + String(wifi_station_get_hostname())+';'+'1'+';'+String(led_intensity);
    else reply=String(command)+/*';'+"CONNECTED" +*/ ';' + WiFi.localIP().toString() + ';' + String(wifi_station_get_hostname())+';'+'0'+';'+String(led_intensity);
    }
/*************Below block is commented as sending sending OK on connect request may block reply from other modules at app side***********/
//    else
//    {
//      reply="OK";
//    }
/****************************************************************************************************************************************/
  }
  else if(String(command).equals("PING"))
  {
    reply=String(command)+';' + WiFi.localIP().toString();
    is_udp_connected=true;      
  }
  else if(String(command).equals("CONNECTED"))//Acknowledgement from app that this device is added in connected list.
  {
    reply=String(command)+';' + WiFi.localIP().toString();
    is_udp_connected=true;      
  }
  else if(String(command).equals("DISCONNECT"))//request from app to disconnect
  {
    reply=String(command)+';' + WiFi.localIP().toString();
    is_udp_connected=false;          
  }
/************Send Reply to App*******************************/  
  if(reply!="")
  {
    Serial.println(reply);
    reply.toCharArray(temp_arr,100);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(temp_arr);
    Udp.endPacket();    
  }
/*************************************************************/
}

void setup_wifi_eventhandlers()
{
  ap_stationConnectedHandler=WiFi.onSoftAPModeStationConnected(&ap_onstationConnected);
  ap_stationDisconnectedHandler=WiFi.onSoftAPModeStationDisconnected(&ap_onstationDisconnected);
  sta_ConnectedHandler=WiFi.onStationModeConnected(&sta_onConnected);
  sta_DisconnectedHandler=WiFi.onStationModeDisconnected(&sta_onDisconnected);
  sta_AuthModeChanged=WiFi.onStationModeAuthModeChanged(&sta_onAuthModeChanged);
  sta_GotIP=WiFi.onStationModeGotIP(&sta_onGotIP);
  sta_DHCPTimeout=WiFi.onStationModeDHCPTimeout(&sta_onDHCPTimeout);
}

void ap_onstationConnected(const WiFiEventSoftAPModeStationConnected& evt)
{
  Serial.println("[WIFI AP EVENT][Station Connected]");
}
void ap_onstationDisconnected(const WiFiEventSoftAPModeStationDisconnected& evt)
{
  Serial.println("[WIFI AP EVENT][Station DisConnected]");  
}
void sta_onConnected(const WiFiEventStationModeConnected& evt)
{
  String ip;
  Serial.println("[WIFI STA EVENT][Connected]");    
  Serial.print("IP = ");
  ip=WiFi.localIP().toString();
  Serial.println(ip);
  if(ip.equals("0.0.0.0"))
  {
    Serial.println("Invalid IP");
  }
  Serial.print("WiFi Status =");
  Serial.println(WiFi.status());          
}
void sta_onDisconnected(const WiFiEventStationModeDisconnected& evt)
{
  Serial.println("[WIFI STA EVENT][DisConnected]");
  is_udp_connected=false;//if Station is disconnected from AP then "is_udp_connected" is made false so that device can start responding to multicast packets and get connected to APP again.this flag is used in "process_udp_request";
  Serial.println("Connection Lost from APP !!!");     
}
void sta_onAuthModeChanged(const WiFiEventStationModeAuthModeChanged& evt)
{
  Serial.println("[WIFI STA EVENT][Auth Mode Changed]");      
}
void sta_onGotIP(const WiFiEventStationModeGotIP& evt)
{
  String ip;
  Serial.print("[WIFI STA EVENT][GOT IP] : ");
  Serial.print("IP = ");
  ip=WiFi.localIP().toString();
  Serial.println(ip);
  if(ip.equals("0.0.0.0"))
  {
    Serial.println("Invalid IP");
  }
  Serial.print("WiFi Status =");
  Serial.println(WiFi.status());          
}
void sta_onDHCPTimeout(void)
{
    Serial.println("[WIFI STA EVENT][DHCP Timeout]");      
}

void update_alive_status()
{
char i;
  for(i=0;i<5;i++)
    {
      if(ws_conn_status[i])
      {
        if((millis()-ws_last_ping_time[i])>1500)
        {
          webSocket->disconnect(i);
          ws_conn_status[i]=false;  
          Serial.print("Current Time.....");
          Serial.println(millis());        
          Serial.print("Last ping Time.....");
          Serial.println(ws_last_ping_time[i]);        
        }
      }
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n\r", num);
            ws_conn_status[num]=false;
            break;
        case WStype_ERROR:
            Serial.printf("[%u] Error Occured!\n\r", num);        
            break;    
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket->remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n\r", num, ip[0], ip[1], ip[2], ip[3], payload);
                ws_conn_status[num]=true;
                ws_last_ping_time[num]=millis();
        
        // send message to client
        //webSocket->sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            //Serial.printf("[%u] get Text: %s\n\r", num, payload);
            ws_conn_status[num]=true;
            process_request(String((char *)payload),num);
            // send message to client
            // webSocket.sendTXT(num, "message here");

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            Serial.printf("[%u] get binary length: %u\n\r", num, length);
            hexdump(payload, length);

            // send message to client
            // webSocket.sendBIN(num, payload, length);
            break;
    }

}

bool check_if_first_boot()
{
  bool result=false;
  EEPROM.begin(512);
  if(EEPROM.read(EEPROM_SIGNATURE_BYTE_1)=='h'&&EEPROM.read(EEPROM_SIGNATURE_BYTE_2)=='s')result=false;
  else result=true;
  EEPROM.end();
  return result;
}

void clear_eeprom()
{
  int i;
  EEPROM.begin(512);
  Serial.print("clearing EEPROM");
  for(i=0;i<512;i++)
  {
    EEPROM.write(i,0);
    Serial.print(".");
    delay(70);
  }
  Serial.println();
  Serial.println("EEPROM Cleared !!");
  EEPROM.end();
}
void write_signature()
{
  EEPROM.begin(512);
  Serial.print("Writing Signature....");
  EEPROM.write(EEPROM_IF_PRE_CONFIGURED_AP,'n');//since it's very first boot or factory reset hence all previous configured AP are deleted and this byte should be set to 'n' to indicate that no pre-configured AP are present.
  EEPROM.write(EEPROM_SIGNATURE_BYTE_1,'h');//byte "510" and "511" contains signature byte to indicate that it's not first boot
  EEPROM.write(EEPROM_SIGNATURE_BYTE_2,'s');
  Serial.println("Done.");
  EEPROM.end();
}

void write_device_name(char * str)
{
  eeprom_write_string(str,EEPROM_DEVICE_NAME_ADDR);
}

bool check_if_pre_configured_ap()//return "true" if byte "509" contains 'y' else return "false"
{
  bool result=false;
  EEPROM.begin(512);
  if(EEPROM.read(EEPROM_IF_PRE_CONFIGURED_AP)=='n')result=false;
  if(EEPROM.read(EEPROM_IF_PRE_CONFIGURED_AP)=='y')result=true;
  EEPROM.end();
  return result;
}

void save_ssid_password(char* ssid,char* password)
{
  eeprom_write_string(ssid,EEPROM_SSID_ADDR);
  eeprom_write_string(password,EEPROM_PASSWORD_ADDR);
  EEPROM.begin(512);
  EEPROM.write(EEPROM_IF_PRE_CONFIGURED_AP,'y');
  EEPROM.end();
}

String get_device_name()
{
  return eeprom_get_string(EEPROM_DEVICE_NAME_ADDR);
}

String get_eeprom_ssid()
{
  return eeprom_get_string(EEPROM_SSID_ADDR);
}

String get_eeprom_password()
{
  return eeprom_get_string(EEPROM_PASSWORD_ADDR);
}

String eeprom_get_string(int addr)
{
//  String temp_str="";
//  char a='a';//init variable other then 0
//  char i=1;
//EEPROM.begin(512);  
//while(a!='\0')
//  {
//    a=EEPROM.read(addr++);
//    temp_str=temp_str+a;  
//    Serial.printf("Read Byte %d....%c\n\r",i++,a);
//  }
//EEPROM.end();  
//return temp_str;  
char arr[40];
char *p;
char a='q';
//char i=1;
p=arr;
EEPROM.begin(512);
  while(a!='\0')
  {
    a=EEPROM.read(addr++);
    *p++=a;
//    Serial.printf("Read Byte %d....%c\n\r",i++,a);
  }
  *p='\0';
EEPROM.end();
return String(arr);
}

void eeprom_write_string(char* str,int addr)
{
//  char i=1;
  EEPROM.begin(512);
  while(*str!='\0')
  {
    EEPROM.write(addr,*str++);
    addr++;
//    Serial.printf("Written Byte %d\n\r",i++);
  }
  EEPROM.write(addr,*str);
  EEPROM.end();
}

void clear_signature()
{
  EEPROM.begin(512);
  EEPROM.write(EEPROM_SIGNATURE_BYTE_1,0);//byte "510" and "511" contains signature byte to indicate that it's not first boot
  EEPROM.write(EEPROM_SIGNATURE_BYTE_2,0);
  EEPROM.end();
}

//WiFi.mode(m) : set mode to WIFI_AP , WIFI_STA , WIFI_AP_STA or WIFI_OFF
void process_request(String str,uint8_t num)
{
String command;
String param_1;
String param_2;
String param_3;
String fail_reason;
String ssid="",password="",device_name="";
char device_name_changed='n';
int param_count;
bool conn_status=false;
bool is_connected=false;
bool ap_found=false;
int wifi_status;
int timeout;
int ap_count;
char temp_arr1[50];
char temp_arr2[50];
char temp_arr3[50];
char temp_arr4[100];

StringSplitter *splitter = new StringSplitter(str, ';', 5);  // new StringSplitter(string_to_split, delimiter, limit)
param_count = splitter->getItemCount();
command=splitter->getItemAtIndex(0);
//Serial.print("Command received : ");
//Serial.println(command);
if(command.equals("CONNECT_TO"))
  {
    WiFi.mode(WIFI_AP);
    param_1=splitter->getItemAtIndex(1);//SSID
    param_2=splitter->getItemAtIndex(2);//Password
    param_3=splitter->getItemAtIndex(3);//Device Name
    Serial.print("SSID : ");
    Serial.println(param_1);
    Serial.print("PASSWORD : ");
    Serial.println(param_2);
    Serial.print("DEVICE NAME : ");
    Serial.println(param_3);
    WiFi.mode(WIFI_AP_STA);
    Serial.println("Changing WiFi mode to STA + AP");
    param_1.toCharArray(temp_arr1,50);
    param_2.toCharArray(temp_arr2,50);
    param_3.toCharArray(temp_arr3,50);
    ssid=param_1;
    password=param_2;
    device_name=param_3;
    if(WiFi.isConnected() && WiFi.localIP().toString()!="0.0.0.0")//check if wifi module is connected to AP
    {
      if(WiFi.SSID().equals(param_1))//check if already connected to requested AP 
      {
        conn_status=true;
//        WiFi.disconnect(true);
//        conn_status=false;
        Serial.println("Already Connected to requested AP");
        if(/*WiFi.hostname()*/String(wifi_station_get_hostname()).equals(device_name))device_name_changed='n';//this is for showing pop up in app that only device name is changed and will be reflected after next reboot
        else device_name_changed='y';
        is_connected=true;
/**********below code section is for changing Device Name at run time during setup but it does not works (intermitent device name chane and if already connected to same AP it does not connects again)***********/
////        if(device_name.equals(get_device_name()))
//        if(WiFi.hostname().equals(device_name))
//        {
//          Serial.print("Device Name is also same....");
//          Serial.println(get_device_name());
//          param_1=command+';'+"SUCCESS"+';'+WiFi.localIP().toString()+';'+get_device_name();
//          is_connected=true;
//        }
//        else
//        {
//          Serial.println("Device Name is different......");
//          Serial.print("Current Device Name :");
////          Serial.println(get_device_name());
//          Serial.println(WiFi.hostname());
//          Serial.print("New Device Name :");
//          Serial.println(device_name);
////          write_device_name(temp_arr3);
////          Serial.print("Device Name changed to....");
////          Serial.println(get_device_name());
//          Serial.println("Disconnecting from Network....");//Need to restart wifi and reconnect so that device name can be updated.
//          WiFi.setAutoConnect(false);
//          WiFi.setAutoReconnect(false);
//          WiFi.mode(WIFI_AP);
//          while (WiFi.status() == WL_CONNECTED)Serial.print(".");//Wait for device disconnect from AP.
//          WiFi.mode(WIFI_AP_STA);
//          while (WiFi.status() == WL_IDLE_STATUS);
//          Serial.println("Disconnected !!!");
//          is_connected=false;//if this is set to false, then reconnection will be taken care in following code.
//        }
/********************************************************************************************************************************************************************************************************************/
      }
      else 
      {
        Serial.println("Module Connected but with different AP.....Disconnecting");
        WiFi.setAutoConnect(false);   //All this is make shift Arrangement
        WiFi.setAutoReconnect(false); //as WiFi.disconnect()
        WiFi.mode(WIFI_AP);           //is crashing esp8266 module.
        WiFi.mode(WIFI_AP_STA);       // on arduino version 1.8.4
//        WiFi.disconnect();          //need to try with other versions of arduino 
//        while (WiFi.status() == WL_IDLE_STATUS);//make sure wi-fi module is not in transit state;
        while (WiFi.status() == WL_CONNECTED)Serial.print(".");//Wait for device disconnect from AP.
        Serial.println("Disconnected !!!");
        is_connected=false;//if not connected to requested AP then disconnect
      }
    }
    else
    {
      Serial.println("Module Not Connected to WiFi");
      is_connected=false;
    }

    if(is_connected==false)
    {
        WiFi.mode(WIFI_AP);//changing mode only to AP because STA and AP uses same radio and while scanning AP's in WIFI_AP_STA mode, makes AP unstable and websocket server connection is intermitant.
        Serial.print("Scanning AP's........");
        ap_count=WiFi.scanNetworks();
        Serial.printf("%d AP's Found\n\r",ap_count);
        Serial.printf("Searching for \"%s\"\n\r",temp_arr1);
        for(int i=0;i<ap_count;i++)
        {
          if(WiFi.SSID(i).equals(String(temp_arr1)))
          {
            Serial.println("AP found !!!");
            ap_found=true;
            break;          
          }
        }
        if(!ap_found)
        {
          Serial.println("AP Not Found");
          fail_reason="AP not found";
          param_1=command + ';' + "FAIL" + ';' + fail_reason;
          ap_found=false;
        }
        else
        {
          WiFi.mode(WIFI_AP_STA);//enabling AP and STA mode for making connection only;
//          delay(1000);   
          Serial.print("Setting WiFi Device Name to ...."); 
          //Serial.println(device_name);
          Serial.printf("%s\n\r",temp_arr3);    
          if(wifi_station_set_hostname(temp_arr3))Serial.println("DEVICE NAME SET.............SUCCESSFULLY");
          else Serial.println("DEVICE NAME SET.............FAIL");
//          delay(250);
          Serial.printf("Trying to connect with : SSID-%s , Password-%s\n\r",temp_arr1,temp_arr2);
          WiFi.begin(temp_arr1,temp_arr2);//Initiate New connection with requested AP
          wifi_status=WiFi.status();
          timeout=1000;//1000*35 = 35000ms or 35 Sec
          while(wifi_status==WL_DISCONNECTED && timeout>0)
          {
            wifi_status=WiFi.status();//wait for connection result
            Serial.print(".");
            delay(35);
            webSocket->loop();
            update_alive_status();
            timeout --;
          }
          switch(wifi_status)
          {
            case WL_CONNECTED://connection successfull
              WiFi.setAutoConnect(true);//Configure module to automatically connect on power on to the last used access point
              WiFi.setAutoReconnect(true);//module will attempt to reconnect to an access point in case it is disconnected
              Serial.println("Connection Successfull !!!!");
              Serial.print("IP address: ");
              Serial.println(WiFi.localIP());
              conn_status=true;
            break;        
            case WL_NO_SSID_AVAIL://either SSID is wrong or not reachable
              Serial.println("Unable to connect");
              Serial.println("Reason : Either SSID is wrong or not reachable");
              conn_status=false;
              fail_reason="SSID wrong or unreachable";
            break;        
            case WL_CONNECT_FAILED://password is incorrect
              Serial.println("Unable to connect");
              Serial.println("Reason : Incorrect Password");
              conn_status=false;
              fail_reason="Incorrect Password";
            break;        
            case WL_IDLE_STATUS://wifi module in changing state
              Serial.println("Unable to connect");
              Serial.println("Reason : WiFi Module is busy");
              conn_status=false;
              fail_reason="WiFi Module Busy";
            break;        
            case WL_DISCONNECTED://module is not configured in station mode
              Serial.println("Unable to connect");
              Serial.println("Reason : Unknown");
              conn_status=false;
              fail_reason="Unknown";
            break;        
          }
        }
    }
      Serial.println("Waiting for server connection");
      timeout=200;
      webSocket->loop();
      update_alive_status();
      while(ws_conn_status[num]==false && timeout>0)
      {
        webSocket->loop();
        update_alive_status();
        Serial.print(".");
        delay(35);
        timeout--;        
      }
      if(timeout>0)
      {
        if(conn_status)
        {
        Serial.println("Sending Success Message to App");
        param_1=command+';'+"SUCCESS"+';'+WiFi.localIP().toString()+';'+/*WiFi.hostname()*/String(wifi_station_get_hostname())+';'+device_name_changed;//get_device_name();
        }
        else
        {
        //WiFi.mode(WIFI_AP);
        Serial.println("Sending Fail Message to App");
        param_1=command+';'+"FAIL"+';'+fail_reason;          
        }
      }
      else
      {
        Serial.println("Connection Lost with App");
        Serial.println("High time to restart ESP8266 Here");
        /*High time to restart ESP8266 Here*/        
      }
//    param_1.toCharArray(temp_arr1,50);
    param_1.toCharArray(temp_arr4,100);
    webSocket->sendTXT(num,temp_arr4);
    if(!conn_status)WiFi.mode(WIFI_AP);//if unable to connect with selected AP then there is no point to remain in STA mode because unconnected STA (Always searching for AP) makes websocket connection unstable.
    if(conn_status)
    {
      //param_1=String(temp_arr1);
      //param_2=String(temp_arr2);
      ssid.toCharArray(temp_arr1,50);
      password.toCharArray(temp_arr2,50);
      device_name.toCharArray(temp_arr3,50);
      //save_ssid_password(temp_arr1,temp_arr2);
    }
    
//      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
//      Udp.write(temp_arr1);
//      Udp.endPacket(); 
//    }
  }
else if(command.equals("PING"))
  {
    ws_last_ping_time[num]=millis();
//    Serial.print("Received PING.....");
//    Serial.println(ws_last_ping_time[num]);
    param_1="PONG";
    param_1.toCharArray(temp_arr1,50);
    webSocket->sendTXT(num,temp_arr1);
  }
else if(command.equals("SAVE"))
  {
    param_1=splitter->getItemAtIndex(1);//SSID
    param_2=splitter->getItemAtIndex(2);//Password
    param_3=splitter->getItemAtIndex(3);//Device Name
    Serial.print("RECEIVED SSID : ");
    Serial.println(param_1);
    Serial.print("RECEIVED PASSWORD : ");
    Serial.println(param_2);
    Serial.print("RECEIVED DEVICE NAME : ");
    Serial.println(param_3);
    param_1.toCharArray(temp_arr1,50);    
    param_2.toCharArray(temp_arr2,50);    
    param_3.toCharArray(temp_arr3,50);
    save_ssid_password(temp_arr1,temp_arr2);
    write_device_name(temp_arr3);    
    Serial.print("SAVED SSID : ");
    Serial.println(get_eeprom_ssid());
    Serial.print("SAVED PASSWORD : ");
    Serial.println(get_eeprom_password());
    Serial.print("SAVED DEVICE NAME : ");
    Serial.println(get_device_name());
    webSocket->sendTXT(num,"SAVED");
  }
else if(command.equals("RESTART"))
{
  ESP.restart();  
}
//else if(command.equals("INT"))
//{
//  param_1=splitter->getItemAtIndex(1);//intensity value as 1st parameter
//  Serial.print("Setting Intensity to....");
//  Serial.println(param_1);
//}
//else if(command.equals("DISCONNECT"))
//{
//Serial.print("[WS] Disconnecting with Client.....");
//Serial.println(num);
//webSocket->disconnect(num);
///**************************************************/
//delete(webSocket);
//WebSocketsServer *webSocket = new WebSocketsServer(81);
//webSocket->begin();
//webSocket->onEvent(webSocketEvent);
///***************************************************/
//}
delete(splitter);
}

