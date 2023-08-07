#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define ONE_WIRE_BUS 17
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define SCREEN_WIDTH 128   // OLED display width, in pixels
#define SCREEN_HEIGHT 32   // OLED display height, in pixels

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define CLOCK_Indicator 23 // Free pin
//#define LED_PIN 26         // Free pin
#define BUTTON_PIN 32      // Free pin

bool ledState = false;
String line1 = "NULL";
String LedCons = "NULL";
//rtos
volatile int seconds = 0;
volatile int minutes = 25;
volatile int hours = 19;

unsigned long previousMillis = 0;
const unsigned long interval = 1000; // Update interval in milliseconds
// millis
unsigned long referenceMillis2;

volatile int hours2 = 19;
volatile int minutes2 = 25;
volatile int seconds2 = 0;

double temperature= 0;
//void LEDfunction(void *pvParameters) {
  //while (1) {
    //if (digitalRead(BUTTON_PIN) == HIGH) {
      //ledState = !ledState;
 //     digitalWrite(LED_PIN, ledState);
   // }
    //vTaskDelay(10 / portTICK_PERIOD_MS);
  //}
//}

void LineDisplay1(void *pvParameters) {
  while (1) {
    line1 = "Hello World";
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void OLED_DISPLAY_Controller(void *pvParameters) {
  while (1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(line1);

    //ledState ? LedCons = "ON" : LedCons = "OFF";
    //display.println("LED: " + LedCons);
    display.printf("MILLIS = %02d:%02d:%02d\n", hours2, minutes2, seconds2);

    display.printf("Tempt = %.2f C \n ", temperature);
    display.printf("RTOS = %02d:%02d:%02d\n", hours, minutes, seconds);
    display.display();
    vTaskDelay(10 / portTICK_PERIOD_MS); // Update display every 10 milliseconds
  }
}

void ClockIndicator(void *pvParameters) {
  while (1) {
    digitalWrite(CLOCK_Indicator, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    digitalWrite(CLOCK_Indicator, LOW);
    vTaskDelay(900 / portTICK_PERIOD_MS);
  }
}

void onTimer(TimerHandle_t xTimer) {
  seconds++;
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

  Serial.printf("%02d:%02d:%02d\n", hours, minutes, seconds);
}

void temptRead(void *pvParameters){
 while (1)
 {
   sensors.requestTemperatures(); // Send the command to get temperatures

  double getTempt= sensors.getTempCByIndex(0); // Read temperature in Celsius
  temperature = getTempt;
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  delay(1000); // Delay for 1 second
 }
}


void setup() {
  Wire.begin();
  Serial.begin(115200);
  referenceMillis2 = millis();
 // pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(CLOCK_Indicator, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  sensors.begin();

  xTaskCreatePinnedToCore(LineDisplay1, "LineDisplay1", 1024, NULL, 4, NULL, 0);
  xTaskCreatePinnedToCore(OLED_DISPLAY_Controller, "OLEDDisplay", 4096, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(ClockIndicator, "ClockIndicator", 1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(temptRead, "temptRead", 1024, NULL, 1, NULL, 0);
  //xTaskCreatePinnedToCore(LEDfunction, "LEDfunction", 1024, NULL, 2, NULL, 0);

  TimerHandle_t timerHandle = xTimerCreate("timer", pdMS_TO_TICKS(interval), pdTRUE, 0, onTimer);
  if (timerHandle != NULL) {
    xTimerStart(timerHandle, 0);
  }
}

void loop() {
unsigned long currentMillis = millis();
  unsigned long elapsedTime = currentMillis - referenceMillis2;

  // Update the clock every 1000 milliseconds
  if (elapsedTime >= 1000) {
    referenceMillis2 += 1000; // Move the reference time forward by 1000 milliseconds

    // Increment the clock
    seconds2++;
    if (seconds2 > 59) {
      seconds2 = 0;
      minutes2++;
      if (minutes2 > 59) {
        minutes2 = 0;
        hours2++;
        if (hours2 > 23) {
          hours2 = 0;
        }
      }
    }

    // Print the updated time
    Serial.printf("%02d:%02d:%02d\n", hours2, minutes2, seconds2);
  }

  // Add a delay to control the loop execution speed
  delay(10);
}
