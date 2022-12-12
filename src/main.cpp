#include <Arduino.h>
#include <PS4Controller.h>
#include <Cytron_SmartDriveDuo.h>

#define IN1 32
#define BAUDRATE 115200
#define LED_PIN 2
#define PS4_ADDR "00:B8:15:F7:39:82"

signed int speedLeft_F, speedRight_F;
signed int speedLeft_R, speedRight_R;

float sin_45 = 0.7071;
float cos_45 = 0.7071;
double length = 0.5; // ロボットの車輪間距離(m)
double rad_sec = 0; // 角速度(rad/s) 今後，回転する操作を付け加えるときに使う
int n = 100; // 速度の出力倍率(%) この値を実験で決める

// FrontLR DIP: 111-000-00
Cytron_SmartDriveDuo motorFrontLR(SERIAL_PACKETIZED, IN1, 0, BAUDRATE);
// FrontRL DIP: 111-001-00
Cytron_SmartDriveDuo motorRearLR(SERIAL_PACKETIZED, IN1, 1, BAUDRATE);

void FourWheelOmniControl(){
    double lstick_x = PS4.LStickX(); // -127 to 127
    double lstick_y = PS4.LStickY(); // -127 to 127

    if (lstick_x > -12.7 && lstick_x < 12.7){
        lstick_x = 0;
    }
    if (lstick_y > -12.7 && lstick_y < 12.7){
        lstick_y = 0;
    }

    // -127 to 127 -> -1.5 to 1.5
    lstick_x = lstick_x / 85.333;
    lstick_y = lstick_y / 85.333;

    double front_right_speed = lstick_x*(-sin_45) + lstick_y*cos_45 + rad_sec*length;
    double front_left_speed = lstick_x*(-cos_45) + lstick_y*(-sin_45) + rad_sec*length;
    double rear_left_speed = lstick_x*sin_45 + lstick_y*(-cos_45) + rad_sec*length;
    double rear_right_speed = lstick_x*cos_45 + lstick_y*sin_45 + rad_sec*length;

    Serial.println("出力倍率: " + String(n) + "%");
    Serial.println("front_left_speed: " + String(front_left_speed) + "[m/s]");
    Serial.println("front_right_speed: " + String(front_right_speed) + "[m/s]");
    Serial.println("rear_left_speed: " + String(rear_left_speed) + "[m/s]");
    Serial.println("rear_right_speed: " + String(rear_right_speed) + "[m/s]");

    // -1.5 to 1.5 -> -100 to 100 (n倍率)
    // 100/1.5 = 66.666
    speedLeft_F = front_left_speed * 66.666 * n / 100;
    speedRight_F = front_right_speed * 66.666 * n / 100;
    speedLeft_R = rear_left_speed * 66.666 * n / 100;
    speedRight_R = rear_right_speed * 66.666 * n / 100;

    motorFrontLR.control(speedLeft_F, speedRight_F);
    motorRearLR.control(speedLeft_R, speedRight_R);
}

void setup() {
    Serial.begin(115200);
    motorFrontLR.initialByte(0x80);
    motorRearLR.initialByte(0x80);
    pinMode(LED_PIN, OUTPUT);
    PS4.begin(PS4_ADDR);
    Serial.println("PS4 Controller Ready!");
    Serial.println("Press PS button to start");
    while(!PS4.isConnected()){}
    Serial.println("PS4 Controller Connected!");
    delay(1000);
}

// control(signed int motorLSpeed, signed int motorRSpeed)
// motorLSpeed: -100 to 100, motorRSpeed: -100 to 100

void loop(){
    if (PS4.LatestPacket()){
        digitalWrite(LED_PIN, HIGH);
        FourWheelOmniControl();
        delay(100);
    } else {
        digitalWrite(LED_PIN, LOW);
        delay(100);
    }
}