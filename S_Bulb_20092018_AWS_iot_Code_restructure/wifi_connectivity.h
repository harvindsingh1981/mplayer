#ifndef __WIFI_CONNECTIVITY_H_
#define __WIFI_CONNECTIVITY_H_

bool check_ap_availablity(String ssid);
bool connect_to_prestored_ap(void);
void ap_onstationConnected(const WiFiEventSoftAPModeStationConnected& evt);
void ap_onstationDisconnected(const WiFiEventSoftAPModeStationDisconnected& evt);
void sta_onConnected(const WiFiEventStationModeConnected& evt);
void sta_onDisconnected(const WiFiEventStationModeDisconnected& evt);
void sta_onAuthModeChanged(const WiFiEventStationModeAuthModeChanged& evt);
void sta_onGotIP(const WiFiEventStationModeGotIP& evt);
void sta_onDHCPTimeout(void);
void setup_wifi_eventhandlers(void);
#endif
