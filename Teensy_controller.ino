
/*
  This program is for use with the teensy 2.0
  and creates a USB Joystick + Keyboard that
  can be used with just about any computer.
  
*/
/*
  Teensy 2.0 Pinout:
  Pins 0 - 10, 23 and 24 are digital I/O.
  Pins 11 - 21 (A10 - A0) and 22 (A11) are digital (analog) I/O     
*/


/*Defining the pin functions:


 # |       Name       |  Input
--------------------------------
1  |  D-Pad - UP      |    0
2  |  D-Pad - LEFT    |    1
3  |  D-Pad - DOWN    |    2
4  |  D-Pad - RIGHT   |    3
5  |  Action - 1      |    4
6  |  Action - 2      |    5
7  |  Action - 3      |    6
8  |  Action - 4      |    7
9  |  Action - 5      |    8
10 |  Action - 6      |    9
11 |  Shoulder - L1   |   10
12 |  Shoulder - L2   |   11
13 |  Shoulder - R1   |   12
14 |  Shoulder - R2   |   13
15 |  Start           |   14
16 |  Select          |   15
17 |  Volume - UP     |   16
18 |  Volume - DOWN   |   17
19 |  Analog - X      | A0(21)
20 |  Analog - Y      | A1(20)
          
*/
#include <Bounce.h>

const int numButtons = 18; //For Pins 0 - 17
const int actionButtons = 12;//Volume and D-Pad removed; Pins 4 - 15
byte allButtons[actionButtons];
int angle = 0;//i dont think i need this

//Volume buttons, 2 ms delay
Bounce button16 = Bounce(16, 2);
Bounce button17 = Bounce(17, 2);

void setup() {
  // configure the joystick to manual send mode.  This gives precise
  // control over when the computer receives updates, but it does
  // require you to manually call Joystick.send_now().
  Joystick.useManualSend(true);

  //initialize control over the keyboard
  Keyboard.begin();

  //Set pins as inputs with pullups.
  //HIGH --> off
  //LOW  --> on
  for (int i=0; i<numButtons; i++) {
    pinMode(i, INPUT_PULLUP);
  }
}



void loop() {
  /* Check each button for "falling" edge.
     falling = high (not pressed - voltage from pullup resistor)
               to low (pressed - button connects pin to ground)
  */
  
  //Volume Buttons
  button16.update();
  button17.update();
  
  if (button16.fallingEdge()) {
    //left_ctrl --> 0x80
    //left_shift --> 0x81
    Keyboard.press(0x80 | 0x81);
    Keyboard.press('=');
    Keyboard.releaseAll();
  }
  
  if (button17.fallingEdge()) {
    Keyboard.press(0x80);
    Keyboard.press('-');
    Keyboard.releaseAll();
  }

  // read 2 analog inputs and use them for the joystick axis
  Joystick.X(analogRead(0));
  Joystick.Y(analogRead(1));

  // read digital pins and use them for the buttons
  for (int i = 0; i < actionButtons; i++) {
    if (digitalRead(i + 4)) {
      // when a pin reads high, the button is not pressed
      // the pullup resistor creates the "on" signal
      allButtons[i] = 0;
    } else {
      // when a pin reads low, the button is connecting to ground.
      allButtons[i] = 1;
    }
    Joystick.button(i + 1, allButtons[i]);
  } 
   
  //read the D-Pad (Hat) inputs 0-3
  angle = hatAngle();
  Joystick.hat(angle);

  // Transmit all Joystick inputs and a brief delay, so this runs "only" 500 times per second
  Joystick.send_now();
  delay(2);
}


int hatAngle() {
/*read the D-Pad (Hat) inputs 0-3

|     |                  |         |           |
|  #  |       Name       |  Input  |   Angle   |
------------------------------------------------
|  1  |  D-Pad - UP      |    0    |     90    |
|  2  |  D-Pad - LEFT    |    1    |    180    |
|  3  |  D-Pad - DOWN    |    2    |    270    |
|  4  |  D-Pad - RIGHT   |    3    |      0    |

Hat angle can be: 0,45,90,135,180,225,270,315
Set to -1 for neutral position
*/

  int buttons[] = {90, 180, 270, 0};
  int hatangle = 0;
  int right = 0;
  int count = 0;

  //read buttons
  for (int i = 0; i > 3; i ++) {
    if (digitalRead(i)==LOW){
      count = count + 1;
      //Right button flag
      if (buttons[i] == 0) {
        right = 1;
      }
      hatangle = hatangle + buttons[i];
    }
  }

  //Down right --> When right and down are pressed add 360 to angle
  if (right == 1 && count == 2 && hatangle == 270) {
    hatangle = hatangle + 360;
  }

  //Average angle OR set to -1
  if (count == 1 || count == 2) {
    hatangle = hatangle / count;
  }else{
    hatangle = -1;
  }
  return hatangle;
}

