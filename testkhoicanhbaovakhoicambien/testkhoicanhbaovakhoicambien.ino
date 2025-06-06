#define TRIG_PIN     4
#define ECHO_PIN     16
#define PIR_PIN      17
#define OUTPUT_PIN   12

// Cấu hình
const unsigned long TIME_WINDOW = 10000; // 10 giây
const float MIN_DISTANCE = 0.0;
const float MAX_DISTANCE = 30.0;
const uint8_t TARGET_MOTIONS = 5;

// Biến trạng thái
uint8_t motionCount = 0;
bool objectInRange = false;
unsigned long windowStart = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  
  digitalWrite(OUTPUT_PIN, HIGH);
  digitalWrite(TRIG_PIN, LOW);
}

float measureDistance() {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  return duration * 0.034 / 2;
}

void activateOutput() {
  Serial.println("Kích hoạt OUTPUT");
  digitalWrite(OUTPUT_PIN, LOW);
  delay(5000);
  digitalWrite(OUTPUT_PIN, HIGH);
}

void resetCounter() {
  motionCount = 0;
  windowStart = 0;
  Serial.println("Reset bộ đếm");
}

void loop() {
  // Đo khoảng cách
  float distance = measureDistance();
  
  // Kiểm tra vật có trong vùng 0-30cm không
  bool nowInRange = (distance >= MIN_DISTANCE) && (distance <= MAX_DISTANCE);
  
  // Xử lý khi trạng thái thay đổi
  if (nowInRange && !objectInRange) {
    // Vật vừa vào vùng
    objectInRange = true;
    Serial.println("Vật vào vùng 0-30cm");
  } 
  else if (!nowInRange && objectInRange) {
    // Vật vừa ra khỏi vùng
    objectInRange = false;
    resetCounter();
    Serial.println("Vật ra khỏi vùng");
  }

  // Chỉ xử lý PIR nếu vật trong vùng
  if (objectInRange) {
    if (digitalRead(PIR_PIN) == HIGH) {
      delay(50); // Debounce
      if (digitalRead(PIR_PIN) == HIGH) {
        // Lần đầu phát hiện -> bắt đầu đếm thời gian
        if (motionCount == 0) {
          windowStart = millis();
        }
        
        motionCount++;
        Serial.print("Đếm chuyển động: ");
        Serial.println(motionCount);
        
        // Kiểm tra đủ 5 lần
        if (motionCount >= TARGET_MOTIONS) {
          activateOutput();
          resetCounter();
        }
      }
    }
    
    // Kiểm tra timeout 10 giây
    if (motionCount > 0 && (millis() - windowStart >= TIME_WINDOW)) {
      resetCounter();
    }
  }
  
  delay(100);
}