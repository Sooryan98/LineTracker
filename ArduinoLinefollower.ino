#include <Servo.h>
// Rservo range(0-90):clockwise;(90-180)anticlockwise; 90 :stop
// LServo range(0-90):clockwise;(90-180)anticlockwise; 90 :stop
// indicator LEDs
//const int led1=;
//const int led2=;
boolean flag = 0;
boolean flag2 = 0;
// IR sensors
const int irl = 3;
const int irr = 2;
const int irm = 4;
int parkingcounter;

const int ledpark = 12;
const int ledobs = 11;

// Button222
//const int btn=7;

const int deg = 3;
const int odd_dist = 25;
const int odd_trace = odd_dist * deg;
const int even_dist = 25;
const int even_trace = even_dist * deg;
const int d = 0;
unsigned int nowmillis;
const int oldmillis = 0;
// Ultrasonic
const int echo = 7;
const int trig = 8;
int us1dist;       //overall us distance
long us1duration;  //overall us duration check
int us2dist;       //bike check us distance
long us2duration;  //bikcheck us duration
const int us1thresh = 20;
const int us2thresh = 25;
//float ustime=0.0;
//float usdist=0.0;
float usendtime = 0.0;
float usenddist = 0.0;

//side1
const int dt11 = 25;
const int dt12 = 14;
//side2
const int dt21 = 50;
const int dt22 = 30;
const int dt31 = 12;
//junction variables
int junc_counter = 0;
boolean obs = LOW;
boolean obs1 = LOW;
boolean obs2 = LOW;
boolean begin = HIGH;
Servo rs, ls;  // servo
int pdist = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  //pinMode(led1,OUTPUT);
  //pinMode(led2,OUTPUT);
  pinMode(irl, INPUT);
  pinMode(irr, INPUT);
  pinMode(irm, INPUT);
  //pinMode(btn,INPUT_PULLUP);
  //pinMode(echo,INPUT);
  //pinMode(trig,OUTPUT);
  //rs.attach(6);
  //ls.attach(5);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  Serial.begin(9600);
}
void wait(int d);
void forward(int d);
void forward_move(int d);
void back(int d);
void right_turn(int d);
void left_turn(int d);
void linefollow();
void odd_junction();
void even_junction();
boolean move_oddline(int sideodd);
boolean parkalgo(int sideeven);
boolean uscheck();
boolean endbike();
void parkled();
void obsled();


void loop() {

  if (begin == 1) {
    while (1)

    {
      move();
      wait(1000);
      // uscheck();
      //int  sideodd=2;
      // move_oddline(sideodd);
    }
  }

  else {
    rs.detach();
    ls.detach();
  }
}
boolean move() {
  if (digitalRead(irl) == LOW && digitalRead(irm) == HIGH && digitalRead(irr) == LOW) {
    forward_move(10);
  }
  if (digitalRead(irl) == HIGH && digitalRead(irm) == LOW && digitalRead(irr) == HIGH) {
    forward(10);

  }

  else if (digitalRead(irl) == LOW && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH) {
    right_turn(40);

  } else if (digitalRead(irl) == LOW && digitalRead(irm) == LOW && digitalRead(irr) == HIGH) {
    right_turn(50);

  } else if (digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == LOW) {
    left_turn(40);

  } else if (digitalRead(irl) == HIGH && digitalRead(irm) == LOW && digitalRead(irr) == LOW) {
    left_turn(50);

  } else if (digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH) {
    wait(2000);
    junc_counter++;
    Serial.print("Junction number : ");
    Serial.println(junc_counter);
    wait(2000);
    if (junc_counter % 2 != 0) {
      odd_junction();
      //wait(500);
    } else {
      even_junction();
      //wait(500);
    }

  } else if (digitalRead(irl) == LOW && digitalRead(irm) == LOW && digitalRead(irr) == LOW) {
    back(40);

  } else {
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
  }
}

