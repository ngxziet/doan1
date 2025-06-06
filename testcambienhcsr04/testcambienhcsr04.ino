#define TRIG_PIN  4 // Chân Trig của cảm biến HC-SR04
#define ECHO_PIN  16 // Chân Echo của cảm biến HC-SR04

void setup() {
    Serial.begin(115200);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

void loop() {
    long duration;
    float distance;
    
    // Phát xung trigger
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // Đọc thời gian xung phản hồi từ Echo
    duration = pulseIn(ECHO_PIN, HIGH);
    
    // Tính toán khoảng cách (tốc độ âm thanh = 34300 cm/s)
    distance = (duration * 0.0343) / 2;
    
    // In kết quả ra Serial Monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    delay(500); // Chờ trước khi đo lần tiếp theo
}

