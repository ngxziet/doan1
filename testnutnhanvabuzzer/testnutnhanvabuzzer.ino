// Định nghĩa chân GPIO
#define OUTPUT_PIN 12
#define BUTTON_PIN_1 25
#define BUTTON_PIN_2 26
#define BUTTON_PIN_3 27

// Thời gian giữ mức thấp (ms)
#define HOLD_TIME 3000

// Biến thời gian
unsigned long triggerTime = 0;
bool outputState = HIGH;
bool buttonPressed = false;

void setup() {
  // Khởi tạo Serial
  Serial.begin(115200);
  
  // Cấu hình chân GPIO
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(BUTTON_PIN_1, INPUT_PULLDOWN); // Sử dụng điện trở kéo xuống nội
  pinMode(BUTTON_PIN_2, INPUT_PULLDOWN);
  pinMode(BUTTON_PIN_3, INPUT_PULLDOWN);
  
  // Đặt trạng thái ban đầu
  digitalWrite(OUTPUT_PIN, outputState);
  
  Serial.println("System started. Waiting for button press...");
}

void loop() {
  // Kiểm tra nút nhấn
  bool currentButtonState = digitalRead(BUTTON_PIN_1) || digitalRead(BUTTON_PIN_2) || digitalRead(BUTTON_PIN_3);
  
  // Phát hiện cạnh lên của nút nhấn
  if (currentButtonState && !buttonPressed) {
    buttonPressed = true;
    outputState = LOW;
    digitalWrite(OUTPUT_PIN, outputState);
    triggerTime = millis();
    Serial.println("Button pressed. Setting output to LOW.");
  }
  
  // Kiểm tra thời gian
  if (outputState == LOW && (millis() - triggerTime) >= HOLD_TIME) {
    outputState = HIGH;
    digitalWrite(OUTPUT_PIN, outputState);
    buttonPressed = false;
    Serial.println("3 seconds elapsed. Setting output to HIGH.");
  }
  
  // Cập nhật trạng thái nút nhấn
  if (!currentButtonState) {
    buttonPressed = false;
  }
  
  // Chống nhiễu (debounce)
  delay(10);
}