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
//  DDRK = 0xFF;
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
    Serial.print(" I received:");
    Serial.println(incomingByte);
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
          
      
    }
   }
}

void clearCRLF() {
  char ch;
  if (Serial.available() >0 )
  {
    ch=Serial.peek();
    if ((ch == '\n') || (ch == '\r')) {
//      Serial.println(F("dumping char"));
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





void punchMessage() {

  char ch;
  for (int i = 0; i<message.length() ; i++) 
  {
    ch = message.charAt(i);
    punchChar(ch);
  }
  
}

void menuInfo() {
  Serial.println(F("Facit 4070 printer arduino interface"));
  Serial.println(F("===================================="));
  Serial.println(F("? - show this information"));
  Serial.println(F("d - display message"));
  Serial.println(F("m - enter message"));
  Serial.println(F("c - clear message"));
  Serial.println(F("p - punch message human readable"));
  Serial.println(F("a - punch message in ascii"));
  Serial.println(F("e - punch message in ebcdic"));
  Serial.println(F("s - advance tape 10 spaces"));
  Serial.println(F("f - advance tape 10 spaces and punch feed holes"));
  Serial.println();
}



/* ***************** Facit 4070 interface ********************* */


void punchChar(byte ch) 
{
//  PORTK = ch;
  digitalWrite(feedhole,HIGH);
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

