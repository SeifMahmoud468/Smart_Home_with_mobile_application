#define REMOTEXY_MODE__ESP8266_HARDSERIAL_POINT
#include <RemoteXY.h>
#define REMOTEXY_SERIAL Serial3
#define REMOTEXY_SERIAL_SPEED 115200
#define REMOTEXY_WIFI_SSID "Smart House"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377
#include <Keypad.h>
#include <LiquidCrystal.h>
#define Password_Length 6
#include <dht.h>
#define DHT11_PIN 2
#define flameIn 3
#define lightIn A0
#define signalPin 4
#include <Servo.h>
#define trigPin 8
#define echoPin 9
#define houseRelay 31
#define gardenRelay 33


// RemoteXY configurate
#pragma pack(push, 1)


long duration;
int distance;
int intial;
int pos = 0;
dht DHT;
int light;
char Data[Password_Length];
char Master[Password_Length] = "1234A";
char Change[Password_Length] = "####A";
byte data_count = 0, master_count = 0;
char customKey;
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {22, 24, 26, 28};
byte colPins[COLS] = {30, 32, 34, 36};
bool someone;
int tryNo;
int chk;
long unsigned timeStored;
char str[11];
bool gardenLight, gardenSensor;

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal lcd2(21, 20, 19, 18, 17, 16);
LiquidCrystal lcd1(53, 51, 49, 47, 45, 43);
Servo myservo1;

void security() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
}
void clearData() {
  while (data_count != 0) {
    Data[data_count--] = 0;
  }
  return;
}
void reading() {
  chk = DHT.read11(DHT11_PIN);
  light = analogRead(lightIn);
  timeStored = millis();
}

#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
{ 255, 5, 0, 22, 0, 230, 0, 14, 8, 1,
  129, 0, 3, 32, 14, 6, 16, 68, 111, 111,
  114, 0, 129, 0, 3, 40, 36, 6, 16, 84,
  101, 109, 112, 101, 114, 97, 116, 117, 114, 101,
  0, 129, 0, 3, 49, 25, 6, 16, 72, 117,
  109, 105, 100, 105, 116, 121, 0, 129, 0, 3,
  58, 34, 6, 16, 72, 111, 117, 115, 101, 32,
  76, 105, 103, 104, 116, 0, 129, 0, 3, 75,
  17, 6, 16, 65, 108, 97, 114, 109, 0, 129,
  0, 8, 9, 55, 9, 17, 83, 109, 97, 114,
  116, 32, 72, 111, 117, 115, 101, 0, 129, 0,
  21, 17, 27, 6, 17, 99, 111, 110, 116, 114,
  111, 108, 108, 101, 114, 0, 1, 2, 37, 32,
  11, 5, 134, 31, 79, 112, 101, 110, 0, 67,
  5, 44, 40, 13, 6, 36, 8, 11, 67, 4,
  45, 49, 13, 6, 17, 8, 11, 1, 2, 42,
  76, 17, 5, 1, 31, 79, 70, 70, 0, 2,
  1, 42, 58, 17, 5, 2, 26, 31, 31, 79,
  78, 0, 79, 70, 70, 0, 1, 2, 49, 32,
  11, 5, 36, 31, 67, 108, 111, 115, 101, 0,
  129, 0, 3, 67, 37, 6, 16, 71, 97, 114,
  100, 101, 110, 32, 76, 105, 103, 104, 116, 0,
  2, 1, 42, 67, 17, 5, 2, 26, 31, 31,
  79, 78, 0, 79, 70, 70, 0
};

// this structure defines all the variables and events of your control interface
struct {

  // input variables
  uint8_t opened; // =1 if button pressed, else =0
  uint8_t fire; // =1 if button pressed, else =0
  uint8_t house; // =1 if switch ON and =0 if OFF
  uint8_t closed; // =1 if button pressed, else =0
  uint8_t garden; // =1 if switch ON and =0 if OFF

