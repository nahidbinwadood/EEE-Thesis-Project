#define BLYNK_TEMPLATE_ID "TMPLidw0Vaum"
#define BLYNK_TEMPLATE_NAME "Test"
#define BLYNK_AUTH_TOKEN "8tIjtqZbiQuQGHInPCDxWFWVrZ-ntvk_"
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
BlynkTimer timer;
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

char ssid[] = "NaduFai";       // Your WiFi network name
char pass[] = "fjl.lv.ndu";   // Your WiFi network password
int drainbuttonState;
int dobuttonState;

const int analogPin = 32; //NH3





const int phpin= 34; 



//Do part

#define TWO_POINT_CALIBRATION 0
#define DO_PIN 35
#define VREF 3300
#define ADC_RES 4095
#define READ_TEMP (25)
#define Cal1_V (1455)
#define CAL1_T (25)
#define CAL2_V (1300)
#define CAL2_T (25)
const uint16_t DO_Table[41] = {14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530, 11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270, 9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690, 7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};
uint8_t Temperaturet;
uint16_t ADC_RAW;
uint16_t ADC_Voltage;
uint16_t DO;
uint16_t readDO(uint32_t voltage_mv, uint8_t temperature_c)
{
  #if TWO_POINT_CALIBRATION == 0
  uint16_t V_saturation = (uint32_t)Cal1_V + (uint32_t)35*temperature_c - (uint32_t)CAL1_T*35;
  return(voltage_mv*DO_Table[temperature_c]/V_saturation); 
  #else
  uint16_t V_saturation = (uint16_t)((int8_t)temperature_c - CAL2_T)*((uint16_t)CAL1_V - CAL2_V)/ ((uint8_t)CAL1_T - CCAL2_T) + CAL2_V;
  return(voltage_mv*DO_Table[temperature_c]/V_saturation);
  #endif
}

// Do part end



void setup() {
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(14,OUTPUT);
  pinMode(15,OUTPUT);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  digitalWrite(14, LOW);
  digitalWrite(15, HIGH);
  lcd.init(); 
  lcd.backlight(); // initialize the LCD
  lcd.clear();             
  lcd.setCursor(0, 0);
  lcd.print("FYDP-C Group8");
  lcd.setCursor(0, 1);
  lcd.print("bottmcln prjct");
  delay(3000);
  lcd.clear();
  Serial.begin(115200);
  delay(1000);
  
  // Connect to WiFi network
  WiFi.begin(ssid, pass);
  delay(3000);
  lcd.clear();
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    
    delay(3000);
    lcd.clear();
    lcd.print("connecting");
  }
  delay(3000);
  lcd.clear();
  lcd.println();
  delay(3000);
  lcd.clear();
  lcd.println("WiFi connected");
  
Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
delay(3000);
  lcd.clear();
}




BLYNK_WRITE(V0) {
  drainbuttonState = param.asInt(); // get the state of the button
  digitalWrite(12, drainbuttonState); // turn the LED on or off depending on the button state
  digitalWrite(13, drainbuttonState);
  if(drainbuttonState == 0){
    Blynk.virtualWrite(V5, "Drainmotor on manually");
    }
  
}

BLYNK_WRITE(V4) {
  dobuttonState = param.asInt(); // get the state of the button
  digitalWrite(15, dobuttonState); // turn the LED on or off depending on the button state
  if(dobuttonState == 0){
    Blynk.virtualWrite(V6, "DO generator on manually");
    }
}





void loop() {
  Blynk.run(); // Run the Blynk library

  //.......................................................................................




  //................................
  int sensorValue = analogRead(analogPin);  // read the analog voltage
  float voltage = sensorValue * (3.3 / 4095.0);  // convert the value to voltage
  float nh3v = voltage*1.5;

  float rs= (50/nh3v)-10;
  float ro= rs/3.6;
  float ratio= rs/ro;
  float ppm= pow(10,((log10(ratio)-0.42)/(-0.263)));

   //..........................................



  

    float   Value= analogRead(phpin);
    float   phvoltage=Value*(3.3/4095.0);
    float   ph=(3.3*phvoltage);

//DO part start
  Temperaturet = (uint8_t)READ_TEMP;
  ADC_RAW = analogRead(DO_PIN);
  ADC_Voltage = uint32_t(VREF)*ADC_RAW/ADC_RES;
  float Do= readDO(ADC_Voltage,Temperaturet)/1000;

  delay(1000);
  lcd.init(); 
  lcd.backlight(); // initialize the LCD
  lcd.clear();             
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("NH3: ");;
  lcd.print(ppm, 2);
  Blynk.virtualWrite(V1,ppm);// print the voltage with 2 decimal places
  Blynk.virtualWrite(V2,ph);
  Blynk.virtualWrite(V3,Do);
  lcd.print("PPM");
  lcd.setCursor(0, 1);
  lcd.print("PH:");
  lcd.print(ph,2);
  lcd.print(" DO:");
  lcd.print(Do);

  //..................
  if(ppm>=0.6 || (ph>8.5 || ph<6 ) && drainbuttonState==1){
    Blynk.virtualWrite(V5, "Drainmotor on Due to Water Quality Fall");
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
    }
  else if(ppm<=0.4 && (ph<8.5 || ph>6) && drainbuttonState==1){
    
    Blynk.virtualWrite(V5, "Drainmotor Off");
      digitalWrite(12, HIGH);
      digitalWrite(13, HIGH);
    }
   if(Do<5 && dobuttonState==1){
    digitalWrite(15, LOW);
    Blynk.virtualWrite(V6, "Extra DO generation started due to low DO");
    }
    else if(Do>5 && dobuttonState==1){
      digitalWrite(15, HIGH);
    Blynk.virtualWrite(V6, "DO normal extra DO deneration off");
      }

  
  delay(3000); 
  
}
