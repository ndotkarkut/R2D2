/* nRF24L01 Circuitry
 
   1 - VCC 3.3V // 3.3v Not 5V ! 
   2 - GND
   3 - CE - Arduino pin 7
   4 - CSN - Arduino pin 8
   5 - SCK - Arduino pin 13
   6 - MOSI - Arduino pin 11
   7 - MISO - Arduino pin 12
   8 - UNUSED rf
 
 */
 
#include <Arduino.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SoftwareSerial.h>
 
#define CE_PIN   A2                        //RF module pins
#define CSN_PIN  A3
 
#define enA 10                            //Motor LEFT
#define in1 A4
#define in2 A5
 
#define enB 5                        	  //Motor RIGHT
#define in3 6
#define in4 9
 
#define enC 3                             //Motor HEAD
#define in5 4
#define in6 2
 
#define DEBUG                        //Uncomment this line to enable Serial readout debug
 
#ifdef DEBUG
  #define DEBUG_SERIALBEGIN(...) Serial.begin(__VA_ARGS__)
  #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
  #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
  #define DEBUG_MODE 1
#else
  #define DEBUG_SERIALBEGIN(...) 
  #define DEBUG_PRINT(...)
  #define DEBUG_PRINTLN(...)
  #define DEBUG_MODE 0
#endif
 
 
const uint64_t pipe = 0xE8E8F0F0E1LL; // This is the transmit pipe for communication
RF24 radio(CE_PIN, CSN_PIN);          // Activate the Radio
SoftwareSerial BTserial(A0, A1);      // RX | TX , connect HC-05 through these 2 pins, not pin 0/1.
int data[4];                          // array holding the RF data
char c = ' ';                         // Char holding the BT data
 
 
void setup()  
{
  DEBUG_SERIALBEGIN(9600); /* Opening the Serial Communication */
  DEBUG_PRINTLN("DEBUG_MODE is ON, COMMENT OUT FOR FINAL VER.");
  DEBUG_PRINT("UPLOADED ON: "); DEBUG_PRINTLN(__DATE__);
  DEBUG_PRINTLN("BY: QUANG & NICK");
  DEBUG_PRINTLN("***** START *****");
  DEBUG_PRINTLN("Nrf24L01 Receiver Starting");
  
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
 
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
 
  pinMode(enC, OUTPUT);
  pinMode(in5, OUTPUT);
  pinMode(in6, OUTPUT);
 
  BTserial.begin(9600);
 
  radio.begin();
  radio.openReadingPipe(0, pipe);
  radio.startListening();
  radio.printDetails();
  delay(1000);
}
 
void loop()   
{
  if ( radio.available() )
  {
      radio.read(&data, sizeof(data) );
      DEBUG_PRINT("RF24 \t");
      DEBUG_PRINT("X = ");
      DEBUG_PRINT(data[0]);
 
      DEBUG_PRINT(" || Y = ");      
      DEBUG_PRINT(data[1]);
 
      DEBUG_PRINT(" || DOME_L = ");
      DEBUG_PRINT(data[2]);
 
      DEBUG_PRINT(" || DOME_R = ");
      DEBUG_PRINTLN(data[3]);
 
      delay(50);
  }
  else if( BTserial.available() )
  {
    c = BTserial.read();                  // Reads the data from the serial port
    DEBUG_PRINT("BT \t");
    DEBUG_PRINT(c);
 
    if ( c == '1' ) {
      DEBUG_PRINTLN("  || FORWARD"); 
 
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
 
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
 
      for( int i = 0 ; i < 255 ; i++ ) {  //Acceleration so motor doesn't jerk
        analogWrite( enA , i );
        analogWrite( enB , i );
        delayMicroseconds(750);
      }
    }
    else if ( c == '2' ) {
      DEBUG_PRINTLN(" || REVERSE");
 
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
 
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
 
      for( int i = 0 ; i < 255 ; i++) {
        analogWrite( enA , i );
        analogWrite( enB , i );
        delayMicroseconds(750);
      }
    }
    else if ( c == '3' ) {
      DEBUG_PRINTLN(" || LEFT");
 
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
 
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
 
      for( int i = 0 ; i < 255 ; i++ ) {
        analogWrite( enA , i );
        analogWrite( enB , i );
        delayMicroseconds(750);
      }
    }
    else if ( c == '4' ) {
      DEBUG_PRINTLN(" || RIGHT");
 
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
 
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
 
      for( int i = 0 ; i < 255 ; i++ ) {
        analogWrite( enA , i );
        analogWrite( enB , i );
        delayMicroseconds(750);
      }
    }
    else if ( c == '5' ) {
      DEBUG_PRINTLN(" || DOME-LEFT");
 
      digitalWrite(in5, HIGH);
      digitalWrite(in6, LOW);
 
      for( int i = 0 ; i < 255 ; i++ ) {
        analogWrite( enC , i );
        delayMicroseconds(750);
      }
    }
    else if ( c == '6' ) {
      DEBUG_PRINTLN(" || DOME-RIGHT");
 
      digitalWrite(in5, LOW);
      digitalWrite(in6, HIGH);
 
      for( int i = 0 ; i < 255 ; i++ ) {
        analogWrite( enC , i );
        delayMicroseconds(750);
      }
    }
    else if ( c == '0' ) {                  //Arduino app sends a "0" if there's no user input
      DEBUG_PRINTLN(" || ALL MOTORS STOPPED");
      digitalWrite(in1, LOW);               //Stop LEFT motor
      digitalWrite(in2, LOW);
      analogWrite( enA , 0 );
 
      digitalWrite(in3, LOW);               //Stop RIGHT motor
      digitalWrite(in4, LOW);
      analogWrite( enB , 0 );
 
      digitalWrite(in5, LOW);               //Stop HEAD motor
      digitalWrite(in6, LOW);
      analogWrite( enC , 0 );
    }
  }else{
    DEBUG_PRINTLN("No connection found");
    delay(50);
  }
}