void odd_junction() {
  boolean parkcheck1, parkcheck2;
  int sideodd, sideeven;
  sideodd = 1;
  obs1 = move_oddline(sideodd);
  //obs1=HIGH;
  //if(junc_counter>2)obs1=LOW;
  //else obs1=HIGH;
  if (obs1 == LOW) {
    sideeven = 1;
    parkcheck1 = parkalgo(sideeven);
    if (parkcheck1 == 1) {
      Serial.println("Parked at side 1");
      wait(1000);
      parkled();
      exit(0);
    }

  } else {
    if (digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH) {
      forward(200);
      sideodd = 2;

      obs2 = move_oddline(sideodd);
      if (obs2 == LOW) {
        sideeven = 2;
        parkcheck2 = parkalgo(sideeven);
        if (parkcheck2 == 1) {
          Serial.println("Parked at side 2");
          wait(1000);
          parkled();
          exit(0);
        }

      } else {
        forward(300);
        wait(2000);
        right_turn(590);
        wait(2000);
      }
    }
  }
  sideodd = 0;
  sideeven = 0;
}

void even_junction() {
  forward(200);
  wait(2000);
}
boolean move_oddline(int sideodd) {
  if (sideodd == 1) {
    boolean f1 = 0;
    boolean f2 = 0;
    boolean Tcheck = LOW;
    boolean bikecheck = LOW;
    float d1 = 0.0;
    forward(400);
    wait(1000);
    left_turn(600);
    wait(1000);
    forward(400);
    do {
      linefollow();


      Tcheck = uscheck();
      if (Tcheck == HIGH) {
        wait(1000);
        parkled();
        Serial.println("  Object on left parking");
        wait(1000);
        f1 = 1;
        break;
      }
      d1 += 0.5;
      if (f1 == 1) break;
    } while (d1 < 20);
    wait(1000);
    if (Tcheck == LOW) {
      do {
        linefollow();
        bikecheck = endbike();
        if (bikecheck == HIGH) {
          Serial.println("Obstacle beyond the T");
          wait(1000);
          parkled();
          f2 = 1;
          break;
        }
        if (f2 == 1) break;
      } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));
    }
    forward(300);
    wait(1000);
    right_turn(600);
    wait(1000);
    right_turn(600);
    do {
      linefollow();
    } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));
    if (Tcheck == LOW) return LOW;
    else return HIGH;

  }

  else {
    boolean Tcheck = LOW;
    boolean bikecheck = LOW;
    boolean f1 = 0;
    boolean f2 = 0;

    float d1 = 0.0;
    do {
      linefollow();

    } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));
    wait(1000);
    forward(400);
    wait(1000);
    left_turn(600);
    wait(1000);
    left_turn(620);
    wait(1000);
forward(50);    
    wait(1000);
    do {
      linefollow();
      bikecheck = endbike();
      if (bikecheck == HIGH) {
        Serial.println("Obstacle beyond the T");
        wait(2000);
        parkled();
        wait(2000);
        f2 = 1;break;
      }
      d1 += 0.5;if(f2==1)break;
    } while (d1 < 5);
    wait(2000);
    forward(800);
    wait(2000);
    do {
      linefollow();
      Tcheck = uscheck();
      if (Tcheck == HIGH) {
        wait(1000);
        parkled();
        wait(2000);
        Serial.println("   Obejct on right parking");
        wait(1000);
        f1 = 1;
        break;
      }
      if (f1 == 1) break;

    } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));
    wait(2000);
    wait(2000); /*
do{
  linefollow();
}while(!(digitalRead(irl)==HIGH && digitalRead(irm)==HIGH && digitalRead(irr)==HIGH));*/
    if (Tcheck == HIGH) {
      do {
        linefollow();
      } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));
      return HIGH;
    } else 
    {
      do{
        linefollow();

      }while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));
return LOW;
    }
  }
}



