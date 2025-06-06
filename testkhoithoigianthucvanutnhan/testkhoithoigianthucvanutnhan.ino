#include <Wire.h>
#include "RTClib.h"

#define SER   5
#define SRCLK 19
#define RCLK  18
#define BUTTON 27

RTC_DS1307 rtc;

// Mã số 0-9 cho LED 7 đoạn (Common Anode - Đảo bit từ CC)
const byte digitCode[11] = {
  0xC0, 0xF9, 0xA4, 0xB0,
  0x99, 0x92, 0x82, 0xF8,
  0x80, 0x90, 0xFF // blank
};


// Mã chọn LED (Common Anode - Chân chung điều khiển mức HIGH)
const byte digitSelect[8] = {
    0b00000001,  // LED 1
    0b00000010,  // LED 2
    0b00000100,  // LED 3
    0b00001000,  // LED 4
    0b00010000,  // LED 5
    0b00100000,  // LED 6
    0b01000000,  // LED 7
    0b10000000   // LED 8
};

bool displayMode = true; // true: hiển thị giờ, false: hiển thị ngày tháng năm
bool lastButtonState = HIGH;
bool currentButtonState;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void shiftOutByte(byte data) {
    for (int i = 0; i < 8; i++) {
        digitalWrite(SRCLK, LOW);
        digitalWrite(SER, (data >> (7 - i)) & 1);
        digitalWrite(SRCLK, HIGH);
    }
}

void displayTime(int h, int m, int s) {
  static unsigned long lastUpdate = 0;
  static int currentDigit = 0;
  
  if (millis() - lastUpdate > 1) { // Quét nhanh hơn (1ms)
    lastUpdate = millis();
    
    int d[8] = {s % 10, s / 10, 10, m % 10, m / 10, 10, h % 10, h / 10};
    
    // Tắt toàn bộ display trước
    digitalWrite(RCLK, LOW);
    shiftOutByte(0xFF);
    shiftOutByte(0x00);
    digitalWrite(RCLK, HIGH);
    
    // Hiển thị digit hiện tại
    digitalWrite(RCLK, LOW);
    shiftOutByte(digitCode[d[currentDigit]]);
    shiftOutByte(digitSelect[currentDigit]);
    digitalWrite(RCLK, HIGH);
    
    currentDigit = (currentDigit + 1) % 8;
  }
}

void displayDate(int day, int month, int year) {
    // Chỉ hiển thị 2 số cuối của năm
    year = year % 100;
    int digits[8] = {year % 10, year / 10, 10, month % 10, month / 10, 10, day % 10, day / 10};
    
    for (int i = 0; i < 8; i++) {
        digitalWrite(RCLK, LOW);
        shiftOutByte(digitCode[digits[i]]);
        shiftOutByte(digitSelect[i]);
        digitalWrite(RCLK, HIGH);
        delay(1);
    }
}

void setup() {
    pinMode(SER, OUTPUT);
    pinMode(SRCLK, OUTPUT);
    pinMode(RCLK, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP); // Sử dụng điện trở kéo lên nội
    Wire.begin();
    rtc.begin();
    if (!rtc.isrunning()) {
        rtc.adjust(DateTime(__DATE__, __TIME__));
    }
}

void loop() {
    // Đọc trạng thái nút bấm với debounce
    bool reading = digitalRead(BUTTON);
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }
    
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != currentButtonState) {
            currentButtonState = reading;
            if (currentButtonState == LOW) {
                displayMode = !displayMode; // Đảo chế độ hiển thị
            }
        }
    }
    lastButtonState = reading;

    DateTime now = rtc.now();
    if (displayMode) {
        displayTime(now.hour(), now.minute(), now.second());
    } else {
        displayDate(now.day(), now.month(), now.year());
    }
}