// Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
#include "RTClib.h"

RTC_PCF8523 rtc;

enum Day_e
{
  DOTW_Sunday,
  DOTW_Monday,
  DOTW_Tuesday,
  DOTW_Wednesday,
  DOTW_Thursday,
  DOTW_Friday,
  DOTW_Saturday
};

struct Alarm_t
{
  // Use military time for the struct instead of adding an a.m./p.m. boolean
  int hour;
  int minute;
  int dotw;  
};

const int SUNDAY_FLG = 0x1 << 0;
const int MONDAY_FLG = 0x1 << 1;
const int TUESDAY_FLG = 0x1 << 2;
const int WEDNESDAY_FLG = 0x1 << 3;
const int THURSDAY_FLG = 0x1 << 4;
const int FRIDAY_FLG = 0x1 << 5;
const int SATURDAY_FLG = 0x1 << 6;

// I'll have some customization system at some point, but this is the one I want so *shrug*.
const static struct Alarm_t g_alarm = {
  .hour = 5,
  .minute = 0,
  .dotw = MONDAY_FLG | TUESDAY_FLG | WEDNESDAY_FLG | THURSDAY_FLG | FRIDAY_FLG,
};

const int NOTE_C6 = 1047;
const int NOTE_CS6 = 1109;
const int NOTE_D6 = 1175;
const int NOTE_DS6 = 1245;
const int NOTE_E6 = 1319;
const int NOTE_F6 = 1397;

enum Duration_e
{
  DUR_EIGHTH = 125,
  DUR_QUARTER = 250,
  DUR_HALF = 500,
  DUR_WHOLE = 1000,
};

const int alarm_tone[6] = {NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6};
const int ALARM_LENGTH = 6;

const int TONE_PIN = 8;

const int ANA0 = 0;
const int PULSE_SENSOR = ANA0; 

const int RTC_BAUDRATE = 57600;

static int sig;

void setup() {

  while (!Serial)
  {
    delay(1); // We're using the Leonardo so this is neccessary.
  }
  
  // put your setup code here, to run once:
  Serial.begin(RTC_BAUDRATE);
  if (!rtc.begin())
  {
    while(1); // PANIC
  }

  if (! rtc.initialized())
  {
  }

}

bool check_heart_rate()
{
  sig = analogRead(PULSE_SENSOR);
  return true; // Would rather it think that the heartrate is valid than invalid.
}

/*
 * Check if our DateTime matches the information in the specified alarm.
 * Not sure if I should allow for multiple alarms?
 */
bool check_alarm(struct Alarm_t alarm)
{
  DateTime now = rtc.now();

  enum Day_e dotw = now.dayOfTheWeek();

  // Check the time since that'll be the check that is often the least amount of time.
  if (now.hour() != alarm.hour || now.minute() != alarm.minute)
  {
    return false;
  }

  // I'll bother with different days of the week later.
  if (dotw == DOTW_Monday && alarm.dotw & MONDAY_FLG)
  {
    return true;
  } else if (dotw == DOTW_Tuesday && alarm.dotw & TUESDAY_FLG)
  {
    return true;
  } else if (dotw == DOTW_Wednesday && alarm.dotw & WEDNESDAY_FLG)
  {
    return true;
  } else if (dotw == DOTW_Thursday && alarm.dotw & THURSDAY_FLG)
  {
    return true;
  } else if (dotw == DOTW_Friday && alarm.dotw & FRIDAY_FLG)
  {
    return true;
  }
}

void play_tone(enum Duration_e dur, int itone)
{
  tone(TONE_PIN, itone, dur);
  delay(dur);
  noTone(TONE_PIN);
}

void play_alarm()
{
  for (int i = 0; i < ALARM_LENGTH; i++)
  { 
    play_tone(DUR_EIGHTH, alarm_tone[i]);
  }

  for (int i = ALARM_LENGTH - 1; i >= 0; i--)
  { 
    play_tone(DUR_EIGHTH, alarm_tone[i]);
  }
}

void loop() {
  // How to handle the logic of it's been activated now ensure that the heart rate stays above sleeping. And for how long? Like 1 hour? Honestly might be worth it to ensure not 
  // even resting heart rate until what? 15 minutes. Even better if it can't be turned off. Need some way to charge the LiPo though...
  if (check_alarm(g_alarm))
  {
    play_alarm();     
  }
  // put your main code here, to run repeatedly:
}
