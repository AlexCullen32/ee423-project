#include <LiquidCrystal.h>
#define DELAY 1000

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);          //Initiallize LCD pins

int Countdown = 31;                                 //Default game length 30 seconds
int introDelay = 0;                                 //Delay for into (default null)

//LED pins:
const int LED_ONE = 9;
const int LED_TWO = 8;
const int LED_THREE = 7;
const int LED_FOUR = 6;

//Button pins
const int BUTTON_ONE = A0;
const int BUTTON_TWO = A1;
const int BUTTON_THREE = A2;
const int BUTTON_FOUR = A3;

double firstLEDPress = 0;
double secondLEDPress = 0;
double thirdLEDPress = 0;
double fourthLEDPress = 0;
//double LEDPress = 0;

//Button states
int buttonStateOne = 0;
int buttonStateTwo = 0;
int buttonStateThree = 0;
int buttonStateFour = 0;

//LED States
int ledStateOne = 0;
int ledStateTwo = 0;
int ledStateThree = 0;
int ledStateFour = 0;

//Initialize calculation/game variables
int OLD_LED;
int NEXT_LED = 0;
double CURRENT_STATE = 0;
double NEXT_STATE = 0;
double led_on = 0;
int firstTime = 0;
double difference = 0;
int playGame = 0;
int averageCount = 0;
double average = 0;
double eachTime = 0;
double newMillis = 0;

//Define time function
void timeFunction(int *LED_OLD, int buttonState, int ledState, int *nextLed);

void setup() {
  Serial.begin(2000000);                            //Baud rate 2000000

  lcd.begin(16, 2);                                 //Set up the LCD's number of columns and rows:

  lcd.print("  Purdue Test!");                      //Print game name to the LCD

  //Set up LEDs
  pinMode(LED_ONE, OUTPUT);
  pinMode(LED_TWO, OUTPUT);
  pinMode(LED_THREE, OUTPUT);
  pinMode(LED_FOUR, OUTPUT);
  
  //Set up buttons using pull-up resistors
  pinMode(BUTTON_ONE, INPUT_PULLUP);
  pinMode(BUTTON_TWO, INPUT_PULLUP);
  pinMode(BUTTON_THREE, INPUT_PULLUP);
  pinMode(BUTTON_FOUR, INPUT_PULLUP);

  delay(1000);                                      //Delay 1 second

  cli();                                            //Stop interrupts

  //Set timer1 interrupt at 1Hz (1 second)
  TCCR1A = 0;                                       //Set entire TCCR1A register to 0
  TCCR1B = 0;                                       //Same for TCCR1B
  TCNT1  = 0;                                       //Initialize counter value to 0
  //Set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  //Turn on CTC mode
  TCCR1B |= (1 << WGM12);
  //Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  //Enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();                                            //Allow interrupts
}

ISR(TIMER1_COMPA_vect) {                            //Timer1 interrupt 1Hz toggles pin 13 (LED)
//Generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)

  if (Countdown >= 0 && introDelay == 1) {          //Checks if game is active

    Countdown = Countdown - 1;                      //Time reduces by 1 second
    if (Countdown >= 0) {                           //Checks if game is still active
      lcd.setCursor(0, 0);
      lcd.print("Time Remaining:");
      lcd.setCursor(0, 1);

      lcd.print("       ");
      lcd.print(Countdown);                         //Prints current time to LCD
      lcd.print("       ");
    }
  }
}

