#include <FastLED.h>
#include <OneButton.h>
#include <Arduino.h>
#include <BLEMidi.h>


#define LED_PIN D13
#define NUM_LEDS 30
CRGB leds[NUM_LEDS];

#define MIDI_CHANNEL 1 // Std foot controller MIDI channel

#define BRIGHTNESS 255

const int DEBUG = 1; 

enum LoopBtnPressedStates {
  STOP,
  RECORD,
  PLAY
};
const char* loopStateStr[] = {"STOP", "RECORD", "PLAY"};

/////////////////////////////////////////////
// MIDI
byte midiCh = 1; //* MIDI channel to be used
byte note = 1; //* Lowest note to be used
byte cc = 1; //* Lowest MIDI CC to be used

/////////////////////////////////////////////
// Taskmanager to blink leds while not using a delay command
#define LIMIT 20
class BlinkManager {
  public:
    // C++ cannot handle dynamically assigned array, so we set up a bank of 20 reusable tasks
    // A single task can blink a single led a number of times across a set delay.
    // I have never managed to get more than 5 tasks at any one time in real life.
    struct timerData {
      int led;
      int pulsesLeft;
      int delay;
      unsigned long runTime;
      CRGB colour;
      CRGB currentColour;
      int beingUsed;
    };
    struct timerData timers[LIMIT];
    int numTasks = 0;

    void pulse(){ 
      if (numTasks >= 0) {
        for (int i = 0; i <= numTasks-1; i++) {
          if (timers[i].beingUsed == 1) {        
            if (millis() >= timers[i].runTime) {
              // This task is due to be run now
              if (timers[i].pulsesLeft > 0){
                // More pulses to come so just swap the colour to black or vise versa
                if (timers[i].currentColour == CRGB::Black){
                  leds[timers[i].led] = timers[i].colour;
                  timers[i].currentColour = timers[i].colour;
                } else {
                  leds[timers[i].led] = CRGB::Black;
                  timers[i].currentColour = CRGB::Black;
                }
                timers[i].runTime = timers[i].runTime + timers[i].delay;
                timers[i].pulsesLeft--;
              } else {
                // Last pulse so close it out to be reused by another task
                leds[timers[i].led] = CRGB::Black;
                timers[i].currentColour = CRGB::Black;
                timers[i].beingUsed = 0; // Allow the task to be reused
              }
            }
          }
        }
        FastLED.show();
      }
    }

    void addTask(int led, unsigned long startTime, int pulses, int delay, CRGB colour){
      int task = -1;
      boolean needNewTask = true;
      // find the first unused task
      for (int i = 0; i < numTasks; i++) {
        if (timers[i].beingUsed == 0) {
          if (task == -1) {
            task = i;
            needNewTask = false;
          }
        }
      }
      if (task == -1) {
        task = numTasks;
        needNewTask = true;
      }  
      timers[task].led = led;
      timers[task].runTime = startTime;
      timers[task].pulsesLeft = pulses*2;
      timers[task].delay = delay;
      timers[task].beingUsed = 1;
      timers[task].colour = colour;
      if (needNewTask){
        numTasks++;
      }   
    }

    void cancelTask(int led){
      for (int i = 0; i < numTasks; i++) {
        if (timers[i].led == led){
          timers[i].beingUsed = 0;
        }
      }
    }

    void debug(String msg) {
      if (DEBUG == 1) {
        Serial.println(msg);
      }
    }
};
BlinkManager blinkManager;

/////////////////////////////////////////////
class MidiButton {
  public:
    const int _pin;
    const int _led;
    OneButton _btn;
    LoopBtnPressedStates _pressedState = STOP;

    MidiButton(int pin, int led) : 
      _pin(pin),
      _led(led),
      _btn(OneButton(pin, true))
    {
    };

    void tick(){ _btn.tick(); }

    void debug(String msg) {
      if (DEBUG == 1) {
        Serial.println(msg);
      }
    }

  private:

};

// /////////////////////////////////////////////////////////////
class LoopButton: public MidiButton {
  public:
    bool _loopRecorded = false;
    int _midiCcPlayStop;
    int _midiCcRecord;
    int _midiCcClear;

