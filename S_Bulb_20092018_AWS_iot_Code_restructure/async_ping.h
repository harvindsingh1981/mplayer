#ifndef __ASYNC_PING_H_
#define __ASYNC_PING_H_
#define HTTP_TEST_URL                   "google.com"
#define HTTP_TEST_INTERVAL              5000
#include "AsyncPing.h" //https://github.com/akaJes/AsyncPing
void init_ping(void);
void http_test(void);
bool ping_reply(const AsyncPingResponse& response);
bool ping_complete(const AsyncPingResponse& response);
void send_ping(char * ping_url);
#endif
