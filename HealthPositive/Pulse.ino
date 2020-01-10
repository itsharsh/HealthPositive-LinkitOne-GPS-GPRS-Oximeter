void initPulseSensor() {
#ifdef OLED
  SeeedOled.setTextXY(4, 0); SeeedOled.putString("Init Pulse..");
#endif

  sensor.begin(pw1600, i50, sr100 );
  sensor.setSPO2(sr100);

#ifdef OLED
  delay(1500);
  SeeedOled.setTextXY(0, 12); SeeedOled.putString("Done");
#endif
}

void getPulseSensorData() {
  sensor.readSensor();
  prev_IRR = IRR;
  IRR = sensor.IR;
  IRR = (0.05 * IRR) + (0.95 * prev_IRR);

  if (checkForBeat(IRR) && IRR > 50000) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);
    //    hrt = beatsPerMinute;
    if (beatsPerMinute < 255 && beatsPerMinute > 20)    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
      hrt = beatAvg;
    }
  }
  delay(10);
}
/*
  void getDataFromPulseSensor() {
  if (Serial1.available()) {
    hrt = Serial1.parseInt();
    checkSensorsNewData();
  }

  if (Serial.available()) {
    int index = Serial.parseInt();
    switch (index) {
      case 1:
        bp = Serial.parseInt();
        break;
      case 2:
        hrt = Serial.parseInt();
        break;
      case 3:
        bol = Serial.parseInt();
        break;
      default:
        break;
    }
    checkSensorsNewData();
  }
  }
*/
void printPulseSensorData() {
#ifdef SERIAL_DISPLAY
  Serial.print("BP:\t");
  Serial.print(bp); Serial.print("\t");
  Serial.print("HRT:\t");
  Serial.print(hrt); Serial.print("\t");
  Serial.print("BOL:\t");
  Serial.print(bol); Serial.print("\n");
#endif

#ifdef OLED
  SeeedOled.setTextXY(2, 0); SeeedOled.putString("PUL:");
  SeeedOled.setTextXY(2, 4); SeeedOled.putNumber(beatsPerMinute);
  SeeedOled.setTextXY(2, 8); SeeedOled.putString("AVG:");
  SeeedOled.setTextXY(2, 12); SeeedOled.putNumber(beatAvg);
  SeeedOled.setTextXY(4, 0); SeeedOled.putString("BOL:");
  SeeedOled.setTextXY(4, 4); SeeedOled.putNumber(bol);
  SeeedOled.setTextXY(4, 8); SeeedOled.putString("BP:");
  SeeedOled.setTextXY(4, 11); SeeedOled.putNumber(bp);
#endif
}

void checkPulseSensorNewData() {
  if (hrt > 0 && hrt < 300) {
    if (!(prev_hrt == hrt && prev_bol == bol && prev_bp == bp)) {
      prev_hrt = hrt;
      prev_bp = bp;
      prev_bol = bol;

#ifdef SERIAL_DISPLAY
      Serial.println("Pulse New Data");
#endif

#ifdef OLED
      if (OLED_DISP == 'G') {
        SeeedOled.clearDisplay();
        OLED_DISP = 'P';
      }
      SeeedOled.setTextXY(0, 0);
      SeeedOled.putString("PULSE NEW DATA");
#endif
      printPulseSensorData();

      send_pulse++;
      if (send_pulse > 5) {
        send_pulse = 0;
        send_request[0] = true;
        sendVitalsToServer(vitals_insert_route);
      }
    }
  }
}
