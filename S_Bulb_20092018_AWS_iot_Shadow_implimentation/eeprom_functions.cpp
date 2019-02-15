#include "global.h"
#include "eeprom_functions.h"
char encoded_uid[25];

void clear_eeprom()
{
  int i;
  EEPROM.begin(512);
  Serial.print("clearing EEPROM");
  for(i=0;i<512;i++)
  {
    EEPROM.write(i,0);
    Serial.print(".");
    delay(70);
  }
  Serial.println();
  Serial.println("EEPROM Cleared !!");
  EEPROM.end();
}
void write_signature()
{
  EEPROM.begin(512);
  Serial.print("Writing Signature....");
  EEPROM.write(EEPROM_IF_PRE_CONFIGURED_AP,'n');//since it's very first boot or factory reset hence all previous configured AP are deleted and this byte should be set to 'n' to indicate that no pre-configured AP are present.
  EEPROM.write(EEPROM_SIGNATURE_BYTE_1,'h');//byte "510" and "511" contains signature byte to indicate that it's not first boot
  EEPROM.write(EEPROM_SIGNATURE_BYTE_2,'s');
  Serial.println("Done.");
  EEPROM.end();
}

void write_device_name(char * str)
{
  eeprom_write_string(str,EEPROM_DEVICE_NAME_ADDR);
}

bool check_if_pre_configured_ap()//return "true" if byte "509" contains 'y' else return "false"
{
  bool result=false;
  EEPROM.begin(512);
  if(EEPROM.read(EEPROM_IF_PRE_CONFIGURED_AP)=='n')result=false;
  if(EEPROM.read(EEPROM_IF_PRE_CONFIGURED_AP)=='y')result=true;
  EEPROM.end();
  if(SETUP_MODE_FORCE_ENTRY)return false;
  return result;
}

void save_ssid_password(char* ssid,char* password)
{
  eeprom_write_string(ssid,EEPROM_SSID_ADDR);
  eeprom_write_string(password,EEPROM_PASSWORD_ADDR);
  EEPROM.begin(512);
  EEPROM.write(EEPROM_IF_PRE_CONFIGURED_AP,'y');
  EEPROM.end();
}

void save_user_id(char * user_id)
{
  eeprom_write_string(user_id,EEPROM_USER_ID);
}

String get_device_name()
{
  return eeprom_get_string(EEPROM_DEVICE_NAME_ADDR);
}

String get_eeprom_ssid()
{
  return eeprom_get_string(EEPROM_SSID_ADDR);
}

String get_eeprom_password()
{
  return eeprom_get_string(EEPROM_PASSWORD_ADDR);
}

String get_user_id()
{
  return eeprom_get_string(EEPROM_USER_ID);
}

String encode_user_id(String uid)
{
/*********************************/
//Encoded String format = USERID(without special characters)_CHECKSUM(original UID)
//checksum = (Position*character Ascii Value) + .................+(Position*Ascii Value)
//Starting position = 1 
/*********************************/  
  char arr[256];
  char *p;
  unsigned long checksum=0;
  String final_str="";
  int i=1;
  uid="harvindsingh1981@gmail.com";
  uid.toCharArray(arr,256);
  p=arr;
  while(*p!='\0')
  {
    checksum=checksum+(i**p);
  /* //currently encoding uid as checksum only because of limited MQTT Topic size         
  if((*p>='a' && *p<='z')||(*p>='A' && *p<='Z')||(*p>='0' && *p<='9'))
    {
      final_str+=*p;
    }
   */ 
    i++;
    p++;
  }
// final_str=/*final_str*/"SMAYANTR" + '_' + String(checksum);
// return final_str;
return "SMAYANTR_"+String(checksum); 
}

String eeprom_get_string(int addr)
{
char arr[40];
char *p;
char a='q';
p=arr;
EEPROM.begin(512);
  while(a!='\0')
  {
    a=EEPROM.read(addr++);
    *p++=a;
  }
  *p='\0';
EEPROM.end();
return String(arr);
}

void eeprom_write_string(char* str,int addr)
{
//  char i=1;
  EEPROM.begin(512);
  while(*str!='\0')
  {
    EEPROM.write(addr,*str++);
    addr++;
//    Serial.printf("Written Byte %d\n\r",i++);
  }
  EEPROM.write(addr,*str);
  EEPROM.end();
}

void clear_signature()
{
  EEPROM.begin(512);
  EEPROM.write(EEPROM_SIGNATURE_BYTE_1,0);//byte "510" and "511" contains signature byte to indicate that it's not first boot
  EEPROM.write(EEPROM_SIGNATURE_BYTE_2,0);
  EEPROM.end();
}

