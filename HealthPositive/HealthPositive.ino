#include <Wire.h>
#include <SeeedOLED.h>
#include <LGPS.h>
#include <LGPRS.h>
#include <LGPRSClient.h>
#include "heartRate.h"
#include "Protocentral_MAX30102.h"

#define OLED
#define SERIAL_DISPLAY

char OLED_DISP = 'N';

//GPS
gpsSentenceInfoStruct info;
char buff[256];
double latitude, prev_latitude = 0;
double longitude, prev_longitude = 0;
int hour, minute, second, num;
String time;

//GSM
LGPRSClient client;
int port = 80;
char server[] = "68.183.88.242";
String vitals_insert_route = "/setbioinfo/?";
String vitals_get_route = "/getbioinfo/?";
String vitals_update_route = "/upbioinfo/?";
String gps_insert_route = "/setgpsinfo/?";
String gps_get_route = "/getgpsinfo/?";
String request_type = "POST ";
boolean send_request[2] = {true, true};     //0 is for vitals and 1 is for GPS
int connection_failed[2] = {0, 0};
String server_response = "";
long request_time;
byte send_pulse = 0, send_GPS = 0;

//MAX30102 pulse;
MAX30100 sensor;
volatile unsigned int IRR, prev_IRR;
const byte RATE_SIZE = 4; //Increase this for more averaging..
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

String email = "2602a@mail.com";
int hrt = 1, prev_hrt = 1;
int bp = 1, prev_bp = 1;
int bol = 1, prev_bol = 1;
boolean user_exists = false;

void setup() {
  Wire.begin();
#ifdef SERIAL_DISPLAY
  Serial.begin(115200);
  Serial.println("Setup Begin..");
#endif
#ifdef OLED
  initDisplay();
  SeeedOled.clearDisplay();
#endif

  Wire.begin();

  initGPS();
  initGPRS();
  initPulseSensor();

  //  checkUserInDB();

#ifdef SERIAL_DISPLAY
  Serial.println("End of Setup");
#endif

#ifdef OLED
  SeeedOled.setTextXY(6, 0);
  SeeedOled.putString("END");
#endif
  delay(4000);
}

void loop() {
  LGPS.getData(&info);
  parseGPGGA((const char*)info.GPGGA);
  checkGPSNewData();

  getPulseSensorData();
  checkPulseSensorNewData();
  //  readResponse();

  //if the server's disconnected, stop the client:
  if (!client.available() && !client.connected()) {
    client.stop();
  }
#ifdef OLED
  SeeedOled.clearDisplay();
#endif
  //  Serial.println();
}
