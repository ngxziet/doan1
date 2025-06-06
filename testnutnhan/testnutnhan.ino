// Khai báo chân nút nhấn
const int buttonPin1 = 25;
const int buttonPin2 = 26;
const int buttonPin3 = 27;

void setup() {
  Serial.begin(115200);  // Khởi tạo Serial
  pinMode(buttonPin1, INPUT);  // Nếu nút nhấn lên mức cao thì không dùng INPUT_PULLUP
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
}

void loop() {
  // Đọc trạng thái của các nút
  int buttonState1 = digitalRead(buttonPin1);
  int buttonState2 = digitalRead(buttonPin2);
  int buttonState3 = digitalRead(buttonPin3);

  // Kiểm tra nếu nút được nhấn (mức HIGH)
  if (buttonState1 == HIGH) {
    Serial.println("Nút GPIO25 được nhấn");
  }

  if (buttonState2 == HIGH) {
    Serial.println("Nút GPIO26 được nhấn");
  }

  if (buttonState3 == HIGH) {
    Serial.println("Nút GPIO27 được nhấn");
  }

  delay(200);  // Chống dội nút và tránh spam log
}
