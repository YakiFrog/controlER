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

float sin_45 = 0.7071;
float cos_45 = 0.7071;
double length = 0.5; // ロボットの車輪間距離(m)
double rad_sec = 0; // 角速度(rad/s)
int n = 10; // 速度の倍率

// モータのインスタンス化 (モータのピン番号を指定)
// PWM_DIR: DIP 10-11-1-1-0-0
CytronMD motorFL(PWM_DIR, 32, 25); // AN1, IN1
CytronMD motorFR(PWM_DIR, 33, 26); // AN2, IN2
CytronMD motorRL(PWM_DIR, 27, 12); // AN1, IN1
CytronMD motorRR(PWM_DIR, 14, 13); // AN2, IN2

// 4輪オムニの制御関数
void FourWheelOmniControl() {
  // PS4コントローラからの入力を取得
  double lstick_x = PS4.LStickX(); // 左スティックのX軸 -127~127
  double lstick_y = PS4.LStickY(); // 左スティックのY軸 -127~127

  // ドリフトを考慮して，0-5の誤差を許容
  // 127の10％は，
  if (lstick_x > -12.7 && lstick_x < 12.7) {
    lstick_x = 0;
  }
  if (lstick_y > -12.7 && lstick_y < 12.7) {
    lstick_y = 0;
  }

  // -127~127の範囲を-1.5~1.5に変換 (1.5は最大速度[m/s])
  lstick_x = lstick_x / 85.333;
  lstick_y = lstick_y / 85.333;

  Serial.println("lstick_x: " + String(lstick_x));
  Serial.println("lstick_y: " + String(lstick_y));

  // モータの速度を計算
  double front_right_speed = lstick_x*(-sin_45) + lstick_y*cos_45 + rad_sec*length;
  double front_left_speed = lstick_x*(-cos_45) + lstick_y*(-sin_45) + rad_sec*length;
  double rear_left_speed = lstick_x*sin_45 + lstick_y*(-cos_45) + rad_sec*length;
  double rear_right_speed = lstick_x*cos_45 + lstick_y*sin_45 + rad_sec*length;

  // モーターに速度を指令 (速度は-255~255の範囲で指定)
  motorFL.setSpeed(int(front_left_speed * n)); 
  motorFR.setSpeed(int(front_right_speed * n));
  motorRL.setSpeed(int(rear_left_speed * n));
  motorRR.setSpeed(int(rear_right_speed * n));

  Serial.println("front_left_speed: " + String(front_left_speed) + "[m/s], " + String(int(front_left_speed * n)) + "[PWM]");
  Serial.println("front_right_speed: " + String(front_right_speed) + "[m/s], " + String(int(front_right_speed * n)) + "[PWM]");
  Serial.println("rear_left_speed: " + String(rear_left_speed) + "[m/s], " + String(int(rear_left_speed * n)) + "[PWM]");
  Serial.println("rear_right_speed: " + String(rear_right_speed) + "[m/s, " + String(int(rear_right_speed * n)) + "[PWM]");
  
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  // PS4コントローラの初期化
  PS4.begin("60:8C:4A:71:05:E2"); 
  Serial.println("Ready.");
  while (!PS4.isConnected()) {}
  Serial.println("Connected.");
  delay(1000);
}

void loop() {
  if (PS4.LatestPacket()){
    digitalWrite(LED_PIN, HIGH); // LEDを点灯
    FourWheelOmniControl();
    delay(50);
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