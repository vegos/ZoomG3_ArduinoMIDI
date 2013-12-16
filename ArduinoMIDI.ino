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

int KeyboardInput = 0;
#define KbdPin     A0
#define btnRIGHT    0
#define btnUP       1
#define btnDOWN     2
#define btnLEFT     3
#define btnSELECT   4
#define btnNONE     5

USB  Usb;
MIDI  Midi(&Usb);

byte Preset;

void setup()
{
  lcd.begin(16, 2);                  // start the library
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
  DisplayAtLine(Preset,0);
  DisplayAtLine(Preset,1);
}


void loop()
{
  switch (read_LCD_buttons())        // Read if a button is pressed
  {
    case btnLEFT:                    // LEFT: Choose the previous patch
      Preset-=1;
      if (Preset<1)
        Preset=100;
      DisplayAtLine(Preset,1);
      delay(100);
      break;
    case btnRIGHT:                   // RIGHT: Choose the next patch
      Preset+=1;
      if (Preset>100)
        Preset=1;
      DisplayAtLine(Preset,1);
      delay(100);
      break;
    case btnSELECT:                  // SELECT: Activate the current patch
      SendMIDI(Preset);
      DisplayAtLine(Preset,0);
      delay(100);
      break;
    case btnDOWN:                    // DOWN: Activate the previous patch
      Preset-=1;
      if (Preset<1)
        Preset=100;
      SendMIDI(Preset);
      DisplayAtLine(Preset,0);
      DisplayAtLine(Preset,1);
      delay(100);   
      break;
    case btnUP:                       // UP: Activate the next patch
      Preset+=1;
      if (Preset>100)
        Preset=1;
      SendMIDI(Preset);
      DisplayAtLine(Preset,0);
      DisplayAtLine(Preset,1);
      delay(100);   
      break;
  }
}


// Send "Program Change" MIDI Message

void SendMIDI(byte number)
{
  number-=1;                         // Change 1 to 0, 100 to 99.
  Usb.Task();
  if( Usb.getUsbTaskState() == USB_STATE_RUNNING )
  { 
    byte Message[2];                 // Construct the midi message (2 bytes)
    Message[0]=0xC0;                 // 0xC0 is for Program Change (Change to MIDI channel 0)
    Message[1]=number;               // Number is the program/patch (Only 0 to 99 is valid for ZOOM G3)
    Midi.SendData(Message);          // Send the message
    delay(10); 
  } 
}


void DisplayAtLine(byte number, byte line)
{
  number-=1;                         // Change 1 to 0, 100 to 99.  
  byte first = number/10;            // Get the first digit
  byte second = number % 10;         // Get the second digit
  lcd.setCursor(14,line);
  lcd.print(char(65+first));         // Print the char (A to J for 0-9)
  lcd.print(second);                 // Print the second digit
}



int read_LCD_buttons()
{
 KeyboardInput = analogRead(KbdPin); // Read the value from the keyboard
                                     // Buttons when read are centered at these valies: 0, 131, 306, 478, 720
                                     // We add approx 50 to those values and check to see if we are close
 if (KeyboardInput > 1000) return btnNONE;     // We make this the 1st option for speed reasons since it will be the most likely result
 if (KeyboardInput < 50)   return btnRIGHT; 
 if (KeyboardInput < 185)  return btnUP;
 if (KeyboardInput < 360)  return btnDOWN;
 if (KeyboardInput < 530)  return btnLEFT;
 if (KeyboardInput < 770)  return btnSELECT;  
 return btnNONE;                     // When all others fail, return this...
}
