/*
  Swap out LCD to an I2C bus one to save ports when needed
  Library by David Mellis, Limor Fried and Tom Igoe
 
|| @author cmoz
|| Trifork Leeds
||
|| @description
|| | integrating the systems for ForkId.
|| | wifi shield
|| | number keypad (currently using switch 
|| | LCD
|| | RFID reader / SL018 / Mifare Cards 
|| | GPS Module
|| #

  Web client
 
 This example is written for a network using WPA encryption. For 
 WEP or WPA, change the Wifi.begin() call accordingly.
 
 Circuit:
 * WiFi shield attached
 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */

// include the lcd library
#include <LiquidCrystal.h>
// keypad library for the keypad
#include <Keypad.h>

#include <SPI.h>
#include <WiFi.h>
#include <Phant.h>

Phant phant("data.sparkfun.com", "wp5Xm8lZm5Hn23KMjZv5", "wzYDjJaBjYTgyxMvJbAK");

char ssid[] = "XXXXXXXXX"; //  your network SSID (name) 
char pass[] = "XXXXXXXXX";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

// keypad information
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
// based on matrix drrawn out for the row and columns... 
// on the device left to right to pin... 
// 13, 10, 9,8,7, 4, A5
byte rowPins[ROWS] = {4, A0, A1, A2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A3, A4, A5}; //connect to the column pinouts of the keypad

  Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//--------------------------------------------------------------
// pins I'm using for the LCD 
LiquidCrystal lcd(8, 9, 6, 5, 3, 2); // note the change of pins 5,4 -> 6,5
// 12 -> 8, 11 ->9

#define ledPin 12
//--------------------------------------------------------------
int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "data.sparkfun.com";    // name address for Google (using DNS)

const String publicKey = "wp5Xm8lZm5Hn23KMjZv5";
const String privateKey = "wzYDjJaBjYTgyxMvJbAK";

const byte NUM_FIELDS = 2;
const String fieldNames[NUM_FIELDS] = {"errormsg", "truck"};
String fieldData[NUM_FIELDS];

// Initialize the client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;
unsigned long lastConnectionTime = 0;           // last time you connected to the server, in milliseconds
boolean lastConnected = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ SET UP ++++++++++++++++++++++++++++++++++++
   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ SET UP ++++++++++++++++++++++++++++++++++++
   */
   
boolean incomingData = false;

void setup() {
  	//--------------------------------------------------------------
	//Initialize serial and wait for port to open:
  Serial.begin(9600); 
  while (!Serial) {
		; // wait for serial port to connect. Needed for Leonardo only
	}
	//--------------------------------------------------------------
	// check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  }
   
  //--------------------------------------------------------------
	// attempt to connect to Wifi network:
	while ( status != WL_CONNECTED) { 
		Serial.print("--- Attempting to connect to SSID: ");
		Serial.println(ssid);
		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
		status = WiFi.begin(ssid, pass);

		// wait 10 seconds for connection:
		delay(10000);
	} 
	//--------------------------------------------------------------
	// you're connected now, so print out the status:
	printWifiStatus(); 
  
  pinMode(ledPin,OUTPUT);
  lcd.clear();
  // number of columns and rows for the lcd
  lcd.begin(16,2);
  Serial.println("Connected to wifi");
  lcd.setCursor(0,0); // position, row
  lcd.print(" Trifork Leeds");
  lcd.setCursor(0,1); // position, row
  lcd.print(" Connecting... ");
  printWifiStatus();
  Serial.println("Ready");
  lcd.setCursor(0,0); // position, row
  lcd.print(" Trifork Leeds");
  lcd.setCursor(0,1); // position, row
  lcd.print(" Ready          ");
  
}

void loop() {
  //--------------------------------------------------------------
  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  //--------------------------------------------------------------
  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    //getKeyCode();
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
  //--------------------------------------------------------------
  // The function to take the input from the key pressed on the pad
  // then send the data to the server
  getKeyCode();

}

