#include "global.h"
#include "eeprom_functions.h"
#include "websocket_server.h"
bool ws_conn_status[5]={false,false,false,false,false};
unsigned long ws_last_ping_time[5]={0,0,0,0,0};
WebSocketsServer *webSocket = new WebSocketsServer(81);

void init_websocket_server(void)
{
 webSocket->begin();
 webSocket->onEvent(webSocketEvent);
}

void monitor_websocket_server(void)
{
  webSocket->loop();
  update_alive_status();
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

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) 
{

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

