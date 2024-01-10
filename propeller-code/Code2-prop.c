// rs : 0-75:anticlock,-75-0 : clock
// ls : -75-0: anticlock, 0-75:clock
#include "simpletools.h"                      // Include simple tools

#include "servo.h"

#include "ping.h"

unsigned int stack2[45 + 25];
unsigned int stack3[45 + 25];
unsigned int stack4[45 + 25];
unsigned int stack5[45 + 25];
unsigned int stack6[45 + 25];
#define rs 16
#define ls 17
/*
#define led1 8 // yellow
#define led2 7 // red
#define led3 6 // blue
#define led4 5  // green*/
#define led1 14 // yellow
int start = 1;
int path = 0;
int flaga = 0;
int flagb = 0;
int pathflag = 0;
int distflag = 0;
int prime = 0;
const int trig1 = 0;
const int echo1 = 1;
const int trig2 = 3;
const int echo2 = 2;
int obs = 0;
int junc = 0;
int obs0 = 0;
int obs1 = 0;
int obs2 = 0;
int obs3 = 0;
int obsA = 0;
int obsB = 0;
int juncA = 0;
int juncB = 0;
int distjunc = 0;
int pobj = 0;
int side;
int cog1, cog2, cog3, cog4, cog5, cog6;
int irl, irm, irr;
void linefollow();
void forward(int side);
void back(int side);
void left_turn();
void right_turn();
void left(int s);
void right(int s);
void blink_led(void * par1);
void dev_path();
void junc_check();
int linefollowfordev();
void forpush();
void forpush2();
void Aline();
int Bline();
int obs_check();
void stopper();
int maincheck();
int stationcheck(int side);
void pp_led(void * par1);
void killbot();
void dispobj();
void juncdisp(void * par4);
void devjuncdisp();
void disablesensor();
void forpushAB();
void path_led(void * par1);
void pathdisplay(int i);
void place_led(void * par1);
void pick_led(void * par1);

int main() // Main function
{
  while (start == 1) {
    irl = input(15);
    irm = input(13);
    irr = input(12);
    linefollow();
    // stationcheck(2);

  }
}

void linefollow() {
  side = 1;
  if (irl == 0 && irm == 1 && irr == 0) {
    forward(side);
  } else if (irl == 1 && irm == 0 && irr == 1) {
    forward(side);
  } else if (irl == 1 && irm == 1 && irr == 0) {
    left(10);
  } else if (irl == 1 && irm == 0 && irr == 0) {
    left(20);
  } else if (irl == 0 && irm == 1 && irr == 1) {
    right(10);
  } else if (irl == 0 && irm == 0 && irr == 1) {
    right(20);
  } else if (irl == 1 && irm == 1 && irr == 1) {
    pause(100);
    forpush();
    junc++;
    cog5 = cogstart((void * ) juncdisp, NULL, stack5, sizeof(stack5));
    if (junc < 5) {
      obs = maincheck();
      if (obs == 0) {
        forpush();
      } else {
        forpush();
        left_turn();
        dev_path();
      }
    } else {
      forpush2();
      right_turn();
      Aline();
    }
  } else {
    back(side);
  }
}

void blink_led(void * par1) // yellow
{
  high(led1);
  pause(500);
  low(led1);
  pause(500);
  cogstop(cog2);
}

void juncdisp(void * par4) {
  set_directions(11, 4, 0b11111111);
  pause(100);
  set_outputs(11, 4, 0b11000111);
  pause(500);
  set_outputs(11, 4, 0b00000000);
  pause(500);
  cogstop(cog5);
}

void pick_led(void * par1) // red
{
  set_directions(11, 4, 0b11111111);
  pause(100);
  set_outputs(11, 4, 0b11110001);
  pause(500);
  set_outputs(11, 4, 0b00000000);
  pause(500);
  cogstop(cog4);
}

void place_led(void * par1){
  set_directions(11, 4, 0b11111111);
  pause(100);
  set_outputs(11, 4, 0b11110001);
  pause(500);
  set_outputs(11, 4, 0b00000000);
  pause(500);
  set_outputs(11, 4, 0b11110001);
  pause(500);
  set_outputs(11, 4, 0b00000000);
  pause(500);
  cogstop(cog4);
}

void forward(int side) {
  if (side == 1) {
    servo_speed(rs, -60);
    servo_speed(ls, 60);
    pause(200);
  } else {
    servo_speed(rs, -40);
    servo_speed(ls, 40);
    pause(200);
  }
}

void forpush() {
  pause(100);
  servo_speed(rs, -60);
  servo_speed(ls, 60);
  pause(100);
}

