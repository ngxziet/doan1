#define PIR_PIN 17  // Chân kết nối với cảm biến HC-SR501

void setup() {
    Serial.begin(115200);
    pinMode(PIR_PIN, INPUT);
}

void loop() {
    int motion = digitalRead(PIR_PIN);
    if (motion == HIGH) {
        Serial.println("Phát hiện chuyển động!");
    }
    delay(500);
}
