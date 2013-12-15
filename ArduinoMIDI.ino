//
//   ZOOM G3 Patch Change using MIDI over USB
//           (c)2013, Antonis Maglaras
//              maglaras@gmail.com
//
// Library Used:
// https://github.com/YuuichiAkagawa/USBH_MIDI
//
//
// To Do:
// - Some push buttons for quick access to specific patches (programmable)
// - LCD Screen for displaying information
// - ...
//

#include <Usb.h>
#include <usbhub.h>
#include <usbh_midi.h>

USB  Usb;
MIDI  Midi(&Usb);

#define PotPin A0
byte Preset;


void setup()
{
  //Workaround for non UHS2.0 Shield 
//  pinMode(7,OUTPUT);
//  digitalWrite(7,HIGH);

  if (Usb.Init() == -1) 
  {
    while(1); //halt
  }//if (Usb.Init() == -1...
  delay(200);
  Serial.begin(115200);
  Serial.println("STARTED");
  Preset = map(analogRead(PotPin),0,1023,0,99);
}

void loop()
{
  byte tmp=map(analogRead(PotPin),0,1023,0,99);
  if (tmp != Preset)
  {
    Usb.Task();
    if( Usb.getUsbTaskState() == USB_STATE_RUNNING )
    {
      Preset=tmp;
      PrintNumber(Preset);
      byte Message[2];
      Message[0]=0xc0;
      Message[1]=Preset;
      Midi.SendData(Message);
      delay(250);
    }
  }
}

void PrintNumber(byte number)
{
  byte first = number/10;
  byte second = number % 10;
  Serial.print("Sending program (");
  Serial.print(number);
  Serial.print("): ");
  Serial.print(char(65+first));
  Serial.println(second);
}
