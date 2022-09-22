boolean BoutonAppuye = 0;
const int bouton = 36;
const int led = 2;
int counter = 0;

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(bouton, INPUT);
  attachInterrupt(digitalPinToInterrupt(bouton), blink, CHANGE);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, HIGH);
}

void loop() {
}

void blink() {
  boolean state_button = digitalRead(bouton);
  if (state_button == 0 && BoutonAppuye == 0) {
    BoutonAppuye = 1;
    digitalWrite(2, LOW);
    esp_light_sleep_start();
  }
  if (state_button == 1 && BoutonAppuye == 1) {
    BoutonAppuye = 0;
    digitalWrite(2, HIGH);
  }
}
