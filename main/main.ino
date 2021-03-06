#include "main.hpp"
#include "wheel.hpp"
#include "imu.hpp"
#include "display.hpp"

#define RESET_PIN 2
#define MODE_PIN 12

unsigned long nowTime, oldTime;
float dt;
int invert_mode = 4;
int tuning_mode = 0;
std::array<float, 3> theta;
std::array<float, 3> dot_theta;
std::array<float, 3> omega;
std::array<float, 3> input;
std::array<float, 3> gain;

IMU imu;
WheelsController controller(8.6, 0.95, 0.033); // kp, ki, kd

void setup()
{
  controller.WheelBrakeOn();
  Wire.begin();
  Serial.begin(115200);

  pinMode(RESET_PIN, INPUT_PULLUP);
  pinMode(MODE_PIN, INPUT_PULLUP);
  // attachInterrupt(RESET_PIN, Reset, FALLING);
  // attachInterrupt(MODE_PIN, SelectInvertMode, FALLING);

  // manualy tune PID gains
  attachInterrupt(MODE_PIN, SelectTuningMode, FALLING);

  DispInit();
  // SetUpEncoder();
  imu.Init();
  imu.OffsetCalc();
  imu.KalmanInit();
}

void loop()
{
  nowTime = micros();
  dt = (float)(nowTime - oldTime) / 1000000.0; // [µs]to[s]
  oldTime = nowTime;

  theta = imu.GetEstAngle(dt);
  dot_theta = imu.GetEstGyro();
  // omega = controller.GetWheelVelocity(dt);
  input = controller.GetInput();
  gain = controller.GetGain();

  // manualy tune PID gains
  if (digitalRead(RESET_PIN) == LOW)
  {
    PID_Tuning();
  }

  // controller.TestDrive();
  controller.Invert_point(theta, dot_theta);
  delay(8);

  // side inverted
  //  if (invert_mode == 1)
  //  {
  //    controller.Invert_side_C(theta, dot_theta, omega);
  //  }
  //  else if (invert_mode == 2)
  //  {
  //    controller.Invert_side_L(theta, dot_theta, omega);
  //  }
  // else if (invert_mode == 3)
  //  {
  //    controller.Invert_side_R(theta, dot_theta, omega);
  //  }

  //  if (invert_mode == 4)
  //  {
  // point inverted
  //    controller.Invert_point(theta, dot_theta);
  //  }
}

void Reset()
{
  invert_mode = 0;
}

void SelectInvertMode()
{
  Serial.println("Select Invert Mode");
  if (invert_mode == 0)
  {
    invert_mode = 1;
  }
  else if (invert_mode == 1)
  {
    invert_mode = 2;
  }
  else if (invert_mode == 2)
  {
    invert_mode = 3;
  }
  else if (invert_mode == 3)
  {
    invert_mode = 4;
  }
  else if (invert_mode == 4)
  {
    invert_mode = 0;
  }
}

void SelectTuningMode()
{
  tuning_mode++;
  if (tuning_mode == 3)
    tuning_mode = 0;
}

void PID_Tuning()
{
  controller.PID_Tuning(tuning_mode);
}

// void SetUpEncoder()
// {
//   attachInterrupt(ENCC_A, ReadEncoderC, CHANGE);
//   attachInterrupt(ENCC_B, ReadEncoderC, CHANGE);
//   attachInterrupt(ENCL_A, ReadEncoderL, CHANGE);
//   attachInterrupt(ENCL_B, ReadEncoderL, CHANGE);
//   attachInterrupt(ENCR_A, ReadEncoderR, CHANGE);
//   attachInterrupt(ENCR_B, ReadEncoderR, CHANGE);
// }

// void ReadEncoderC()
// {
//   controller.wheel_c.ReadEncoder();
// }

// void ReadEncoderL()
// {
//   controller.wheel_l.ReadEncoder();
// }

// void ReadEncoderR()
// {
//   controller.wheel_r.ReadEncoder();
// }
