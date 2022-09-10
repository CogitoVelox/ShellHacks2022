#include <ADXL345.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define ADXL343_SCK 13
#define ADXL343_MISO 12
#define ADXL343_MOSI 11
#define ADXL343_CS 10
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
const int buzzer = 10;

/* Assign a unique ID to this sensor at the same time */
/* Uncomment following line for default Wire bus      */
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

/* NeoTrellis M4, etc.                    */
/* Uncomment following line for Wire1 bus */
//Adafruit_ADXL343 accel = Adafruit_ADXL343(12345, &Wire1);

/* Uncomment for software SPI */
//Adafruit_ADXL343 accel = Adafruit_ADXL343(ADXL343_SCK, ADXL343_MISO, ADXL343_MOSI, ADXL343_CS, 12345);

/* Uncomment for hardware SPI */
//Adafruit_ADXL343 accel = Adafruit_ADXL343(ADXL343_CS, &SPI, 12345);

/* Number of high acceleration events recorded */
int BadDriverEvents = 0;

/* Address which stores the number of bad driving events */
int EEPROMADR = 0;

/* Initial acceleration values at powerup */
float InitAccelX;
float InitAccelY;
float InitAccelZ;

/* Initialize global variables for time control */
int i = 0;
int m=0;
int h=0;
double a = millis();
double c ;
bool isRunning = false;

void displayDataRate(void)
{
  Serial.print  ("Data Rate:    ");

  switch(accel.getDataRate())
  {
    case ADXL343_DATARATE_3200_HZ:
      Serial.print  ("3200 ");
      break;
    case ADXL343_DATARATE_1600_HZ:
      Serial.print  ("1600 ");
      break;
    case ADXL343_DATARATE_800_HZ:
      Serial.print  ("800 ");
      break;
    case ADXL343_DATARATE_400_HZ:
      Serial.print  ("400 ");
      break;
    case ADXL343_DATARATE_200_HZ:
      Serial.print  ("200 ");
      break;
    case ADXL343_DATARATE_100_HZ:
      Serial.print  ("100 ");
      break;
    case ADXL343_DATARATE_50_HZ:
      Serial.print  ("50 ");
      break;
    case ADXL343_DATARATE_25_HZ:
      Serial.print  ("25 ");
      break;
    case ADXL343_DATARATE_12_5_HZ:
      Serial.print  ("12.5 ");
      break;
    case ADXL343_DATARATE_6_25HZ:
      Serial.print  ("6.25 ");
      break;
    case ADXL343_DATARATE_3_13_HZ:
      Serial.print  ("3.13 ");
      break;
    case ADXL343_DATARATE_1_56_HZ:
      Serial.print  ("1.56 ");
      break;
    case ADXL343_DATARATE_0_78_HZ:
      Serial.print  ("0.78 ");
      break;
    case ADXL343_DATARATE_0_39_HZ:
      Serial.print  ("0.39 ");
      break;
    case ADXL343_DATARATE_0_20_HZ:
      Serial.print  ("0.20 ");
      break;
    case ADXL343_DATARATE_0_10_HZ:
      Serial.print  ("0.10 ");
      break;
    default:
      Serial.print  ("???? ");
      break;
  }
  Serial.println(" Hz");
}

void displayRange(void)
{
  Serial.print  ("Range:         +/- ");

  switch(accel.getRange())
  {
    case ADXL343_RANGE_16_G:
      Serial.print  ("16 ");
      break;
    case ADXL343_RANGE_8_G:
      Serial.print  ("8 ");
      break;
    case ADXL343_RANGE_4_G:
      Serial.print  ("4 ");
      break;
    case ADXL343_RANGE_2_G:
      Serial.print  ("2 ");
      break;
    default:
      Serial.print  ("?? ");
      break;
  }
  Serial.println(" g");
}

void setup(void)
{
  lcd.begin(16, 2);
  lcd.clear();
  Serial.begin(115200);

  /*Setup Buzzer */
  pinMode(buzzer, OUTPUT);
  
  pinMode(9, INPUT);
  digitalWrite(9, HIGH);
  
  while (!Serial);
  Serial.println("Accelerometer Test"); Serial.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }

  /* Set bad driving events to 0 */
  EEPROM.put(EEPROMADR,0);
  
  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL343_RANGE_16_G);
  // accel.setRange(ADXL343_RANGE_8_G);
  // accel.setRange(ADXL343_RANGE_4_G);
  // accel.setRange(ADXL343_RANGE_2_G);

  /* Get a new sensor event */
  sensors_event_t event;
  accel.getEvent(&event);
  
  /* Set idle acceleration parameters */
  delay(1000);
  InitAccelX = event.acceleration.x;
  InitAccelY = event.acceleration.y;
  InitAccelZ = event.acceleration.z;

  /* Display some basic information on this sensor */
  accel.printSensorDetails();
  displayDataRate();
  displayRange();
  Serial.println("");
}

void loop(void)
{

  /* While it isn't running, wait for button press on start screen */
  while (!isRunning) {
  
    lcd.clear();
    lcd.print("Press Start");

    delay(100);
    if(digitalRead(9) == LOW) {
      lcd.clear();
      isRunning = !isRunning;
    }
    
    a = millis();
  }
  
  /* Get a new sensor event */
  sensors_event_t event;
  accel.getEvent(&event);

 /* Record a bad driving event */
  if(event.acceleration.x > (InitAccelX + 9) || event.acceleration.x < (InitAccelX - 9)|| 
     event.acceleration.y > (InitAccelY + 9) || event.acceleration.y < (InitAccelY - 9)|| 
     event.acceleration.z > (InitAccelZ + 9) || event.acceleration.z < (InitAccelZ - 9))
  {
    EEPROM.put(EEPROMADR, (EEPROM.get(EEPROMADR, BadDriverEvents)+1));
    /* Buzz */
    tone(buzzer, 2000);
    delay(1000);
    noTone(buzzer);
  }

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
  Serial.print("Num Incidents: "); Serial.print(EEPROM.get(EEPROMADR, BadDriverEvents)); Serial.print("  ");

  /* Calculate the time to display */
  c = millis();
  i = (c - a) / 1000;
  if(i==60){
    m++;
    a=millis();  
  }
  if(m>=60){
    h++;
    m=0;
    }
  if(h>0){
   if (h<10){
    lcd.print(0);
    } 
    lcd.print(h);
    lcd.print(":");
   }
  if (m<10){
    lcd.print(0);
    }
  lcd.print(m);
  lcd.print(":");
  if (i<10){
     lcd.print(0);
  }
  lcd.print(i);
  lcd.setCursor(0,1);

  /* Print the number of infractions */
  lcd.print("Infractions:");
  lcd.print(EEPROM.get(EEPROMADR, BadDriverEvents));
  lcd.setCursor(0,0);
  Serial.println(c);
  Serial.println(a);
  Serial.println(i);
  /*Serial.println("......");*/

  delay(200);
  /* Check button */
  if(digitalRead(9) == LOW) {
     isRunning = !isRunning;
     delay(200);
     EEPROM.put(EEPROMADR,0);
     while(digitalRead(9) == HIGH)
     {
       delay(100);
       m=0;
     }
  }

  
}
