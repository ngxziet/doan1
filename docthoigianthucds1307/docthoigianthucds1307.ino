#include <Wire.h>
#include <RTClib.h>

// Tùy chọn chân SDA/SCL cho ESP32
#define SDA_PIN 21
#define SCL_PIN 22

RTC_DS1307 rtc;

void setup() {
  Serial.begin(115200);
  delay(100); // Chờ serial ổn định

  // Khởi động I2C với chân tự chỉ định
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!rtc.begin()) {
    Serial.println("Không tìm thấy DS1307, kiểm tra kết nối I2C!");
    while (1); // Dừng chương trình
  }

  if (!rtc.isrunning()) {
    Serial.println("DS1307 chưa chạy, đang thiết lập thời gian hệ thống...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Cập nhật theo thời gian biên dịch
  } else {
    Serial.println("DS1307 đang hoạt động.");
  }
}

void loop() {
  DateTime now = rtc.now();

  Serial.print("Thời gian: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.second());

  Serial.print("  Ngày: ");
  Serial.print(now.day());
  Serial.print("/");
  Serial.print(now.month());
  Serial.print("/");
  Serial.println(now.year());

  delay(1000); // Cập nhật mỗi giây
}
