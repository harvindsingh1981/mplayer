/******************************************************************************************************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
       /**********MODIFY ALL FUNCTIONS RETURNING LOCAL VARIABLES WITH REFRENCE***************/
/******************************************************************************************************/
extern "C" {
#include "user_interface.h"
}
#include <stdlib.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <WebSocketsServer.h>
#include <StringSplitter.h>
#include <ESP8266WebServer.h>
/********************NTP Time Server********************************************************************/
#define NTP_TIME_SERVER                 "pool.ntp.org"
#define NTP_TIME_SYNC_INTERVAL          5 //Duration in seconds , this is time between two successive syncs
//long ntp_last_retry_attempt=0;
char timeServer[]= NTP_TIME_SERVER;
const unsigned long timeZone = (5*3600)+(30*60);//currently set for india i.e +5:30 Hrs, Format : ([Hours * 3600] + [Minutes*60])
/********************MQTT Settings********************************************************************/
#define MQTT_BROKER_IP                              "192.168.43.225"
#define MQTT_BROKER_USERNAME                        "harvind"
#define MQTT_BROKER_PASSWORD                        "singh"
#define MQTT_PORT                                   1883
#define MQTT_BROKER_RECONNECT_INTERVAL              5000
#define MQTT_SUBSCRIBED_TOPIC_MONITOR_INTERVAL      5000
#include <AsyncMqttClient.h> //https://github.com/marvinroger/async-mqtt-client + me-no-dev/ESPAsyncTCP library required for asyncmqttclient library.
AsyncMqttClient mqtt_client;
long mqtt_last_retry_attempt=0;
long subscribed_topic_monitor=0;
char clientid_array[25];
char encoded_uid[25];

#define TOTAL_SUBSCRIBED_TOPIC  5
int mqtt_topic_subscription_status[TOTAL_SUBSCRIBED_TOPIC][2];//To maintain status if topic subscription request in successfull or not
                                                              //format [Topic(predefined)][[0]Subscriber ID [1]Status]
                                                              //Topic 0:Beacon , 1:Int , 2:On_off
#define TOPIC_BEACON_INDEX      0
#define TOPIC_INT_INDEX         1
#define TOPIC_ON_OFF_INDEX      2
                                                              
/*****************************************************************************************************/
/***************Async Ping Library********************************************************************/
#define HTTP_TEST_URL                   "google.com"
#define HTTP_TEST_INTERVAL              5000
#include "AsyncPing.h" //https://github.com/akaJes/AsyncPing
AsyncPing ping;
long last_http_test_time=0;
bool is_connected_to_internet=false;//Global flag to check if device is connected to internet.
/*****************************************************************************************************/

const char *ssid = "test";
const char *password = "password";
//String ret_val;
bool ws_conn_status[5]={false,false,false,false,false};
unsigned long ws_last_ping_time[5]={0,0,0,0,0};
#define DEVICE_NAME                   "Harvind_Singh"
#define EEPROM_SSID_ADDR              0         //Max 32 characters excluding '\0' Start@0,  END@31,  '\0'@32 //max length can be 32 characters excluding '\0'
#define EEPROM_PASSWORD_ADDR          40        //Max 20 charscters excluding '\0' Start@40, END@59,  '\0'@60 //max length can be 63 characters excluding '\0'
#define EEPROM_DEVICE_NAME_ADDR       61        //Max 32 characters excluding '\0' Start@61, END@92 , '\0'@93 
#define EEPROM_USER_ID                94        //Max 100 characters excluding '\0' Starts@94, END@193, '\0'@194 // Currently limited to 100 character due to eeprom limitation, actual valid email address can be 254 characters, need to change in future.
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

#define SETUP_MODE_FORCE_ENTRY  false

