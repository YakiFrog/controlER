#include <Arduino.h>
#include <PS4Controller.h>
#include "CytronMotorDriver.h"

#define LED_PIN 2

/*
 IN1: DigitalSignal(Direction:DIR) for motor LEFT 回転方向
 IN2: DigitalSignal(Direction:DIR) for motor RIGHT 回転方向
 AN1: AnalogSignal(Speed:PWM) for motor LEFT 回転速度
 AN2: AnalogSignal(Speed:PWM) for motor RIGHT 回転速度
*/

// モータのインスタンス化 (モータのピン番号を指定)
CytronMD motorLF(PWM_DIR, 3, 4);
CytronMD motorRF(PWM_DIR, 5, 6); 
CytronMD motorLB(PWM_DIR, 7, 8);
CytronMD motorRB(PWM_DIR, 9, 10);

// 4輪オムニの制御関数
void FourWheelOmniControl() {
  // PS4コントローラからの入力を取得
  double lstick_x = PS4.LStickX(); // 左スティックのX軸
  double lstick_y = PS4.LStickY(); // 左スティックのY軸
  double rstick_x = PS4.RStickX(); // 右スティックのX軸
  double rstick_y = PS4.RStickY(); // 右スティックのY軸

  // 4輪のモーター速度を計算
  double front_left_speed = rstick_y + lstick_x + rstick_x;
  double front_right_speed = rstick_y - lstick_x + rstick_x;
  double rear_left_speed = rstick_y - lstick_x - rstick_x;
  double rear_right_speed = rstick_y + lstick_x - rstick_x;

  // モーターに速度を指令
  motorLF.setSpeed(front_left_speed);
  motorRF.setSpeed(front_right_speed);
  motorLB.setSpeed(rear_left_speed);
  motorRB.setSpeed(rear_right_speed);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  // PS4コントローラの初期化
  PS4.begin("0C:B8:15:D8:64:76"); // 白のコントローラ
  Serial.begin(115200);
  Serial.println("Ready.");
  while (!PS4.isConnected()) {}
  Serial.println("Connected.");
  delay(100);
}

void loop() {
  if (PS4.LatestPacket()){
    digitalWrite(LED_PIN, HIGH); // LEDを点灯
    FourWheelOmniControl();
    delay(100);
  } else {
    digitalWrite(LED_PIN, LOW); // LEDを消灯
    delay(100);
  }
}

/*
白：0C:B8:15:D8:64:76
青：00:1B:DC:F8:44:BA
*/