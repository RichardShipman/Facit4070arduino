// Copyright (C) Richard Shipman 2018
// MIT License
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or 
// substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

//
// Driver for Arduino Mega to connect to Facit 4070 paper tape punch.
//

#include "font5x7.c"

//#define DEBUG 1
#if defined(ARDUINO_AVR_MEGA2560)
  #define BOARD "Mega"
#elif defined(ARDUINO_AVR_MEGA)
  #define BOARD "Mega"
#else
  #define BOARD "Uno"
#endif

String version_text = " Version 1.0   19th April 2018   ASCII and Human readable ";
String device_greeting = "+++ Welcome to the Facit 4070 tape punch +++\r\nEnter ? for help.";

String top_menu_title = "Punch Main Menu";

String message;

// output punch feed hole
static int feedhole = 9;
// output stepping direction LOW=forward, HIGH=backward
static int SDpin = 10;
// output punch instruction, min duration 100uS
static int PIpin = 11;
// input punch ready HIGH to LOW when data ready, LOW to HIGH when punching complete
static int PRpin = 12;

// port K set up to connect to pin 1 to 8 on D25 connector on 4070 tape punch.


void setup() {

  Serial.begin(115200);
  #if defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA) 
    DDRK = 0xFF;
  #endif
  pinMode(feedhole,OUTPUT); 
  pinMode(SDpin,OUTPUT);
  pinMode(PIpin,OUTPUT);
  pinMode(PRpin,INPUT);

  Serial.println(device_greeting);
}

void loop() 
{

  if (Serial.available() > 0) 
  {
    char incomingByte = Serial.read(); // read the incoming byte:
#if defined(DEBUG)
    Serial.print(" I received:");
    Serial.println(incomingByte);
#else
//    Serial.println(" ");
#endif
    clearCRLF();
    switch (incomingByte) {
      case '?':
        menuInfo();
        break;
      case 'm':
         enterMessage();
         break;
      case 'd':
          displayMessage();
          break;
      case 'c':
          clearMessage();
          break;
      case 'a':
          punchMessage();
          break;
      case 'p':
          punchHumanMessage();
          break;
      case 'v':
          displayVersion();
          break;
      case 's':
          moveTape(10,false);
          break;
      case 'f':
          moveTape(10,true);
          break;
      
    }
   }
}

void clearCRLF() {
  char ch;
  delay(10);
  if (Serial.available() >0 )
  {
    ch=Serial.peek();
    if ((ch == '\n') || (ch == '\r')) {
#if defined(DEBUG)
      Serial.println(F("dumping char"));
#else
//      Serial.print(F("."));
#endif
      Serial.flush();
      Serial.read();
      clearCRLF();
    }
  }
}



/* *********** Callbacks ************ */

void enterMessage() {
  Serial.println(F("Enter your message:"));
  char ch;  
  while (ch != '\n') 
  {
    if (Serial.available()) 
    {
      ch=Serial.read();
      message += ch;
    }
  }
  Serial.println(F("Finished"));
}

void displayMessage() {
  Serial.println(F("Stored message is:"));
  Serial.println(message);
}

void clearMessage() {
  Serial.println(F("Clearing message"));
  message="";
}

void displayVersion() {
  Serial.println(F("Version information:"));
  Serial.println(version_text);
  Serial.print(F("Compiled for "));
  Serial.println(BOARD);
}




void punchMessage() {

  char ch;
  for (int i = 0; i<message.length() ; i++) 
  {
    ch = message.charAt(i);
    punchChar(ch);
  } 
}

void punchHumanMessage() {
  char ch;
  for (int i = 0; i<message.length() ; i++) 
  {
    ch = message.charAt(i);
    punchHumanChar(ch);
  } 
}

void punchHumanChar(char ch) {
  unsigned char c;
  for (int i=0; i<5; i++) 
  {
    c=pgm_read_byte_near(&font[ch*5+i]);
    punchChar(c);
#if defined(DEBUG)
    Serial.print(ch*5+i);
    Serial.print(" ");
    Serial.print(c,HEX);
    Serial.print(" - ");
#endif
  }
#if defined(DEBUG)
  Serial.println();
#endif
}

void moveTape(int num, boolean feedHole) 
{
  for (int i=0; i<num; i++ )
  {
    punchChar(0,feedHole);
  }
}

void menuInfo() {
  Serial.println(F("Facit 4070 printer arduino interface"));
  Serial.println(F("===================================="));
  Serial.println(F("? - show this information"));
  Serial.println(F("v - show version information"));
  Serial.println(F("d - display message"));
  Serial.println(F("m - enter message"));
  Serial.println(F("c - clear message"));
  Serial.println(F("p - punch message human readable"));
  Serial.println(F("a - punch message in ascii"));
  Serial.println(F("e - punch message in ebcdic*"));
  Serial.println(F("s - advance tape 10 spaces"));
  Serial.println(F("f - advance tape 10 spaces and punch feed holes"));
  Serial.println();
}



/* ***************** Facit 4070 interface ********************* */

void punchChar(byte ch)
{
  punchChar(ch,true);
} 




void punchChar(byte ch, boolean feed) 
{
  #if defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA) 
    PORTK = ch;
  #endif
  if (feed)
  {
    digitalWrite(feedhole,HIGH);
  } else {
    digitalWrite(feedhole,LOW);
  }
  digitalWrite(SDpin, LOW);
  delay(10);
//  while (digitalRead(PRpin))
//  {
//    Serial.println("waiting for ready");
//    delay(10);
//  }
  digitalWrite(PIpin, HIGH);
  delay(10);
//  while (!digitalRead(PRpin)) 
//  {
//    Serial.println("waiting for punched");
//    delay(10);
//  }
  digitalWrite(PIpin,LOW);
}

