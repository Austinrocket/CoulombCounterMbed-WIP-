#include "mbed.h"

DigitalOut led1(LED1);
Serial pc(USBTX, USBRX);
//      SDL  SCA
I2C i2c(p28, p27);

//I2C scanning algorithm
void Checki2c() {
    char buf[10]; //initialize character buffer
    int c = 0; //initialize address location
    buf[0] = 0;
    for (int adrs = 0; adrs < 0x80; adrs += 16) {
        for (int i = 0; i < 16; i++) {
            //loop through hardware address space and scan for an I2C device
            i2c.write(((adrs + i) << 1), buf, 1);
            int ret = i2c.read(((adrs + i) << 1), buf, 2);
            //the I2C we have located the device if it returns a 0
            if (ret == 0) {
                c = ((adrs + i) << 1);
            }
        }
    }
pc.printf("i2c DEVICE LOCATED\n");
pc.printf("Address is: %02x \n",c);
}

int main() {
    pc.printf("\nSerial Connection: ESTABLISHED\n");
    pc.printf("Checking i2c bus...\n");
    Checki2c();
    pc.printf("Idle...");
    
    //testing reading from the serial input
    char c;
    char buffer[128];
    pc.gets(buffer, 4);
    pc.printf("I got '%s'\n", buffer);    
}
