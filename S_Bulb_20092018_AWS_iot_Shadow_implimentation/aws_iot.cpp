#include "global.h"
#include "aws_iot.h"
#include "utility_functions.h"
#include "eeprom_functions.h"
/************For IR remote hub only*******************/
//#include <IRremoteESP8266.h>
//#include <IRsend.h>
//const uint16_t kIrLed = 14;
//IRsend irsend(kIrLed);
//uint16_t rawData[68] = {4600,4350,700,1550,650,1550,650,1600,650,450,650,450,650,450,650,450,700,400,700,1550,650,1550,650,1600,650,450,650,450,650,450,700,450,650,450,650,450,650,1550,700,450,650,450,650,450,650,450,650,450,700,400,650,1600,650,450,650,1550,650,1600,650,1550,650,1550,700,1550,650,1550,650};
 
/*****************************************************/

char aws_endpoint[]    = "XXXXXXXXXXXXXXXXXXXXXXX";
char aws_key[]         = "XXXXXXXXXXXXXXXXXXXXXXX";
char aws_secret[]      = "XXXXXXXXXXXXXXXXXXXXXXX";
char aws_region[]      = "XXXXXXXXXXXXXXXXXXXXXXX";
const char* aws_topic  = "test2";
int port = 443;

//MQTT config
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;


AWSWebSocketClient awsWSclient(1000);
PubSubClient mqtt_client(awsWSclient);


extern char clientid_array[25];
extern char encoded_uid[25];
extern bool is_connected_to_internet;
extern bool led_status;
extern int led_intensity;

//#define MQTT_MAX_PACKET_SIZE 2048

void aws_iot_set_credentials(void)
{
    awsWSclient.setAWSRegion(aws_region);
    awsWSclient.setAWSDomain(aws_endpoint);
    awsWSclient.setAWSKeyID(aws_key);
    awsWSclient.setAWSSecretKey(aws_secret);
    awsWSclient.setUseSSL(true);
//    irsend.begin();  
}

void aws_message_handler(char* topic, byte* payload, unsigned int length)
{
  //StaticJsonBuffer<MQTT_MAX_PACKET_SIZE> jsonBuffer;//128 is default message length for pub-sub client, can be changed in "PubSubClient.h"
  DynamicJsonBuffer jsonBuffer(1536);//declared "dynamic buffer" instead of "static buffer" because dynamic buffer is declared in heap where as static buffer is declared in stack and esp8266 has stack of 4k bytes only.
  String temp_str="";
  char mqtt_topic[100];
  char mqtt_payload[MQTT_MAX_PACKET_SIZE];
   
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) 
  {
    Serial.println("Failed to parse JSON Object");
    return;
  }
  if(String(topic).equals(String(encoded_uid)+'/'+"Beacon"))
  {
    aws_publish("Info");
    aws_subscribe("On_off");
    aws_subscribe("Int");
  }
  else if(String(topic).equals(String(encoded_uid)+'/'+String(clientid_array)+'/'+"On_off"))
  {
   bool device_status=root["device_status"];
   if(device_status)
   {
    analogWrite(LED,1023);
    led_status=1; 
   }
   else
   {
    analogWrite(LED,0);
    led_status=0; 
   }      
//    aws_publish("Status");
     root["namespace"]="Alexa.PowerController";
     root["name"]="powerState";
     JsonObject& directive = root.createNestedObject("directive");
     JsonObject& header = directive.createNestedObject("header");
     header["namespace"]="Device.AsyncResponse";
     header["name"]="On_Off";
     temp_str=String(encoded_uid)+'/'+String(clientid_array)+'/'+"Status";
     temp_str.toCharArray(mqtt_topic,100);
     Serial.print("Message Payload : ");
     root.printTo(Serial);
     Serial.println();
     root.printTo(mqtt_payload);
     Serial.print("Publish to Topic");
     Serial.println(temp_str);
     int rc = mqtt_client.publish(mqtt_topic,mqtt_payload);
     Serial.print("Publish Return Code = ");
     Serial.println(rc); 

//     aws_publish("Shadow_Update");
     jsonBuffer.clear();
     JsonObject& root1 = jsonBuffer.createObject();
     Serial.print("Root Object after clearing : ");
     root1.printTo(Serial);
     Serial.println("");
     JsonObject& state = root1.createNestedObject("state");
     JsonObject& reported = state.createNestedObject("reported");
     if(led_status)reported["status"]="ON";
     else reported["status"]="OFF";
     temp_str="$aws/things/"+String(clientid_array)+"/shadow/update";
     temp_str.toCharArray(mqtt_topic,100);
     root1.printTo(Serial);
     Serial.println();
     root1.printTo(mqtt_payload);
     Serial.print("Publish to Topic");
     Serial.println(temp_str);
     rc = mqtt_client.publish(mqtt_topic,mqtt_payload);
     Serial.print("Publish Return Code = ");
     Serial.println(rc); 
     Serial.println();     
  }
  else if(String(topic).equals(String(encoded_uid)+'/'+String(clientid_array)+'/'+"Int"))
  {
    int intensity=root["intensity"];
    led_intensity=intensity;
    analogWrite(LED,intensity);
    aws_publish("Status");
  }
}

