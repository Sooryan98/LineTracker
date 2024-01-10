// rs : 0-75:anticlock,-75-0 : clock
// ls : -75-0: anticlock, 0-75:clock

#include "simpletools.h"
#include "servo.h"

unsigned int stack2[45 + 25];
unsigned int stack3[45 + 25];
unsigned int stack4[45 + 25];

#define rs 16  // right servo
#define ls 17  // left servo
#define led1 4 // yellow LED

/// RPI signals//
const int rin = 5;
const int c3 = 6;
const int c3start = 7;
const int comm1 = 9;
const int comm2 = 8;
int opt;

int one, two, targ;
int rpiflag = 0;
int dr[10];
const int trig1 = 3;
const int echo1 = 2; // left Ulttrasonic sensor
const int echo2 = 1;
const int trig2 = 0; // right Ultrasonic Sensor
const int rpi = 9;   // communication pin 1
const int rpi2 = 10;

int cog1, cog2, cog3;
int irl, irm, irr, irrx, irlx;
int side; // is 0 for left side, and 1 for right side. value is taken from communication from RPi
int start = 1;
int flag = 0; // is initialized as 0. When a station is detected, flag=1
int junc = 0; // junction counter
int check = 0;
int rect = 0; // no. of rectangles completed
int obs = 0;

int rflag = 0;
int lflag = 0;
// functions
void linefollow();      // This function follows the line and makes decisions based on the number of rectangles travelled and communications from RaspberryPi to take left/rright turn at junctions i1, i3, i5 and C3.
void side_linefollow(); // This function enables the robot to follow the path when the robot is at the sides.
                        // Robot Navigates the No man's land
void forpush();    // pushes the robot forward
void forward();    // moves the robot forward
void left(int s);  // orients the robot on the path by turning left
void right(int s); // orients the robot on the path by turning right
void left_turn();  // function to take a left turn
void right_turn(); // function to take a right turn
void back();       // function to move back
int left_check();  // function to check for stations on the left side
int right_check(); // function to check for stations on the right side
void rside();
void lside();
void stopper();
int discsum(int darr[]);
void arclinefollow();
void C3_l();
void C3_r();
void killbot();
void jr();
void jl();
void blink_led(void *par1);

int main()
{
  while (start == 1)
  {
    low(c3);
    int i = 0;
    irl = input(15);
    irr = input(12);
    irm = input(13);

    one = input(comm1);
    two = input(comm2);
    while (i < 10)
    {
      if (one == 0 && two == 1)
      {
        dr[i] = -1;
        i++;
      }
      else
      {
        dr[i] = 1;
        i++;
      }
    }
    if (rect < 3)
      linefollow();
    else
      arclinefollow();
  }
  // low(c3);
  /*  lflag=left_check();
    do{
    linefollow();
  }while(lflag=0);
    if(lflag=1){
      forpush();
      pause(2000);
      left_turn();
      back();
      pause(1000);
      stopper();
      pause(2000);
      forpush();
      pause(1000);
      left_turn();*/
}

void linefollow()
{
  if (irl == 0 && irm == 1 && irr == 0)
  {
    forward();
  }
  else if (irl == 1 && irm == 0 && irr == 1)
  {
    forward();
  }
  else if (irl == 1 && irm == 1 && irr == 0)
  {
    left(10);
  }
  else if (irl == 1 && irm == 0 && irr == 0)
  {
    left(20);
  }
  else if (irl == 0 && irm == 1 && irr == 1)
  {
    right(10);
  }
  else if (irl == 0 && irm == 0 && irr == 1)
  {
    right(20);
  }
  else if (irl == 1 && irm == 1 && irr == 1)
  {
    cog2 = cogstart((void *)blink_led, NULL, stack2, sizeof(stack2));
    pause(100);

    // rect++;
    forpush();
    pause(100);
    stopper();
    pause(2000);

    if (discsum(dr) == -1)
    {
      for (int j = 0; j < 10; j++)
      {
        dr[j] = 0;
      }
      forpush();
      pause(500);

      lside();
    }

    else
    {
      for (int j = 0; j < 10; j++)
      {
        dr[j] = 0;
      }
      forpush();
      pause(500);

      rside();
    }
  }

  else
  {
    back(side);
  }
}

