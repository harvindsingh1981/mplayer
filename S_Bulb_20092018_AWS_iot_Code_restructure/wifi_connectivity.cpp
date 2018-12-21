#include "global.h"
#include "wifi_connectivity.h"
#include "eeprom_functions.h"

/***********WiFi Event Handlers*******************************/
WiFiEventHandler ap_stationConnectedHandler;
WiFiEventHandler ap_stationDisconnectedHandler;
WiFiEventHandler sta_ConnectedHandler;
WiFiEventHandler sta_DisconnectedHandler;
WiFiEventHandler sta_AuthModeChanged;
WiFiEventHandler sta_GotIP;
WiFiEventHandler sta_DHCPTimeout;
/*************************************************************/
const char *ssid = "test";  //Soft-AP SSID
const char *password = "password";//Soft-AP Password

extern bool is_udp_connected;

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