void getKeyCode() {
  //--------------------------------------------------------------
  // basic code to map the keys pressed
  char key = keypad.getKey();
  
  if (key){
      Serial.println(key);
      String truckId = ("4386574");
      //lcd.clear();
      lcd.setCursor(0,0); // position, row
      lcd.print("Trifork         ");
      lcd.setCursor(0, 1);
      lcd.print("Code Number ");
      lcd.setCursor(15, 1);
      lcd.print(key);
      digitalWrite(ledPin, LOW);
          
           switch (key)
    {
           case '1':
  fieldData[0] = String("1 - I'm Stuck");
  fieldData[1] = truckId;
  postData();
          break;
          
           case '2':
  fieldData[0] = String("2 - new fork error");
  fieldData[1] = truckId;
  postData();
          break;
          
           case '3':
  fieldData[0] = String("3 - donut error");
  fieldData[1] = truckId;
  postData();
          break;
          
           case '4':
  fieldData[0] = String("4 - fork error");
  fieldData[1] = truckId;
  postData();
          break;
          
           case '5':
  fieldData[0] = String("5 - five error");
  fieldData[1] = truckId;
  postData();
          break;
          
           case '6':
  fieldData[0] = String("6 - send help");
  fieldData[1] = truckId;
  postData();
          break;
          
           case '7':
  fieldData[0] = String("7 - load too heavy");
  fieldData[1] = truckId;
  postData();
          break;
          
           case '8':
  fieldData[0] = String("8 - late eight");
  fieldData[1] = truckId;
  postData();
          break;
          
           case '9':
  fieldData[0] = String("9 - nope");
  fieldData[1] = truckId;
  postData();
          break;
          
           case '0':
  fieldData[0] = String("0 - Oh-Oh");
  fieldData[1] = truckId;
  postData();
          break;
          
      case '*':
      errorCharEnt('*');
          break;
      case '#':
      errorCharEnt('#');
        break;
      default:
        Serial.println(key);
    }
    
  }
}

//--------------------------------------------------------------
// function if they have pressed the * or # keys
// ask them to enter another number and possibly light a red led 
// to signify an error
void errorCharEnt(char x){
        digitalWrite(ledPin, HIGH);
        lcd.clear();
        lcd.setCursor(0,0); // position, row
        lcd.print("Not a number");
        lcd.setCursor(0, 1);
        lcd.print("(choose 0-9)");
        lcd.setCursor(15, 1);
        lcd.print(x);
}

//--------------------------------------------------------------
// send the data to the server with the public and private keys
// currently using data.sparkfun.com where a new database was set up
void postData()
{
  // Make a TCP connection to remote host
  if (client.connect(server, 80))
  {
    lcd.setCursor(0,0); // position, row
    lcd.print("  Please Wait   ");

    Serial.println("connected to server");
    // Post the data! Request should look a little something like:
    // GET /input/publicKey?private_key=privateKey&light=1024&switch=0&name=Jim HTTP/1.1\n
    // Host: data.sparkfun.com\n
    // Connection: close\n
    // \n
    client.print("GET /input/");
    client.print(publicKey);
    client.print("?private_key=");
    client.print(privateKey);
    for (int i=0; i<NUM_FIELDS; i++)
    {
      client.print("&");
      client.print(fieldNames[i]);
      client.print("=");
      client.print(fieldData[i]);
    }
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
/*
      lcd.setCursor(0,0); // position, row
      lcd.print("   *Trifork*    ");
      lcd.setCursor(0, 1);
      lcd.print("Code Number ");
      */
      delay(1000);                                 //  +++++++++++++++++++++++++++++++++++++++++++++++
      lastConnectionTime = millis();               //  +++++++++++++++++++++++++++++++++++++++++++++++
  }
  else
  {
    Serial.println(F("Connection failed"));
  } 
  
  delay(1000);                                      // +++++++++++++++++++++++++++++++++++++++++++++++
  
  
  //--------------------------------------------------------------
	// if there's incoming data from the net connection.
	// send it out the serial port.  This is for debugging
	// purposes only:	
	while (client.available() && status == WL_CONNECTED) {
		if (incomingData == false)
		{
			Serial.println();
			Serial.println("--- Incoming data Start ---");
			incomingData = true;
		}
		char c = client.read();
		Serial.write(c);
	}
 //--------------------------------------------------------------
 // Needed to add this to not run out of sockets! oops
        client.flush();
        client.stop();
        
	// Debugging --------------------------------------------------
	if (incomingData == true)
	{
		Serial.println("--- Incoming data End");
		incomingData = false; 
	}
	if (status == WL_CONNECTED){
		Serial.println("--- WiFi connected");
	}
	else{
		Serial.println("--- WiFi not connected");
	}
      
      lcd.setCursor(0,0); // position, row +++++++++++++++++++++++++++++++++++++++++++++CHECK CEHECK
      lcd.print("Trifork         ");
      lcd.setCursor(0, 1);
      lcd.print("Code Number ");
}
    
    //--------------------------------------------------------------
    // once we connect to the server list in the console the details
    // of our connection at the start up.
    void printWifiStatus() {
      // print the SSID of the network you're attached to:
      Serial.print("SSID: ");
      Serial.println(WiFi.SSID());
    
      // print your WiFi shield's IP address:
      IPAddress ip = WiFi.localIP();
      Serial.print("IP Address: ");
      Serial.println(ip);
    
      // print the received signal strength:
      long rssi = WiFi.RSSI();
      Serial.print("signal strength (RSSI):");
      Serial.print(rssi);
      Serial.println(" dBm");
}





