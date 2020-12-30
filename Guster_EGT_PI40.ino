#include <SoftwareSerial.h>

SoftwareSerial sSerial(10, 11); // RX, TX

/*
Wait software serial buffer available and read/return one byte.
*/
int readSoftSerialByte() {
  while (!sSerial.available());
  return sSerial.read();
}

/*
Prints the data of one EGT channel (input/cylinder)
*/
void printChannel() {

  int dataByte1 = readSoftSerialByte();
  int dataByte2 = readSoftSerialByte();

  // |7|6|5|4|3|2|1|0|
  //
  // dataByte1[2-3] (0xC) is the channel number

  int read_channel = (dataByte1 & 0xC) >> 2;

  Serial.print("(channel ");
  Serial.print(read_channel);
  Serial.print("): ");

  // |7|6|5|4|3|2|1|0|
  //
  // dataByte2 uses only 7 bits [0-6]
  // dataByte2 is the less significant part
  // dataByte1[6] (0x40) is the 8th bit
  // dataByte1[0] (0x1) is the 9th bit
  // dataByte1[1] (0x2) is the 10th bit

  Serial.print("Raw Byte 1 [");
  Serial.print(dataByte1, DEC);
  Serial.print("] Raw Byte 2 [");
  Serial.print(dataByte2, DEC);
  Serial.print("] ");

  int temperature = (((dataByte1 & 0x40) >> 6) | // 6th dataByte1 bit goto 1st
                     ((dataByte1 & 0x1) << 1) |  // 1st dataByte1 bit goto 2nd
                     ((dataByte1 & 0x2) << 1)    // 2nd dataByte1 bit goto 3rd
                     ) << 7 |                    // align to dataByte2
                    dataByte2;

  Serial.print(temperature);

  //TODO: need map?
  //Serial.print(" ");
  //temperature =  map(temperature, 268, 360, 100, 400);
  //Serial.print(temperature);

  Serial.println();
}

void setup() {

  // open serial communication (serial to debug or PC communication)
  Serial.begin(57600);

  // open SoftwareSerial communication (serial to communicate with Guster equipment)
  // 9600 is the equipment data rate
  sSerial.begin(9600);

  // wait equipment sync byte
  while (readSoftSerialByte() != 255);
}

void loop() {

  printChannel();

  Serial.println();
}
