#include <Wire.h>
#include "RTClib.h"

#define SER   5  // Chân dữ liệu (DS)
#define SRCLK 19   // Chân xung clock (SHCP)
#define RCLK  18  // Chân chốt dữ liệu (STCP)

RTC_DS1307 rtc;

// Mã số 0-9 cho LED 7 đoạn (Common Anode - Đảo bit từ CC)
const byte digitCode[11] = {
    ~0b00111111,  // 0
    ~0b00000110,  // 1
    ~0b01011011,  // 2
    ~0b01001111,  // 3
    ~0b01100110,  // 4
    ~0b01101101,  // 5
    ~0b01111101,  // 6
    ~0b00000111,  // 7
    ~0b01111111,  // 8
    ~0b01101111,  // 9
    ~0b00000000   //led khong sang
};

// Mã chọn LED (Common Anode - Chân chung điều khiển mức HIGH)
const byte digitSelect[8] = {
    0b00000001,  // LED 1
    0b00000010,  // LED 2
    0b00000100,  // LED 3
    0b00001000,  // LED 4
    0b00010000,  // LED 5
    0b00100000,   // LED 6
    0b01000000,   //LED7
    0b10000000 //LED8
};

void shiftOutByte(byte data) {
    for (int i = 0; i < 8; i++) {
        digitalWrite(SRCLK, LOW);
        digitalWrite(SER, (data >> (7 - i)) & 1);
        digitalWrite(SRCLK, HIGH);
    }
}

void displayTime(int hour, int minute, int second) {
    int digits[8] = {second % 10, second / 10, 10, minute % 10, minute / 10, 10, hour % 10, hour / 10};
    
    for (int i = 0; i < 8; i++) {
        digitalWrite(RCLK, LOW);
        shiftOutByte(digitCode[digits[i]]);
        shiftOutByte(digitSelect[i]);
        digitalWrite(RCLK, HIGH);
        delay(1); // Giảm nhấp nháy
    }
}

void setup() {
    pinMode(SER, OUTPUT);
    pinMode(SRCLK, OUTPUT);
    pinMode(RCLK, OUTPUT);
    Wire.begin();
    rtc.begin();
    if (!rtc.isrunning()) {
        rtc.adjust(DateTime(__DATE__, __TIME__)); // Cài đặt thời gian hiện tại
    }
}

void loop() {
    DateTime now = rtc.now();
    displayTime(now.hour(), now.minute(), now.second());
}
