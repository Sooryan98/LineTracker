#include <Servo.h>

//LEDs
//LED to indicate whether the robot has parked.
const int parked = A0;//yellow
//LED to indicate whether the robot has encountered a junction.
const int junction = A4;//green
//LED to indicate the number of occupied parking spaces encountered.
const int occparkingspaces = A3;//blue
//LED to indicate whether the robot has sensed a car parked at the parking space.
const int carparked = A2;//red
//LED to indicate whether the robot has sensed a bike parked at the parking space.
const int bikeparked = A1;//white

// IR sensors
const int irl = 3;
const int irr = 2;
const int irm = 4;

//integer to count the number of parking spaces encountered
int parkingcounter=0;

//Servo
const int deg = 3;
const int odd_dist = 25;
const int odd_trace = odd_dist * deg;
const int even_dist = 25;
const int even_trace = even_dist * deg;
const int d = 0;

// Ultrasonic
const int echo = 7;
const int trig = 8;

//Global Variables

// variable to check the presence of a bike beyond the T.
boolean beyondt=0;

// integer to check the number of parked vehicles encountered.
int bikeno=0;
int carno=0;

//variables to determine the distance and time from the ultrasonic sensor
float usendtime = 0.0;
float usenddist = 0.0;

//side measurements
const int dt11 = 25;
const int dt12 = 14;

//junction variable
int junc_counter = 0;

//variable to check if parking space is available.
boolean obs = LOW;
boolean obs1 = LOW;
boolean obs2 = LOW;
//variable to begin. begin==LOW once parked
boolean begin = HIGH;
Servo rs, ls;  // servo

void setup() {
  // Setup code here, to run once:
  pinMode(13, OUTPUT);
  pinMode(irl, INPUT);
  pinMode(irr, INPUT);
  pinMode(irm, INPUT);
  pinMode(A0, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  Serial.begin(9600);
}

//implements odd_junction and even_junction functions
boolean move();
//function to perform waiting, stopping the robot.
void wait(int d);
//function to move forward. 
void forward(int d);
//function to move forward from a junction.
void forward_move(int d);
//function to move back.
void back(int d);
//function to take a right turn.
void right_turn(int d);
//function to make a left turn.
void left_turn(int d);
//function to follow the line path.
void linefollow();
//function to implement the algorithm at the odd juction.
void odd_junction();
//function to implement algorithm at the even junction - parking space availability checked using move_oddline function. 
  //Parking function implemented.
void even_junction();
//function to move and check if the parking space is available at the odd junction line.
boolean move_oddline(int sideodd);
//function to park at the even junction
boolean parkalgo(int sideeven);
//function to check if there is a bike or a car parked at the parking space.
boolean uscheck();
//function to check if a bike is parked beyond the T line.
boolean endbike();

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
  //line following using IR Sensors
  if (digitalRead(irl) == LOW && digitalRead(irm) == HIGH && digitalRead(irr) == LOW) {
    forward_move(10);
  }
  else if (digitalRead(irl) == HIGH && digitalRead(irm) == LOW && digitalRead(irr) == HIGH) {
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
    //junction encountered
    wait(2000);
    junc_counter++;
    junctionindicator();//green light
    Serial.print("Junction number : ");
    Serial.println(junc_counter);
    wait(2000);
    if (junc_counter % 2 != 0) {
      odd_junction();
    } else {
      even_junction();
    }
  } else if (digitalRead(irl) == LOW && digitalRead(irm) == LOW && digitalRead(irr) == LOW) {
    //readjusting to follow the line
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
  if (obs1 == LOW) {
    sideeven = 1;
    //parks at side 1
    parkcheck1 = parkalgo(sideeven);
    if (parkcheck1 == 1) {
      Serial.println("Parked at side 1");
      wait(1000);
       parkingindicator();
        if(beyondt==1)
          {bikeindicator();
          wait(500);
          bikeindicator();
          wait(500);          
          }          
        wait(2000);
        occupiedparkingspacescounter(parkingcounter);
        delay(2000);
        carnumb(carno);
        delay(2000);
        bikenumb(bikeno);      
      exit(0);
    }
  } else {
    //parks at side 2
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
          parkingindicator();
          if(beyondt==1)
          {bikeindicator();
          wait(500);
          bikeindicator();
          wait(500);          
          }
          wait(2000);
          occupiedparkingspacescounter(parkingcounter);
          delay(2000);
          carnumb(carno);
          delay(2000);
          bikenumb(bikeno);          
          exit(0);
        }
      } else {
        forward(350);
        wait(2000);
        right_turn(570);
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
  //side 1
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
      //checks for a car/bike in the parking space
      Tcheck = uscheck();
      if (Tcheck == HIGH) {
        //if high, there is a car or a bike in the parking space. 
        //car is indicated by a red LED blink
        //bike is indicated by a white LED blink
        wait(1000);
        Serial.println("Object on left parking");
        parkingcounter++;
        wait(1000);
        f1 = 1;
        break;
      }
      d1 += 0.5;
      if (f1 == 1) break;
    } while (d1 < 22);
    wait(1000);
    if (Tcheck == LOW) {
      //if Tcheck is low, there is no car or bike in the parking space. The parking space is available
      do {
        linefollow();
        //now we check for a bike at the end, beyond the T.
        bikecheck = endbike();
        if (bikecheck == HIGH) {
          //if there is a bike, the White LED will blink twice once the robot has parked in fron tof it.
          Serial.println("Obstacle beyond the T");
          beyondt=1;
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
  //side 2
  else {
    boolean Tcheck = LOW;
    boolean bikecheck = LOW;
    boolean f1 = 0;
    boolean f2 = 0;
    boolean f3=0;
    float d1 = 0.0;
    do {
      linefollow();
    } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));
    wait(1000);
    forward(300);
    wait(1000);
    left_turn(600);
    wait(1000);
    left_turn(620);
    wait(1000);
    forward(100);    
    wait(1000);  
    do {
     linefollow();
      bikecheck = endbike();
      // As the robot has Ultrasonic sensors on the side, on side 2, it will go until the end and make a u-turn.
      // on this side, we will check for parking space occupied by a bike beyond the T. 
      if (bikecheck == HIGH) {
        Serial.println("Obstacle beyond the T");
        beyondt=1;
      }
      d1 += 0.1;
    } while (d1 < 4.5);   
    d1=0.0;
    do {
      linefollow();
      //The robot will then conduct a parking check
      Tcheck = uscheck();
      if (Tcheck == HIGH) {
        wait(1000);
        Serial.println("Object on right parking");
        parkingcounter++;
        wait(1000);
        f1 = 1;
        break;
      }
      if (f1 == 1) break;
    } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));
    wait(2000);
    wait(2000); 
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
    //parking algorith starting from the odd junction.
    if (sideeven == 1)
    {
      forward(400);
      wait(2000);
      left_turn(600);
      wait(2000);
      do {
        linefollow();
      } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));
      //even junction encountered
      junc_counter++;
      junctionindicator();
      Serial.print("Junction number : ");
      Serial.println(junc_counter);
      wait(1000);
      forward(400);
      wait(2000);
      left_turn(600);
      wait(2000);
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
      junctionindicator();
      Serial.print("Junction number : ");
      Serial.println(junc_counter);
      wait(1000);
      forward(400);
      wait(2000);
      right_turn(600);
      wait(2000);
      do {
        linefollow();
      } while (!(digitalRead(irl) == HIGH && digitalRead(irm) == HIGH && digitalRead(irr) == HIGH));
      return HIGH;
    }
    //return global variable to its original state
    sideeven = 0;
  }
}

