#include <Arduino.h>
//#include<MD_MAX72xx.h>

/*
 Repeat timer example

 This example shows how to use hardware timer in ESP32. The timer calls onTimer
 function every second. The timer can be stopped with button attached to PIN 0
 (IO0).

 This example code is in the public domain.
 */
//Initialization data for timer
int Minutes = 3;
int Seconds = 0;
int centSeconds =0;
int Periods = 1;
int pointsLeft = 0;
int pointsRight = 0;
char buf[10];

int vrijeme=0;

hw_timer_t * timer = NULL;

volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;

void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
if (centSeconds != 0)
{
  centSeconds--;
}
else {
  centSeconds = 99;
  if (Seconds != 0)
  {
    Seconds--;
  }
  else
  {
    Seconds=59;
    if (Minutes != 0)
    {
      Minutes--;
    }

  }
}
if (Minutes==0 && Seconds==0 && centSeconds==0)
{
  timerStop(timer);
}
  //isrCounter++;
  //lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
}

void setup() {
  Serial.begin(115200);

  // Set BTN_STOP_ALARM to input mode
//  pinMode(BTN_STOP_ALARM, INPUT);

  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function 100 times per second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 10000, true);

  // Start an alarm
  delay(10000);
  timerAlarmEnable(timer);
}

void loop() {
  // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
      sprintf(buf,"%02d:%02d:%02d", Minutes,Seconds,centSeconds);
      Serial.println(buf);
    //vrijeme++;
    if (Minutes==0 && Seconds==0 && centSeconds==0)
    {  delay(5000);
      Minutes=3;
      Seconds=0;
      centSeconds=0;
        //timer = NULL;

        //timer = timerBegin(0, 80, true);
          timerStart(timer);
    }
  }


}