    LoopButton(int pin, int led, int midiCcPlayStop) : 
      MidiButton(pin,led)
    {
      _btn.attachClick(clickCallBack, this);
      _btn.attachLongPressStart(longPressCallBack, this);
      this->_midiCcPlayStop = midiCcPlayStop;
      this->_midiCcRecord = midiCcPlayStop + 1;
      this->_midiCcClear = midiCcPlayStop + 2;
    }

    static void clickCallBack(void *ptr) {
      LoopButton *btnPtr = (LoopButton *)ptr;
      btnPtr->clickHandler();
    }
    static void longPressCallBack(void *ptr) {
      LoopButton *btnPtr = (LoopButton *)ptr;
      btnPtr->longPressHandler();
    }

  private:
    void clickHandler(){
      CRGB colour;
      //MIDI.sendNoteOn(69, 127, 1);
      Serial.println("Sending CC");
      debug("Loop buton pressed");
      switch (_pressedState) {
      case STOP:
        if (_loopRecorded){
          colour = CRGB::Green;
          _pressedState = PLAY;
          BLEMidiServer.controlChange(MIDI_CHANNEL, _midiCcPlayStop, 127);
        } else {
           colour = CRGB::Orange;
           _pressedState = RECORD;
           BLEMidiServer.controlChange(MIDI_CHANNEL, _midiCcRecord, 127);
        }
        break;
      case RECORD:
        // Playing
        colour = CRGB::Green;
        _pressedState = PLAY;
        _loopRecorded = true;
        BLEMidiServer.controlChange(MIDI_CHANNEL, _midiCcPlayStop, 127);
        break;
      default: //
        if (_loopRecorded){
          colour = CRGB::Blue;
        } else {
          colour = CRGB::Black;
        }
        _pressedState = STOP;
        BLEMidiServer.controlChange(MIDI_CHANNEL, _midiCcPlayStop, 127);
        break;
      }
      leds[_led] = colour;
      FastLED.show();
      if (_loopRecorded){       
      }
    };

    void longPressHandler(){
      _loopRecorded = false;
      _pressedState = STOP;
      BLEMidiServer.controlChange(MIDI_CHANNEL, _midiCcClear, 127); // Clear
      leds[_led] = CRGB::Black;
      FastLED.show();
    };
};

// ////////////////////////////////////////////////////
class PulseButton: public MidiButton {
  public:
    int _midiCcPulse;
    PulseButton(int pin, int led, int midiCcPulse) : 
      MidiButton(pin,led)
    {
      _btn.attachClick(clickCallBack, this);
      _btn.attachLongPressStart(longPressCallBack, this);
      this->_midiCcPulse = midiCcPulse;
    }

    static void clickCallBack(void *ptr) {
      PulseButton *btnPtr = (PulseButton *)ptr;
      btnPtr->clickHandler();
    }
    static void longPressCallBack(void *ptr) {
      PulseButton *btnPtr = (PulseButton *)ptr;
      btnPtr->longPressHandler();
    }
  private:
    void clickHandler(){
      debug("PulseButton clicked");
      BLEMidiServer.controlChange(MIDI_CHANNEL, _midiCcPulse, 127); // Clear
      blinkManager.addTask(_led, millis(), 1, 100, CRGB::Blue);
    };
    void longPressHandler(){
    };
};

// ////////////////////////////////////////////////////
class OnOffButton: public MidiButton {
  public:
  int onOff = 0;
    OnOffButton(int pin, int led) : 
      MidiButton(pin,led)
    {
      _btn.attachClick(clickCallBack, this);
      _btn.attachLongPressStart(longPressCallBack, this);
    }

    static void clickCallBack(void *ptr) {
      OnOffButton *btnPtr = (OnOffButton *)ptr;
      btnPtr->clickHandler();
    }
    static void longPressCallBack(void *ptr) {
      OnOffButton *btnPtr = (OnOffButton *)ptr;
      btnPtr->longPressHandler();
    }
  private:
    void clickHandler(){
      debug("OnOffButton clicked");
      if (onOff == 0){
        onOff = 1;
        leds[_led] = CRGB::Green;
      } else {
        onOff = 0;
        leds[_led] = CRGB::Black;
      }
      FastLED.show();
    };
    void longPressHandler(){
      //button6.cancelBlink();
    };
};

