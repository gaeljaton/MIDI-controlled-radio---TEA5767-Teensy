#include <MIDI.h>
#include <Wire.h>

unsigned char frequencyH = 0;
unsigned char frequencyL = 0;

unsigned int frequencyB;
unsigned char mute = 0x58;
double frequency = 0;
float freq1 = 0;
float minFreq = 89.0;
float maxFreq = 105.0;
int numKeys = 127;
int currentKey = 0;
float scaleunit = .33;
int firstkey = 1+(int((127-numKeys)/2));
int numVoices = 0;
void setup()
{
  scaleunit=(maxFreq-minFreq)/numKeys;
  pinMode(13,OUTPUT);
  Wire.begin();
  frequency = 83.0; //starting frequency
  setFrequency();
  MIDI.begin(MIDI_CHANNEL_OMNI);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn); 
  Serial.begin(9600);
}

void loop()
{
  frequency = minFreq+((currentKey-firstkey)*scaleunit);
  freq1 = (int(frequency*10))/10.0;
  setFrequency();
  
int type, note, velocity, channel, d1, d2;
  if (MIDI.read()) {   
  // Is there a MIDI message incoming ?
    byte type = MIDI.getType();
    switch (type) {
      case NoteOn:
        note = MIDI.getData1();
        velocity = MIDI.getData2();
        channel = MIDI.getChannel();
        if (velocity > 0) {
          currentKey = note;
          numVoices++;
        } else {
         //   numVoices--;
        }
        break;
        case NoteOff:
          numVoices--;
        break;    
      default:
        d1 = MIDI.getData1();
        d2 = MIDI.getData2();   
    }
    Serial.println(frequency);
  }



if (numVoices > 0) {
digitalWrite(13,HIGH);
  mute = 0x10;
}
 else {
   mute = 0x58;
         digitalWrite(13,LOW);
 }
    usbMIDI.read();

}

void setFrequency()
{
  frequencyB = 4 * (freq1 * 1000000 + 225000) / 32768;
  frequencyH = frequencyB >> 8;
  frequencyL = frequencyB & 0XFF;
  delay(5);

  Wire.beginTransmission(0x60);
  Wire.write(frequencyH);
  Wire.write(frequencyL);
  Wire.write(0xB0);
  Wire.write(mute);
  Wire.write((byte)0x00);
  Wire.endTransmission();
  delay(5);  

}

void OnNoteOn(byte channel, byte note, byte velocity) {
currentKey = note;
numVoices++;

}


void OnNoteOff(byte channel, byte note, byte velocity) {
numVoices--;  
}
