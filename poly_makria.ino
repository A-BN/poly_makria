/*
     Welcome to the drone

*/

//#include <ADC.h>  // Teensy 3.1 uncomment this line and install http://github.com/pedvide/ADC
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <mozzi_rand.h> // To check.
#include <mozzi_fixmath.h>

#include <tables/sin2048_int8.h> // sine table for oscillator
#include <tables/square_no_alias_2048_int8.h> // square analogue table for oscillator
#include <tables/saw2048_int8.h>
#include <tables/cos2048_int8.h> // table for vibrato

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> OSC_1(SIN2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> OSC_2(SAW2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> OSC_3(SAW2048_DATA);

Oscil <SIN2048_NUM_CELLS, CONTROL_RATE> VIB_1(SIN2048_DATA);

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // powers of 2 please


// mien
const int KNOB_1_PIN = 0;
const int KNOB_2_PIN = 1;
const int KNOB_3_PIN = 2;

const int KNOB_v_PIN = 3;

const int BUTTON_1_PIN = 2;


int knob_1 = 2;
int knob_2 = 2;
int knob_3 = 2;
int knob_v = 0;

// Button
byte but_1_press = 0;
bool but_1_prev_state = true;
bool but_1_curr_state = false;
bool but_1_reading;
unsigned long bounce_time = 0;
// ___________________

// Vibrato !
float vib_freq = 1.0; // t in the equation
float vib_depth = 0.02;
float vib; 
// Vibrato

long out;
// _____________

void setup() {
  startMozzi(CONTROL_RATE); 
  
  pinMode(BUTTON_1_PIN, INPUT);

  Serial.begin(115200);

  // _____________
}


void updateControl() {
  // put changing controls in here
  // mien


  /*
    Serial.print(button_1_state);
    Serial.print('\t');
    Serial.println(digitalRead(BUTTON_1_PIN));
  */


  int knob_1 = mozziAnalogRead(KNOB_1_PIN);
  int knob_2 = mozziAnalogRead(KNOB_2_PIN);
  int knob_3 = mozziAnalogRead(KNOB_3_PIN);

  int knob_v = mozziAnalogRead(KNOB_v_PIN);

  knob_1 = map(knob_1, 0, 1023, 20, 500);
  knob_2 = map(knob_2, 0, 1023, 20, 500);
  knob_3 = map(knob_3, 0, 1023, 20, 500);
  knob_v = map(knob_v, 0, 1023, 100, 10000);
  
  vib_freq = float(knob_v) / 100;


  VIB_1.setFreq(vib_freq);
  vib = vib_depth * VIB_1.next();
   
  OSC_1.setFreq((float) knob_1 + vib);
  //OSC_1.setFreq(knob_1 + (int)vib_amount);
  OSC_2.setFreq(knob_2);
  OSC_3.setFreq(knob_3);


// OSC_1 wave selection
  but_1_curr_state = digitalRead(BUTTON_1_PIN);
  if (but_1_curr_state && but_1_prev_state &&  audioTicks() - bounce_time > 8000) {
    but_1_prev_state = but_1_curr_state;
    bounce_time = audioTicks();
    but_1_press +=1;
    if (but_1_press > 2) {
      but_1_press = 0;
    }
  }


  if (rand(0,16) == 0) {
    //Serial.println("______");
    //Serial.println(vib_time);
    //Serial.println(vib_nstep);
    //Serial.println(vib_cur_step);
    //Serial.println(vib_amount);
    Serial.println(vib_freq);
    //Serial.println("______");
  }  
}


int updateAudio() {

  switch(but_1_press) {
    case 0:
     OSC_1.setTable(SIN2048_DATA);
    break;
    case 1:
      OSC_1.setTable(SQUARE_NO_ALIAS_2048_DATA);;
    break;
    case 2:
    OSC_1.setTable(SAW2048_DATA);;
    break;
  }
    
    //out = OSC_1.next() >> 1;
    out = (OSC_1.next() + OSC_2.next() + OSC_3.next()) >> 2; // bit shifting for division by
    return (out);


}


void loop() {
  audioHook(); // required here
}
