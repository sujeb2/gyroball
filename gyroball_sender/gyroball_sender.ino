#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>
#include <SPI.h>
#include <I2Cdev.h>

#include <MPU6050.h>

#define GYRO_ANALOG1 A4
#define GYRO_ANALOG2 A5
#define GYRO_INT 2

#define NRF_CE 9
#define NRF_SCN 10
#define NRF_SCK 13
#define NRF_MOSI 11
#define NRF_MISO 12

RF24 sender(NRF_CE, NRF_SCN);
byte send_address[6] = "sender";

MPU6050 mpu;

int16_t ax, ay, az;
int16_t gx, gy, gz;
int msg;

struct Packet {
  uint8_t id;
  uint32_t data;
  uint32_t failTx_C;
};
Packet _packetData;

void setup() {
  const char init_msg[] = "Connected start listening.";

  // rf init
  sender.begin();
  sender.openWritingPipe(send_address);
  sender.stopListening();
  sender.write(&init_msg, sizeof(init_msg));
  
  // mpu init
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #endif
  Serial.begin(38400);
  mpu.initialize();
  if(!mpu.testConnection()) {
    Serial.println("MPU init failed, check connection.");
    while(1);
  }
  Serial.println("Start calibrating accel, wait 20s");
  mpu.CalibrateAccel();
  Serial.println("Start calibrating gyro, wait 20s");
  mpu.CalibrateGyro();
  Serial.println("MPU init done, reset gyro.");
  resetGyro();
}

void resetGyro() {
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);
}

void loop() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  parseAccelInfo(ax, ay, az);
}

void parseAccelInfo(int16_t ax, int16_t ay, int16_t az) {
  while(ax <= -8000) {
    msg=1;
    _packetData.data=msg;
    Serial.println("foward");
    sendMotorAccelInformation(msg);
  }
  while(ax >= -8000) {
    msg=2;
    _packetData.data=msg;
    Serial.println("backward");
    sendMotorAccelInformation(msg);
  }
  while(ay <= -8000) {
    msg=3;
    _packetData.data=msg;
    Serial.println("left");
    sendMotorAccelInformation(msg);
  }
}

void sendMotorAccelInformation(int32_t accel) {
  if(sender.write(&_packetData, sizeof(_packetData))) {
    Serial.println("data sent, data: " + msg);
  } else {
    Serial.println("failed");
    _packetData.failTx_C++;
  }
}
