#include <SoftwareSerial.h>
#include <Wire.h>  // Wire library - used for I2C communication
SoftwareSerial TX(5, 6);
bool calibrate = false;
int little  = 0, Cal_little  = 720;
int ring    = 0, Cal_ring    = 720;
int middle  = 0, Cal_middle  = 720;
int index   = 0, Cal_index   = 720;
int thumb   = 0, Cal_thumb   = 720;

int button = 11;
int mgnd = 9;
int mled = 8;

String temp = "";


float X_out, Y_out, Z_out;  // Outputs

byte FLEX_PIN1 = A6; // little finger
byte FLEX_PIN2 = A3; // ring finger
byte FLEX_PIN3 = A2; // middle finger
byte FLEX_PIN4 = A1; // index finger
byte FLEX_PIN5 = A0; // thumb



void printfun(String cp) //to avoid printing repeating symbols
{
  if(cp!=temp)
  {
    digitalWrite(mled,HIGH);
    TX.print("AT+");
//    delay(200);
    TX.println(cp);
    Serial.println("*********************");
    Serial.println(cp);
    Serial.println("*********************");
    temp=cp;
    delay(400);
   }
}

void setup()
{
  //MString.reserve(70);
  Serial.begin(9600);
  TX.begin(9600);
  pinMode(button, INPUT);
  pinMode(mgnd, OUTPUT);
  digitalWrite(mgnd,LOW);
  pinMode(mled, OUTPUT);
  digitalWrite(mled,HIGH);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(0x53); // Start communicating with the device
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable
  Wire.endTransmission();
  delay(10);
  if (calibrate == false)
  {
    printfun("calibrateing");
    for (int j = 0; j < 300; j++)
    {
      delay(12);
      little = analogRead(FLEX_PIN1) - Cal_little;
      if (little > 1)Cal_little++; if (little < 0)Cal_little--;
      ring = analogRead(FLEX_PIN2) - Cal_ring;
      if (ring > 1)Cal_ring++; if (ring < 0)Cal_ring--;
      middle = analogRead(FLEX_PIN3) - Cal_middle;
      if (middle > 1)Cal_middle++; if (middle < 0)Cal_middle--;
      index = analogRead(FLEX_PIN4) - Cal_index;
      if (index > 1)Cal_index++; if (index < 0)Cal_index--;
      thumb = analogRead(FLEX_PIN5) - Cal_thumb;
      if (thumb > 1)Cal_thumb++; if (thumb < 0)Cal_thumb--;

      Serial.print("calibrateing      Thumb");
      Serial.print(thumb);
      Serial.print("  Index ");
      Serial.print(index);
      Serial.print("  Middle ");
      Serial.print(middle);
      Serial.print("  Ring ");
      Serial.print(ring);
      Serial.print("  Little ");
      Serial.println(little);
    }
    calibrate = true;
  }
  digitalWrite(mled,LOW);
}
void loop()
{
  Wire.beginTransmission(0x53);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(0x53, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read() | Wire.read() << 8); // X-axis value
  // X_out = X_out/256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read() | Wire.read() << 8); // Y-axis value
  //Y_out = Y_out/256;
  Z_out = ( Wire.read() | Wire.read() << 8); // Z-axis value


  little = analogRead(FLEX_PIN1) - Cal_little;
  ring = analogRead(FLEX_PIN2) - Cal_ring;
  middle = analogRead(FLEX_PIN3) - Cal_middle;
  index = analogRead(FLEX_PIN4) - Cal_index;
  thumb = analogRead(FLEX_PIN5) - Cal_thumb;

  // reading sensor value
  if (calibrate)
  {
    if(little<0)little=0;
    if(ring<0)ring=0;
    if(middle<0)middle=0;
    if(index<0)index=0;
    if(thumb<0)thumb=0;
    
    little  = little * 3.0;
    ring    = ring * 3.0;
    middle  = middle * 3.0;
    index   = index * 3.0;
    thumb   = thumb * 3.0;
  }


  

  int xadc = (X_out + 255) * 2;
  int yadc = (Y_out + 255) * 2;
  if (xadc < 0)xadc = 0;
  if (xadc > 1023)xadc = 1023;
  if (yadc < 0)yadc = 0;
  if (yadc > 1023)yadc = 1023;

if (index >= 13 && thumb >= 16 && middle >= 13 && ring >= 12 && little >= 12 && xadc>=300 && xadc<=550 && yadc>=923)
  printfun("hello");

if (index <= 13 && thumb <= 12 && middle <= 13 && ring >= 12 && little >= 12 && xadc>=300 && xadc<=550 && yadc>=923)
  printfun("I will kill u");

if (index >= 13 && thumb <= 12 && middle >= 13 && ring >= 12 && little >= 12 && xadc>=300 && xadc<=550 && yadc>=923)
  printfun("Understood");

  
if (index <= 13 && thumb >= 12 && middle >= 13 && ring >= 12 && little >= 12 && xadc>=300 && xadc<=550 && yadc>=923)
  printfun("please go and check the door");

if (index >= 15 && thumb <= 18 && middle >= 13 && ring >= 12 && little <= 12 && xadc>=300 && xadc<=550 && yadc>=923 )
  printfun("take my number");

if (index >= 15 && thumb >= 15 && middle >= 15 && ring >= 15 && little >= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
  printfun("I support peace");


  
if (index >= 13 && thumb >= 13 && middle <= 15 && ring <= 12 && little <= 12 && xadc >= 0 && xadc <=100 && yadc >= 220 && yadc <= 620)
  printfun("Amazing");

if (index <= 13 && thumb >= 13 && middle >= 13 && ring <= 13 && little <= 12 && xadc >= 0 && xadc <=100 && yadc >= 220 && yadc <= 620)
  printfun("bird");

if (index <= 15 && thumb <= 15 && middle <= 15 && ring <= 15 && little <= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
  printfun("hi");


if (index>= 15 && thumb <= 15 && middle <= 15 && ring >= 15 && little <= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
  printfun("Good quality");



if (index >= 15 && thumb <= 15 && middle <= 15 && ring <= 15 && little >= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
  printfun("it was a good movie");

if (index <= 15 && thumb <= 15 && middle >= 15 && ring <= 15 && little >= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
  printfun("vegetables");

if (index <= 15 && thumb <= 15 && middle <= 15 && ring >= 15 && little <= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
  printfun("I was thinking about you");


if (index <= 15 && thumb >= 15 && middle <= 15 && ring >= 15 && little <= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
  printfun("I am leaving");

if (index <= 15 && thumb >= 15 && middle <= 15 && ring <= 15 && little >= 15 && xadc >= 0 && xadc <=120 && yadc >= 220 && yadc <= 620)
  printfun("Lets eat");



if (index <= 13 && thumb <= 13 && middle >= 13 && ring >= 13 && little >= 12 && xadc >= 0 && xadc <=100 && yadc >= 220 && yadc <= 620)
  printfun("click a picture");

if (index >= 13 && thumb <= 13 && middle >= 13 && ring >= 13 && little >= 12 && xadc >= 0 && xadc <=100 && yadc >= 220 && yadc <= 620)
  printfun("water please");




if (index <= 13 && thumb >= 13 && middle <= 13 && ring <= 13 && little <= 12 && xadc >= 0 && xadc <=100 && yadc >= 220 && yadc <= 620)
  printfun("want to sleep");

if (index <= 15 && thumb <= 15 && middle <= 15 && ring <= 15 && little <= 15 && xadc >=650 && xadc <=1000 && yadc >=350 && yadc <=850)
  printfun("no more work");

if (index <= 13 && thumb >= 13 && middle <= 13 && ring >= 13 && little >= 12 && xadc >= 0 && xadc <=100 && yadc >= 220 && yadc <= 620)
  printfun("nice to meet you");

if (index <= 15 && thumb >= 15 && middle >= 15 && ring >= 15 && little >= 15 && xadc >=650 && xadc <=1000 && yadc >=350 && yadc <=850)
  printfun("down");

if (index <= 15 && thumb <= 15 && middle >= 15 && ring >= 15 && little <= 15 && xadc >=650 && xadc <=1000 && yadc >=350 && yadc <=550)
  printfun("no noice");


if (index <= 15 && thumb <= 15 && middle <= 15 && ring >= 15 && little >= 15 && xadc >=650 && xadc <=1000 && yadc >=350 && yadc <=850)
  printfun("talk later");



if (index >= 15 && thumb >= 15 && middle >= 15 && ring >= 15 && little >= 15 && xadc >=480 && xadc <=750 && yadc >=0 && yadc <=100)
  printfun("pause please");


if (index >= 15 && thumb >= 15 && middle >= 15 && ring >= 15 && little <= 15 && xadc >=480 && xadc <=750 && yadc >=0 && yadc <=100)
  printfun("I am out of this");

if (index >= 15 && thumb <= 15 && middle >= 15 && ring >= 15 && little >= 15 && xadc >=480 && xadc <=750 && yadc >=0 && yadc <=100)
  printfun("not ok");







if (index <= 13 && thumb <= 13 && middle >= 13 && ring >= 13 && little <= 12 && xadc >= 0 && xadc <=100 && yadc >= 220 && yadc <= 620)
  printfun("music");

  TX.print(" TX >>>> Thumb ");
  TX.print(thumb);
  TX.print("  Index ");
  TX.print(index);
  TX.print("  Middle ");
  TX.print(middle);
  TX.print("  Ring ");
  TX.print(ring);
  TX.print("  Little ");
  TX.print(little);
  TX.print("  X ");
  TX.print(xadc);
  TX.print("  Y ");
  TX.println(yadc);
  delay(200);

  Serial.print(" TX >>>> Thumb ");
  Serial.print(thumb);
  Serial.print("  Index ");
  Serial.print(index);
  Serial.print("  Middle ");
  Serial.print(middle);
  Serial.print("  Ring ");
  Serial.print(ring);
  Serial.print("  Little ");
  Serial.print(little);
  Serial.print("  X ");
  Serial.print(xadc);
  Serial.print("  Y ");
  Serial.println(yadc);

  delay(200);
  digitalWrite(mled,LOW);
}
