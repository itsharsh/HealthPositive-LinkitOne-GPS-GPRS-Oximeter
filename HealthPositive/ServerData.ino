void checkUserInDB() {
  String path = request_type + vitals_get_route + "email=" + email;
  while (user_exists != true) {
    if (send_request[0]) {
      sendRequest(path, 0);
    }
    readResponse();
    parseVitalsResponse();
    if (millis() - request_time > 5000) {
#ifdef SERIAL_DISPLAY
      Serial.print("Internet Issue");
#endif
      send_request[0] = true;
    }
  }
}

void sendVitalsToServer(String route) {
  if (send_request) {
    String path = request_type + route + "email=" + email + "&bp=" + bp + "&hrt=" + hrt + "&bol=" + bol;
#ifdef SERIAL_DISPLAY
    Serial.print("Sending Vitals Request...");
#endif
    sendRequest(path, 0);
  }
}

void sendGPSToServer(String route) {
  if (send_request) {
    String lat_str = convertGPSToString(latitude);
    String long_str = convertGPSToString(longitude);
    String path = request_type + route + "email=" + email + "&latitude=" + lat_str + "&longitude=" + long_str  + "&time=" + time;
#ifdef SERIAL_DISPLAY
    Serial.print("Sending GPS Request...");
#endif
    sendRequest(path, 1);
  }
}

String convertGPSToString(float no) {
  long tmp = no;
  String str = (String)tmp + ".";
  no -= tmp;
  tmp = no * 1000000;
  str += (String)tmp;
  return str;
}

void parseVitalsResponse() {
  if (server_response.length() > 0) {
    if (server_response.substring(186).equals("[]")) {                      //Data not exist, Inserting Data
#ifdef SERIAL_DISPLAY
      Serial.println("Inserting Data...");
#endif
#ifdef OLED
      SeeedOled.clearDisplay();
      SeeedOled.setTextXY(6, 0);
      SeeedOled.putString("INSERTING");
#endif
      send_request[0] = true;
      sendVitalsToServer(vitals_insert_route);   //insert Data to server
    }
    else if (server_response.substring(256, 256 + email.length()).equals(email)) {
#ifdef SERIAL_DISPLAY
      Serial.println("User Exists");
#endif

#ifdef OLED
      SeeedOled.clearDisplay();
      SeeedOled.setTextXY(0, 0);
      SeeedOled.putString("USER EXISTS");
#endif

      user_exists = true;
    }
    else {
      send_request[0] = true;
    }
  }
}
