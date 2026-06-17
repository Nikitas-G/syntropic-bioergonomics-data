/*
  Connect an MPU6050 module (e.g. GU 521) to Arduino (e.g. Arduino Uno) and read angles - No library is used
  Connect an one-axis or two-axis flex sensor - SparkFun_Displacement_Sensor_Arduino_Library
  https://github.com/sparkfun/SparkFun_Displacement_Sensor_Arduino_Library 

  
  Ver 1.0
  Makis Chatzopoulos
  Based on https://www.youtube.com/watch?v=yhz3bRQLvBY&list=PLeuMA6tJBPKsAfRfFuGrEljpBow5hPVD4&index=6   and https://www.youtube.com/watch?v=7VW_XVbtu9k
  Credits:  Carbon Aeronautics

  Hardware:
  -----------------------------------
  Arduino Pin ->  MPU6050 module
  -----------------------------------
  +5V             Vcc   Orange
  GND             GND   Black
  A4              SCA   White
  A5              SCL   Blue - Cyan
  -----------------------------------

*/

#include <Wire.h>                                         // Include the I2C (Wire library) library to send and read data to I2C devices
#include "SparkFun_Displacement_Sensor_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_Displacement_Sensor

float RateRoll, RatePitch, RateYaw;       // Define RateRoll, RatePitch, RateYaw as floating numbers
float AccX, AccY, AccZ;                   // Define AccX, AccY, AccZ as floating numbers
float AngleRoll, AnglePitch, AngleYaw;    // Define AngleRoll, AnglePitch, AngleYaw as floating numbers

float AngleRollError;
float AnglePitchError; 
float AngleYawError;                      // Define 

float AngleRoll_new = 0;
float AnglePitch_new  = 0;
float AngleYaw_new = 0; 

// Insert the X,Y,Z, accelerometers errors
// Substract from 1g the value of each axis
// E.g. if Z value is 1.12 then AccZerror should be 1 - 1.12 = -0.12
// Rotate the MPU6050 sensor to X axis thus you must read a X accelerometer value equal to 1 (but you read +1.01)
// Rotate the MPU6050 sensor to Y axis thus you must read a Y accelerometer value equal to 1 (but you read -1.01)
// Before calibration set the following to 0. After calibration comment these lines

float AccXerror = -0.08;                   // Set the AccX error based on your measuremts
float AccYerror = 0.01;                    // Set the AccY error based on your measuremts
float AccZerror = -0.14;

ADS myFlexSensor; //Create instance of the Angular Displacement Sensor (ADS) class
//long timer = 0;
float pre_timi = 0;       //
float timi = 0;
float flex_error_threshold = 10.0;
int flex_error_count = 0;

// After calibration set the values to AccXerror, AccYerror, AccZerror according to your measuremnts and uncomment the following lines
// float AccXerror = 0.01;                   // Set the AccX error based on your measuremts
// float AccYerror = 0.01;                    // Set the AccY error based on your measuremts
// float AccZerror = 0.12;                   // Set the AccZ error based on your measuremts

// MPU6050
const int MPU = 0x68;                     // I2C address of the MPU-6050. If you want you can change by setting ADO pin to HIGH or LOW

// Variables to store raw data from MPU6050

void setup() {
  pinMode(12, INPUT);   // Connect pin12 to a button
  Serial.begin(9600); // Initialize serial port for data output
  Serial.println("Start program");

  Serial.println("Wire ....");
  Wire.setClock(400000); // The MPU supports I2C communications at up to 40KHz
  Wire.begin(); // Initialize I2C bus
  delay(250); // Give MPU time to start;

  Serial.println("Check flex sensor");
  // Check the flex sensor
  if (myFlexSensor.begin() == false)
  {
    Serial.println(F("No sensor detected. Check wiring. Freezing..."));
    while (1)
      ;
  }
  delay(250); // Give Flex sensor to start;

  // Start the Gyro in power mode
  // See datasheet - 4.28 Register 107 - Power Management
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);    // PWR_MGMT_1 register
  Wire.write(0x00);    // Set to zero (wakes up the MPU-6050)
  Wire.endTransmission(); // Terminate the connection with MPU
  
  /*
  Serial.println("Start reading MPU Sensor to find errors");
  while(!digitalRead(12)) {
    gyro_signals();
    Serial.print("\t X[g] = ");   Serial.print(AccX);
    Serial.print("\t Y[g] = ");   Serial.print(AccY);
    Serial.print("\t Z[g] = ");   Serial.print(AccZ);
    Serial.print("\t X° = ");  Serial.print(AngleRoll);
    Serial.print("\t Y° = ");  Serial.print(AnglePitch);
    Serial.print("\t Z° = ");  Serial.print(AngleYaw);
    Serial.println();
  }
  
  AngleRollError = AngleRoll;     // Calculate errors
  AnglePitchError = AnglePitch;  
  AngleYawError = AngleYaw;

  
  // Display calculated Angle Errors
  Serial.println("Calculated Angle Errors");
  Serial.print("\t AngleRollError = ");
  Serial.print(AngleRollError);
  Serial.print("\t AnglePitchError = ");
  Serial.print(AnglePitchError);
  Serial.print("\t AngleYawError = ");
  Serial.print(AngleYawError);
  Serial.println();
  // Read the first initial value pre_timi 
  Wire.begin();

  Serial.println("Ready to log measurements");
  Serial.println("You have to disconnect from Arduino Serial Monitor and Connect to Excel");
  Serial.println("Press the button when you are to log");
  delay(1000);
  while(!digitalRead(12)) {
     Serial.print(".");
     delay(100);
  }

  */
  AngleRollError = 0;
  AnglePitchError = 0;  
  AngleYawError = 0;
  
}

