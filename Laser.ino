void enableLasercutter() {
  if ( serviceStatus == OO_SERVICE ) return;
  Serial.println( F( "Enabled" ) );
  digitalWrite( interlockRelay, LOW );
  disableTimer.reset();
}

void disableLasercutter() {
  Serial.println( F( "Disabled" ) );
  digitalWrite( interlockRelay, HIGH );
}

void checkService() {
  serviceStatus = EEPROM.read( 0x00 );
}

void toggleService() {
  delay( 1000 );
  if ( ! digitalRead( oosButton ) ) return;
  
  if ( serviceStatus == IN_SERVICE ) {
    serviceStatus = OO_SERVICE;
    EEPROM.write( 0x00, false );
  } else {
    if ( ! validateRemote( previous_card_number, deviceIDSU ) ) return;
    serviceStatus = IN_SERVICE;
    EEPROM.write( 0x00, true );
  }
}
