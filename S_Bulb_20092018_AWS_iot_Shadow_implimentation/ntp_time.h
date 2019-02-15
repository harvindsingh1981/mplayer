#ifndef __NTP_TIME_H_
#define __NTP_TIME_H_
#define NTP_TIME_SERVER                 "pool.ntp.org"
#define NTP_TIME_SYNC_INTERVAL          5 //Duration in seconds , this is time between two successive syncs

void init_ntp(void);
void sync_ntp_time(void);
void sendNTPpacket(char *address/*IPAddress &address*/);
time_t getNtpTime();
void digitalClockDisplay(void);
void printDigits(int digits);
#endif
