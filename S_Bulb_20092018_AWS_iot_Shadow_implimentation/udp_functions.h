#ifndef __UDP_FUNCTIONS_H_
#define __UDP_FUNCTIONS_H_
#define UDP_BROADCAST_INTERVAL        3000
void monitor_udp_connection(void);
void handle_udp_requests(void);
void send_broadcast(void);
void process_udp_request(IPAddress ip,int port,char* command);
#endif
