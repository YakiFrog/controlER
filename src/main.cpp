#include <Arduino.h>
#include <PS4Controller.h>
#include "CytronMotorDriver.h"
#include <math.h>

#define LED_PIN 2

/*
 IN1: DigitalSignal(Direction:DIR) for motor LEFT 回転方向
 IN2: DigitalSignal(Direction:DIR) for motor RIGHT 回転方向
 AN1: AnalogSignal(Speed:PWM) for motor LEFT 回転速度
 AN2: AnalogSignal(Speed:PWM) for motor RIGHT 回転速度
*/

double sin_45 = sin((0.25)*M_PI);
double cos_45 = cos((0.25)*M_PI);
double length = 0.4; // ロボットの車輪間距離(m)
double rad_sec = 0; // 角速度(rad/s)

// モータのインスタンス化 (モータのピン番号を指定)
CytronMD motorLF(PWM_DIR, 32, 33);  // PWM: Analog, DIR: Digital
CytronMD motorRF(PWM_DIR, 25, 26); 
CytronMD motorLB(PWM_DIR, 27, 14);
CytronMD motorRB(PWM_DIR, 12, 13);

// 4輪オムニの制御関数
void FourWheelOmniControl() {
  // PS4コントローラからの入力を取得
  double lstick_x = PS4.LStickX(); // 左スティックのX軸
  double lstick_y = PS4.LStickY(); // 左スティックのY軸

  double front_right_speed = - lstick_x*sin_45 + lstick_y*cos_45 + rad_sec*length;
  double front_left_speed = - lstick_x*cos_45 - lstick_y*cos_45 + rad_sec*length;
  double rear_left_speed = lstick_x*sin_45 - lstick_y*cos_45 + rad_sec*length;
  double rear_right_speed = lstick_x*cos_45 + lstick_y*sin_45 + rad_sec*length;

  // モーターに速度を指令 (速度は-255~255の範囲で指定)
  
  motorLF.setSpeed(front_left_speed);
  motorRF.setSpeed(front_right_speed);
  motorLB.setSpeed(rear_left_speed);
  motorRB.setSpeed(rear_right_speed);

  Serial.println("front_right_speed: " + String(front_right_speed));
  Serial.println("front_left_speed: " + String(front_left_speed));
  Serial.println("rear_left_speed: " + String(rear_left_speed));
  Serial.println("rear_right_speed: " + String(rear_right_speed));
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  // PS4コントローラの初期化
  PS4.begin("60:8C:4A:71:05:E2"); // 白のコントローラ
  Serial.println("Ready.");
  while (!PS4.isConnected()) {}
  Serial.println("Connected.");
  delay(1000);
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
青：00:B8:15:F7:39:82
黒：60:8C:4A:71:05:E2
*/