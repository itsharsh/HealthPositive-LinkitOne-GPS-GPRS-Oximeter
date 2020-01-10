void initDisplay() {
#ifdef SERIAL_DISPLAY
  Serial.println("Initializing OLED...");
#endif
  SeeedOled.init();  //initialze SEEED OLED display
  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
  SeeedOled.setTextXY(0, 0);         //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString("     OXYGEN"); //Print the String
  SeeedOled.setTextXY(2, 0);         //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString("       TO"); //Print the String
  SeeedOled.setTextXY(4, 0);         //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString("   INNOVATION"); //Print the String
  delay(2000);
}