void forpushAB() { //pause(250);
  servo_speed(rs, -40);
  servo_speed(ls, 40);
  pause(1000);
}

void forpush2() {
  pause(100);
  servo_speed(rs, -60);
  servo_speed(ls, 60);
  pause(150);
}

void back(int side) {
  if (side == 1) {
    servo_speed(rs, 60);
    servo_speed(ls, -60);
  } else {
    servo_speed(rs, 40);
    servo_speed(ls, -40);
  }
}

void left(int s) {
  servo_speed(rs, -s - 5);
  servo_speed(ls, -s - 5);
  pause(100);
}

void right(int s) {
  servo_speed(rs, s + 5);
  servo_speed(ls, s + 5);
  pause(100);
}

void left_turn() {
  pause(100);
  servo_set(rs, 900);
  servo_set(ls, 900);
  pause(700);
  servo_set(rs, 1500);
  servo_set(ls, 1500);
}

void right_turn() {
  pause(100);
  servo_set(rs, 1600);
  servo_set(ls, 1600);
  pause(725);
  servo_set(rs, 1500);
  servo_set(ls, 1500);
}

void stopper() {
  servo_speed(rs, 0);
  servo_speed(ls, 0);
}

void killbot() {
  high(rs);
  high(ls);
}

void dev_path() {
  do {
    irl = input(15);
    irm = input(13);
    irr = input(12);
    linefollowfordev();
  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (irl == 1 && irm == 1 && irr == 1) {
    cog5 = cogstart((void * ) juncdisp, NULL, stack5, sizeof(stack5));
  }
  forpushAB();
  right_turn();
  do {
    obs0 = obs_check();
    if (obs0 == 1) {
      stopper();
      pause(2000);
    }
    irl = input(15);
    irm = input(13);
    irr = input(12);
    linefollowfordev();
  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (irl == 1 && irm == 1 && irr == 1) {

    cog5 = cogstart((void * ) juncdisp, NULL, stack5, sizeof(stack5));
  }
  forpushAB();
  right_turn();
  do {
    irl = input(15);
    irm = input(13);
    irr = input(12);
    linefollowfordev();
  } while (!(irl == 1 && irm == 1 && irr == 1));
  forpushAB();
  junc++;
  cog5 = cogstart((void * ) juncdisp, NULL, stack5, sizeof(stack5));
  if (junc < 5) {
    left_turn();
    obs3 = maincheck();
    if (obs3 == 1) {
      left_turn();
      dev_path();

    } else {
      forpush();
    }
  } else {
    forpushAB();
    Aline();
  }
}

int linefollowfordev() {
  side = 2;
  if (irl == 0 && irm == 1 && irr == 0) {
    forward(side);
  } else if (irl == 1 && irm == 0 && irr == 1) {
    forward(side);
  } else if (irl == 1 && irm == 1 && irr == 0) {
    left(15);
  } else if (irl == 1 && irm == 0 && irr == 0) {
    left(20);
  } else if (irl == 0 && irm == 1 && irr == 1) {
    right(15);
  } else if (irl == 0 && irm == 0 && irr == 1) {
    right(20);
  } else if (irl == 1 && irm == 1 && irr == 1) { ///////////////////////////////////////////////////////
    while (1) {
      int disable = 100;
      if (disable >= 0) {
        disable -= 10;
        continue;
      }
    }
  } else {
    back(side);

  }
}

void Aline(){
  do {
    irl = input(15);
    irm = input(13);
    irr = input(12);
    linefollowfordev();
  } while (!(irl == 1 && irm == 1 && irr == 1));
  forpushAB();
  right_turn();

  if (irl == 1 && irm == 1 && irr == 1) {
    juncA++;
    cog5 = cogstart((void * ) juncdisp, NULL, stack5, sizeof(stack5));
    obsA = stationcheck(1);
    if (obsA == 1) {
      prime = juncA;
      distflag = 1;
      stopper();

      pause(2000);
      path += 40;

      pobj++;
    }
  }
  while (juncA < 6) {
    print("%d", juncA);
    do {
      obs1 = obs_check();
      if (obs1 == 1) {
        stopper();
        pause(1000);
        //
        break;
      }
      irl = input(15);
      irm = input(13);
      irr = input(12);
      linefollowfordev();
    } while (!(irl == 1 && irm == 1 && irr == 1));
    if (irl == 1 && irm == 1 && irr == 1) {
      juncA++;
      /* if(distflag==1)
       { distjunc++;
         }*/

      cog5 = cogstart((void * ) juncdisp, NULL, stack5, sizeof(stack5));

      obsA = stationcheck(1);
      forpushAB();

      if (obsA == 1) {
        prime = juncA;
        /* if(distflag==1)
         {distjunc++;}
         else
         {
         distflag=1;
         distjunc++;
                    }*/

        stopper();
        pause(2000);
      }
    }
    if (juncA == 5)
      break;

  }
  right_turn();
  do {
    irl = input(15);
    irm = input(13);
    irr = input(12);
    linefollowfordev();
  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (irl == 1 && irm == 1 && irr == 1) {
    cog5 = cogstart((void * ) juncdisp, NULL, stack5, sizeof(stack5));
  }
  forpushAB();
  Bline();
}

int Bline() {
  do {
    irl = input(15);
    irm = input(13);
    irr = input(12);
    linefollowfordev();
  } while (!(irl == 1 && irm == 1 && irr == 1));
  forpushAB();
  right_turn();
  if (irl == 1 && irm == 1 && irr == 1) {
    juncB++;
    cog5 = cogstart((void * ) juncdisp, NULL, stack5, sizeof(stack5));
    obsB = stationcheck(2);
    if (obsB == 1) {
      stopper();
      pause(2000);
      int pathn = (5 - prime) + juncB + 1;
      pathdisplay(pathn);
      killbot();

    }
  }
  while (juncB < 6) {
    print("%d", juncB);
    do {
      obs2 = obs_check();
      if (obs2 == 1) {
        stopper();
        pause(1000);
        //
        break;
      }
      irl = input(15);
      irm = input(13);
      irr = input(12);
      linefollowfordev();
    } while (!(irl == 1 && irm == 1 && irr == 1));
    if (irl == 1 && irm == 1 && irr == 1) {
      juncB++;
      cog5 = cogstart((void * ) juncdisp, NULL, stack5, sizeof(stack5));
      obsB = stationcheck(2);
      forpushAB();
      if (obsB == 1) {
        flagb = juncB;
        stopper();
        pause(2000);
        int pathn = (5 - prime) + juncB + 1;
        pathdisplay(pathn);

        killbot();
      }
    }
    if (juncB == 5) {
      print("%d", juncB);

    }
  }
}

int obs_check() // red led
{
  while (1) {
    low(trig1);
    pulse_out(trig1, 10);
    long echo = pulse_in(echo1, 1);
    long dist = echo / 58;
    if (dist < 20) { //check
      cog2 = cogstart((void * ) blink_led, NULL, stack2, sizeof(stack2));

      return 1;
    } else return 0;

  }
}

int stationcheck(int side) {
  while (1) {
    low(trig2);
    pulse_out(trig2, 10);
    long echo = pulse_in(echo2, 1);
    long dist = echo / 58;
    if (dist < 20) {
      print("%d\n", dist);
      //check
      if (side == 1)
        cog4 = cogstart((void * ) pick_led, NULL, stack4, sizeof(stack4));
      // } 
      else
        cog4 = cogstart((void * ) place_led, NULL, stack4, sizeof(stack4));
      return 1;
    } else
      return 0;
  }
}

int maincheck() {
  while (1) {
    low(trig1);
    pulse_out(trig1, 10);
    long echo = pulse_in(echo1, 1);
    long dist = echo / 58;
    if (dist < 30) { //check
      cog2 = cogstart((void * ) blink_led, NULL, stack2, sizeof(stack2));

      return 1;
    } else return 0;
  }
}

void pathdisplay(int i) {
  set_directions(11, 4, 0b11111111);
  pause(100);

  if (i == 1) {
    set_outputs(11, 4, 0b10000100); // 1
  } else if (i == 2) {
    set_outputs(11, 4, 0b11010011); // 2
  } else if (i == 3) {
    set_outputs(11, 4, 0b11010110); // 3
  } else if (i == 4) {
    set_outputs(11, 4, 0b10110100); // 4
  } else if (i == 5) {
    set_outputs(11, 4, 0b01110110); // 5
  } else if (i == 6) {
    set_outputs(11, 4, 0b01110111); // 6
  } else if (i == 7) {
    set_outputs(11, 4, 0b11000100); // 7
  } else if (i == 8) {
    set_outputs(11, 4, 0b11110111); // 8
  }
  if (i == 9) {
    set_outputs(11, 4, 0b11110110); // 9
  } else if (i == 10) {
    set_outputs(11, 4, 0b10000100); // 1
    pause(500);
    set_outputs(11, 4, 0b11100111); // 0
    pause(500);
  }
  //else set_outputs(11, 4, 0b11100111);             // 0
}
