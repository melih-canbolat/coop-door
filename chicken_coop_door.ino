/*---Melih_Canbolat---*/
/* Automated chicken coop door code. */

// The door uses the stepper 28BYJ-48 with ULN2003
// DS1307 RTC Module is used.(SDA to A4,SCL to A5)

/* Functions of the code; */
// Opens/closes the door when the time comes (different for each month)
// Opens/closes the door when the button is pressed regardless of time
// Turns the power to the stepper off to preserve power
// Displays the error regarding the DS1307
// May control coop light (future work)

#include <Stepper.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

int limit_1_status; // LOW if fully closed
int limit_2_status; // LOW if fully opened
int current_position;
int limit_1 = 11; // Limit switch 1
int limit_2 = 12; // Limit switch 2
int button = 10; // Manual open/close button
int t_open=0; //Opening time(hour)
int t_close=0; //Closing time(hour)
//int relay = 9; //Relay module signal pin (9)

const int steps_to_move = 32; // One revolution is 2048 steps
                              // 256 steps = 1 mm linear movement

Stepper CoopStepper(2048, 2, 4, 3, 5); //(steps_per_rev,in1,in3,in2,in4)

void setup() 
{
  Serial.begin(9600);
  CoopStepper.setSpeed(12); //(rpm)
  pinMode(limit_1,INPUT_PULLUP);
  pinMode(limit_2,INPUT_PULLUP);
  pinMode(button,INPUT_PULLUP);
  //pinMode(relay, OUTPUT);
  //digitalWrite(relay,HIGH); // Turn off the relay
  
  limit_1_status=digitalRead(limit_1);
  limit_2_status=digitalRead(limit_2);
  
  if (limit_1_status==LOW) //Store the position of the door
  {
    current_position=1;
  }
  else //Even if the door is half open, assumes open.
  {
    current_position=2;
  }
}

void loop()
{
  tmElements_t tm;
  RTC.read(tm);
  
  switch (tm.Month)
  {
    case 1: //January
      t_open=10; //10:00
      t_close=19; //19:00
      door_function();
      break;
      
    case 2: //February
      t_open=9; //09:00
      t_close=19; //19:00
      door_function();
      break;
      
    case 3: //March
      t_open=9; //09:00
      t_close=20; //20:00
      door_function();
      break;
      
    case 4: //April
      t_open=9; //08:00
      t_close=20; //20:00
      door_function();
      break;
      
    case 5: //May
      t_open=9; //07:00
      t_close=21; //21:00
      door_function();
      break;
      
    case 6: //June
      t_open=9; //07:00
      t_close=21; //21:00
      door_function();
      break;
      
    case 7: //July
      t_open=9; //07:00
      t_close=21; //21:00
      door_function();
      break;
      
    case 8: //August <-------------------------
      t_open=9; //07:00
      t_close=21; //21:00
      door_function();
      break;
      
    case 9: //September
      t_open=9; //08:00
      t_close=20; //20:00
      door_function();
      break;
      
    case 10: //October
      t_open=9; //08:00
      t_close=19; //19:00
      door_function();
      break;
      
    case 11: //November
      t_open=9; //09:00
      t_close=18; //18:00
      door_function();
      break;
      
    case 12: //December
      t_open=10; //10:00
      t_close=18; //18:00
      door_function();
      break;
  }
  
  if (!digitalRead(button)) //Opening/closing using the button
  {
    switch (current_position)
    {
      case 1: //If the current position is 1, go to 2
        limit_2_status=digitalRead(limit_2);
        while(limit_2_status==HIGH) //Rotate until the limit switch 2 is pressed
        {
          CoopStepper.step(-steps_to_move);
          limit_2_status=digitalRead(limit_2);
        }
        current_position=2;
        Serial.println("The door has been opened.");
        delay(2000);
        digitalWrite(2,LOW);
        digitalWrite(3,LOW);
        digitalWrite(4,LOW);
        digitalWrite(5,LOW);
        break;

      case 2: //If the current position is 2, go to 1
        limit_1_status=digitalRead(limit_1);
        while(limit_1_status==HIGH) //Rotate until the limit switch 1 is pressed
        {
          CoopStepper.step(steps_to_move);
          limit_1_status=digitalRead(limit_1);
        }
        current_position=1;
        Serial.println("The door has been closed.");
        delay(2000);
        digitalWrite(2,LOW);
        digitalWrite(3,LOW);
        digitalWrite(4,LOW);
        digitalWrite(5,LOW);
        break;
    }
  }
  
  if (!RTC.read(tm)) //displaying the error (DS1307)
  {
    if (RTC.chipPresent()) 
    {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } 
    else
    {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(3000);
  }
}
void door_function()
{
  tmElements_t tm;
  RTC.read(tm);
  
  if (tm.Hour == t_close) //(tm.Hour == t_close)
  {
    limit_1_status=digitalRead(limit_1);
    while(limit_1_status==HIGH) //Rotate until the limit switch 1 is pressed
    {
      CoopStepper.step(steps_to_move);
      limit_1_status=digitalRead(limit_1);
    }
    current_position=1;
    Serial.println("The door has been closed.");
    delay(2000);
    digitalWrite(2,LOW);
    digitalWrite(3,LOW);
    digitalWrite(4,LOW);
    digitalWrite(5,LOW);
    delay(3600000); //Wait for 1 hour
  }
  else if (tm.Hour == t_open) //(tm.Hour == t_open)
  {
    limit_2_status=digitalRead(limit_2);
    while(limit_2_status==HIGH) //Rotate until the limit switch 2 is pressed
    {
      CoopStepper.step(-steps_to_move);
      limit_2_status=digitalRead(limit_2);
    }
    current_position=2;
    Serial.println("The door has been opened.");
    delay(2000);
    digitalWrite(2,LOW);
    digitalWrite(3,LOW);
    digitalWrite(4,LOW);
    digitalWrite(5,LOW);
    delay(3600000); //Wait for 1 hour
  }
}
