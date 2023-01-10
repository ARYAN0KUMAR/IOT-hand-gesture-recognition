#include <SoftwareSerial.h>
#include <Wire.h>  // Wire library - used for I2C communication
SoftwareSerial mySerial(2,3);
SoftwareSerial RX(5,6);
String temp = "";
bool calibrate = false;
int little  = 0,Cal_little  = 720;
int ring    = 0,Cal_ring    = 720;
int middle  = 0,Cal_middle  = 720;
int index   = 0,Cal_index   = 720;
int thumb   = 0,Cal_thumb   = 720;
int button = 11;
int mgnd = 9;
int mled = 8;
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
float X_out, Y_out, Z_out;  // Outputs
int FLEX_PIN1 = A0; // little finger
int FLEX_PIN2 = A1; // ring finger
int FLEX_PIN3 = A2; // middle finger
int FLEX_PIN4 = A3; // index finger
int FLEX_PIN5 = A6; // thumb  

int Mlength = 0;
bool copystart = false;
String digit = " ";

void get_Digi(int x)
{
  digit = "";
  for(int i=x; i<=inputString.length(); i++){
      if(isdigit(inputString[i])){
          digit += inputString[i];
          copystart = true;
          Mlength = i;
        }
        if(inputString[i] == ' ' && (copystart == true))
        {
          Mlength = i;
          break;
        }
      }
     //Serial.print("ANS  ");Serial.print(digit); Serial.print("  ");Serial.println(Mlength); 
}

