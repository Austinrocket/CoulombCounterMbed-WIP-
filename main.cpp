#include "mbed.h"
#include "INA219.h"

DigitalOut led1(LED1);
//      SDL  SCA
I2C i2c(p28, p27);//new I2C object called... i2c
INA219 INA(p28, p27, 0x80);//new INA219 object called... INA
Serial pc(USBTX, USBRX);
AnalogIn vltg(p20);//new analog object mapped to analog pin 20

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
INA219 INA(p28, p27, c); // set new I2C address if current address isn't 80
}

////Variance formula
////[subtract avg from each individual value]
////[square each value]
////[sum the squared values]
////[divide by (number of values - 1)]
////[done]


void readvoltage (int durr){
//input how many milliseconds to sample
//output average voltage and voltage variance over all the samples

    float duration[durr];
    float f = 0;
    float avg = 0;
    float var = 0;
    
    for(int i=0; i < durr; i++){
        wait(.001); //wait is in seconds and we need ms
        f = vltg.read();//read the voltage on the analog pin
        f = (f*3.3);//mbed runs on 3.3 volts so the analogIn pins need to be multiplied by that amount since they measure a precentage in terms of system voltage
        duration[i] = f;//put into array
        avg += duration[i];//sum
        //pc.printf("%f[V] ",duration[i]); //uncomment for all voltage readings
        }
        avg = avg/(durr);//take average
        pc.printf("\nAVG: %f \n",avg);
    
    //take variance [see formula]
    for(int j=0; j < durr; j++){
        var += (duration[j]-avg)*(duration[j]-avg);
        }
    var = var/(durr-1);
    pc.printf("VAR: %f \n\n",var);
}

void readcurrent (int durr){
//input how many milliseconds to sample
//output average current and current variance over all the samples
    float duration[durr];
    float f = 0;
    float avg = 0;
    float var = 0;
    
    for(int i=0; i < durr; i++){
        wait(.001); //wait is in seconds and we need ms
        f = INA.read_current();//take a reading from the INA219
        duration[i] = (f*.01); //should be .2 amps, the current sensor was showing 20 so I multiply by .01
        avg += duration[i];//sum
        //pc.printf("%f[A] ",duration[i]); //uncomment for all current readings
        }
        avg = avg/(durr);//take average
        pc.printf("\nAVG: %f \n",avg);
    
    //see variance formula
    for(int j=0; j < durr; j++){
        var += (duration[j]-avg)*(duration[j]-avg);
        }
    var = var/(durr-1);
    pc.printf("VAR: %f \n\n",var);
}

int main() {
    pc.printf("\nSerial Connection: ESTABLISHED\n");
    pc.printf("Checking i2c bus...\n");
    Checki2c();
    pc.printf("Idle... Enter Command\n");

while(1){
    //read serial input
    //not the most robust, but I catered to the the required input
    char command[22]; //reads up to 21 (+ null character) characters before throwing an error
    
    while(1) {
        int num = 0;
        bool status = 1; //so only one if statement will run
        if (pc.readable()) {
            pc.gets(command, 22); //read-input-current(18) <ms>(20-21) | read-output-voltage(19) <ms>(21-22)
//            for (int i=0; i<22; i++) { //uncomment to read ascii of characters read in
//                pc.printf("%d ", command[i]);
//            }
//            pc.printf("\n");
            
    if(status){
        if(command[0] == 'h' || command[0] == 'H'){//looks for h in "help"
            status = 0;
            pc.printf("Commands are:\n   read-input-current <ms>\n   read-output-voltage <ms>\n");
            }
        }
        
    if(status){
        if(command[5] == 'i' || command[5] == 'I'){//looks for i in "input"
            status = 0;
            //pc.printf("read-input-current \n"); //debug parsing
            num = (command[19])-48;//ascii number must be subtracted down to real int number
            //pc.printf("num = %d \n", num); //debug parsing
            readcurrent(num);
            }
        }
        
    if(status){
        if(command[5] == 'o' || command[5] == 'O'){//looks for o in "output"
            status = 0;
            //pc.printf("read-output-voltage \n"); //debug parsing
            num = (command[20])-48;//ascii number must be subtracted down to real int number
            //pc.printf("num = %d \n", num); //debug parsing
            readvoltage(num);
            }
        }
    if(status){
        pc.printf("INCORRECT COMMAND (type help)");
        }
    }
}
}
}
