#ifndef __AWS_IOT_H_
#define __AWS_IOT_H_

void aws_iot_set_credentials(void);
void aws_message_handler(char* topic, byte* payload, unsigned int length);
bool aws_connect(void);
void aws_disconnect(void);
bool aws_subscribe(char * topic);
void aws_publish(char* topic);
void monitor_aws_connection(void);
//void setup_clientid(void);
#endif