void setup() 
{
  Serial.begin(9600);
  RX.begin(9600);
  mySerial.begin(9600);
  temp.reserve(70);
  inputString.reserve(70);
  pinMode(button, INPUT_PULLUP);
  pinMode(mgnd, OUTPUT);
  digitalWrite(mgnd,LOW);
  pinMode(mled, OUTPUT);
  digitalWrite(mled,HIGH);
  while(!Serial) 
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
  mySerial.println("calibrateing");

  if(calibrate == false)
  {
    for(int j =0;j<300;j++)
    {
      delay(12);
      little = analogRead(FLEX_PIN1) - Cal_little;
      if(little>1)Cal_little++;if(little<0)Cal_little--;
      ring = analogRead(FLEX_PIN2) - Cal_ring;
      if(ring>1)Cal_ring++;if(ring<0)Cal_ring--;
      middle = analogRead(FLEX_PIN3) - Cal_middle;
      if(middle>1)Cal_middle++;if(middle<0)Cal_middle--;
      index = analogRead(FLEX_PIN4) - Cal_index;
      if(index>1)Cal_index++;if(index<0)Cal_index--;
      thumb = analogRead(FLEX_PIN5) - Cal_thumb;
      if(thumb>1)Cal_thumb++;if(thumb<0)Cal_thumb--;
      
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
  mySerial.println("Start");
}


void printfun(String cp) //to avoid printing repeating symbols
{
  if(cp!=temp)
  {
    digitalWrite(mled,HIGH);
    mySerial.println(cp);
    Serial.println("*******");
    Serial.println(cp);
    Serial.println("*******");
    temp=cp;
    delay(50);
   }
}

void loop() 
{
  
  inputString = "";
  loop1:
  //Serial.print(".");
  RX.listen();
  for(int i =0;i<60;i++)
  {
    if(!stringComplete){Event();delay(5);}
  }
  if(stringComplete == false){goto loop1;}

  bool ans=digitalRead(button);
  Serial.println(inputString);
  if(inputString.startsWith("AT+") && ans== false){ inputString.remove(0,3);  printfun(inputString); stringComplete =false; inputString = "";}

 
  Wire.beginTransmission(0x53);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(0x53, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read()| Wire.read() << 8); // X-axis value
  // X_out = X_out/256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis value
  //Y_out = Y_out/256;
  Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis value
  //Z_out = Z_out/256;
/*
  Serial.print(">>> Xa= ");
  Serial.print(X_out);
  Serial.print("   Ya= ");
  Serial.print(Y_out);
  Serial.print("   Za= ");
  Serial.println(Z_out);*/


  
  // reading sensor value
  little = analogRead(FLEX_PIN1) - Cal_little;
  ring = analogRead(FLEX_PIN2) - Cal_ring;
  middle = analogRead(FLEX_PIN3) - Cal_middle;
  index = analogRead(FLEX_PIN4) - Cal_index;
  thumb = analogRead(FLEX_PIN5) - Cal_thumb;
  // reading sensor value
  if(calibrate)
  {
    if(little<0)little=0;
    if(ring<0)ring=0;
    if(middle<0)middle=0;
    if(index<0)index=0;
    if(thumb<0)thumb=0;
//
//    little  = little*3.0;
//    ring    = ring*3.0;
//    middle  = middle*3.0;
//    index   = index*3.0;
//    thumb   = thumb*3.0;
  }

  int xadc = (X_out + 255)*2;
  int yadc = (Y_out + 255)*2;
  if(xadc < 0)xadc =0;
  if(xadc > 1023)xadc =1023;
  if(yadc < 0)yadc =0;
  if(yadc > 1023)yadc =1023;

  if(ans){ 
//     TX >>>> Thumb 3  Index 3  Middle 6  Ring 6  Little 3  X 578  Y 642
      Serial.println("Duble");
      copystart = false;
      get_Digi(0);
      int Mthumb   = digit.toInt();
      copystart = false;
      get_Digi(Mlength);
      int Mindex   = digit.toInt();
      copystart = false;
      get_Digi(Mlength);
      int Mmiddle  = digit.toInt();
      copystart = false;
      get_Digi(Mlength);
      int Mring    = digit.toInt();
      copystart = false;
      get_Digi(Mlength);
      int Mlittle  = digit.toInt();
      copystart = false;
      get_Digi(Mlength);
      int Mxadc = digit.toInt();
      copystart = false;
      get_Digi(Mlength);
      int Myadc = digit.toInt();

         //   if (index >= 13 && thumb >= 12 && middle >= 13 && ring >= 12 && little >= 12 && xadc>=350 && xadc<=550 && yadc>=923 && yadc<=1123)
    //    printfun("it’s holiday today");

      if (index <= 12 && thumb <= 12 && middle <= 12 && ring <= 12 && little <= 12 && xadc>=0 && xadc<=200 && yadc>=200 && yadc<=650
      && Mindex <= 12 && Mthumb <= 12 && Mmiddle <= 12 && Mring <= 12 && Mlittle <= 12 && Mxadc>=0 && Mxadc<=200 && Myadc>=200 && Myadc<=650)
        printfun("I surrender");

      if (index >= 12 && thumb >= 12 && middle >= 12 && ring >= 12 && little >= 12 && xadc>=400 && xadc<=700 && yadc>=0 && yadc<=150
      && Mindex >= 12 && Mthumb >= 12 && Mmiddle >= 12 && Mring >= 12 && Mlittle >= 12 && Mxadc>=200 && Mxadc<=500 && Myadc>=800 && Myadc<=1050 )
        printfun("Fight with me");

      if (index >= 13 && thumb >= 13 && middle >= 13 && ring >= 13 && little >= 13 && xadc>=400 && xadc<=700 && yadc>=0 && yadc<=150
      && Mindex <= 13 && Mthumb <= 13 && Mmiddle >= 13 && Mring >= 13 && Mlittle >= 13 && Mxadc>=600 && Mxadc<=1000 && Myadc>=200 && Myadc<=900 )
        printfun("Sit Down");  

      if (index <= 12 && thumb <= 12 && middle <= 12 && ring <= 12 && little <= 12 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650
      && Mindex <= 12 && Mthumb <= 12 && Mmiddle >= 12 && Mring >= 12 && Mlittle >= 12 && Mxadc>=0 && Mxadc<=200 && Myadc>=300 && Myadc<=650 )
        printfun("Stand Up");

      if (index >= 13 && thumb <= 12 && middle >= 13 && ring >= 12 && little >= 12 && xadc>=400 && xadc<=700 && yadc>=0 && yadc<=150
      && Mindex >= 13 && Mthumb <= 12 && Mmiddle >= 12 && Mring >= 12 && Mlittle >= 12 && Mxadc>=200 && Mxadc<=500 && Myadc>=800 && Myadc<=1050 )
        printfun("All the best");

//      if (index <= 15 && thumb <= 15 && middle <= 15 && ring <= 15 && little <= 15 && xadc>=1000 && xadc<=1023 && yadc>=550 && yadc<=600
//      && Mindex <= 15 && Mthumb <= 15 && Mmiddle <= 15 && Mring <= 15 && Mlittle <= 15 && Mxadc>=0 && Mxadc<=50 && Myadc>=580 && Myadc<=680 )
//        printfun("Surrender myself");

      //if (index <= 8 && thumb <= 8 && middle <= 8 && ring <= 8 && little <= 8 && xadc>=900 && xadc<=1023 && yadc>=450 && yadc<=650
   //   && Mindex <= 8 && Mthumb <= 8 && Mmiddle <= 8 && Mring <= 8 && Mlittle <= 8 && Mxadc>=0 && Mxadc<=50 && Myadc>=450 && Myadc<=650 )
    //    printfun("Greetings!");  //"Be unite" 
    
      Serial.print("MMTX <<<< Thumb ");
      Serial.print(Mthumb);
      Serial.print("  Index ");
      Serial.print(Mindex);
      Serial.print("  Middle ");
      Serial.print(Mmiddle);
      Serial.print("  Ring ");
      Serial.print(Mring);
      Serial.print("  Little ");
      Serial.print(Mlittle);
      Serial.print("  X ");
      Serial.print(Mxadc);
      Serial.print("  Y ");
      Serial.print(Myadc);

      Serial.print("    MMRX <<<< Thumb ");
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
      stringComplete =false;
      inputString = "";
      delay(200);
      goto loop1;
    }
  





  if (index >= 13 && thumb >= 12 && middle >= 13 && ring >= 12 && little >= 12 && xadc>=400 && xadc<=700 && yadc>=0 && yadc<=150)
    printfun("it’s holiday today");
  
  if (index <= 13 && thumb <= 12 && middle <= 13 && ring >= 12 && little >= 12 && xadc>=400 && xadc<=700 && yadc>=0 && yadc<=150)
    printfun("danger");

   if (index >= 13 && thumb <= 12 && middle >= 13 && ring >= 12 && little >= 12 && xadc>=400 && xadc<=700 && yadc>=0 && yadc<=150)
    printfun("yes final");

    
   if (index <= 13 && thumb >= 12 && middle >= 13 && ring >= 12 && little >= 12 && xadc>=400 && xadc<=700 && yadc>=0 && yadc<=150)
    printfun("please wait outside");

  if (index >= 13 && thumb <= 12 && middle >= 13 && ring >= 12 && little <= 12 && xadc>=400 && xadc<=700 && yadc>=0 && yadc<=150)
    printfun("it was okay");
  
  if (index >= 15 && thumb >= 15 && middle >= 15 && ring >= 15 && little >= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("Unity");


    
 if (index >= 13 && thumb >= 13 && middle <= 15 && ring <= 12 && little <= 12 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("Nice food");

 if (index <= 13 && thumb >= 13 && middle >= 13 && ring <= 13 && little <= 12 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("dog");

 if (index <= 15 && thumb <= 15 && middle <= 15 && ring <= 15 && little <= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("see you soon");

if (index <= 15 && thumb >= 15 && middle <= 15 && ring <= 15 && little >= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("its done now");


  
 if (index <= 13 && thumb <= 13 && middle >= 13 && ring >= 13 && little >= 12 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("let it be ");

 if (index >= 13 && thumb <= 13 && middle >= 13 && ring >= 13 && little >= 12 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("let’s go for a walk");



 
 if (index <= 13 && thumb >= 13 && middle <= 13 && ring <= 13 && little <= 12 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("see you at 4");

 if (index <= 15 && thumb <= 15 && middle <= 15 && ring <= 15 && little <= 15 && xadc>=600 && xadc<=900 && yadc>=200 && yadc<=600)
    printfun("sorry to hear that");

 if (index <= 13 && thumb >= 13 && middle <= 13 && ring >= 13 && little >= 12 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("will talk this on call");

  if (index <= 15 && thumb >= 15 && middle >= 15 && ring >= 15 && little >= 15 && xadc>=600 && xadc<=900 && yadc>=200 && yadc<=600)
    printfun("This is where we started");

  if (index <= 15 && thumb <= 15 && middle >= 15 && ring >= 15 && little <= 15 && xadc>=600 && xadc<=900 && yadc>=200 && yadc<=600)
    printfun("party is done ");

   if (index <= 15 && thumb >= 15 && middle <= 15 && ring >= 15 && little <= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("I am coming");


  if (index <= 15 && thumb <= 15 && middle <= 15 && ring >= 15 && little <= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("can we meet?");

   if (index <= 15 && thumb <= 15 && middle >= 15 && ring <= 15 && little >= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("fruits");

  if (index >= 15 && thumb <= 15 && middle <= 15 && ring <= 15 && little >= 15 && xadc >= 800 && yadc >= 420 && yadc <= 620)
    printfun("I cook good food");

 
 if (index>= 15 && thumb <= 15 && middle <= 15 && ring >= 15 && little <= 15 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("Bad quality");

 
  if (index <= 15 && thumb <= 15 && middle <= 15 && ring >= 15 && little >= 15 && xadc>=600 && xadc<=900 && yadc>=200 && yadc<=600)
    printfun("I am angry");

 
 
  if (index >= 15 && thumb >= 15 && middle >= 15 && ring >= 15 && little >= 15 && xadc >=450 && xadc <=750 && yadc >=850 && yadc <=1050)
    printfun("let’s start ");

 
  if (index >= 15 && thumb >= 15 && middle >= 15 && ring >= 15 && little <= 15 && xadc >=450 && xadc <=750 && yadc >=850 && yadc <=1050)
    printfun("This is not mine");

 if (index >= 15 && thumb <= 15 && middle >= 15 && ring >= 15 && little >= 15 && xadc >=450 && xadc <=750 && yadc >=850 && yadc <=1050)
    printfun("Please leave me alone");

 
 if (index >= 13 && thumb <= 13 && middle >= 13 && ring >= 13 && little >= 12 && xadc >= 850 && yadc >= 420 && yadc <= 620)
    printfun("go and have it by yourself");

 
 
 if (index <= 13 && thumb <= 13 && middle >= 13 && ring >= 13 && little <= 12 && xadc>=0 && xadc<=200 && yadc>=300 && yadc<=650)
    printfun("let’s chill for some time");



Serial.print("RX <<<< Thumb ");
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
stringComplete = false;
inputString = "";
delay(200);
digitalWrite(mled,LOW);
}


void Event() {
  while (RX.available()) {
    // get the new byte:
    char inChar = (char)RX.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
