#include "CytronMotorDriver.h"


// インスタンス化
CytronMD::CytronMD(MODE mode, uint8_t pin1, uint8_t pin2)
{
  _mode = mode;
  _pin1 = pin1;
  _pin2 = pin2;
  
  pinMode(_pin1, OUTPUT);
  pinMode(_pin2, OUTPUT);
  
  digitalWrite(_pin1, LOW);
  digitalWrite(_pin2, LOW);
}

// モータのスピードと，方向を決める関数
void CytronMD::setSpeed(int16_t speed)
{
  // Make sure the speed is within the limit.
  if (speed > 255) {
    speed = 255;
  } else if (speed < -255) {
    speed = -255;
  }
  
  // Set the speed and direction.
  switch (_mode) {
    case PWM_DIR:
      if (speed >= 0) { // speedが正の場合
        analogWrite(_pin1, speed); // PWM
        digitalWrite(_pin2, LOW); // DIR
      } else {
        analogWrite(_pin1, -speed); // PWM
        digitalWrite(_pin2, HIGH); // DIR
      }
      break;
      
    case PWM_PWM:
      if (speed >= 0) {
        analogWrite(_pin1, speed);
        analogWrite(_pin2, 0);
      } else {
        analogWrite(_pin1, 0);
        analogWrite(_pin2, -speed);
      }
      break;
  }
}