void loop() {
  if(myFlexSensor.available() == true) {
    // Read current value
     timi = myFlexSensor.getX();
    } // end Flex readings
  gyro_signals();
  Serial.print(timi);   // Flex sensor
  Serial.print(",");
  Serial.print(AccX);   // X Accelaration                 Serial.print("\t X[g] = ");
  Serial.print(",");
  Serial.print(AccY);   // Y Accelaration                 Serial.print("\t Y[g] = ");
  Serial.print(",");
  Serial.print(AngleRoll);   // X° - Roll                 Serial.print("\t X° = "); 
  Serial.print(",");
  Serial.print(AnglePitch); // Y° - Pitch                 Serial.print("\t Y° = ");    // Pitch
  Serial.print(",");
  Serial.print(AngleRoll_new);  // corrected X° - Roll    Serial.print("\t Xn° = ");    // Roll
  Serial.print(",");
  Serial.print(AnglePitch_new); // corrected Y° - Pitch   Serial.print("\t Yn° = ");    // Pitch 
  Serial.println();				// Change a line into Serial monitor
  delay(1);
}


void gyro_signals() {
  // MPU6050 Initialization
  // Send command to wake up MPU-6050
  // See datasheet - 4.34 Register 117 - Who Am I
  Wire.beginTransmission(MPU);          //  I2C address of the MPU-6050 is set to 0x68. (look at  MPU = 0x68;)
  // See datasheet - 4.3 Register 26 - Configuration
  // Switch on the low-pass filter

  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();

  // Configure the accelerometer output
  // See datasheet - 4.5 Register 28 - Accelerometer Configuration
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);                   // The accelerometer configuration settings is set at the register (hex) 1C
  Wire.write(0x10);                   // Choose a full scale range of +/-8g (AFS_SEL value equals to Binary 10, or Decimal 2) starting at the Bit3 of the register
  Wire.endTransmission();

  // Pull the accelerometer measuremts from the sensor
  // See datasheet - 4.18 Register 59 to 64 - Accelerometers Measurements
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);                               // Trigger, indicate the first Register 3B for Accelerometer measurments
  Wire.endTransmission();

  // See datasheet - 4.18 Register 59 to 64 - Accelerometers Measurements - Registers 3B to 40(Hex)
  Wire.requestFrom(MPU,6);                        // Request 6 bytes from the MPU
  int16_t AccXLSB = Wire.read()<<8 | Wire.read(); // The accelerometer measurements are spread out over 3 times two registers with each 8 bits
  int16_t AccYLSB = Wire.read()<<8 | Wire.read();
  int16_t AccZLSB = Wire.read()<<8 | Wire.read();

  // Set the sensitivity scale factor
  // See datasheet - 4.4 Register 27 - Gyroscope Configuration
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);
  Wire.write(0x8);
  Wire.endTransmission();

  // Access registers storing gyro measurements
  // See datasheet - 4.20 Register 67 to 72 - Gyroscope Measurements
  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission();

  // Read the Gyro measurements around the X axis
  // See datasheet - 4.20 Register 67 to 72 - Gyroscope Measurements
  Wire.requestFrom(MPU,6);                      // Request 6 bytes from the MPU
  int16_t GyroX=Wire.read()<<8 | Wire.read();   // The gyro measurements are spread out over 3 times two registers with each 8 bits
  int16_t GyroY=Wire.read()<<8 | Wire.read();
  int16_t GyroZ=Wire.read()<<8 | Wire.read();

  // Convert the Gyro measurements units to degrees/sec
  RateRoll = float(GyroX)/65.5;
  RatePitch = float(GyroY)/65.5;
  RateYaw = float(GyroZ)/65.5;

  // Convert the Accelerometer measurements units to physical values
  // Remember we configured AFS_SEL value to 2 meaning a full scale range +/-8g with the LSB Sensitiviy set at 4096 LSB/g
  AccX=float(AccXLSB)/4096 - AccXerror;    
  AccY=float(AccYLSB)/4096 - AccYerror;
  AccZ=float(AccZLSB)/4096 - AccZerror;

  // Calculate the Roll and Pitch Angle
  AngleRoll = atan(AccY/sqrt(AccX*AccX+AccZ*AccZ))*1/(3.142/180);   // atan() returns a value in radians not in degrees, so we divide this by 180
  AnglePitch = atan(-AccX/sqrt(AccY*AccY+AccZ*AccZ))*1/(3.142/180);
  AngleYaw = atan(AccX/sqrt(AccY*AccY+AccZ*AccZ))*1/(3.142/180);
  AngleRoll_new   = AngleRoll - AngleRollError;
  AnglePitch_new  = AnglePitch - AnglePitchError;
  AngleYaw_new    = AngleYaw - AngleYawError;
} // end gyro_signals()


