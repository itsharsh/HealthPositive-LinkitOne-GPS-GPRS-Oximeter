void initGPS() {
#ifdef SERIAL_DISPLAY
  Serial.println("LGPS Power on, and waiting ...");
#endif

#ifdef OLED
  SeeedOled.setTextXY(0, 0);
  SeeedOled.putString("Init GPS....");
#endif
  LGPS.powerOn();
  delay(1500);
#ifdef OLED
  SeeedOled.setTextXY(0, 12);
  SeeedOled.putString("Done");
#endif
}

static unsigned char getComma(unsigned char num, const char *str)
{
  unsigned char i, j = 0;
  int len = strlen(str);
  for (i = 0; i < len; i ++)
  {
    if (str[i] == ',')
      j++;
    if (j == num)
      return i + 1;
  }
  return 0;
}

static double getDoubleNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;

  i = getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev = atof(buf);
  return rev;
}

static double getIntNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;

  i = getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev = atoi(buf);
  return rev;
}

void parseGPGGA(const char* GPGGAstr)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
     Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
     Where:
      GGA          Global Positioning System Fix Data
      123519       Fix taken at 12:35:19 UTC
      4807.038,N   Latitude 48 deg 07.038' N
      01131.000,E  Longitude 11 deg 31.000' E
      1            Fix quality: 0 = invalid
                                1 = GPS fix (SPS)
                                2 = DGPS fix
                                3 = PPS fix
                                4 = Real Time Kinematic
                                5 = Float RTK
                                6 = estimated (dead reckoning) (2.3 feature)
                                7 = Manual input mode
                                8 = Simulation mode
      08           Number of satellites being tracked
      0.9          Horizontal dilution of position
      545.4,M      Altitude, Meters, above mean sea level
      46.9,M       Height of geoid (mean sea level) above WGS84
                       ellipsoid
      (empty field) time in seconds since last DGPS update
      (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
  */
  int tmp;

  if (GPGGAstr[0] == '$')  {
    tmp = getComma(1, GPGGAstr);
    time = "";
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');

    hour += 5;
    minute += 30;

    if (minute >= 60) {
      minute -= 60;
      hour++;
    }

    if (hour >= 24) {
      hour -= 24;
    }

    if (hour < 10) {
      time += 0;
    }
    time += hour;
    if (minute < 10) {
      time += 0;
    }
    time += minute;
    if (second < 10) {
      time += 0;
    }
    time += second;

    //    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    //    Serial.println(buff);

    tmp = getComma(2, GPGGAstr);
    double la = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    double lo = getDoubleNumber(&GPGGAstr[tmp]);
    //    sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
    //    Serial.println(buff);

    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);
    //    sprintf(buff, "satellites number = %d", num);
    //    Serial.println(buff);
    //    Serial.print("La:");
    //    Serial.print(la, 4); Serial.print("\t");
    //    Serial.print("Lo: ");
    //    Serial.print(lo, 4); Serial.print("\n");
    if (num != 0) {
      latitude = convertToDegMin(la);
      longitude = convertToDegMin(lo);
    }
  }
  else {
    Serial.println("Not get data");
  }
}

double convertToDegMin(double coordinate) {
  int degree = coordinate / 100;
  coordinate = coordinate - (degree * 100);
  return degree + (coordinate / 60);
}


void checkGPSNewData() {
  if (prev_latitude != latitude && prev_longitude != longitude && longitude != 0 && latitude != 0 && num != 0) {
    prev_latitude = latitude;
    prev_longitude = longitude;

#ifdef SERIAL_DISPLAY
    Serial.print("GPS NEW DATA"); Serial.print("\t");
    printGPSData();
#endif

#ifdef OLED
    if (OLED_DISP == 'P') {
      SeeedOled.clearDisplay();
      OLED_DISP = 'G';
    }
    SeeedOled.setTextXY(0, 0); SeeedOled.putNumber(hour);
    SeeedOled.setTextXY(0, 2); SeeedOled.putString(":  ");
    SeeedOled.setTextXY(0, 3); SeeedOled.putNumber(minute);
    SeeedOled.setTextXY(0, 5); SeeedOled.putString(":  ");
    SeeedOled.setTextXY(0, 6); SeeedOled.putNumber(second);
    SeeedOled.setTextXY(0, 10); SeeedOled.putString("SAT:  ");
    SeeedOled.setTextXY(0, 14); SeeedOled.putNumber(num);
    SeeedOled.setTextXY(2, 0); SeeedOled.putString("LAT:      ");
    SeeedOled.setTextXY(2, 4); SeeedOled.putFloat(latitude, 5);
    SeeedOled.setTextXY(4, 0); SeeedOled.putString("LON:      ");
    SeeedOled.setTextXY(4, 4); SeeedOled.putFloat(longitude, 5);
#endif

    send_GPS++;
    if (send_GPS > 10) {
      send_GPS = 0;
      send_request[1] = true;
      sendGPSToServer(gps_insert_route);
    }
  }
  else if (num == 0) {
#ifdef SERIAL_DISPLAY
    Serial.print("No sat available"); Serial.print("\n");
#endif
  }
}

void printGPSData() {
  Serial.print((char*)info.GPGGA);
#ifdef SERIAL_DISPLAY
  Serial.print("Time: ");
  Serial.print(time); Serial.print("\t");
  //  Serial.print(hour); Serial.print(":");
  //  Serial.print(minute); Serial.print(":");
  //  Serial.print(second); Serial.print("\t");

  Serial.print("Lat: ");
  Serial.print(latitude, 8); Serial.print("\t");
  Serial.print("Long: ");
  Serial.print(longitude, 8); Serial.print("\t");

  Serial.print("Sat: ");
  Serial.print(num); Serial.print("\t");
#endif
}
