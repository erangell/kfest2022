//MIDI IN CAPTURE, FILTER, TRANSFORM, OUTPUT

//Requires curcuitry for MIDI IN feeding in to Serial1 Rx1
//MIDI OUT is sent from Serial1 Tx1

bool circbuf=1; //maintain a circular buffer of midi data received
bool hexdump=1; //dump midi in data to serial monitor (requires circbuf=1)

bool filterAftertouch=1;  //filter aftertouch messages
bool filterCtrl=0;        //filter control change messages
bool filterPgm=1;         //filter program change messages
bool filterPressure=1;    //filter channel pressure messages
bool filterBend=1;        //filter pitch bend messages

bool filterChannel[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

bool transformAftertouchToVolume = 0;       //transformation for wind controller
bool transformChannelPressureToVolume = 0;  //transformation for wind controller

byte inbyte;  //current midi in byte
int state = 0;  //state machine

int bufbytes = 1000;  //size of circular buffer
byte midibuf[1000];   //circular buffer
int ixhead=0;         //bytes added at head
int ixtail=0;         //bytes removed at tail

int noteDown = LOW; //note on or note off
int note;

int currchnl;
int data1;

void setup() {
    Serial.begin(9600);
    Serial.println("Please wait 3 seconds"); //prevent problems when uploading sketch
    
    Serial1.begin(31250);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN,HIGH);
    
    delay(3000);
    
    digitalWrite(LED_BUILTIN,LOW);
    Serial.println("OK to play now");
}

void loop() 
{
  if (Serial1.available() > 0)
  {
    inbyte = Serial1.read();
    if (inbyte > 0xF0)
    {
      state=0xFF;  //ignore all realtime messages
    }
    else if (inbyte == 0xF0)
    {
      state=0xF0;  //ignore sysex messages
    }
    else
    {    
      if (circbuf)
      {
        midibuf[ixhead]=inbyte;
        ixhead++;
        if (ixhead>=bufbytes)
        {
          ixhead = 0;
        }
      }
    }            
    switch(state) {
        case 0:
          doState0();
          break;          
          
        case 0x91:
          if (inbyte < 0x80)
          {
            note = inbyte;
            state = 0x92;
          }
          else  //if status byte received: cancel running status and process status byte 
          {
            state = 0;
            doState0();
          }
          break;
          
        case 0x92:
          if (inbyte < 0x80)
          {
            doNote(note, inbyte, noteDown);
          }
          state = 0x91;  //may have running status
          break;

        case 0xA1:
          if (inbyte < 0x80)
          {
            data1 = inbyte;
            state = 0xA2;
          }
          else  //if status byte received: cancel running status and process status byte 
          {
            state = 0;
            doState0();
          }          
          break;
          
        case 0xA2:
          if (inbyte < 0x80)
          {
            do2ByteMsg(0xA0+currchnl, data1, inbyte);
          }
          state = 0xA1;  //may have running status          
          break;        
          
        case 0xB1:
          if (inbyte < 0x80)
          {
            data1 = inbyte;
            state = 0xB2;
          }
          else  //if status byte received: cancel running status and process status byte 
          {
            state = 0;
            doState0();
          }                    
          break;
          
        case 0xB2:          
          if (inbyte < 0x80)
          {
            do2ByteMsg(0xB0+currchnl, data1, inbyte);
          }
          state = 0xB1;  //may have running status          
          break;       
          
        case 0xC1:
          if (inbyte < 0x80)
          {
            do1ByteMsg(0xC0+currchnl, inbyte);
          }
          state = 0xC1;  //may have running status          
          break;     
          
        case 0xD1:          
          if (inbyte < 0x80)
          {
            do1ByteMsg(0xD0+currchnl, inbyte);
          }
          state = 0xD1;  //may have running status          
          break;     
          
        case 0xE1:
          if (inbyte < 0x80)
          {
            data1 = inbyte;
            state = 0xE2;
          }
          else  //if status byte received: cancel running status and process status byte 
          {
            state = 0;
            doState0();
          }                    
          break;
                    
        case 0xE2:     
          if (inbyte < 0x80)
          {
            do2ByteMsg(0xE0+currchnl, data1, inbyte);
          }
          state = 0xE1;  //may have running status          
          break;                 
                    
        case 0xF0:
          if (inbyte >= 0x80)  //any status byte (not just F7) ends sysex message
          {
            state = 0;
            doState0();
          }
          break;
        
        case 0xFF:
          if (inbyte >= 0x80)  //any status byte ends realtime message
          {
            state = 0;
            doState0();
          }
          break;
          
    } //switch    
  } //if Serial1 available 
  else  //can do work here while waiting for next message
  {
    if (hexdump)
    {
      while (ixtail != ixhead)
      {
        if (midibuf[ixtail] >= 0x80)
        {
          Serial.println();
        }
        Serial.print(midibuf[ixtail],HEX);
        Serial.print(" ");
        ixtail++;
        if (ixtail >= bufbytes)
        {
          ixtail=0;
        }
      } //while
    } //if hexdump
  } //else
} //loop

void doState0 ()
{
    //stay in state 0 until state changed or a supported status byte received
    if ((inbyte &0xf0) == 0x80)
    {
       noteDown = LOW;
       state = 0x91;
    }
    else if ((inbyte & 0xf0) == 0x90)
    {
       noteDown = HIGH;
       state = 0x91;    
    }
    else if ((inbyte & 0xf0) == 0xA0)
    {
       if (!filterAftertouch)
       {
          currchnl = inbyte & 0x0F;
          state = 0xA1;
       }
    }
    else if ((inbyte & 0xf0) == 0xB0)
    {
       if (!filterCtrl)
       {
          currchnl = inbyte & 0x0F;
          state = 0xB1;
       }
    }
    else if ((inbyte & 0xf0) == 0xC0)
    {
       if (!filterPgm)
       {
          currchnl = inbyte & 0x0F;
          state = 0xC1;
       }
    }    
    else if ((inbyte & 0xf0) == 0xD0)
    {
       if (!filterPressure)
       {
          currchnl = inbyte & 0x0F;
          state = 0xD1;
       }
    }
    else if ((inbyte & 0xf0) == 0xE0)
    {
       if (!filterBend)
       {
          currchnl = inbyte & 0x0F;
          state = 0xE1;
       }
    }
}

void doNote (byte note, byte velocity, int down) {
  if ((down == HIGH) && (velocity == 0))
  {
    down = LOW;
  }
  if (down == LOW)
  {
    digitalWrite(LED_BUILTIN, LOW);
    Serial1.write(0x90);
    Serial1.write(note);
    Serial1.write(0);
  }
  else
  {
    digitalWrite(LED_BUILTIN,HIGH);
    Serial1.write(0x90);
    Serial1.write(note);
    Serial1.write(velocity);
  }
}

void do2ByteMsg (byte stsbyte, byte dbyte1, int dbyte2) {    
    Serial1.write(stsbyte);
    Serial1.write(dbyte1);
    Serial1.write(dbyte2);
}

void do1ByteMsg (byte stsbyte, byte dbyte1) {    
    Serial1.write(stsbyte);
    Serial1.write(dbyte1);    
}

