#include <driver/adc.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <HTTPClient.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

#define MQTTpubQos 2                          

//Création wifi and server
WiFiManager manager;
const char *mqtt_server = "broker.hivemq.com";
void callback(char* topic, byte* message, unsigned int length);
const char *ssid = "";    //hotspot
const char *password = "";    //password

//Wifi Client
WiFiClient espClient;
PubSubClient client(espClient);

//Variable
const int pin_temp = 39;
const int pin_button = 36;
RTC_DATA_ATTR boolean BoutonAppuye = 1;
RTC_DATA_ATTR float affiche_temp;


LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void IRAM_ATTR ecran_lcd() {
  BoutonAppuye = 0;
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  Serial.println("Setup");
  // initialize the lcd
  lcd.init();
  //Wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("");
    delay(1000);
  }
  Serial.println("Connection établie");
  //Connection au Broker
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.connect("ESP32Client");

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(pin_button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin_button), ecran_lcd, CHANGE);

  //précision de la température
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_0);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, HIGH);

  esp_sleep_enable_timer_wakeup(10000000);
}

// the loop function runs over and over again forever
void loop() {
  //Reconnecter au serveur MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  boolean state_button = digitalRead(pin_button);
  //calcule de la température
  int value = adc1_get_raw(ADC1_CHANNEL_3);
  float voltage = (float)value * 1100. / 4095.;
  float temperature = voltage / 10;
  affiche_temp = temperature;
  //Création du message a envoyer
  String message;
  message += String("{\"temperature\":");
  message += String(temperature);
  message += String("}");
  //envoie du message
  client.publish("IoT_Cours", message.c_str());
  delay(1000);
  Serial.println(message);
  
  if (BoutonAppuye == 0) {
    Serial.println("Bouton appuyé");
    lcd.init();                      // initialize the lcd
    lcd.backlight();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Temp = ");
    lcd.print(affiche_temp);
    lcd.print(" C");
    delay(10000);
    BoutonAppuye = 1;
  }
  if (BoutonAppuye == 1) {
    Serial.println("Bouton fin");
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.noBacklight();
    lcd.noDisplay();
    esp_light_sleep_start();
    ESP.restart();
  }

}