void lside()
{ // rect++;
  int sr = 0;
  int sl = 0;
  left_turn();
  do
  {
    irl = input(15);
    irm = input(13);
    irr = input(12);
    side_linefollow();
  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (irl == 1 && irm == 1 && irr == 1)
  {
    forpush();
    pause(600);
    right_turn();
  }

  do
  {
    lflag = left_check();
    if (lflag == 1)
    {
      sl = 1;
      left_turn();
      back();
      pause(500);
      stopper();
      pause(2000);
      forpush();
      pause(500);
      break;
    }
    irl = input(15);
    irm = input(13);
    irr = input(12);
    side_linefollow();
  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (sl == 1)
  {
    left_turn();
  }
  else
  {
    left_turn();
    left_turn();
  }
  do
  {
    irl = input(15);
    irm = input(13);
    irr = input(12);
    side_linefollow();

  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (irl == 1 && irm == 1 && irr == 1)
  {
    forpush();
    pause(600);
    left_turn();
  }

  ////////////////////moving to right side
  do
  {
    irlx = input(14);
    irl = input(15);
    irm = input(13);
    irr = input(12);
    irrx = input(11);
    side_linefollow();
  } while (!(irr == 1 && irrx == 1 && irm == 1));
  if ((irr == 1 && irrx == 1 && irm == 1))
  {
    forpush();
    pause(800);
  }

  do
  {
    irl = input(15);
    irm = input(13);
    irr = input(12);
    side_linefollow();

  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (irl == 1 && irm == 1 && irr == 1)
  {
    forpush();
    pause(600);
    left_turn();
  }

  do
  {

    rflag = right_check();
    if (rflag == 1)
    {
      // forpush();
      ////pause(200);
      // stopper();
      // pause(200);

      right_turn();
      back();
      pause(500);
      stopper();
      pause(2000);
      forpush();
      pause(500);
      break;
    }
    irl = input(15);
    irm = input(13);
    irr = input(12);
    side_linefollow();
  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (rflag == 1)
  {
    left_turn();
    do
    {
      irl = input(15);
      irm = input(13);
      irr = input(12);
      side_linefollow();

    } while (!(irl == 1 && irm == 1 && irr == 1));
  }
  if (irl == 1 && irm == 1 && irr == 1)
  {
    forpush();
    pause(400);
    left_turn();
  }

  do
  {
    irlx = input(14);
    irl = input(15);
    irm = input(13);
    irr = input(12);
    irrx = input(11);
    side_linefollow();
  } while (!((irl == 1 || irrx == 1) && irm == 1 && (irr == 1 || irlx == 1)));
  if ((irl == 1 || irrx == 1) && irm == 1 && (irr == 1 || irlx == 1))
  {
    forpush();
    pause(600);
    right_turn();

    rect++;
    cog2 = cogstart((void *)blink_led, NULL, stack2, sizeof(stack2));
    pause(100);
  }
}

void rside()
{
  int sr = 0;
  int sl = 0;
  right_turn();
  do
  {
    irl = input(15);
    irm = input(13);
    irr = input(12);
    side_linefollow();
  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (irl == 1 && irm == 1 && irr == 1)
  {
    forpush();
    pause(500);
    left_turn();
  }
  do
  {
    rflag = right_check();
    if (rflag == 1)
    {
      sr = 1;
      right_turn();
      back();
      pause(500);
      stopper();
      pause(2000);
      forpush();
      pause(500);
      break;
    }
    irl = input(15);
    irm = input(13);
    irr = input(12);
    side_linefollow();
  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (sr == 1)
  {
    right_turn();
  }
  else
  {
    right_turn();
    right_turn();
  }

  do
  {
    irl = input(15);
    irm = input(13);
    irr = input(12);
    side_linefollow();

  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (irl == 1 && irm == 1 && irr == 1)
  {
    forpush();
    pause(600);
    right_turn();
  }
  //////////////moving to left side
  do
  {
    irlx = input(14);
    irl = input(15);
    irm = input(13);
    irr = input(12);
    irrx = input(11);
    side_linefollow();
  } while (!(irl == 1 && irlx == 1 && irm == 1));
  if (irl == 1 && irlx == 1 && irm == 1)
  {
    forpush();
    pause(800);
  }
  do
  {
    irl = input(15);
    irm = input(13);
    irr = input(12);
    side_linefollow();

  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (irl == 1 && irm == 1 && irr == 1)
  {
    forpush();
    pause(500);
    right_turn();
  }
  do
  {

    lflag = left_check();
    if (lflag == 1)
    {
      sl = 1;
      // forpush();
      // pause(200);
      // stopper();
      // pause(200);
      // back();
      // pause(100);
      left_turn();
      back();
      pause(500);
      stopper();
      pause(2000);
      forpush();
      pause(500);
      break;
    }
    irl = input(15);
    irm = input(13);
    irr = input(12);
    side_linefollow();
  } while (!(irl == 1 && irm == 1 && irr == 1));
  if (sl == 1)
  {
    right_turn();
    do
    {
      irl = input(15);
      irm = input(13);
      irr = input(12);
      side_linefollow();

    } while (!(irl == 1 && irm == 1 && irr == 1));
  }
  if (irl == 1 && irm == 1 && irr == 1)
  {
    forpush();
    pause(500);
    right_turn();
  }
  do
  {
    irlx = input(14);
    irl = input(15);
    irm = input(13);
    irr = input(12);
    irrx = input(11);
    side_linefollow();
  } while (!((irl == 1 || irrx == 1) && irm == 1 && (irr == 1 || irlx == 1)));

  if ((irl == 1 || irrx == 1) && irm == 1 && (irr == 1 || irlx == 1))
  {
    forpush();
    pause(500);
    left_turn();
    rect++;
    cog2 = cogstart((void *)blink_led, NULL, stack2, sizeof(stack2));
    pause(100);
  }
}

void side_linefollow()
{
  if (irl == 0 && irm == 1 && irr == 0)
  {
    forward();
  }
  else if (irl == 1 && irm == 0 && irr == 1)
  {
    forward();
  }
  else if (irl == 1 && irm == 1 && irr == 0)
  {
    left(10);
  }
  else if (irl == 1 && irm == 0 && irr == 0)
  {
    left(20);
  }
  else if (irl == 0 && irm == 1 && irr == 1)
  {
    right(10);
  }
  else if (irl == 0 && irm == 0 && irr == 1)
  {
    right(20);
  }
  else if (irl == 1 && irm == 1 && irr == 1)
  {
  }
  else
  {
    back();
  }
}

void forpush()
{
  pause(100);
  servo_speed(rs, -60);
  servo_speed(ls, 60);
  pause(100);
}

void forward()
{
  servo_speed(rs, -75);
  servo_speed(ls, 75);
}

void left(int s)
{
  servo_speed(rs, -s - 5);
  servo_speed(ls, -s - 5);
  pause(98);
}

void right(int s)
{
  servo_speed(rs, s + 5);
  servo_speed(ls, s + 5);
  pause(98);
}

void left_turn()
{
  pause(175);
  servo_set(rs, 900);
  servo_set(ls, 900);
  pause(810);
  servo_set(rs, 1500);
  servo_set(ls, 1500);
}

void right_turn()
{
  pause(175);
  servo_set(rs, 1600);
  servo_set(ls, 1600);
  pause(810);
  servo_set(rs, 1500);
  servo_set(ls, 1500);
}

void back()
{
  servo_speed(rs, 60);
  servo_speed(ls, -60);
}

void stopper()
{
  servo_speed(rs, 0);
  servo_speed(ls, 0);
}

void killbot()
{
  high(rs);
  high(ls);
}
int left_check()
{
  while (1)
  {
    low(trig1);
    pulse_out(trig1, 10);
    long echo = pulse_in(echo1, 1);
    long dist = echo / 58;
    if (dist < 15)
    {
      obs = 1;
      return obs;
    }
    else
      obs = 0;
    return obs;
  }
}

int right_check()
{
  while (1)
  {
    low(trig2);
    pulse_out(trig2, 10);
    long echo = pulse_in(echo2, 1);
    long dist = echo / 58;
    if (dist < 15)
    {
      obs = 1;
      return obs;
    }
    else
      obs = 0;
    return obs;
  }
}
int discsum(int darr[])
{
  int n = sizeof(darr) / sizeof(darr[0]); // Calculate the length of the array
  int sum = 0;                            // Initialize the sum to zero

  for (int i = 0; i < n; i++)
  {
    sum += darr[i]; // Add the current element to the sum
  }

  if (sum > 0)
    return 1;
  else
    return -1;
}

void arclinefollow()
{
  int i = 0;

  irlx = input(14);
  irl = input(15);
  irm = input(13);
  irr = input(12);
  irrx = input(11);

  one = input(comm1);
  two = input(comm2);
  while (i < 10)
  {
    if (one == 0 && two == 1)
    {
      dr[i] = -1;
      i++;
    }
    else
    {
      dr[i] = 1;
      i++;
    }
  }

  if (irl == 0 && irm == 1 && irr == 0)
  {
    forward();
  }
  else if (irl == 1 && irm == 0 && irr == 1)
  {
    forward();
  }
  else if (irl == 1 && irm == 1 && irr == 0)
  {
    left(10);
  }
  else if (irl == 1 && irm == 0 && irr == 0)
  {
    left(20);
  }
  else if (irl == 0 && irm == 1 && irr == 1)
  {
    right(10);
  }
  else if (irl == 0 && irm == 0 && irr == 1)
  {
    right(20);
  }
  else if (irl == 0 && irm == 0 && irr == 0)
  {
    stopper();
    pause(3000);
    low(c3);
    // pause(2000);

    ///////while rin.........
    if (discsum(dr) == -1)
    {

      C3_l();
    }
    else
    {

      C3_r();
    }
  }
  else
  {
    killbot();
  }
}

void C3_l()
{
  high(c3);
  targ = 0;
  pause(1000);

  do
  {
    print("checking for target\n");
    targ = input(rin);
    if (targ == 0)
    {
      servo_set(rs, 900);
      servo_set(ls, 900);
      pause(100);
      stopper();
      pause(200);
    }
    else
      break;
  } while (!(targ == 1));
  if (targ == 1)
  {
    print("Found the target\n");
    pause(1000);
  }
  // pause(1000);
  // high(c3start);

  int c = 0;
  while (!((irl == 1 && irrx == 1) && irm == 1 && (irr == 1 && irlx == 1)))
  {

    irlx = input(14);
    irl = input(15);
    irm = input(13);
    irr = input(12);
    irrx = input(11);

    one = input(comm1);
    two = input(comm2);

    if (!(irl == 1 && irr == 1 && irm == 1))
    {
      forpush();
      pause(100);
      stopper();
      pause(100);
    }
    else
    {
      killbot();
      break;
    }
    /*if(irl==1&&irr==1&&irm==1)
    {killbot();
    break;
 } */
    if (c < 4)
    {
      print("%d\n", c);
      if (one == 0 && two == 1)
      {
        c++;
        print(" Move forward and right\n");
        // pause(1000);
        forward();
        if (irl == 1 && irr == 1 && irm == 1)
        {
          killbot();
          low(c3);
          break;
        }
        // pause(100);
        // right(10);
        // pause(100);
        jr();

        // pause(200);
      }
      else if (one == 1 && two == 0)
      {
        c++;

        print("Move back and left\n");
        // pause(1000);
        back();
        // pause(75);
        jl();

        // pause(200);

        // left(10);
        // pause(100);
      }
      else if (one == 0 && two == 0)
      {
        print("Move forward\n");
        // pause(1000);
        forward();
        if (irl == 1 && irr == 1 && irm == 1)
        {
          killbot();
          low(c3);
          break;
        }
        // pause(100);
      }
      else
      {
        print("Not going anywhere\n");
        pause(1000);
      }
    }

    else
    {

      print("GOing Forward");
      forward();
      if (irl == 1 && irr == 1 && irm == 1)
      {
        killbot();
        low(c3);
        break;
      }
      // pause(200);
      c = 0;
    }

    /*do{
      irl = input(15);
      irm = input(13);
      irr = input(12);
      one = input(comm1);
      two = input(comm2);

      if(one==0&&two==1){
        print(" Move forward and right\n");
        //pause(1000);
        forward();
        //pause(500);
        right(10);
        //pause(500);
      }
      else if(one ==1 && two==0){
        print("Move back and left\n");
       // pause(1000);
        back();
        //pause(500);
        left(10);
        //pause(500);
      }
      else if(one==0 && two==0){
        print("Move forward\n");
        //pause(1000);
        forward();
        pause(100);
      }
      else {
        print("Not going anywhere\n");
        pause(1000);
      }


    }while(!((irl==1||irrx==1)&&irm==1&&(irr==1||irlx==1)));*/
  }
  if ((irl == 1 || irlx == 1) && irm == 1 && (irr == 1 || irrx == 1))
  {
    print("Reached");
    low(c3);
    killbot();
  }
}

void C3_r()

{
  high(c3);
  targ = 0;
  pause(1000);
  do
  {
    print("checking for target\n");

    targ = input(rin);
    if (targ == 0)
    {
      servo_set(rs, 1600);
      servo_set(ls, 1600);
      pause(90);
      stopper();
      pause(200);
    }
    else
      break;
    // pause(1000);
  } while (!(targ == 1));
  if (targ == 1)
  {
    print("Found the target\n");
    pause(1000);
  }

  // pause(1000);
  // high(c3start);
  // two=0;
  // one=0;
  int c = 0;
  int r = 0;

  while (!((irl == 1 || irrx == 1) || irm == 1 || (irr == 1 || irlx == 1)))
  {

    irlx = input(14);
    irl = input(15);
    irm = input(13);
    irr = input(12);
    irrx = input(11);

    one = input(comm1);
    two = input(comm2);

    forpush();
    pause(100);
    stopper();
    pause(100);
    if (irl == 1 && irr == 1 && irm == 1)
    {
      killbot();
      low(c3);
      break;
    }
    if (c < 4)
    {
      print("%d\n", c);
      if (one == 0 && two == 1)
      {
        c++;
        r++;
        print(" Move forward and right\n");
        // pause(1000);
        forward();
        if (irl == 1 && irr == 1 && irm == 1)
        {
          killbot();
          low(c3);
          break;
        }
        // pause(100);
        // right(10);
        // pause(100);
        jr();

        // pause(200);
      }
      else if (one == 1 && two == 0)
      {
        c++;
        print("Move back and left\n");
        // pause(1000);
        back();
        // pause(75);
        jl();

        // pause(200);

        // left(10);
        // pause(100);
      }
      else if (one == 0 && two == 0)
      {
        print("Move forward\n");
        // pause(1000);
        forward();
        if (irl == 1 && irr == 1 && irm == 1)
        {
          killbot();
          low(c3);
          break;
        }

        pause(100);
      }

      else

      {
        print("GOING BACK");
      }
    }

    else
    {

      print("GOing Forward");
      forward();
      // pause(200);

      c = 0;
    }

    /* if(r>3)
     {back();
     pause(200);
     jl();
     pause(100);
     r=0;
       } */  }
    if ((irl == 1 || irlx == 1) || irm == 1 || (irr == 1 || irrx == 1))
    {
      print("Reacged");
      killbot();
      low(c3);
    }
}

void jr()
{
    servo_set(rs, 1600);
    servo_set(ls, 1600);
    pause(100);
    stopper();
    pause(200);
}
void jl()
{
    servo_set(rs, 900);
    servo_set(ls, 900);
    pause(100);
    stopper();
    pause(200);
}
void blink_led(void *par1)
{
    high(led1);
    pause(500);
    low(led1);
    pause(500);
    cogstop(cog2);
}