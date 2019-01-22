#include "mbed.h"
//#include "INA219.cpp"

DigitalOut led1(LED1);
//      SDL  SCA
I2C i2c(p28, p27);
Serial pc(USBTX, USBRX);
AnalogIn vltg(p20);
float voltage;

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

float variance (float average, float data[]){
//Variance
//[subtract avg from each individual value]
//[square each value]
//[sum the squared values]
//[divide by (number of values - 1)]
//[done]
    float var = 0;
    int size = sizeof(data);
    for(int i=0; i<=size; i++){
        var += (data[i]-average)*(data[i]-average);
        }
    var = var/(size-1);
    return var;
}

float average (float data[]){
    float avg = 0;
    int size = sizeof(data);
    for(int i=0; i <= size; i++){
        avg += data[i];
        }
    avg = avg/(size);
    return avg;
    }

float readvoltage (int durr){
    //returns array of floats for average and variance
    float duration[durr];
    float f = 0;
    for(int i=0; i <= durr; i++){
        wait(.1); //wait is in seconds
        f = vltg.read();
        f = f*(5.0/ 4096.0);
        duration[i] = f;
        }
    return duration[]; //yeah I know I can't do this
}

int main() {
    pc.printf("\nSerial Connection: ESTABLISHED\n");
    pc.printf("Checking i2c bus...\n");
    Checki2c();
    pc.printf("Idle...\n");
    
    //testing reading from the serial input
while(1){
    //char c;
    char buffer[256];
    pc.gets(buffer, 5);
    pc.printf("I got '%s'\n", buffer);   
    
    if(buffer == "read-input-current\0"){
        pc.printf("\nEnter the amount of time (in milliseconds) you'd like to measure CURRENT for: \n");
        pc.gets(buffer, 5);
        pc.printf("Reading CURRENT for %s milliseconds... \n", buffer);
        //TODO readcurrent
        } 
    if(buffer == "read-output-voltage\0"){
        pc.printf("\nEnter the amount of time (in milliseconds) you'd like to measure VOLTAGE for: \n");
        pc.gets(buffer, 5);
        pc.printf("Reading VOLTAGE for %s milliseconds... \n", buffer);
        //TODO readvoltage
           
        } 
    if(buffer == "help\0"){
        pc.printf("Enter either 'read-input-current <ms>' or 'read-output-voltage <ms>' ");   
        } 
    }
}
