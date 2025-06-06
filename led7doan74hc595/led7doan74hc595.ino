#define SER   5  // Chân dữ liệu (DS)
#define SRCLK 19   // Chân xung clock (SHCP)
#define RCLK  18  // Chân chốt dữ liệu (STCP)
#define LED 2 // chân gpio 2

// Mã số 0-9 cho LED 7 đoạn (Common Anode - Đảo bit từ CC)
const byte digitCode[10] = {
    ~0b00111111,  // 0
    ~0b00000110,  // 1
    ~0b01011011,  // 2
    ~0b01001111,  // 3
    ~0b01100110,  // 4
    ~0b01101101,  // 5
    ~0b01111101,  // 6
    ~0b00000111,  // 7
    ~0b01111111,  // 8
    ~0b01101111   // 9
};

// Mã chọn LED (Common Anode - Chân chung điều khiển mức HIGH)
const byte digitSelect[8] = {
    0b00000001,  // LED 1 sáng
    0b00000010,  // LED 2 sáng
    0b00000100,  // LED 3 sáng
    0b00001000,  // LED 4 sáng
    0b00010000,  // LED 5 sáng
    0b00100000,  // LED 6 sáng
    0b01000000,  // LED 7 sáng
    0b10000000   // LED 8 sáng
};

// Mảng số cần hiển thị (1-8)
const byte displayNumbers[8] = {1, 2, 3, 4, 5, 6, 7, 8};

void shiftOutByte(byte data) {
    for (int i = 0; i < 8; i++) {
        digitalWrite(SRCLK, LOW);
        digitalWrite(SER, (data >> (7 - i)) & 1);
        digitalWrite(SRCLK, HIGH);
    }
}

void displayNumbersOnLEDs() {
    for (int i = 0; i < 8; i++) {
        digitalWrite(RCLK, LOW);
        shiftOutByte(digitCode[displayNumbers[i]]);  // Gửi số đến IC1
        shiftOutByte(digitSelect[i]);  // Chọn LED từ IC2
        digitalWrite(RCLK, HIGH);
        delay(2);  // Tránh nhấp nháy
    }
}

void setup() {
    pinMode(SER, OUTPUT);
    pinMode(SRCLK, OUTPUT);
    pinMode(RCLK, OUTPUT);
    pinMode(LED, OUTPUT);
}

void loop() {
    displayNumbersOnLEDs();
    digitalWrite(LED,HIGH);
}