// ////////////////////////////////////////////////////
class BankButton: public MidiButton {
  public:
  int onOff = 0;
  int blinking = 0;
    BankButton(int pin, int led) : 
      MidiButton(pin,led)
    {
      _btn.attachClick(clickCallBack, this);
      _btn.attachLongPressStart(longPressCallBack, this);
    }

    static void clickCallBack(void *ptr) {
      BankButton *btnPtr = (BankButton *)ptr;
      btnPtr->clickHandler();
    }
    static void longPressCallBack(void *ptr) {
      BankButton *btnPtr = (BankButton *)ptr;
      btnPtr->longPressHandler();
    } 
    
  private:
    void clickHandler(){
      Serial.println("Bank button clicked");
      if (onOff == 0){
        onOff = 1;
        leds[_led] = CRGB::Green;
      } else {
        onOff = 0;
        leds[_led] = CRGB::Black;
      }
      FastLED.show();
    };
    void longPressHandler(){
      // connecting bluetooth now...
      if (blinking == 0){
        blinkManager.addTask(_led, millis(), 100, 100, CRGB::Aqua);
        blinking = 1;
      } else {
        blinkManager.cancelTask(_led);
        blinking = 0;
        onOff = 0;
        leds[_led] = CRGB::Black;
        FastLED.show();
      }
    };
};


LoopButton button1 = LoopButton(D2,0,10);
LoopButton button2 = LoopButton(D3,1,20);
//LoopButton button3 = LoopButton(D4,2,30);
//LoopButton button4 = LoopButton(D5,3,40);
PulseButton button5 = PulseButton(D6,4,50);
BankButton button6 = BankButton(D7,5);
PulseButton button7 = PulseButton(D8,6,70);
OnOffButton button8 = OnOffButton(D9,7);
//LoopButton button9 = LoopButton(D10,8);
//LoopButton button10 = LoopButton(D11,9);
LoopButton button11 = LoopButton(D12,10, 110);

volatile byte checkPins = false;

void IRAM_ATTR checkTicks() {
  checkPins = true;
}

void OnConnect() {
  Serial.println("Connected...");
  leds[12] = CRGB::Cyan;
  FastLED.show();
}

void OnDisconnect() {
  Serial.println("Disconnected...");
  leds[12] = CRGB::Red;
  FastLED.show();
}

void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp){
    Serial.printf("Control changexx : channel %d, controller %d, value %d (timestamp %dms)\n", channel, controller, value, timestamp);
}

///////////////////////////////////
void setup() {
  // Baud Rate: 115200 for Hairless MIDI
  Serial.begin(115200);
  delay(2000); // necessary or Serial wont run in the setup. Seems to be an ESP32 bug
    
  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, INPUT_PULLUP);
  pinMode(D4, INPUT_PULLUP);
  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
  pinMode(D8, INPUT_PULLUP);
  pinMode(D9, INPUT_PULLUP);
  pinMode(D10, INPUT_PULLUP);
  pinMode(D11, INPUT_PULLUP);
  pinMode(D12, INPUT_PULLUP);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  attachInterrupt(digitalPinToInterrupt(2), checkTicks, RISING);

  //OnDisconnect();
  BLEMidiServer.setOnConnectCallback(OnConnect);
  BLEMidiServer.setOnDisconnectCallback(OnDisconnect);
  BLEMidiServer.begin("Neil's MIDI Stomp");
  Serial.println("Waiting for connections...");
  //BLEMidiServer.setControlChangeCallback(onControlChange);
  BLEMidiServer.enableDebugging();
}

//  BLEMidiServer.noteOn(0, 69, 127); // look in the Midi.cpp file for all available commands

void loop() {
  if (checkPins){
    button1.tick();
    checkPins = false;
  }
  button2.tick();
//  button3.tick();
//  button4.tick();
  button5.tick();
  button6.tick();
  button7.tick();
  button8.tick();
//  button9.tick();
//  button10.tick();
  button11.tick();
  blinkManager.pulse();

//  if(BLEMidiServer.isConnected()) {
//    OnConnected();
//  } else {
//    OnDisconnected();
//  }
}
