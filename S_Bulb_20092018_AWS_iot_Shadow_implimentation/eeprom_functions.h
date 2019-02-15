#ifndef __EEPROM_FUNCTIONS_H_
#define __EEPROM_FUNCTIONS_H_
#define DEVICE_NAME                   "Harvind_Singh"
#define EEPROM_SSID_ADDR              0         //Max 32 characters excluding '\0' Start@0,  END@31,  '\0'@32 //max length can be 32 characters excluding '\0'
#define EEPROM_PASSWORD_ADDR          40        //Max 20 charscters excluding '\0' Start@40, END@59,  '\0'@60 //max length can be 63 characters excluding '\0'
#define EEPROM_DEVICE_NAME_ADDR       61        //Max 32 characters excluding '\0' Start@61, END@92 , '\0'@93 
#define EEPROM_USER_ID                94        //Max 100 characters excluding '\0' Starts@94, END@193, '\0'@194 // Currently limited to 100 character due to eeprom limitation, actual valid email address can be 254 characters, need to change in future.
#define EEPROM_IF_PRE_CONFIGURED_AP   509       //'y' for yes and 'n' for no
#define EEPROM_SIGNATURE_BYTE_1       510       //'h'
#define EEPROM_SIGNATURE_BYTE_2       511       //'s'

void clear_eeprom(void);
void write_signature(void);
void write_device_name(char * str);
bool check_if_pre_configured_ap(void);//return "true" if byte "509" contains 'y' else return "false"
void save_ssid_password(char* ssid,char* password);
void save_user_id(char * user_id);
String get_device_name(void);
String get_eeprom_ssid(void);
String get_eeprom_ssid(void);
String get_eeprom_password(void);
String get_user_id(void);
String encode_user_id(String uid);
String eeprom_get_string(int addr);
void eeprom_write_string(char* str,int addr);
void clear_signature(void);
#endif
