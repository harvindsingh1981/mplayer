#ifndef __WEBSOCKET_SERVER_H_
#define __WEBSOCKET_SERVER_H_
void init_websocket_server(void);
void monitor_websocket_server(void);
void update_alive_status(void);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void process_request(String str,uint8_t num);
#endif
