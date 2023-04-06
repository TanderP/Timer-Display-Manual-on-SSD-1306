#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);


/*
Safe to use pin
VN(GPIO39) inp only 23
32          22
25          19
26          18
           17
           16*/
//for devkit v4
#define CLOCK_Indicator 16 // free pin

#define LED_PIN 32 //free pin
#define BUTTON_PIN 17 //free pin

//connect SDA and SCL/SCK pin to the oled
// Usualy SDA 21 and SCL 23

//-----------------
bool ledState = false;
//---
String line1 = "NULL";
String LedCons = "NULL";
int numb  = 0;
//---
//_________________
int seconds = 1;
int minutes = 8;
int hours = 02;
//_________________


void LEDfunction(void *pvParameters ){
  while (1) {
    if (digitalRead(BUTTON_PIN) == HIGH) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);

      delay(100);
 
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void IRAM_ATTR LineDisplay1(void *pvParameters ) {
  while (1) {
    line1 = "Hello World";
    delay(100);
  }

}
//timer clock


void IRAM_ATTR onTimer(void *pvParameters) {
 while (1) {seconds++;
  if (seconds > 59) {
    seconds = 0;
    minutes++;
    if (minutes > 59) {
      minutes = 0;
      hours++;
      if (hours > 23) {
        hours = 0;
      }
    }
    }
    delay (1000);
    
  }
  
}
// oled control
void OLED_DISPLAY_Controller(void *pvParameters) {
  while (1) {

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(line1);
    
    ledState ? LedCons = "ON" : LedCons = "OFF";
    display.println("LED is : " + LedCons );

    

    display.println("Time: ");
    display.printf("%02d:%02d:%02d\n", hours, minutes, seconds);
    display.display();
    vTaskDelay(10 / portTICK_PERIOD_MS); // so its updated every 100 ms an not everytime the loop runs
  }
}
// indicator of code running
void ClockIndicator(void *pvParameters) {
  while (1) {
    digitalWrite(CLOCK_Indicator, HIGH);
    delay(100);
    digitalWrite(CLOCK_Indicator, LOW);
    delay(900);
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(CLOCK_Indicator, OUTPUT);


  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

xTaskCreatePinnedToCore(LineDisplay1, "LineDisplay1", 1024, NULL, 4, NULL, 0);

xTaskCreatePinnedToCore(OLED_DISPLAY_Controller, "OLEDDisplay", 4096, NULL, 2, NULL, 0);
xTaskCreatePinnedToCore(ClockIndicator, "ClockIndicator", 1024, NULL, 1, NULL, 0);
xTaskCreatePinnedToCore(LEDfunction, "LEDfunction", 1024, NULL, 2, NULL, 0);
xTaskCreatePinnedToCore(onTimer, "onTimer", 2048, NULL, 1, NULL, 1); // last int to set core pin used

}

void loop() {
 
}

// there is a probally a delay on the timer latter for a long run. because of using the Rtos.
// if we use too many input and task of course.
// if you not running so many task then it will be fine
// to avoid this im using the clock on different core(maybe will work)