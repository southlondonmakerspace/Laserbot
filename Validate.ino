boolean validateCard( String number ) {
  // Card validation chain
  // 1. Check most recent cached card
  // 2. Check local list of master cards
  // 3. Check remote server for card
  // 4. Fail validation

  Serial.println( "." );
  if ( validateCache( number ) ) {
    Serial.println( F( "Card validate cache" ) );
    return true;
  } else if ( validateLocal( number ) ) {
    Serial.println( F( "Card validated locally" ) );
    return true;
  } else if ( validateRemote( number, deviceID ) ) {
    Serial.println( F( "Card validated remotely" ) );
    return true;
  }
  
  // Always return false
  Serial.println( F( "Card invalid" ) );
  return false;
}

boolean validateCache( String number ) {
  // Check to see if current card is the previous card and it was valid
  if ( number == previous_card_number && previous_card_valid ) {
    Serial.println( F( "No change in valid card" ) );
    return true;
  }
  
  // Always return false
  return false;
}

boolean validateLocal( String number ) {
  // Itterate through master cards list
  for ( int i = 0; i < sizeof( masterCards ) / sizeof( int ); i++ ) {
    
    // Get current card and prepare both strings for comparison
    String s = masterCards[i];
    number.toLowerCase();
    s.toLowerCase();
    
    // Check is both cards match and return true
    if ( number.compareTo( s ) == 0 ) return true;
  }
  
  // Always return false
  return false;
}

boolean validateRemote( String number, String device ) {
  // Enable CS to Ethernet shield
  EnableSPI_Ethernet();
  
  // Connect to Nodebot
  if ( client.connect( nodebotServer, port ) ) {
    Serial.println( F( "Connected to server." ) );
    
    // Send device ID and card number to Nodebot
    client.println( device );
    client.println( number );

    // Wait for response from Nodebot, disconnection or timeout
    netTimeout.reset();
    while( client.connected() && client.available() == 0 && ! netTimeout.check() ) {}
    
    // If there is data process it and disconnect
    if ( client.available() ) {
      byte data = client.read();
      client.stop();
      if ( data == 0x31 ) return true;
      return false;
    }
  } else {
    Serial.println( F( "Connection Error" ) );
  }
  
  // Always disconnect the client and return false
  client.stop();
  return false;
}