void loop() {

    while (playGame == 0) {                         //Checks to see if game has been started (playgame == 1)

    //Reads states of buttons
    buttonStateOne = digitalRead(BUTTON_ONE);
    buttonStateTwo = digitalRead(BUTTON_TWO);
    buttonStateThree = digitalRead(BUTTON_THREE);
    buttonStateFour = digitalRead(BUTTON_FOUR);
    
    if (buttonStateOne == LOW) {                    //If button 1 is pressed

      digitalWrite(LED_ONE, HIGH);
      
      lcd.setCursor(0, 0);
      lcd.print("Score:    ");
      lcd.print(averageCount);                      //Prints previous games score
      lcd.print("     ");
      lcd.setCursor(0, 1);

      lcd.print("Average: ");
      lcd.print(average);                           //Prints previous games average score
      lcd.print(" s");
    }

    if (buttonStateTwo == LOW) {                    //If button 2 is pressed
      digitalWrite(LED_ONE, LOW);
      digitalWrite(LED_TWO, HIGH);
      Countdown = Countdown + 5;                    //Increase game length by 5 seconds

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("New Game Length:");
      lcd.setCursor(0,1);
      lcd.print("       ");
      lcd.print(Countdown-1);                       //Print new game length
      
      delay(500);                                   //500 ms delay for button debouncing
      //Serial.println(Countdown);
      digitalWrite(LED_TWO, LOW);
    }

    if (buttonStateThree == LOW && Countdown > 5) { //If button 3 is pressed
      digitalWrite(LED_ONE, LOW);
      digitalWrite(LED_THREE, HIGH);
      Countdown = Countdown - 5;                    //Decrease game length by 5 seconds

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("New Game Length:");
      lcd.setCursor(0,1);
      lcd.print("       ");
      lcd.print(Countdown-1);                       //Print new game length
      
      delay(500);                                   //500 ms delay for button debouncing

      digitalWrite(LED_THREE, LOW);
    }


    if (buttonStateFour == LOW) {                   //If button 4 pressed
      playGame = 1;                                 //Start game
      newMillis = millis();                         //Sets time since device was last reset
      firstTime = 0;                  

      lcd.clear();                                  //Clear LCD
    }
  }

  playGame = 0;                                     //Reset start game condition
  averageCount = 0;                                 //Reset number of buttons
  average = 0;                                      //Reset average time
  eachTime = 0;                                     //Resets eachTime

  //Start sequence:
  digitalWrite(LED_ONE, HIGH);
  digitalWrite(LED_TWO, HIGH);
  digitalWrite(LED_THREE, HIGH);
  digitalWrite(LED_FOUR, HIGH);

  lcd.setCursor(3, 1);
  Serial.println("Get Ready!");
  Serial.println("4");
  delay(DELAY);
  digitalWrite(LED_ONE, LOW);
  lcd.setCursor(3, 1);
  lcd.print("Start in 3");
  Serial.println("3");
  delay(DELAY);
  digitalWrite(LED_TWO, LOW);
  lcd.setCursor(3, 1);
  lcd.print("Start in 2");
  Serial.println("2");
  delay(DELAY);
  digitalWrite(LED_THREE, LOW);
  lcd.setCursor(3, 1);
  lcd.print("Start in 1");
  Serial.println("1");

  introDelay = 1;                                   //Delay condition set to 1

  delay(DELAY);
  digitalWrite(LED_FOUR, LOW);
  Serial.println("GO!");
  Serial.println("");

  digitalWrite(LED_ONE, HIGH);
  OLD_LED = LED_ONE;                                //LED 1 is always 1st LED
  NEXT_LED = LED_ONE;                               //Until function is called 2nd LED is alos LED 1
  led_on = millis();
  unsigned long programStart = millis();            //Start of program

  while (Countdown >= 0) {                          //While game is active

    //Read button states
    buttonStateOne = digitalRead(BUTTON_ONE);
    buttonStateTwo = digitalRead(BUTTON_TWO);
    buttonStateThree = digitalRead(BUTTON_THREE);
    buttonStateFour = digitalRead(BUTTON_FOUR);

    //Read LED states
    ledStateOne = digitalRead(LED_ONE);
    ledStateTwo = digitalRead(LED_TWO);
    ledStateThree = digitalRead(LED_THREE);
    ledStateFour = digitalRead(LED_FOUR);

    //Conditions to check what LED is lit
    //Calls time function with parameters based on lit LED
    if (NEXT_LED == LED_ONE) {
      timeFunction(&OLD_LED, buttonStateOne, ledStateOne, &NEXT_LED);
    }
    else if (NEXT_LED == LED_TWO) {
      timeFunction(&OLD_LED, buttonStateTwo, ledStateTwo, &NEXT_LED);
    }
    else if (NEXT_LED == LED_THREE) {
      timeFunction(&OLD_LED, buttonStateThree, ledStateThree, &NEXT_LED);
    }
    else if (NEXT_LED == LED_FOUR) {
      timeFunction(&OLD_LED, buttonStateFour, ledStateFour, &NEXT_LED);
    }
  }

  introDelay = 0;           //Intro delay set to 0
  Countdown = 31;           //Default Game Length

  //End game sequence
  digitalWrite(LED_ONE, HIGH);
  digitalWrite(LED_TWO, HIGH);
  digitalWrite(LED_THREE, HIGH);
  digitalWrite(LED_FOUR, HIGH);
  delay(200);
  digitalWrite(LED_ONE, LOW);
  digitalWrite(LED_TWO, LOW);
  digitalWrite(LED_THREE, LOW);
  digitalWrite(LED_FOUR, LOW);
  delay(200);
  digitalWrite(LED_ONE, HIGH);
  digitalWrite(LED_TWO, HIGH);
  digitalWrite(LED_THREE, HIGH);
  digitalWrite(LED_FOUR, HIGH);
  delay(200);
  digitalWrite(LED_ONE, LOW);
  digitalWrite(LED_TWO, LOW);
  digitalWrite(LED_THREE, LOW);
  digitalWrite(LED_FOUR, LOW);
  delay(200);
  digitalWrite(LED_ONE, HIGH);
  digitalWrite(LED_TWO, HIGH);
  digitalWrite(LED_THREE, HIGH);
  digitalWrite(LED_FOUR, HIGH);
  delay(200);
  digitalWrite(LED_ONE, LOW);
  digitalWrite(LED_TWO, LOW);
  digitalWrite(LED_THREE, LOW);
  digitalWrite(LED_FOUR, LOW);

  //Reset LCD
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");

}