boolean parkalgo(int sideeven) {
  if (digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH) {
    if (sideeven == 1)

    {
      forward(400);
      wait(2000);
      left_turn(600);
      wait(2000);
      do {
        linefollow();


      } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));


      junc_counter++;
      Serial.print("Junction number : ");
      Serial.println(junc_counter);
      wait(1000);
      forward(400);
      wait(2000);
      left_turn(600);
      wait(2000);
      //int usflag=2;
      // usbikecheck();

      do {
        linefollow();


      } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));


      return HIGH;

    } else {
      forward(400);
      wait(2000);
      right_turn(600);
      wait(2000);
      do {
        linefollow();


      } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));


      junc_counter++;
      Serial.print("Junction number : ");
      Serial.println(junc_counter);
      wait(1000);
      forward(400);
      wait(2000);
      right_turn(600);
      wait(2000);
      // usbikecheck();

      do {
        linefollow();


      } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));
      return HIGH;
    }
    sideeven = 0;
  }
}
void forward_move(int d) {
  do {
    linefollow();

  } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));
  wait(1000);
}
void forward(int d) {
  ls.attach(5);
  rs.attach(6);
  ls.write(100);
  rs.write(80);
  delay(d);
}
void back(int d) {
  ls.attach(5);
  rs.attach(6);
  ls.write(80);
  rs.write(100);
  delay(d);
}
void left_turn(int d) {
  ls.attach(5);
  rs.attach(6);
  ls.write(80);
  rs.write(80);
  delay(d);
}
void right_turn(int d) {
  ls.attach(5);
  rs.attach(6);
  ls.write(100);
  rs.write(100);
  delay(d);
}

void wait(int d) {
  ls.detach();
  rs.detach();
  delay(d);
}


void linefollow() {


  if (digitalRead(irl) == LOW && digitalRead(irm) == HIGH && digitalRead(irr) == LOW) {
    forward(20);

  } else if (digitalRead(irl) == HIGH && digitalRead(irm) == LOW && digitalRead(irr) == HIGH) {
    forward(20);

  }

  else if (digitalRead(irl) == LOW && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH) {
    right_turn(40);

  } else if (digitalRead(irl) == LOW && digitalRead(irm) == LOW && digitalRead(irr) == HIGH) {
    right_turn(50);

  } else if (digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == LOW) {
    left_turn(40);

  } else if (digitalRead(irl) == HIGH && digitalRead(irm) == LOW && digitalRead(irr) == LOW) {
    left_turn(50);

  }


  else if (digitalRead(irl) == LOW && digitalRead(irm) == LOW && digitalRead(irr) == LOW) {
    back(25);

  } else {
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
  }
}



void parkled() {
  digitalWrite(ledpark, HIGH);
  delay(1000);
  digitalWrite(ledpark, LOW);
  delay(1000);
}

void parkingledcounter(int counter) {  //displays no. of occupied parking spaces
  for (int i = 1; i <= counter; i++) {
    parkled();
  }
}

void obsled() {
  digitalWrite(ledobs, HIGH);
  delay(1000);
  digitalWrite(ledobs, LOW);
  delay(1000);
}


boolean uscheck() {
  float ustime = 0;
  float usdist = 0;
  for (int i = 0; i < 10; i++) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    ustime = pulseIn(echo, HIGH);
    usdist = ustime * 0.034 / 2;

    if (usdist <= 17) {
      //Serial.println(usdist);
      parkingcounter++;
      return HIGH;
    } else {
      // Serial.println(usdist);
      return LOW;
    }
  }
  delay(100);
}


boolean endbike() {
  float ustime = 0;
  float usdist = 0;
  for (int i = 0; i < 10; i++) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    usendtime = pulseIn(echo, HIGH);
    usenddist = usendtime * 0.034 / 2;
    if (usenddist <= 17) {
      //Serial.println(usenddist);
      return HIGH;
    } else {
      // Serial.println(usenddist);
      return LOW;
    }
  }
}
