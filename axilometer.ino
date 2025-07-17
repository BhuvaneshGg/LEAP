#include <Wire.h>
 
int ADXL345=0x53;
String x,y,z; 

void setup() 
{
  Serial.begin(9600);             
  Serial.println("Iniciar");
  Serial.println();
Wire.begin() ;
 
}

void loop() 
{
  //leer los valores e imprimirlos
 Wire.beginTransmission(ADXL345);
 Wire.write(0x32);
 Wire.endTransmission(false);
 Wire.requestFrom(ADXL345, 6 ,true);
 x =getWireRead(); 
  y =getWireRead(); 
   z =getWireRead(); 

   Serial.println(" X : " + x + "g   Y : " + y + "g         Z : " + z + " g    " );
   delay(100);
}     

             
float   getWireRead(){

float value = (  Wire.read () | Wire.read() <<8 );  
return value/256;
}