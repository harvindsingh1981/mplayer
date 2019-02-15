#ifndef __ABCDEFG_H_
#define __ABCDEFG_H_

#define SETUP_MODE_FORCE_ENTRY  false
#include <Arduino.h>
#include <stdlib.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <WebSocketsServer.h>
#include <StringSplitter.h>
#include <ESP8266WebServer.h>
#define RED_LED     12//13
#define GREEN_LED   2 //12
#define BLUE_LED    4 //15
#define LED         12
#define WIFI_DISCONNECTED     1024,0,0
#define WIFI_CONNECTED        0,1024,0
#define CLOUD_DISCONNECTED    0,1024,0
#define CLOUD_CONNECTED       0,0,1024
#define SETUP_MODE            1024,1024,0
/***********AWS***************/
#include <Stream.h>
//AWS
#include "sha256.h"
#include "Utils.h"
//WEBSockets
#include <Hash.h>
#include <WebSocketsClient.h>
//MQTT PUBSUBCLIENT LIB
#include <PubSubClient.h>
//AWS MQTT Websocket
#include "Client.h"
#include "AWSWebSocketClient.h"
#include "CircularByteBuffer.h"
/*******************************/
void set_indicator(int red,int green,int blue);
#include <ArduinoJson.h>
#endif
