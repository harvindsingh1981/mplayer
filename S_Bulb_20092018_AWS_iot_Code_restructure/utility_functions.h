#ifndef __UTILITY_FUNCTIONS_H_
#define __UTILITY_FUNCTIONS_H_
void initialize(void);
bool check_if_first_boot(void);
void setup_userid(void);
void setup_clientid(void);
void handle_first_boot(void);
void setup_device_name_n_connect_to_preconfigured_ap(void);
void handle_easy_setup_mode(void);
void stop_running_services(void);
void start_services(void);
#endif
