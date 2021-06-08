#include <dht11.h>

#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>//LCD library
LiquidCrystal lcd = LiquidCrystal(2, 3, 4, 5, 6, 7);//lcd(RS,En,D4,D5,D6,D7)
dht11 DHT11;

#define DHT11PIN 8 //humidity and temperature sensor
#define M_SENSOR A1//Moisture sensor
#define LED 9 // LED
#define LDR_SENSOR A0 // LDR sensor

// ETHERNET config.

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 177); // my IPAddress (192, 168, 68, 177)
// Wade IPAddress (192, 168, 1, 177)
// TTU IPAddress (10, 161, 13, 256)

// ASKSENSORS config.
char server[] = "asksensors.com";                               // ASKSENSORS host name
const int port = 80;                                      // Port: HTTP=80
const char* apiKeyIn = "i9v0EfLrTskDf366DYLdncfC988S36OO";      // API KEY IN (change it with your API KEY IN)
const char* lightModule = "?module1=";
const char* humidityModule = "?module4=";
const char* temperatureModule = "?module5=";
const char* moistureModule = "?module6=";



// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;



void setup() {
  Serial.begin(9600);
  
  pinMode(LDR_SENSOR, INPUT); //LDR sensor at pin A0
  pinMode(LED, OUTPUT); //LED
  
  lcd.begin(16, 2);
  lcd.clear();
  // print sensor name on the LCD
  lcd.setCursor(0, 0);
  lcd.print("LDR=  ");//LDR Sensor data will be displayed at(3,0)
  lcd.setCursor(8, 0);
  lcd.print("M=  ");//Moisture Sensor data will be displayed at(8,0)
  lcd.setCursor(0, 1);// Move cursor to the next line of LCD
  lcd.print("HS=   ,   ");//Humidity & temperature Sensor data will be displayed at(3,1) &(8,1) respectivel

  // Open serial communications and wait for port to open:

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);

}

void push(int data, char* module) {
  Serial.println("connecting...");
  // if you get a connection, report back via serial:
  if (client.connect(server, port)) {
    Serial.print("connected asksensors.com");
    //Create a URL for the request
    String url = "http://asksensors.com/api.asksensors/write/";
    url += apiKeyIn;
    url += module;
    url += data;
    Serial.print("********** requesting URL: ");
    Serial.println(url);
    //Make a HTTP request:
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n");
    client.println();

    Serial.println("> Request sent to ASKSENSORS");
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }

  void loop() {

    int lightData = analogRead(A0);
    lightData = map(lightData, 0, 1023, 0, 100);
    push(lightData, lightModule);
    int brightness = 100 - lightData;
    brightness = map(brightness, 0, 100, 0, 255);
    lcd.setCursor(4, 0);
    lcd.print(lightData);


    analogWrite(LED, brightness);


    int chk = DHT11.read(DHT11PIN);
    int temperature = (int)DHT11.temperature;
    temperature = int((temperature * 1.8) + 32);
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" F");
    lcd.setCursor(3, 1);
    lcd.print(temperature);
    lcd.print("*");//temperature in degree
    push(temperature, temperatureModule);


    int humidity = (int)DHT11.humidity;
    // humidity = map(humidity, 0, 255, 0, 100);
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    lcd.setCursor(8, 1);
    lcd.print(humidity);
    lcd.print("%");
    push(humidity, humidityModule);

    int moisture = analogRead(M_SENSOR);
    moisture = map(moisture, 0, 1023, 0, 100);
    Serial.print("Moisture: ");
    Serial.print(moisture);
    Serial.println("%");
    lcd.setCursor(10, 0);
    lcd.print(moisture);
    lcd.print("%");
    push(moisture, moistureModule);


    // if there are incoming bytes available
    // from the server, read them and print them:
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    // if the server's disconnected, stop the client:
    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();

      // do nothing forevermore:
      while (true);
    }
    delay(10000);// update data every 10 seconds
  }


}
