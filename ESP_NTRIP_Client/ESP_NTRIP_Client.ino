#include <WiFi.h>
#include <SoftwareSerial.h>
#include "NTRIPClient.h"

// Software serial pins
#define TX 26
#define RX 25
SoftwareSerial swSerial;

// WiFi network, enter your information
const char* ssid     = "your_ssid";
const char* password = "your_password";

// NTRIP caster host
char* host = "rtk2go.com";

// NTRIP caster port (2101 is a typical default)
int httpPort = 2101;

// Mount point or base station to receive data from
char* mntpnt = "mountpoint";

// User name (leave blank as "" if not needed)
char* user   = "";

// NTRIP caster password
char* passwd = "password";

NTRIPClient ntrip_c;

void setup()
{
    // USB serial
    Serial.begin(115200);
    delay(10);

    // Software serial for RTCM data
    swSerial.begin(57600, SWSERIAL_8N1, RX, TX, false);
    delay(10);
    
    // Confirm serial initializes, don't continue until it is valid
    if (!swSerial)
    {
        Serial.println("Invalid SoftwareSerial pin configuration, check config"); 
        while (1) 
        {
            delay (1000);
        }
    }
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(500);
      Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("Requesting SourceTable.");
  if(ntrip_c.reqSrcTbl(host,httpPort))
  {
      char buffer[512];
      delay(5);
      while(ntrip_c.available())
      {
          ntrip_c.readLine(buffer,sizeof(buffer));
          Serial.print(buffer); 
      }
  }
  else
  {
      Serial.println("SourceTable request error");
  }
  Serial.print("Requesting SourceTable is OK\n");
  ntrip_c.stop();
  
  Serial.println("Requesting MountPoint's Raw data");
  if(!ntrip_c.reqRaw(host,httpPort,mntpnt,user,passwd))
  {
      delay(15000);
      ESP.restart();
  }
  Serial.println("Requesting MountPoint is OK");

  
}

void loop()
{
    // Read available data from the NTRIP Caster and write to the GPS receiver correction input
    while(ntrip_c.available())
    {
          char ch = ntrip_c.read();        
          Serial.print(ch);
          swSerial.print(ch);
          
    }
}
