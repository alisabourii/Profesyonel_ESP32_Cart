#include <WiFi.h>

const char* ssid     = "TTNET_ZyXEL_RVAN";
const char* password = "3DaE1B2443497";
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String ileri_durum = "off";
String geri_durum = "off";
String sag_durum = "off";
String sol_durum = "off";

// Assign output variables to GPIO pins
const int ENA = 12;
const int mtr1A = 26;
const int mtr1B = 27;

const int ENB = 14;
const int mtr2A = 33;
const int mtr2B = 25;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(ENA, OUTPUT);
  pinMode(mtr1A, OUTPUT);
  pinMode(mtr1B, OUTPUT);

   pinMode(ENB, OUTPUT);
  pinMode(mtr2A, OUTPUT);
  pinMode(mtr2B, OUTPUT);
  // Set outputs to LOW
 /* digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);
  digitalWrite(output25, LOW);*/

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
}

void loop(){
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
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("ILERI on");
              ileri_durum = "on";
              ileri(); } 
            else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("ILERI off");
              ileri_durum = "off";
              dur();}

            else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GERI on");
              geri_durum = "on";
              geri();}
            else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GERI off");
              geri_durum = "off";
              dur(); }

            else if (header.indexOf("GET /sag/on") >= 0) {
              Serial.println("SAG on");
              sag_durum = "on";
              sag();}
            else if (header.indexOf("GET /sag/off") >= 0) {
              Serial.println("SAG off");
              sag_durum = "off";
              dur(); }

            else if (header.indexOf("GET /sol/on") >= 0) {
              Serial.println("SOL on");
              sol_durum = "on";
              sol();}
            else if (header.indexOf("GET /sol/off") >= 0) {
              Serial.println("SOL off");
              sol_durum = "off";
              dur(); }


            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Araba projesi</h1>");
            

            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>ileri " + ileri_durum + "</p>");
            // If the output26State is off, it displays the ON button       
            if (ileri_durum=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ILERI</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">DUR</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>Geri " + geri_durum + "</p>");
            // If the output27State is off, it displays the ON button       
            if (geri_durum=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">GERI</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">DUR</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>Sag " + sag_durum + "</p>");
            // If the output27State is off, it displays the ON button       
            if (sag_durum=="off") {
              client.println("<p><a href=\"/sag/on\"><button class=\"button\">SAG</button></a></p>");
            } else {
              client.println("<p><a href=\"/sag/off\"><button class=\"button button2\">DUR</button></a></p>");
            }

            client.println("<p>Sol " + sol_durum + "</p>");
            // If the output27State is off, it displays the ON button       
            if (sol_durum=="off") {
              client.println("<p><a href=\"/sol/on\"><button class=\"button\">SOL</button></a></p>");
            } else {
              client.println("<p><a href=\"/sol/off\"><button class=\"button button2\">DUR</button></a></p>");
            }

            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
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

void ileri(){
  digitalWrite(ENA, 1);
  digitalWrite(mtr1A,1);
  digitalWrite(mtr1B,0);
  digitalWrite(ENB, 1);
  digitalWrite(mtr2A,1);
  digitalWrite(mtr2B,0);
}

void geri(){
  digitalWrite(ENA, 1);
  digitalWrite(mtr1A,0);
  digitalWrite(mtr1B,1);
  digitalWrite(ENB, 1);
  digitalWrite(mtr2A,0);
  digitalWrite(mtr2B,1);
}

void sol(){
  digitalWrite(ENA, 1);
  digitalWrite(mtr1A,1);
  digitalWrite(mtr1B,0);

  digitalWrite(ENB, 1);
  digitalWrite(mtr2A,0);
  digitalWrite(mtr2B,1);
}

void sag(){
  digitalWrite(ENA, 1);
  digitalWrite(mtr1A,0);
  digitalWrite(mtr1B,1);

  digitalWrite(ENB, 1);
  digitalWrite(mtr2A,1);
  digitalWrite(mtr2B,0);
}

void dur(){
  digitalWrite(ENA, 0);
  digitalWrite(mtr1A,0);
  digitalWrite(mtr1B,0);
  digitalWrite(ENB, 0);
  digitalWrite(mtr2A,0);
  digitalWrite(mtr2B,0);
}