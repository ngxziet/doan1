#define GPIO_12 12
#define GPIO_14 14

void setup() {
  Serial.begin(115200);

  pinMode(GPIO_12, OUTPUT);
  pinMode(GPIO_14, OUTPUT);

  // Ban đầu tắt cả 2 LED (GPIO = HIGH)
  digitalWrite(GPIO_12, HIGH);
  digitalWrite(GPIO_14, HIGH);
  Serial.println("Bắt đầu: Cả 2 LED đều tắt");

  delay(5000); // Đợi 5 giây

  // Sau 5 giây, bật cả 2 LED (GPIO = LOW)
  digitalWrite(GPIO_12, LOW);
  digitalWrite(GPIO_14, LOW);
  Serial.println("Sau 5s: Cả 2 LED đều sáng");
}

void loop() {
  // Không cần làm gì thêm
}