  // output variables
  char temp[11];  // string UTF8 end zero
  char hum[11];  // string UTF8 end zero

  // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////


void setup()
{
  Serial.begin(9600);
  RemoteXY_Init ();
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(houseRelay, OUTPUT);
  pinMode(gardenRelay, OUTPUT);
  lcd2.begin(20, 4);
  lcd1.begin(16, 2);
  pinMode(signalPin, OUTPUT);
  myservo1.attach(10);
  tryNo = 3;
  myservo1.write(110);
  security();
  intial = distance;
  someone = false;
  reading();
  gardenLight = false;
  gardenSensor = false;
}

void loop()
{
  RemoteXY_Handler ();
  if (RemoteXY.house == 0) {
    digitalWrite(houseRelay, HIGH);
  } else {
    digitalWrite(houseRelay, LOW);
  }
  if (RemoteXY.garden == 0 && gardenSensor == false) {
    digitalWrite(gardenRelay, HIGH);
    gardenLight = false;
  } else if (RemoteXY.garden == 1) {
    lcd2.setCursor(0, 2);
    lcd2.print("Garden Light is ON ");
    digitalWrite(gardenRelay, LOW);
    gardenLight = true;
  }
  if (RemoteXY.fire != 0) {
    /*  button pressed */
    tryNo = 3;
    Serial.println("Changed");
    digitalWrite(signalPin, LOW);
    clearData();
    RemoteXY.fire = 0;
  }
  if (RemoteXY.opened != 0) {
    /*  button pressed */
    lcd1.print("    Correct!");
    lcd1.setCursor(0, 1);
    lcd1.print("    Welcome!");
    digitalWrite(signalPin, HIGH);
    someone = true;
    myservo1.write(0);
    delay(1000);
    myservo1.write(110);
    digitalWrite(signalPin, LOW);
    RemoteXY.opened = 0;
    lcd1.clear();
  }
  if (RemoteXY.closed != 0) {
    tryNo = 3;
    clearData();
    lcd1.clear();
    lcd1.print("    Good Bye");
    someone = false;
    digitalWrite(signalPin, HIGH);
    delay(1000);
    digitalWrite(signalPin, LOW);
    RemoteXY.closed = 0;
  }
  if (tryNo != 0) {
    lcd1.setCursor(0, 0);
    lcd1.print("Enter Password:  ");
  }
  char customKey = customKeypad.getKey();

  ///////////////////////////
  ////// keypad work ////////
  ///////////////////////////
  if (customKey) {
    //////////// clear button /////////////
    if (customKey == 'C') {
      lcd1.clear();
      clearData();
    }///////// End Of Clear button /////////
    else if (customKey == 'D') {
      tryNo = 3;
      clearData();
      lcd1.clear();
      lcd1.print("    Good Bye");
      someone = false;
      digitalWrite(signalPin, HIGH);
      delay(1000);
      digitalWrite(signalPin, LOW);
    }
    ///////// Bazzer button ////////
    else if (customKey == 'B') {
      tryNo = 3;
      digitalWrite(signalPin, LOW);
      clearData();
    }///////// End of Bazzer button ////////
    //////// Read Data ////////
    else {
      lcd1.setCursor(0, 0);
      lcd1.print("Enter Password:");
      Data[data_count] = customKey;
      lcd1.setCursor(data_count, 1);
      lcd1.print(Data[data_count]);
      data_count++;
    }////////// End Of Reading Data //////////
  }

  //////// Comparing of Password /////////
  if (data_count == Password_Length - 1) {
    lcd1.clear();
    /////// correct Password //////
    if (!strcmp(Data, Master)) {
      lcd1.print("    Correct!");
      lcd1.setCursor(0, 1);
      lcd1.print("    Welcome!");
      digitalWrite(signalPin, HIGH);
      someone = true;
      myservo1.write(0);
      delay(1000);
      myservo1.write(110);
      digitalWrite(signalPin, LOW);
    } ///// End of Correct Password //////

    ////// change password //////
    else if (!strcmp(Data, Change)) {
      lcd1.clear();
      lcd1.print("Old Password: ");
      clearData();
      ////// Readind Data //////
      while (data_count != Password_Length - 1) {
        customKey = customKeypad.getKey();
        if (customKey) {
          Data[data_count] = customKey;
          lcd1.setCursor(data_count, 1);
          lcd1.print(Data[data_count]);
          data_count++;
        }
      } ////// End Of LOOP //////

      //// Compare to change password ////
      if (!strcmp(Data, Master)) {
        lcd1.clear();
        lcd1.print("New Password:");
        clearData();
        ////// Readind Data //////
        while (data_count != Password_Length - 1) {
          customKey = customKeypad.getKey();
          if (customKey) {
            Data[data_count] = customKey;
            lcd1.setCursor(data_count, 1);
            lcd1.print(Data[data_count]);
            data_count++;
          }
        }////// End Of Reading //////
        strcpy(Master, Data);
        lcd1.clear();
        lcd1.print("DONE !!");
        delay(500);
      }////// End Of change //////

      /////// Incorrect to Change //////
      else {
        lcd1.print("Incorrect");
        delay(400);
        tryNo--;
      }
    }
    ////// Incorrect Password //////
    else {
      lcd1.print("Incorrect");
      tryNo--;
      delay(400);
      someone = false;
    }
    lcd1.clear();
    clearData();
  }/////// End OF KEYPAD WORK ////////
  if (tryNo == 0) {
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("Security warning");
    digitalWrite(signalPin, HIGH);
  }

  //////////////////  Loop   //////////////////
  if (millis() - timeStored >= 1000) {
    reading();
    Serial.println(light);
  }
  lcd2.setCursor(0, 0);
  lcd2.print("Temp: ");
  lcd2.print(DHT.temperature);
  lcd2.print((char)223);
  lcd2.print("C      ");
  float temperature = DHT.temperature;
  dtostrf(temperature, 0, 1, str);
  sprintf (RemoteXY.temp, "%s C", str);
  lcd2.setCursor(0, 1);
  lcd2.print("Humidity: ");
  lcd2.print(DHT.humidity);
  lcd2.print("%     ");
  float humidity = DHT.humidity;
  dtostrf(humidity, 0, 1, str);
  strcat(str, " %");
  sprintf (RemoteXY.hum, "%s", str);
  lcd2.setCursor(0, 2);
  lcd2.print("Garden Light is ");
  if (light < 60) {             //HIGH means,light got blocked
    digitalWrite(gardenRelay, LOW);
    lcd2.print("ON ");
    gardenSensor = true;

  }      //if light is not present,LED on
  else if (gardenLight == false && light > 60) {
    digitalWrite(gardenRelay, HIGH);
    lcd2.print("OFF");
    gardenSensor = false;
  }

  int flameSensor = digitalRead(flameIn);
  if (flameSensor == 0) {
    lcd2.setCursor(0, 3);
    lcd2.print("      Safe ^-^      ");
  } else {
    lcd2.clear();
    myservo1.write(0);
    lcd1.clear();
    lcd1.print("     Fire!!");
    while (flameSensor == 1) {
      lcd2.print("FIRE !! ");
      digitalWrite(signalPin, HIGH);
      delay(500);
      digitalWrite(signalPin, LOW);
      flameSensor = digitalRead(flameIn);
    }
    lcd1.clear();
    lcd2.clear();
    myservo1.write(110);
  }
  security();
  if (distance < 10 && !someone) {
    lcd1.setCursor(0, 0);
    lcd1.print("Security warning");
    digitalWrite(signalPin, HIGH);
    delay(500);
    lcd1.clear();
    digitalWrite(signalPin, LOW);
  }
}