bool aws_connect(void)
{
    if(mqtt_client.connected()) 
    {    
        mqtt_client.disconnect();
    }
    Serial.print("Available Heap before AWS Connection : ");
    Serial.println(ESP.getFreeHeap());
    char* clientID = clientid_array;
    mqtt_client.setServer(aws_endpoint, port);
    if(mqtt_client.connect(clientID))
    {
      Serial.println("Connected To AWS");
      Serial.print("Available Heap after AWS Connection : ");
      Serial.println(ESP.getFreeHeap());
      mqtt_client.setCallback(aws_message_handler);
      aws_publish("Info");
      aws_subscribe("Shadow_update_accept");
      aws_subscribe("Shadow_update_rejected");
      aws_subscribe("Beacon");     
      aws_publish("Shadow_Update");
      aws_subscribe("On_off");
      aws_subscribe("Int");
      return true;
    }
    else
    {
      Serial.print("Failed To connect to AWS, Return Code=");
      Serial.println(mqtt_client.state());
      return false;
    }       
}

void aws_disconnect(void)
{
 if(mqtt_client.connected())
  {
    mqtt_client.disconnect();
  } 
}

bool aws_subscribe(char * topic)
{
 String temp_str="";
 char temp_arr[100];
 if(String(topic).equals("Beacon"))temp_str=String(encoded_uid)+'/'+String(topic);
 else if(String(topic).equals("Shadow_update_accept"))temp_str="$aws/things/"+String(clientid_array)+"/shadow/update/accepted";
 else if(String(topic).equals("Shadow_update_rejected"))temp_str="$aws/things/"+String(clientid_array)+"/shadow/update/rejected";
 else temp_str=String(encoded_uid)+'/'+String(clientid_array)+'/'+String(topic);
 temp_str.toCharArray(temp_arr,100);
 Serial.print("Subscribing to topic ");
 Serial.print(temp_arr);
 if(mqtt_client.subscribe(temp_arr))
 {
  Serial.println(".....Success");
  return true;
 }
 else 
 {
  Serial.println(".....Failed"); 
  return false;
 }
}

void aws_publish(char* topic)
{
  Serial.print("Free Heap at AWS_PUBLISH entry = ");
  Serial.println(ESP.getFreeHeap());

  //StaticJsonBuffer<MQTT_MAX_PACKET_SIZE> jsonBuffer;//128 is default message length for pub-sub client, has been changed to 512 in "PubSubClient.h"
  DynamicJsonBuffer jsonBuffer(1024);//declared "dynamic buffer" instead of "static buffer" because dynamic buffer is declared in heap where as static buffer is declared in stack and esp8266 has stack of 4k bytes only. 
  JsonObject& root = jsonBuffer.createObject();
  String temp_str="";
  char mqtt_topic[100];
  char mqtt_payload[MQTT_MAX_PACKET_SIZE];

  if(String(topic).equals("Info"))
  {
    root["user_id"]=String(encoded_uid);
    root["client_id"]=String(clientid_array);
    root["device_name"]=get_device_name();
    root["device_status"]=led_status;
    root["intensity"]=led_intensity;  
  }
  else if(String(topic).equals("Status"))
  {
    root["device_status"]=led_status;
    root["intensity"]=led_intensity;      
  }
  else if(String(topic).equals("Shadow_Update"))
  {
    JsonObject& state = root.createNestedObject("state");
    JsonObject& reported = state.createNestedObject("reported");
    if(led_status)reported["status"]="ON";
    else reported["status"]="OFF";
//    root["device_status"]=led_status;
//    root["intensity"]=led_intensity;      
  }
  if(String(topic).equals("Info"))temp_str=String(encoded_uid)+'/'+String(topic);
  else if(String(topic).equals("Shadow_Update"))temp_str="$aws/things/"+String(clientid_array)+"/shadow/update";
  else temp_str=String(encoded_uid)+'/'+String(clientid_array)+'/'+String(topic);
  temp_str.toCharArray(mqtt_topic,100);
  root.printTo(Serial);
  Serial.println();
  root.printTo(mqtt_payload);
  Serial.print("Publish to Topic");
  Serial.println(topic);
  int rc = mqtt_client.publish(mqtt_topic,mqtt_payload);
  Serial.print("Publish Return Code = ");
  Serial.println(rc); 
  Serial.print("Free Heap at AWS_PUBLISH entry = ");
  Serial.println(ESP.getFreeHeap());
}

void monitor_aws_connection(void)
{
  if(is_connected_to_internet)
  {
    if(awsWSclient.connected())
    {
      set_indicator(CLOUD_CONNECTED);
      mqtt_client.loop();
    }
    else
    {
     if(WiFi.status()==WL_CONNECTED && WiFi.localIP().toString()!="0.0.0.0")set_indicator(WIFI_CONNECTED);
     else set_indicator(WIFI_DISCONNECTED);
      aws_connect();
    }
  }
  else if(mqtt_client.connected())
  {
    mqtt_client.disconnect();
  }
}