void setup() 
{
char temp_arr1[50];
char temp_arr2[50];
String param1="";
String param2="";

WiFi.mode(WIFI_OFF);
delay(1000);
Serial.begin(115200);
//clear_signature();//only for testing;
analogWrite(LED,0);
led_status=0;
setup_wifi_eventhandlers();
if(check_if_first_boot())
  {
    Serial.println("Very First boot!!");
    clear_eeprom();
    write_signature();
    write_device_name(DEVICE_NAME);
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
/*********NTP Setup*****************************************************************************************/
    setSyncProvider(getNtpTime);
    setSyncInterval(NTP_TIME_SYNC_INTERVAL);
/*********Ping Call backs**********************************************************************************/
    ping.on(true,ping_reply);
    ping.on(false,ping_complete);        
/*********Setup handler for different MQTT Client***********************************************************/
    mqtt_client.onConnect(mqtt_onconnect);
    mqtt_client.onDisconnect(mqtt_ondisconnect);
    mqtt_client.onSubscribe(mqtt_onsubscribe);
    mqtt_client.onUnsubscribe(mqtt_unsubscribe);
    mqtt_client.onMessage(mqtt_onmessage);
    mqtt_client.onPublish(mqtt_onpublish);
/*********Setup MQTT Client********************************************************************************/
    mqtt_client.setServer(MQTT_BROKER_IP, MQTT_PORT);
    mqtt_client.setCredentials(MQTT_BROKER_USERNAME,MQTT_BROKER_PASSWORD);
    /*Create MQTT Client ID************************/ 
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
    /*Create MQTT Client ID************************/ 
    mqtt_client.setClientId(clientid_array);
    Serial.print("MQTT Client ID : ");
    Serial.println(clientid_array);
/*********Other MQTT Client options*************************************************************************/
//  mqttClient.setKeepAlive(uint16_t keepAlive);//Keep alive in seconds, Default is 15 Sec
//  mqttClient.setCleanSession(bool cleanSession);//clean session wanted or not, Defaults to true
//  mqttClient.setMaxTopicLength(uint16_t maxTopicLength);//Maximum allowed topic length to receive,Defaults to 128
//  mqttClient. setWill(const char* topic, uint8_t qos, bool retain, const char* payload = nullptr, size_t length = 0);
    /*
     topic: Topic of the LWT
     qos: QoS of the LWT
     retain: Retain flag of the LWT
     payload: Payload of the LWT. If unset, the payload will be empty
     length: Payload length. If unset or set to 0, the payload will be considered as a string and its size will be calculated using strlen(payload)
    */
//   mqtt_client.setSecure(bool secure);//secure: SSL wanted or not,Defaults to false
//   mqtt_client.addServerFingerprint(const uint8_t* fingerprint);//fingerprint: Fingerprint to add, By default, if no fingerprint is added, any fingerprint is accepted
/*********Other MQTT Supported API's*************************************************************************/
/*
Return if the client is currently connected to the broker or not.
*/
//bool connected();
/*
Connect to the server.
*/
//void connect();
/*
Disconnect from the server.
force: Whether to force the disconnection. Defaults to false (clean disconnection).
*/
//void disconnect(bool force = false);
/*
Subscribe to the given topic at the given QoS.
Return the packet ID or 0 if failed.
topic: Topic
qos: QoS
*/
//uint16_t subscribe(const char* topic, uint8_t qos);
/*
Unsubscribe from the given topic.
Return the packet ID or 0 if failed.
topic: Topic
*/
//uint16_t unsubscribe(const char* topic);
/*
Publish a packet.
Return the packet ID (or 1 if QoS 0) or 0 if failed.
topic: Topic
qos: QoS
retain: Retain flag
payload: Payload. If unset, the payload will be empty
length: Payload length. If unset or set to 0, the payload will be considered as a string and its size will be calculated using strlen(payload)
dup: Duplicate flag. If set or set to 1, the payload will be flagged as a duplicate
message_id: The message ID. If unset or set to 0, the message ID will be automtaically assigned. Use this with the DUP flag to identify which message is being duplicated 
*/
//uint16_t publish(const char* topic, uint8_t qos, bool retain, const char* payload = nullptr, size_t length = 0, bool dup = false, uint16_t message_id = 0)

/*****************Start Sending UDP Multicast Packets so that App can listen and get connected****************/             
    /***Listen for broadcast packets***/
    if(wifi_status==WL_CONNECTED)//start UDP Server,Web server and MQTT Client only if module is connected to AP
    {
    Serial.println("Connected To Network");
    Serial.print("Current User ID :");
    Serial.println(get_user_id());
    Serial.print("Encoded User ID :");
    encode_user_id(get_user_id()).toCharArray(encoded_uid,25);
    Serial.println(encoded_uid/*encode_user_id(get_user_id())*/);
    Udp.begin(localUdpPort);//UDP used for device discovery and connection management.
    Serial.printf("Now listening at IP %s, UDP port %d\n\r", WiFi.localIP().toString().c_str(), localUdpPort);
    server.begin();
    mqtt_client.connect();
    mqtt_last_retry_attempt=millis();;
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

time_t prevDisplay = 0;
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
    if(wifi_status==WL_CONNECTED && WiFi.localIP().toString()!="0.0.0.0")
    {
        String temp_string;
        /*********Monitor MQTT Connection***************************************************************************************************************************************************************************************/    
        if(!mqtt_client.connected())
        {
              if(millis()-mqtt_last_retry_attempt > MQTT_BROKER_RECONNECT_INTERVAL)
              {
                Serial.print(millis());
                Serial.print(":");
                Serial.println("Retrying to connect with MQTT Broker");
                mqtt_last_retry_attempt=millis();
                mqtt_client.connect();
              }
        }
        else //Device is connected to MQTT Broker
        {
          if(millis()-subscribed_topic_monitor > MQTT_SUBSCRIBED_TOPIC_MONITOR_INTERVAL)
          {
                if(!mqtt_topic_subscription_status[TOPIC_BEACON_INDEX][1]) //if device is connected to MQTT broker and not subscribed to "[UID]/[Device_id]/Beacon" topic then subscribe it; // This is for increasing robustness
                {
                  if(mqtt_topic_subscription_status[TOPIC_BEACON_INDEX][0]==0)//if mqtt_client returns Sub_id =0 then need to disconnect and re-connect to mqtt broker [REASON NOT KNOWN, WHY CLIENT RETURNS 0 ID, NEED TO FIND] 
                  {
                    Serial.println("Invalid Subscription ID for [UID]/[DEVICE ID]/Beacon Topic......Disconnecting from MQTT Broker");
                    mqtt_client.disconnect(false);//false is passed for clean disconnect, true is for forcefull disconnect.
                                                  //reconnect will be handled by loop() function
                  }
                  {
                     Serial.println("Not subscribed to [UID]/[Device_id]/Beacon.....retrying to Subscribe");
                     subscribe_topic(TOPIC_BEACON_INDEX);
                  }
                }
                else //if it has already subscribed to "[UID]/[Device_id]/Beacon" then check for other topics.
                {
                  
                }
          subscribed_topic_monitor=millis();  
          }
        }
        /***************************************************************************************************************************************************************************************************************************/
        
        /*******HTTP Test**********************************/
        if(millis()-last_http_test_time > HTTP_TEST_INTERVAL)
        {
          //Serial.print("HTTP Test,Sending ping to ...");
          //Serial.println(HTTP_TEST_URL);
          //send_ping(HTTP_TEST_URL);
          last_http_test_time=millis();
        }
        /**************************************************/
        
        /*******NTP Time Update****************************/
        if (timeStatus() != timeNotSet) 
        {
          if (now() != prevDisplay) 
          { //update the display only if time has changed
            prevDisplay = now();
            //digitalClockDisplay();  
          }
        }        
        /**************************************************/     
        
        /*********Handle UDP requests**********************/    
        int packetSize = Udp.parsePacket();
        if (packetSize)
        {
          if(Udp.remotePort()==123)//Handle response from NTP server
          {
            //ignore NTP response in this loop
          }
          else
          {
            // receive incoming UDP packets other then NTP
            Serial.printf("Received %d bytes from %s, port %d\n\r", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
            int len = Udp.read(incomingPacket, 255);
            if (len > 0)
            {
              incomingPacket[len] = 0;//Append '\0' at the end of string
            }
            Serial.printf("UDP packet contents: %s\n\r", incomingPacket);
            process_udp_request(Udp.remoteIP(), Udp.remotePort(),incomingPacket);                
          }
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
        Serial.println("Connected To Network");
        Serial.print("Current User ID :");
        Serial.println(get_user_id());
        Serial.print("Encoded User ID :");
        encode_user_id(get_user_id()).toCharArray(encoded_uid,25);
        Serial.println(encoded_uid/*encode_user_id(get_user_id())*/);
        Udp.begin(localUdpPort);
        server.begin();
        is_udp_n_webserver_running=true;
        mqtt_client.connect();
        Serial.println("Started UDP SERVER,WEBSERVER and MQTT Client");
        mqtt_last_retry_attempt=millis();
      }
    }
  /**************************************************/         
  }
}

/***************MQTT Client Event Handlers****************************************/
void print_sub_stat(void)
{
  int i;
  Serial.printf("TOPIC\tSUB_ID\tSTATUS\n");
  for(i=0;i<TOTAL_SUBSCRIBED_TOPIC;i++)
  {
    switch(i)
    {
      case TOPIC_BEACON_INDEX:
        Serial.print("/Beacon");      
      break;
      case TOPIC_INT_INDEX:
        Serial.print("/Int");      
      break;
      case TOPIC_ON_OFF_INDEX:
        Serial.print("/On_off");      
      break;
      default:
        Serial.print("--NA--");      
      break;
    }
    Serial.print("\t");
    if(mqtt_topic_subscription_status[i][0]!= -1)
    {
      Serial.print(" ");
      Serial.print(mqtt_topic_subscription_status[i][0]);
    }
    else Serial.print(mqtt_topic_subscription_status[i][0]);
    Serial.print("\t");
    if(mqtt_topic_subscription_status[i][1]>0)Serial.println("TRUE");
    else Serial.println("FALSE");
  }
}

void subscribe_topic(int topic_id)
{
 String temp_str="";
 char temp_arr[100];
 int sub_id;
 Serial.print("Subscribing to ");
 switch(topic_id)
 {
    case TOPIC_BEACON_INDEX:
       temp_str=String(encoded_uid)+'/'+"Beacon";
       Serial.print("Topic(For Device Discovery) : ");
    break;
    case TOPIC_INT_INDEX:
      temp_str=String(encoded_uid)+'/'+String(clientid_array)+'/'+"Int";
      Serial.print("Topic(For Intensity Control) : ");
    break;
    case TOPIC_ON_OFF_INDEX:
      temp_str=String(encoded_uid)+'/'+String(clientid_array)+'/'+"On_off";
      Serial.print("Topic(For ON/OFF) : ");
    break;
  }
 Serial.print(temp_str);
 Serial.print(", Sub_id :");
 temp_str.toCharArray(temp_arr,100);
 sub_id=mqtt_client.subscribe(temp_arr, 0);
 Serial.println(sub_id);
 update_mqtt_topic_subscription_id(topic_id,sub_id,false);
}

void clear_mqtt_topic_subscription_status() //Clear All subscribtion status to false
{
 for(int i=0;i<TOTAL_SUBSCRIBED_TOPIC;i++)
 {
  mqtt_topic_subscription_status[i][0]=-1;
  mqtt_topic_subscription_status[i][1]=-1;  
 }
print_sub_stat(); 
}

void update_mqtt_topic_subscription_status(int sub_id,int stat)
{
  int i;
  for(i=0;i<TOTAL_SUBSCRIBED_TOPIC;i++)
  {
    if(mqtt_topic_subscription_status[i][0]==sub_id)break;
  }
  mqtt_topic_subscription_status[i][1]=stat;
//  print_sub_stat();
}

void update_mqtt_topic_subscription_id(int sub_topic,int sub_id,int stat)
{
  mqtt_topic_subscription_status[sub_topic][0]=sub_id;
  mqtt_topic_subscription_status[sub_topic][1]=stat;  
//  print_sub_stat();
}

void mqtt_onmessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  String temp_str="";
  char mqtt_topic[100];
  char mqtt_payload[100];
  int sub_id;
  int pub_id;
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
  Serial.println(encoded_uid);
  if(String(topic).equals(String(encoded_uid)+'/'+"Beacon"))
  {
    temp_str=String(encoded_uid)+'/'+"Info";
    temp_str.toCharArray(mqtt_topic,100);
    temp_str=String(clientid_array)+';'+get_device_name();
    temp_str.toCharArray(mqtt_payload,100);
    Serial.println("Discovery Packet received over MQTT");
    Serial.println("Sending Device Info to App");
    Serial.print("Topic : ");
    Serial.println(mqtt_topic);
    Serial.print("Payload : ");
    Serial.println(mqtt_payload);
//  mqtt_client.publish("test/lol", 0, true, "test 1");//topic,qos,retain,payload
    pub_id=mqtt_client.publish(mqtt_topic,1,true,mqtt_payload);//QOS level is kept as 1 because this message should be delivered surely, App will not be able to publish any messages without this info.
                                                               //Retain is kept true because this message should be delivered immediately after App subscribe to this "Info" Topic
    Serial.println("Publish ID :"+String(pub_id));

    Serial.println("Ready to take commads from APP");
    subscribe_topic(TOPIC_INT_INDEX);
    
    Serial.print("Topic(For ON/OFF) : ");
    subscribe_topic(TOPIC_ON_OFF_INDEX);
  }
  else if(String(topic).equals(String(encoded_uid)+'/'+String(clientid_array)+'/'+"Int"))
  {
    int intensity;
    intensity=String(payload).toInt();
    Serial.print("Setting light Intensity to ....");
    Serial.println(intensity);
    analogWrite(LED,intensity);
    led_intensity=intensity;
  }
  else if(String(topic).equals(String(encoded_uid)+'/'+String(clientid_array)+'/'+"On_off"))
  {
    Serial.println("Inside On_off control");
    Serial.print("Payload : ");
    Serial.println(String(payload));
    String(payload).toCharArray(mqtt_payload,100);
    mqtt_payload[len]='\0';
    Serial.println(mqtt_payload);
    if(String(mqtt_payload/*payload*/).equals("on"))
    {
    analogWrite(LED,255);
    led_status=1;
    }
    else if(String(mqtt_payload/*payload*/).equals("off"))
    {
    analogWrite(LED,0);
    led_status=0;      
    }
  }
}

void mqtt_onconnect(bool sessionPresent)
{
    String temp_str="";
    char temp_arr[100];
    int sub_id;
    Serial.println("Connected to MQTT.");
    Serial.print("Session present: ");
    Serial.println(sessionPresent);
    Serial.println("Marking All Subscribed Topic as Unsubscribed");
    clear_mqtt_topic_subscription_status();
    subscribe_topic(TOPIC_BEACON_INDEX);
    subscribed_topic_monitor=millis();
}

void mqtt_onsubscribe(uint16_t packetId, uint8_t qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
  update_mqtt_topic_subscription_status(packetId,true);
}

void mqtt_unsubscribe(uint16_t packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  update_mqtt_topic_subscription_status(packetId,true);
}

void mqtt_onpublish(uint16_t packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void mqtt_ondisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.println("Disconnected from MQTT broker");
  Serial.println("Marking All Subscribed Topic as Unsubscribed");
  clear_mqtt_topic_subscription_status();
  
//  Serial.print("Reason : ");
//  Serial.println(String(reason));
/*  
  switch(reason)
  {
    case TCP_DISCONNECTED:
      Serial.println("TCP_DISCONNECTED");
    break;
    case MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
      Serial.println("MQTT_UNACCEPTABLE_PROTOCOL_VERSION");
    break;
    case MQTT_IDENTIFIER_REJECTED:
      Serial.println("MQTT_IDENTIFIER_REJECTED");
    break;
    case MQTT_SERVER_UNAVAILABLE:
      Serial.println("MQTT_SERVER_UNAVAILABLE");
    break;
    case MQTT_MALFORMED_CREDENTIALS:
      Serial.println("MQTT_MALFORMED_CREDENTIALS");
    break;
    case MQTT_NOT_AUTHORIZED:
      Serial.println("MQTT_NOT_AUTHORIZED");
    break;
    case ESP8266_NOT_ENOUGH_SPACE:
      Serial.println("ESP8266_NOT_ENOUGH_SPACE");
    break;
    case TLS_BAD_FINGERPRINT:
      Serial.println("TLS_BAD_FINGERPRINT");
    break;
  }
*/  
}
/*********************************************************************************/

/***************AsyncPing Event Handlers******************************************/
bool ping_reply(const AsyncPingResponse& response)
{
  IPAddress addr(response.addr); //to prevent with no const toString() in 2.3.0
  if (response.answer)
  {
    Serial.printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n", response.size, addr.toString().c_str(), response.icmp_seq, response.ttl, response.time);    
  }
  else
  {
    Serial.printf("no answer yet for %s icmp_seq=%d\n", addr.toString().c_str(), response.icmp_seq);    
  }
  return false; //do not stop  
}

bool ping_complete(const AsyncPingResponse& response)
{
  IPAddress addr(response.addr); //to prevent with no const toString() in 2.3.0
  Serial.printf("total answer from %s sent %d recevied %d time %d ms\n",addr.toString().c_str(),response.total_sent,response.total_recv,response.total_time);
  if(response.total_recv)is_connected_to_internet=true;
  else is_connected_to_internet=false;
  if (response.mac)
  {
    Serial.printf("detected eth address " MACSTR "\n",MAC2STR(response.mac->addr));    
  }
  return true; 
}

void send_ping(char * ping_url)
{
 // char *ping_url=HTTP_TEST_URL;
  IPAddress address;
  if(!WiFi.hostByName(ping_url, address))
  {
    address.fromString(ping_url);    
  }
  ping.begin(address);
}
/*********************************************************************************/
/********************HTTP Webserver Event Handlers********************************/
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
/*********************************************************************************/
/*********************Other Utility Functions*************************************/
bool check_ap_availablity(String ssid)
{
  bool ap_found=false;
  int ap_count=0;
  Serial.print("Scanning AP's........");
  ap_count=WiFi.scanNetworks();
  Serial.printf("%d AP's Found\n\r",ap_count);
  Serial.print("Searching for...");
  Serial.println(ssid);
  for(int i=0;i<ap_count;i++)
  {
    if(WiFi.SSID(i).equals(ssid))
    {
      Serial.println("AP found !!!");
      ap_found=true;
      break;          
    }
  }
return ap_found;
}

bool connect_to_prestored_ap(void)
{
    int timeout=0;
    char temp_arr1[50];
    char temp_arr2[50];
    String param1="";
    String param2="";    
    int wifi_status;
    param1=get_eeprom_ssid();
    param2=get_eeprom_password();
    param1.toCharArray(temp_arr1,50);
    param2.toCharArray(temp_arr2,50);
/************Before Connecting check if AP is available*************************************/
/*******************************************************************************************/
    if(check_ap_availablity(param1))//Before Connecting check if AP is available
    {    
        WiFi.disconnect(true);
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
    else
    {
      return false;
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
  if(SETUP_MODE_FORCE_ENTRY)return false;
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

void save_user_id(char * user_id)
{
  eeprom_write_string(user_id,EEPROM_USER_ID);
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

String get_user_id()
{
  return eeprom_get_string(EEPROM_USER_ID);
}

String encode_user_id(String uid)
{
/*********************************/
//Encoded String format = USERID(without special characters)_CHECKSUM(original UID)
//checksum = (Position*character Ascii Value) + .................+(Position*Ascii Value)
//Starting position = 1 
/*********************************/  
  char arr[256];
  char *p;
  unsigned long checksum=0;
  String final_str="";
  int i=1;
  uid.toCharArray(arr,256);
  p=arr;
  while(*p!='\0')
  {
    checksum=checksum+(i**p);
  /* //currently encoding uid as checksum only because of limited MQTT Topic size         
  if((*p>='a' && *p<='z')||(*p>='A' && *p<='Z')||(*p>='0' && *p<='9'))
    {
      final_str+=*p;
    }
   */ 
    i++;
    p++;
  }
// final_str=/*final_str*/"SMAYANTR" + '_' + String(checksum);
// return final_str;
return "SMAYANTR_"+String(checksum); 
}

String eeprom_get_string(int addr)
{
char arr[40];
char *p;
char a='q';
p=arr;
EEPROM.begin(512);
  while(a!='\0')
  {
    a=EEPROM.read(addr++);
    *p++=a;
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
char temp_arr5[256];//specifically for handling user id as max length can be 254
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
else if(command.equals("SAVE_UID"))//Seperate function for saving UID because Arduino websocket Server max payload is 256 whereas user id max length can be 254 bytes , need to implement seperate function in future for SSID, PASSWORD and device name if cumulative length exceeds 256 bytes 
  {
    param_1=splitter->getItemAtIndex(1);//USER_ID
    Serial.print("RECEIVED USER ID : ");
    Serial.println(param_1);    
    param_1.toCharArray(temp_arr5,256);
    save_user_id(temp_arr5);    
    Serial.print("SAVED USER ID : ");
    Serial.println(get_user_id());
    webSocket->sendTXT(num,"SAVED_UID");
  }
else if(command.equals("RESTART"))
{
  ESP.restart();  
}

delete(splitter);
}

/**********************NTP and Time Related Functions******************************************************/
void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets


// send an NTP request to the time server at the given address
void sendNTPpacket(char *address/*IPAddress &address*/)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
//  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
//      Serial.println("Receive NTP Response");
//      Serial.print("Remote Port : ");
//      Serial.println(Udp.remotePort());
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone ;//* SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}
/**********************************************************************************************************/


