/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "UK-Devices";
const char* password = "BigBlueDevices";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

 
int sensmux1 = 0;
int sensmux2 = 0;
int outmux = 0;
int sensval = 0;
float sensvolt = 0.0;
int sensmax = 16;
int outmax = 8;
int values[16][8];

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 16, 17); //Serial1 is used for UART with Arduino
  Serial1.setRxBufferSize(20480);

  //print MAC address
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  delay(5000);
}

void loop(){
  int start = 0;

    while(start != 30){                   //code waits until it recieves a 30 in the rx buffer
      start = Serial1.parseInt();         //selected integer because it won't naturally occur
      Serial.print(start);
    }
    for(int i = 0; i<outmax; i++){        //loop takes data in and parses it into the array of integers
      for(int j = 0; j<sensmax; j++){
        values[j][i] = Serial1.parseInt();
        Serial.print(values[j][i]);
        Serial.print(' ');                 
      }
      Serial.println();
    }

    
    delay(1000);   //1 second delay provides more time for wifi connection

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            client.println("<!DOCTYPE html><html>");
            client.println("<body><h1>Pressure Data ESP32</h1>");

            client.println("<ol type=""1"">");
            for(int i =0; i<outmax; i++){
              for(int j = 0; j<sensmax; j++){
                client.print("<li>");
                client.print(values[j][i]);
                client.print("<i>");
                Serial.print(values[j][i]);
                Serial.print(' ');
                }  
                Serial.println();
              }

            client.println("</ol></body></html>");
           
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            //break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
