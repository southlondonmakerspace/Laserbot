// A struct used for passing the UID of a PICC.
typedef struct {
  byte      size;			// Number of bytes in the UID. 4, 7 or 10.
  byte      uidByte[10];
  byte	    sak;			// The SAK (Select acknowledge) byte returned from the PICC after successful selection.
} RFIDUid;

// A struct used for passing a MIFARE Crypto1 key
typedef struct {
  byte      keyByte[6];
} MIFARE_Key;

// Check for card and get uid
void rfidRead() {
  String card_number;
  MIFARE_Key key;
  MFRC522::Uid *uid = &(mfrc522.uid);  
  
  EnableSPI_RFID();

  // Look for card
  if ( ! mfrc522.PICC_IsNewCardPresent() ) return;

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial() ) return;

  // UID
  Serial.print( F( "Card #:" ) );
  for ( byte i = 0; i < uid->size; i++ ) {
    String byteVal = String( uid->uidByte[i], HEX );
    if (byteVal.length() <= 1) {
      byteVal = "0" + byteVal;
    }
    card_number = card_number + byteVal;
    Serial.print( uid->uidByte[i] < 0x10 ? " 0" : " " );
    Serial.print( uid->uidByte[i], HEX );
  }
  Serial.println();

  if ( validateCard( card_number ) ) {
    enableLasercutter();
    previous_card_valid = true;
    cardTimer.reset();
  } else {
    previous_card_valid = false;
    cardTimer.reset();
  }
  previous_card_number = card_number;
}
