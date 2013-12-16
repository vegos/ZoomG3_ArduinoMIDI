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
// - Too bored to write down my TO-DO list... :)
//

#include <Usb.h>
#include <usbhub.h>
#include <usbh_midi.h>
#include <LiquidCrystal.h>

// I had to modify the LCD Shield, due to pin conflict with the USB Host Shield.
//
// Solution:
// I totally removed the pins D8, D9, D10 (unsoldered) from the LCD Shield.
// I jumper the pin D3 to D8 and D2 to D9.
// The D10 was used for controlling the backlit (?). I left it unconnected.

LiquidCrystal lcd(3, 2, 4, 5, 6, 7);

int KeyboardInput  = 0;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


USB  Usb;
MIDI  Midi(&Usb);

byte Preset;


void setup()
{
//  Workaround for non UHS2.0 Shield 
//  pinMode(7,OUTPUT);
//  digitalWrite(7,HIGH);

  lcd.begin(16, 2);              // start the library
  if (Usb.Init() == -1) 
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("USB Host Shield ");
    lcd.setCursor(0,1);
    lcd.print(" ** FAILED! **  ");
    while(1);               // Halt
  }
  delay(200);
  Preset = 1; 
  lcd.setCursor(0,0);
  lcd.print("     MAGLA     ");
  lcd.setCursor(0,1);
  lcd.print(" MIDI over USB ");
  delay(1500);
  SendMIDI(0);
  lcd.setCursor(0,0);
  lcd.print("Current Patch   ");
  lcd.setCursor(0,1);
  lcd.print("Go to patch     ");
  DisplayGoTo(Preset-1);
  DisplayPatch(Preset-1);
}

void loop()
{
  switch (read_LCD_buttons())
  {
    case btnLEFT:          
      Preset-=1;
      if (Preset<1)
        Preset=100;
      DisplayGoTo(Preset-1);
      delay(100);
      break;
    case btnRIGHT:
      Preset+=1;
      if (Preset>100)
        Preset=1;
      DisplayGoTo(Preset-1);
      delay(100);
      break;
    case btnSELECT:
      SendMIDI(Preset-1);
      DisplayPatch(Preset-1);
      delay(100);
      break;
    case btnDOWN:
      Preset-=1;
      if (Preset<1)
        Preset=100;
      SendMIDI(Preset-1);
      DisplayGoTo(Preset-1);
      DisplayPatch(Preset-1);
      delay(100);   
      break;
    case btnUP:
      Preset+=1;
      if (Preset>100)
        Preset=1;
      SendMIDI(Preset-1);
      DisplayGoTo(Preset-1);
      DisplayPatch(Preset-1);
      delay(100);   
      break;
  }
}


// Send "Program Change" MIDI Message

void SendMIDI(byte number)
{
  Usb.Task();
  if( Usb.getUsbTaskState() == USB_STATE_RUNNING )
  { 
    byte Message[2];
    Message[0]=0xC0;
    Message[1]=number;
    Midi.SendData(Message);
    delay(10); 
  } 
}


void DisplayGoTo(byte number)
{
  byte first = number/10;
  byte second = number % 10;
  lcd.setCursor(14,1);
  lcd.print(char(65+first));
  lcd.print(second);    
}


void DisplayPatch(byte number)
{
  byte first = number/10;
  byte second = number % 10;
  lcd.setCursor(14,0);
  lcd.print(char(65+first));
  lcd.print(second);    
}


int read_LCD_buttons()
{
 KeyboardInput = analogRead(0);      // read the value from the sensor
 // my buttons when read are centered at these valies: 0, 131, 306, 478, 720
 // we add approx 50 to those values and check to see if we are close
 if (KeyboardInput > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (KeyboardInput < 50)   return btnRIGHT; 
 if (KeyboardInput < 185)  return btnUP;
 if (KeyboardInput < 360)  return btnDOWN;
 if (KeyboardInput < 530)  return btnLEFT;
 if (KeyboardInput < 770)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}
