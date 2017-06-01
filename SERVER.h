Network network;


void onReceive(uint32_t from, String& msg) {
  PRINT("got message: ");
  PRINTLN(msg);
  network.onReceive(from, msg);
}

void setup() {
  Serial.begin(115200);
  PRINTLN("setup");
  network.setup(&onReceive);
}

void loop() {
  network.update();
  delay(10);
}