//Time function
//Detects correct button/LED match
//Calculates time between LED/Button event
//Calculates Score
//Calculates average time
void timeFunction(int *LED_OLD, int buttonState, int ledState, int *nextLed) {
  if (buttonState == LOW && ledState == HIGH) {     //Ensures correct button is pressed for corresponding LED

    double button_pressed = millis();               //Stop timing event

    difference = abs(button_pressed - led_on);      //Time of last event

    digitalWrite(*LED_OLD, LOW);

    if (difference / 1000 < 0.13) {                 //Checks if someone is holidng the button (cheating)
      Serial.print("BUTTON HELD, DO NOT CHEAT!!!");
      Serial.println("");
    }
    else {
      Serial.print("LED Pressed, Time: ");
      Serial.print(difference / 1000);              //Convert time to seconds and print to Serial Monitor
      Serial.print(" seconds");
      Serial.println("");

      averageCount = averageCount + 1;              //Current score updated
      eachTime = eachTime + (difference / 1000);    //Updates total time
      average = eachTime / averageCount;            //Updates average time
    }

    delay(500);                                     //500 ms delay to prevent bouncing

    const int leds[4] = {1, 2, 3, 4};               //Array to store 4 LEDs

    int randomLED = random(0, 4);                   //Choose a random number between 1 and 4

    int ledChosen = leds[randomLED];                //Choose a random LED from the array      

    //Sets next LED
    for (int i = 0; i <= 3; i++) {
      if (ledChosen == 1) {
        *nextLed = LED_ONE;
      }
      if (ledChosen == 2) {
        *nextLed = LED_TWO;
      }
      if (ledChosen == 3) {
        *nextLed = LED_THREE;
      }
      if (ledChosen == 4) {
        *nextLed = LED_FOUR;
      }
    }
    digitalWrite(*nextLed, HIGH);                   //Activates next LED
    *LED_OLD = *nextLed;                            //New LED will be old LED after event takes place
    led_on = millis();                              //Start timing event
  }
}
