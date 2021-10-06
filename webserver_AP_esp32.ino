// ESP32 Access Point - Ponto de acesso para envio de mensagens


// Referência:
// https://randomnerdtutorials.com

// Biblioteca para WiFi
#include <WiFi.h>
#include <driver/gpio.h>

#define led GPIO_NUM_2

// Credenciais para acesso do WiFi ESP32
const char* ssid     = "ESP32-Access-Point";

// Número da porta do Web Server
WiFiServer server(80);

// Variable to store the HTTP request
// Variável para armazenar o HTTP request
String header;

// Auxiliar variables to store the current output state
// 
String ledState = "off";

void setup() {
  
  // Initialize the output variables as outputs
  gpio_set_direction(led, GPIO_MODE_OUTPUT);
  gpio_set_level(led, 0);

  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid);

  IPAddress IP = WiFi.softAPIP();
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:

          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /Led/on") >= 0) {
              ledState = "on";
              gpio_set_level(led, 1);
            } else if (header.indexOf("GET /Led/off") >= 0) {
              ledState = "off";
              gpio_set_level(led, 0);
            }
            
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
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>Led - State " + ledState + "</p>");
            // If the output26State is off, it displays the ON button       
            if (ledState=="off") {
              client.println("<p><a href=\"/Led/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Led/off\"><button class=\"button button2\">OFF</button></a></p>");
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
  }
}
