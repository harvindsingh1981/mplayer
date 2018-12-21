#include "global.h"
#include "async_ping.h"
AsyncPing ping;
long last_http_test_time=0;
bool is_connected_to_internet=false;//Global flag to check if device is connected to internet.

void init_ping(void)
{
  ping.on(true,ping_reply);
  ping.on(false,ping_complete);
}

void http_test(void)
{
 if(millis()-last_http_test_time > HTTP_TEST_INTERVAL)
  {
    //Serial.print("HTTP Test,Sending ping to ...");
    //Serial.println(HTTP_TEST_URL);
    send_ping(HTTP_TEST_URL);
    //last_http_test_time=millis();
  }
}

bool ping_reply(const AsyncPingResponse& response)
{
  IPAddress addr(response.addr); //to prevent with no const toString() in 2.3.0
  if (response.answer)
  {
//    Serial.printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n", response.size, addr.toString().c_str(), response.icmp_seq, response.ttl, response.time);    
  }
  else
  {
//    Serial.printf("no answer yet for %s icmp_seq=%d\n", addr.toString().c_str(), response.icmp_seq);    
  }
  return false; //do not stop  
}

bool ping_complete(const AsyncPingResponse& response)
{
  IPAddress addr(response.addr); //to prevent with no const toString() in 2.3.0
//  Serial.printf("total answer from %s sent %d recevied %d time %d ms\n",addr.toString().c_str(),response.total_sent,response.total_recv,response.total_time);
//  if(response.total_sent==response.total_recv)
  if(response.total_recv>0)
  {
    is_connected_to_internet=true;
    Serial.println("Connected to Internet");
  }
  else 
  {
    is_connected_to_internet=false;
    Serial.println("Internet Connection Lost :(");
  }
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
  last_http_test_time=millis();
}

