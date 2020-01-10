void initGPRS() {
  int GPRS_con_failed = 0;
#ifdef OLED
  SeeedOled.setTextXY(2, 0);
  SeeedOled.putString("Init GPRS...");
#endif

#ifdef SERIAL_DISPLAY
  Serial.println("Attach to GPRS network by auto-detect APN setting");
#endif

  while (!LGPRS.attachGPRS())  {
    delay(500);
    GPRS_con_failed++;
    if (GPRS_con_failed > 20) {
#ifdef OLED
      SeeedOled.setTextXY(2, 12);
      SeeedOled.putString("Fail");
#endif
      break;
    }
#ifdef SERIAL_DISPLAY
    Serial.print(".");
#endif
  }
#ifdef OLED
  if (GPRS_con_failed < 20) {
    SeeedOled.setTextXY(2, 12);
    SeeedOled.putString("Done");
  }
#endif
}

void sendRequest(String path, byte index) {
  if (client.connect(server, port)) {
    request_time = millis();
#ifdef SERIAL_DISPLAY
    Serial.println("\nConnected to server");
    Serial.print("\t");
    Serial.println(path);
#endif

#ifdef OLED
    SeeedOled.setTextXY(6, 0);
    SeeedOled.putString("CONNECTED");
#endif

    // Make a HTTP request:
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
    send_request[index] = false;
    connection_failed[index] = 0;
  }
  else  {
    send_request[index] = true;
    connection_failed[index]++;
#ifdef SERIAL_DISPLAY
    Serial.println("\nConnection Failed");
#endif

#ifdef OLED
    SeeedOled.setTextXY(6, 0);
    SeeedOled.putString("FAILED");
#endif
  }
  if (connection_failed[index] > 100) {
    connection_failed[index] = 0;
    initGPRS();
  }
}

void readResponse() {
  server_response = "";
  while (client.available())  {
    server_response += (char)client.read();
  }

  if (!server_response.equals("")) {
#ifdef SERIAL_DISPLAY
    Serial.println(server_response);
#endif
  }
}
