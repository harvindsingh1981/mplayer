#include "global.h"
#include "http_webserver.h"

ESP8266WebServer http_server(80);
extern bool led_status;
extern int led_intensity;
extern bool is_udp_connected;
extern char clientid_array[25];

void setup_http_request_handleres(void)
{
 http_server.on("/Ping",handlePing);//Web server for get and post request handling
 http_server.on("/on",handleon);
 http_server.on("/off",handleoff);
 http_server.on("/int",handleint); 
}

void monitor_http_server(void)
{
 http_server.handleClient();    //Handling of incoming requests 
}

void handleon(void)
{
//  digitalWrite(LED,1);
  analogWrite(LED,255);
  http_server.send(200,"text/plain","1");
  led_status=1;
}
void handleoff(void)
{
//  digitalWrite(LED,0);
  analogWrite(LED,0);
  http_server.send(200,"text/plain","0");
  led_status=0;  
}

void handlePing(void)
{
  String reply;
  char str[]="Pong";
  if(led_status)reply=String(str)+';'+String(wifi_station_get_hostname())+';'+'1'+';'+String(led_intensity)+';'+String(clientid_array);
  else reply=String(str)+';'+String(wifi_station_get_hostname())+';'+'0'+';'+String(led_intensity)+';'+String(clientid_array);
  Serial.println("Received Ping.......");
//  http_server.send(200,"text/plain","Pong");
  http_server.send(200,"text/plain",reply);
  is_udp_connected=true;
}
void handleint(void)
{
  String message = "";
  int intensity;
  if (http_server.arg("Intensity")== ""){     //Parameter not found
//  message = "Intensity Argument not found";
  }
  else
  {     //Parameter found
//  message = "Intensity Argument = ";
//  message += http_server.arg("Intensity");     //Gets the value of the query parameter
//    message=http_server.arg("Intensity");
//    intensity=message.toInt();
//    analogWrite(LED,intensity);
    message="ok";
    intensity=http_server.arg("Intensity").toInt();
    Serial.print("Setting light Intensity to ....");
    Serial.println(intensity);
    analogWrite(LED,intensity);
    led_intensity=intensity;
  }
  http_server.send(200, "text/plain", message); //Returns the HTTP response
}


