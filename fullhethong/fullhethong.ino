#include <Wire.h>
#include "RTClib.h"

// ==== Pin định nghĩa ====
#define PIR_PIN     17
#define TRIG_PIN     4
#define ECHO_PIN    16
#define ALERT_PIN   12

#define SER          5
#define SRCLK       19
#define RCLK        18

#define BTN_MODE    27
#define BTN_RESET   25
#define BTN_ENABLE  26

// ==== RTC ====
RTC_DS1307 rtc;

// ==== LED 7 đoạn ====
const byte digitCode[11] = {
  0xC0, 0xF9, 0xA4, 0xB0,
  0x99, 0x92, 0x82, 0xF8,
  0x80, 0x90, 0xFF // blank
};

const byte digitSelect[8] = {
  0b00000001, 0b00000010, 0b00000100, 0b00001000,
  0b00010000, 0b00100000, 0b01000000, 0b10000000
};

// ==== Trạng thái hệ thống ====
bool displayMode = true;
bool systemEnabled = true;

int alertCount = 0;
unsigned long alertStartTime = 0;

unsigned long lastMotionDetectTime = 0;
const unsigned long motionRepeatInterval = 500;  // Đếm mỗi 500ms nếu còn chuyển động
const unsigned long alertWindow = 5000;          // 5s để tính cảnh báo

bool alertActive = false;
unsigned long alertStartMillis = 0;
const unsigned long alertDuration = 5000;

// ==== Khởi tạo phần cứng ====
void initHardware() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ALERT_PIN, OUTPUT);
  digitalWrite(ALERT_PIN, HIGH);

  pinMode(SER, OUTPUT);
  pinMode(SRCLK, OUTPUT);
  pinMode(RCLK, OUTPUT);

  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);
  pinMode(BTN_ENABLE, INPUT_PULLUP);

  Wire.begin();
  rtc.begin();
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
}

// ==== LED hiển thị ====
void shiftOutByte(byte data) {
  digitalWrite(SRCLK, LOW);
  for (int i = 0; i < 8; i++) {
    digitalWrite(SER, (data >> (7 - i)) & 1);
    digitalWrite(SRCLK, HIGH);
    digitalWrite(SRCLK, LOW);
  }
}

void displayDigit(int digit, int position) {
  digitalWrite(RCLK, LOW);
  shiftOutByte(digitCode[digit]);
  shiftOutByte(digitSelect[position]);
  digitalWrite(RCLK, HIGH);
}

void displayTime(int h, int m, int s) {
  int d[8] = {s % 10, s / 10, 10, m % 10, m / 10, 10, h % 10, h / 10};
  for (int i = 0; i < 8; i++) {
    displayDigit(d[i], i);
    delayMicroseconds(1000);
  }
}

void displayDate(int day, int month, int year) {
  year = year % 100;
  int digits[8] = {year % 10, year / 10, 10, month % 10, month / 10, 10, day % 10, day / 10};
  for (int i = 0; i < 8; i++) {
    displayDigit(digits[i], i);
    delayMicroseconds(1000);
  }
}

// ==== Cảm biến ====
bool detectMotion() {
  return digitalRead(PIR_PIN) == HIGH;
}

float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return (pulseIn(ECHO_PIN, HIGH) * 0.0343) / 2;
}

// Kiểm tra nếu có chuyển động rồi mới đo khoảng cách
bool checkMotionWhenNear(float threshold = 30.0) {
  if (detectMotion()) {
    float dist = readDistanceCM();
    if (dist < threshold) {
      return true;
    }
  }
  return false;
}

