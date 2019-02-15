#include "global.h"
#include "udp_functions.h"
WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char replyPacekt[100];//= "Received......ok";  // a reply string to send back
//char replyPacket[] ="CONNECT_TO;Infra-6A-3;bhjekiuheocyjro";
long last_broadcast_time=0;

bool is_udp_connected=false;
extern bool led_status;
extern int led_intensity;
extern char clientid_array[25];

void monitor_udp_connection(void)
{
       /********Send UDP Broadcast Message if not connected***********************/
        if(!is_udp_connected)
        {
//          if(millis()-last_broadcast_time > UDP_BROADCAST_INTERVAL)
//          {
//            send_broadcast();
//            last_broadcast_time=millis();
//          }
        }
        /**************************************************************************/   
}

void handle_udp_requests(void)
{
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
}

void send_broadcast(void)
{
  String reply;
  char temp_arr[100];
  IPAddress broadcast_ip;
  broadcast_ip = ~WiFi.subnetMask() | WiFi.gatewayIP();
  Serial.print("Broadcast IP is : ");
  Serial.println(broadcast_ip);
  reply="BROADCAST-" + WiFi.localIP().toString();
  Serial.print("Sending Broadcat Message--->");
  Serial.println(reply);
  reply.toCharArray(temp_arr,100);
  Udp.beginPacket(broadcast_ip, localUdpPort);
  Udp.write(temp_arr);
  Udp.endPacket();      
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
    if(led_status)reply=String(command)+/*';'+"CONNECTED" +*/ ';' + WiFi.localIP().toString() + ';' + String(wifi_station_get_hostname())+';'+'1'+';'+String(led_intensity)+';'+String(clientid_array);
    else reply=String(command)+/*';'+"CONNECTED" +*/ ';' + WiFi.localIP().toString() + ';' + String(wifi_station_get_hostname())+';'+'0'+';'+String(led_intensity)+';'+String(clientid_array);
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

