#include <FIMU_ITG3200.h>

#define ADXL345_ADDR_ALT_LOW  0x53
#define FIMU_ACC_ADDR ADXL345_ADDR_ALT_LOW
#define ADXL345_POWER_CTL 0x2d
#define ADXL345_DATAX0 0x32
#define TO_READ (6)

#include <Wire.h>

#define FIMU_ITG3200_DEF_ADDR ITG3200_ADDR_AD0_LOW

#define TIMESLICE_PERIOD 50 //milliseconds

int accelx;
int accely;
int accelz;
ITG3200 gyro;

void writeto( byte device_address, byte address, byte val ){
  Wire.beginTransmission(device_address); // start transmission to device
  Wire.write(address);                    // send register address
  Wire.write(val);                        // send value to write
  Wire.endTransmission();                 // end transmission
}

void imuinit(){
  delay(5);
  
  //power on accelerometer
  writeto(FIMU_ACC_ADDR, ADXL345_POWER_CTL, 8);
}

void readfrom(int device_address, byte address, int num, byte _buff[]){
  Wire.beginTransmission(device_address); // start transmission to device
  Wire.write(address);                    // sends address to read from
  Wire.endTransmission();                 // end transmission

  Wire.beginTransmission(device_address);  // start transmission to device
  Wire.requestFrom(device_address, num);   // request 6 bytes from device

  int i = 0;
  while(Wire.available())         // device may send less than requested (abnormal)
  {
    _buff[i] = Wire.read();       // receive a byte
    i++;
  }

  Wire.endTransmission();         // end transmission
}

void readaccel(int *x, int *y, int *z){
  byte _buff[6] ;
  
  readfrom(FIMU_ACC_ADDR, ADXL345_DATAX0, TO_READ, _buff); //read the acceleration data from the ADXL345

  // each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
  // thus we are converting both bytes in to one int
  *x = (((int)_buff[1]) << 8) | _buff[0];  
  *y = (((int)_buff[3]) << 8) | _buff[2];
  *z = (((int)_buff[5]) << 8) | _buff[4];
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  delay(5);
  imuinit();
  gyro.init(FIMU_ITG3200_DEF_ADDR);
  delay(5);
}

void loop() {
  int gyrox;
  int gyroy;
  int gyroz;
  readaccel(&accelx,&accely,&accelz);
  gyro.readGyroRaw(&gyrox,&gyroy,&gyroz);
  
  Serial.print( accelx );
  Serial.print(",");
  Serial.print( accely );
  Serial.print(",");
  Serial.print( accelz );
  Serial.print(",");
  Serial.print( gyrox );
  Serial.print(",");
  Serial.print( gyroy );
  Serial.print(",");
  Serial.print( gyroz );
  Serial.print(",");
  Serial.print( millis() );
  Serial.print("\n");
 
  delay(TIMESLICE_PERIOD);
}
