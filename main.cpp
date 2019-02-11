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

void variance (int durr, float *duration, float avg){
    
////Variance formula
////[subtract avg from each individual value]
////[square each value]
////[sum the squared values]
////[divide by (number of values - 1)]
////[done]

    float var = 0;
    for(int j=0; j < durr; j++){
        var += (duration[j]-avg)*(duration[j]-avg);
//        pc.printf("%f \n",avg);
//        pc.printf("%f \n",duration[j]);
//        pc.printf("VAR: %f %d\n",var,j);
        }
    var = var/(durr-1);
    pc.printf("VAR: %f \n\n",var);
    }

void readvoltage (int durr){
//input how many milliseconds to sample
//output average voltage and voltage variance over all the samples

    float duration[durr];
    float f = 0;
    float avg = 0;
    
    for(int i=0; i < durr; i++){
        wait(.001); //wait is in seconds and we need ms
        f = vltg.read();//read the voltage on the analog pin
        f = (f*3.3);//mbed runs on 3.3 volts so the analogIn pins need to be multiplied by that amount since they measure a precentage in terms of system voltage
        duration[i] = f;//put into array
        avg += duration[i];//sum
        //pc.printf("%f[V] ",duration[i]); //uncomment for all voltage readings
        }
        avg = avg/(durr);//take average
        pc.printf("Returning current average and variance in Amps \n");
        pc.printf("AVG: %f \n",avg);
    variance(durr, duration, avg);
}

void readcurrent (int durr){
//input how many milliseconds to sample
//output average current and current variance over all the samples
    float duration[durr];
    float f = 0;
    float avg = 0;
    
    for(int i=0; i < durr; i++){
        wait(.001); //wait is in seconds and we need ms
        f = INA.read_current();//take a reading from the INA219
        duration[i] = (f*.01); //should be .2 amps, the current sensor was showing 20 so I multiply by .01
        avg += duration[i];//sum
        //pc.printf("%f[mA] ",duration[i]); //uncomment for all current readings
        }
        avg = avg/(durr);//take average
        pc.printf("Returning current average and variance in Amps \n");
        pc.printf("AVG: %f \n",avg);
    variance(durr, duration, avg);
}

int combine(int a, int b) {
   //this function concatenates integers so instead of typing 99 and getting 18, you get 99
   int times = 1;
   while (times <= b)
      times *= 10;
   return a*times + b;
} 

int main() {
    pc.printf("\nSerial Connection: ESTABLISHED\n");
    pc.printf("Checking i2c bus...\n");
    Checki2c();
    pc.printf("Idle... Enter Command\n");
    int num = 0;
    
while(1){
        char command[100]; //reads up to 101 (+ null character) characters before throwing an error
    
    while(1) {

        if (pc.readable()) {
            pc.gets(command, 100); //read in the command
        
        if(strncmp(command, "read-input-current", 18) == 0) {
            //pc.printf("current read\n"); //debug
            //pc.printf(command);
            num = command[19]-48; //get the actual number from the char
            int i = 0;
            while (command[20+i]-48 >= 0 && command[20+i]-48 <= 9){
                //continue getting numbers from the char array until you run out of numbers
                num = combine(num, command[20+i]-48);
                i++;
                }
            readcurrent(num);
            //pc.printf("%d \n",num); //check the correct number was parsed
            }
        else if(strncmp(command, "read-output-voltage", 19) == 0) {
            //pc.printf("voltage read\n"); //debug
            //pc.printf(command);
            num = command[20]-48; //get the actual number from the char
            int i = 0;
            while (command[21+i]-48 >= 0 && command[21+i]-48 <= 9){
                //continue getting numbers from the char array until you run out of numbers
                num = combine(num, command[21+i]-48);
                i++;
                }
            readvoltage(num);
            //pc.printf("%d \n",num); //check the correct number was parsed
            }
        else if(strncmp(command, "help", 4) == 0) {
            pc.printf("Commands are:\n   read-input-current <ms>\n   read-output-voltage <ms>\n");
            //pc.printf(command);
            }
        else{
            pc.printf("INCORRECT COMMAND (type help)\n");
            //pc.printf(command);
            }
}
}
}
}
