#include <WiFi.h>
#include <SoftwareSerial.h>
#include "NTRIPServer.h"

// Software serial pins
#define TX 26
#define RX 25
SoftwareSerial swSerial;

const char* ssid     = "your_ssid";
const char* password = "your_password";

// NTRIP caster host
char* host = "rtk2go.com";

// NTRIP caster port (2101 is a typical default)
int httpPort = 2101;

// Mount point or base station to send data to
char* mntpnt = "mountpoint";
char* psw   = "password";
char* srcSTR   = "";

NTRIPServer ntrip_s;

void setup()
{

    // USB serial
    Serial.begin(115200);
    delay(10);  
    
    // Software serial for RTCM data
    swSerial.begin(57600, SWSERIAL_8N1, RX, TX, false);
    delay(10);

    // Connect to WiFi network
    WiFi.begin(ssid,password);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Subscribe to NTRIP caster mount point
    Serial.println("Subscribing MountPoint");
    if (!ntrip_s.subStation(host, httpPort, mntpnt,psw,srcSTR)) 
    {
        delay(15000);
        ESP.restart();
    }
    Serial.println("Subscribing MountPoint is OK");
}

char ch[512];
int readcount;
void loop()
{
    // If the connection to the NTRIP caster is open
    if (ntrip_s.connected())
    {
      while (swSerial.available()) 
      {
          // Buffer serial data, stop when data is all read or you reach 511 bytes
          readcount = 0;
          while (swSerial.available())
          {
              ch[readcount] = swSerial.read();
              readcount++;
              if (readcount > 511)
                  break;
          }
          // Write buffered data to NTRIP caster
          ntrip_s.write((uint8_t*)ch, readcount);
      }
    }
    // If the connection to the NTRIP caster is closed, try to reopen
    else 
    {
      ntrip_s.stop();
      Serial.println("reconnect");
      Serial.println("Subscribing MountPoint");
      if (!ntrip_s.subStation(host, httpPort, mntpnt, psw, srcSTR))
      {
          delay(100);
      }
      else
      {
          Serial.println("Subscribing MountPoint is OK");
          delay(10);
      }
  
    }
    delay(10);
}
