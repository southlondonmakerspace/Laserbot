/*
  Laserbot
  For South London Makerspace
  
  To read cards serial numbers 
  Query remote server for access rights.
  Enable/disable laser cutter
*/

#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <inttypes.h>
#include <MFRC522.h>
#include <Metro.h>
#include <utility/w5100.h>

#define interlockRelay 6
#define oosButton  3
#define oosLight   2
#define ethSS     10
#define rfidRST   8
#define rfidSS    9
#define rfidMOSI  11
#define rfidMISO  12
#define rfidSCK   13

#define IN_SERVICE HIGH
#define OO_SERVICE LOW

#define doorReleaseDelay 100
#define deviceID F( "laser" )
#define deviceIDSU F( "laser-su" )

// Ethernet configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };

// numeric IP for server node
IPAddress nodebotServer( 10, 0, 1, 2 );
uint16_t port = 1234;

// Initialize the Ethernet client library
EthernetClient client;
EthernetServer server = EthernetServer( 1234 );

// Initialsie RFID reader
MFRC522 mfrc522( rfidSS, rfidRST );	

String previous_card_number;        // Stores previously read card number for a time
boolean previous_card_valid;        // previously red card state valid true or false
boolean serviceStatus = false;
int attempts = 0;

Metro rfidReadTimer = Metro( 100 );
Metro netTimeout = Metro( 1000 );
Metro cardTimer = Metro( 5000 );
Metro disableTimer = Metro( 10000 );

char* masterCards[] = { "" };

void setup() {
  pinMode( ethSS, OUTPUT );
  pinMode( rfidSS, OUTPUT );
  pinMode( interlockRelay, OUTPUT );
  pinMode( oosLight, OUTPUT );
  pinMode( oosButton, INPUT );
  
  checkService();
  
  // Set default relay state  
  digitalWrite( interlockRelay, HIGH );
 
  Serial.begin( 115200 );
  Serial.println( F( "Laserbot Started" ) );

  Serial.println( F( "RFID" ) );
  SPI.begin();
  EnableSPI_RFID();
  mfrc522.PCD_Init();
 
  Serial.println( F( "Ethernet" ) );
  EnableSPI_Ethernet();
  Ethernet.begin( mac );
  W5100.setRetransmissionTime( 0x07d0 ); // reduce ethernet connection timeout
  W5100.setRetransmissionCount( 3 );     // set ethernet connection retries
  
  Serial.println( Ethernet.localIP() );
}

void loop() {
  if ( rfidReadTimer.check() ) rfidRead();
  if ( cardTimer.check() ) clearCard();
  if ( disableTimer.check() ) disableLasercutter();

  digitalWrite( oosLight, serviceStatus );

  if ( digitalRead( serviceStatus ) ) toggleService();

  if ( Ethernet.maintain() > 0 ) Serial.println( Ethernet.localIP() );
}