void forward_move(int d) {
  //needs to perform linefollow function when not at the junction.
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

//Ultrasonic Sensor Functions
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
    //if usdist between 11 and 15, bike is encountered
    //if usdist less than 11, car is encountered  
    if (usdist <= 16) {
      if(usdist<11){          
        carindicator();
        carno++;
      }
      else if(usdist<=15 && usdist>=11){
        bikeindicator();
        bikeno++;
      }
      //Serial.println(usdist);
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
    //only need to check for the presence of a bike. as this check is trigerred when the robot is beyond the T.
    if (usenddist <= 15) {
      return HIGH;
    } else {
      return LOW;
    }
  }
}

//LED functions
//indicates whether parked
void parkingindicator(){
  digitalWrite(parked, HIGH);
  delay(1000);
  digitalWrite(parked, LOW);
  delay(1000);
}

//indicates junction
void junctionindicator(){
  digitalWrite(junction, HIGH);
  wait(1000);
  digitalWrite(junction, LOW);
  wait(1000);
}

//indicates a car parked in the parking space
void carindicator(){
  digitalWrite(carparked, HIGH);
  wait(1000);
  digitalWrite(carparked, LOW);
  wait(1000);
}

//indicates a bike parked in the parking space
void bikeindicator(){
  digitalWrite(bikeparked, HIGH);
  wait(1000);
  digitalWrite(bikeparked, LOW);
  wait(1000);
}

//counts the number of occupied parking spaces
void occupiedparkingspacescounter(int a){
  for(int i=1;i<=a;i++){
  digitalWrite(occparkingspaces, HIGH);
  delay(1000);
  digitalWrite(occparkingspaces, LOW);
  delay(1000);
  }
}

//counts the number of cars in parking spaces encountered
void carnumb( int no){
  for(int i=1;i<=no;i++){
  digitalWrite(carparked, HIGH);
  delay(1000);
  digitalWrite(carparked, LOW);
  delay(1000);
  }
}

//counts the number of bikes in parking spaces encountered (not including the one beyond the T).
void bikenumb(int no){
  for(int i=1;i<=no;i++){
  digitalWrite(bikeparked, HIGH);
  delay(1000);
  digitalWrite(bikeparked, LOW);
  delay(1000);
  }
}

