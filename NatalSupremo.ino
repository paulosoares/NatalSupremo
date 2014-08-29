#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//Apenas se não funcionar o DHCP
IPAddress ip(192, 168, 25, 36);
IPAddress mydns(8, 8, 8, 8);
IPAddress gateway(192, 168, 25, 1);
IPAddress subnet(255, 255, 255, 0);

EthernetClient client;
char server[] = "api.thingspeak.com";
//IPAddress server(64,131,82,241);
unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 15L * 1000L; // delay between updates, in milliseconds

String response = "";

void setup() {
  strip.begin();
  strip.show();
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  delay(1000);
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // DHCP failed, so use a fixed IP address:
    Ethernet.begin(mac, ip, mydns, gateway, subnet);
  }
  
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    //Serial.write(c);
    response += c;

    if (response.indexOf("vermelho") > 0) {
        Serial.println("vermelho"); 
        colorWipe(strip.Color(0, 0, 0), 25);
        colorWipe(strip.Color(255, 0, 0), 50);
        client.stop();
      } else if (response.indexOf("verde") > 0) {         
        Serial.println("verde");
        colorWipe(strip.Color(0, 0, 0), 25);
        colorWipe(strip.Color(0, 255, 0), 50);
        client.stop();
      } else if (response.indexOf("azul") > 0) {  
        Serial.println("azul");
        colorWipe(strip.Color(0, 0, 0), 25);
        colorWipe(strip.Color(0, 0, 255), 50);
        client.stop();
      }

  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {    
    httpRequest();   
  }

}

void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  response = "";

  if (client.connect(server, 80)) {
    //Serial.println("connecting...");
    
    client.println("GET /channels/8937/field/2/last.txt HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    Serial.println("connection failed");
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