// ==== Xử lý nút bấm ====
void handleButtons() {
  static unsigned long lastModeTime = 0;
  static unsigned long lastResetTime = 0;
  static unsigned long lastEnableTime = 0;

  static bool modePressed = false;
  static bool resetPressed = false;
  static bool enablePressed = false;

  unsigned long currentTime = millis();

  // Nút MODE
  if (digitalRead(BTN_MODE) == LOW) {
    if (!modePressed && (currentTime - lastModeTime > 50)) {
      modePressed = true;
      lastModeTime = currentTime;
      displayMode = !displayMode;
      Serial.println("Đã đổi chế độ hiển thị");
    }
  } else {
    modePressed = false;
  }

  // Nút RESET
  if (digitalRead(BTN_RESET) == LOW) {
    if (!resetPressed && (currentTime - lastResetTime > 50)) {
      resetPressed = true;
      lastResetTime = currentTime;
      alertCount = 0;
      alertStartTime = 0;
      Serial.println("Đã reset biến đếm cảnh báo");
    }
  } else {
    resetPressed = false;
  }

  // Nút ENABLE
  if (digitalRead(BTN_ENABLE) == LOW) {
    if (!enablePressed && (currentTime - lastEnableTime > 50)) {
      enablePressed = true;
      lastEnableTime = currentTime;
      systemEnabled = !systemEnabled;
      Serial.print("Hệ thống cảnh báo: ");
      Serial.println(systemEnabled ? "BẬT" : "TẮT");
    }
  } else {
    enablePressed = false;
  }
}

// ==== Cảnh báo ====
void triggerAlert() {
  Serial.println(">>> CẢNH BÁO <<<");
  digitalWrite(ALERT_PIN, LOW);
  alertActive = true;
  alertStartMillis = millis();
}


// ==== Xử lý cảm biến & logic cảnh báo ====
bool processMotionSensor(unsigned long currentTime, int &count, unsigned long &startTime) {
  bool motionDetected = checkMotionWhenNear();

  if (systemEnabled && motionDetected) {
    if (count == 0) {
      startTime = currentTime;
    }

    if (currentTime - lastMotionDetectTime >= motionRepeatInterval) {
      count++;
      lastMotionDetectTime = currentTime;
      Serial.print("Đếm cảnh báo: ");
      Serial.println(count);
    }
  }

  if (count > 0 && (currentTime - startTime) >= alertWindow) {
    if (count >= 5) {
      return true;
    } else {
      Serial.println("Không đủ cảnh báo trong 5s → Reset.");
    }
    count = 0;
    startTime = 0;
  }

  return false;
}

// ==== Setup & Loop ====
void setup() {
  initHardware();
}

void loop() {
  static unsigned long lastDisplayUpdate = 0;
  static unsigned long lastSensorCheck = 0;

  // Biến để theo dõi trạng thái cảnh báo
  static bool alertActive = false;
  static unsigned long alertStartMillis = 0;
  const unsigned long alertDuration = 5000;

  unsigned long currentTime = millis();

  handleButtons();

  // Cập nhật thời gian hiển thị thường xuyên (mỗi 10ms)
  if (currentTime - lastDisplayUpdate > 10) {
    lastDisplayUpdate = currentTime;
    DateTime now = rtc.now();
    if (displayMode) {
      displayTime(now.hour(), now.minute(), now.second());
    } else {
      displayDate(now.day(), now.month(), now.year());
    }
  }

  // Kiểm tra cảm biến mỗi 100ms
  if (currentTime - lastSensorCheck > 100) {
    lastSensorCheck = currentTime;
    if (!alertActive && processMotionSensor(currentTime, alertCount, alertStartTime)) {
      // Bắt đầu cảnh báo
      Serial.println(">>> CẢNH BÁO <<<");
      digitalWrite(ALERT_PIN, LOW);
      alertActive = true;
      alertStartMillis = currentTime;
      alertCount = 0;
      alertStartTime = 0;
    }
  }

  // Quản lý thời gian kết thúc cảnh báo (non-blocking)
  if (alertActive && (currentTime - alertStartMillis >= alertDuration)) {
    digitalWrite(ALERT_PIN, HIGH);
    alertActive = false;
  }

  delay(1); // giữ vòng lặp ổn định
}

