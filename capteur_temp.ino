#include <driver/adc.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <HTTPClient.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

//Création wifi and server
WiFiManager manager;
const char *mqtt_server = "broker.hivemq.com";
void callback(char* topic, byte* message, unsigned int length);
const char *ssid = "Chambre_Froide";
const char *password = "Nour";

//Wifi Client
WiFiClient espClient;
PubSubClient client(espClient);

//Variable
const int analogInPin = A0;
const int analogInPin = A1;

int val_sensor = 0;


LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// the setup function runs once when you press reset or power the board
void setup() {
  //Wifi
  WiFi.mode(WIFI_STA);
  //wifi Error
  if (!manager.autoConnect(ssid, password)) {
    Serial.println("WiFi Failed!");
    return;
  }
  //Connection au Broker
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.connect("ESP32Client");

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize the lcd
  lcd.init();
  lcd.backlight();

  //précision de la température
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  //Reconnecter au serveur MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //calcule de la température
  int value = adc1_get_raw(ADC1_CHANNEL_0);
  float voltage = (float)value * 1100. / 4095.;
  float temperature = voltage / 10;
  
  String message;
  //Création du message a envoyer
  message = "Temperature: " + String(temperature) + "\xC2\xB0";
  //envoie du message
  client.publish("IoT_Cours", message.c_str());
  Serial.println(message);

  //affichage LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp = ");
  lcd.print(temperature);
  lcd.println(" C  ");

  delay(10000);
}
