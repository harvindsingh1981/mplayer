/**********************************************************************************/


/*********************************ADD IPV6 SUPPORT********************************/


/**********************************************************************************/
#include "global.h"
#include "async_ping.h"
#include "aws_iot.h"
#include "eeprom_functions.h"
#include "http_webserver.h"
#include "ntp_time.h"
#include "udp_functions.h"
#include "websocket_server.h"
#include "wifi_connectivity.h"
#include "utility_functions.h"

char clientid_array[25];
bool in_easy_connect_mode=false;
bool is_udp_n_webserver_running=false;
bool led_status=0;
int led_intensity=0;

void set_indicator(int red,int green,int blue)
{
  analogWrite(RED_LED,red);
  analogWrite(GREEN_LED,green);
  analogWrite(BLUE_LED,blue);
}

void setup() 
{
  set_indicator(WIFI_DISCONNECTED);
  initialize();
  if(check_if_first_boot())
  {
    handle_first_boot();
  }
  else
  {
    Serial.println("Valid Signature detected....");
    setup_wifi_eventhandlers();
    setup_userid();
    setup_clientid();   
  }
  if(check_if_pre_configured_ap())
  {
    setup_device_name_n_connect_to_preconfigured_ap();
  }
  else
  {
    set_indicator(SETUP_MODE);
    handle_easy_setup_mode();
  }
}

void loop() 
{
  if(in_easy_connect_mode)
  {
    monitor_websocket_server();
  }
  else
  {
    int wifi_status=WiFi.status(); 
    if(wifi_status==WL_CONNECTED && WiFi.localIP().toString()!="0.0.0.0")//check if connected to wifi
    {
      set_indicator(WIFI_CONNECTED);
      monitor_aws_connection();
      monitor_udp_connection();
      http_test();
      sync_ntp_time();
      handle_udp_requests();
      monitor_http_server();      
    }
    else //device is disconnected with AP
    {
      set_indicator(WIFI_DISCONNECTED);
      Serial.println("Failed to connect with AP......Trying again");
      stop_running_services();//stop udp and webserver if running and disconnect with AWS_IOT cloud
      if(connect_to_prestored_ap())
      {
       Serial.println("Connected To Network"); 
       start_services();//start udp and webserver and connect with AWS_IOT cloud
      }
    }
  }
}
