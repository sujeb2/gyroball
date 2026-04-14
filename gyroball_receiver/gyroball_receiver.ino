#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>
#include <SPI.h>
#include <L298NX2.h>

#define DRIVER_ANALOG_IN1_A A0 // gyro analog inut
#define DRIVER_ANALOG_IN2_A A1
#define DRIVER_ANALOG_IN1_B A2
#define DRIVER_ANALOG_IN2_B A3

#define NRF_CE 9 // rf pin init
#define NRF_SCN 10
#define NRF_SCK 13
#define NRF_MOSI 11
#define NRF_MISO 12

// receiver init
int msg = 0;
RF24 receiver(NRF_CE, NRF_SCN);
byte read_address[6] = "sender";

struct Packet {
  uint8_t id;
  uint32_t data;
  uint32_t failTx_C;
};
Packet _packetData;

// moter init
L298NX2 driver(DRIVER_ANALOG_IN1_A, DRIVER_ANALOG_IN2_A, DRIVER_ANALOG_IN1_B, DRIVER_ANALOG_IN2_B);

void setup() {
  Serial.begin(9600);
  if(receiver.begin()) {
    receiver.openReadingPipe(1, read_address);
    receiver.startListening();
    Serial.println("init done.");
  }
}

void loop() {
 if(!receiver.available()) return;
  receiver.read(&msg, sizeof(msg));
  Serial.println("RX msg: "+msg);
}
