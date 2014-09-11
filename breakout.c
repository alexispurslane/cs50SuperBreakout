//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 7

// lives
#define LIVES 3

// colors
#define red     0;
#define orange  1;
#define yellow  2;
#define green   3;
#define blue    4;


// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
  // seed pseudorandom number generator
  srand48(time(NULL));

  // instantiate window
  GWindow window = newGWindow(WIDTH, HEIGHT);

  // instantiate bricks
  initBricks(window);

  // instantiate ball, centered in middle of window
  GOval ball = initBall(window);

  // instantiate paddle, centered at bottom of window
  GRect paddle = initPaddle(window);

  // instantiate scoreboard, centered in middle of window, just above ball
  GLabel label = initScoreboard(window);

  // number of bricks initially
  int bricks = COLS * ROWS;

  // number of lives initially
  int lives = LIVES;

  // number of points initially
  int points = 0;

  // how long to wait
  int wait = 9;

  // Velocity for ball
  double velocity = 3.0;
  double xvelocity = 0;

  add(window, label);
  add(window, paddle);
  add(window, ball);
  // keep playing until game over
  waitForClick();
  while (lives > 0 && bricks > 0)
  {
    GEvent event = getNextEvent(MOUSE_EVENT);
    if (event != NULL)
    {
      // if the event was movement
      if (getEventType(event) == MOUSE_MOVED)
      {
        // ensure ball follows top cursor
        double x = getX(event) - getWidth(paddle) / 2;
        setLocation(paddle, x, getY(paddle));
      }
    }
    // move ball along x-axis
    xvelocity = xvelocity <= 0? -drand48() : drand48();
    move(ball, xvelocity, velocity);

    // bounce off right edge of window
    if (getX(ball) + RADIUS*2 >= getWidth(window))
    {
      xvelocity = -xvelocity;
    }
    // bounce off left edge of window
    else if (getX(ball) <= 0)
    {
      xvelocity = -xvelocity;
    }

    // bounce off bottom of window
    if (getY(ball) + RADIUS*2 >= getHeight(window))
    {
      lives--;
      setLocation(ball, WIDTH/2, HEIGHT/2);
      waitForClick();
    }
    // bounce off top of window
    else if (getY(ball) <= 0)
    {
      velocity = -velocity;
    }
    GObject collision = detectCollision(window, ball);
    if (collision != NULL && collision != label)
    {
      if (strcmp(getType(collision), "GRect") == 0 && collision != paddle)
      {
        setVisible(collision, false);
        bricks--;
        velocity = -velocity;
        if (strcmp(getColorGObject(collision), "#FF0000") == 0)
        {
          wait = 5;
          setColor(ball, "#FF0000");
          setColor(paddle, "#FF0000");
          points += 5;
        }
        else if (strcmp(getColorGObject(collision), "#FF9933") == 0)
        {
          if (!(wait < 7))
          {
            wait = 7;
            setColor(ball, "#FF9933");
            setColor(paddle, "#FF9933");
          }
          else
          {
            // leave wait alone...
          }
          points += 3;
        }
        else
        {
          points++;
        }
      } else {
        velocity = -velocity;
        int random = drand48() * 10;
        if (random <= 5)
        {
          // leave xvelocity alone... For now!
        }
        else if (random >= 5)
        {
          xvelocity = -xvelocity;
        }

        if (getY(ball) > getY(collision) + getHeight(collision))
        {
          move(ball, 0, 10);
        }
      }
    }
    // linger before moving again
    pause(wait);
    updateScoreboard(window, label, points);
  }

  if (lives > 0)
  {
    char s[12];
    sprintf(s, "WIN: %i!", points);
    setLabel(label, s);
  } else {
    setLabel(label, "LOOSE!");
  }
  // wait for click before exiting
  waitForClick();

  // game over
  closeGWindow(window);
  return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
  for (int i = 0; i < ROWS; i++)
  {
    for (int j = 0; j < COLS; j++)
    {
      int width = 40;
      int height = 13;
      int offset = 90;
      GRect rect = newGRect(j*(width), (i*(height))+offset, width, height);
      switch (i)
      {
        case 0:
          setColor(rect, "#FF0000");
          break;
        case 1:
          setColor(rect, "#FF9933");
          break;
        case 2:
          setColor(rect, "#FFCC00");
          break;
        case 3:
          setColor(rect, "#009900");
          break;
        case 4:
          setColor(rect, "#3366FF");
          break;
        default:
          exit(1);
          break;
      }
      setFilled(rect, true);
      add(window, rect);
    }
  }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
  int size = RADIUS*2;
  GOval ball = newGOval(WIDTH/2, HEIGHT/2, size, size); 
  setFilled(ball, true);
  setColor(ball, "#3366FF");
  sendToFront(ball);
  return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
  int width = 60;
  int height = 10;

  int x = (WIDTH/2) - width/2;
  int y = HEIGHT - height*6;

  GRect paddle = newGRect(x, y, width, height); 
  setFilled(paddle, true); 
  setColor(paddle, "#3366FF");
  return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
  GLabel label = newGLabel("CLICK");
  double x = (WIDTH - getWidth(label)) / 2;
  double y = (HEIGHT + getFontAscent(label)) / 2;
  setFont(label, "Courier 10 Pitch-bold-50");
  setColor(label, "#cccccc");
  setLocation(label, x, y);
  sendToBack(label);
  return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
  // update label
  char s[12];
  sprintf(s, "%i", points);
  setLabel(label, s);

  // center label in window
  double x = (getWidth(window) - getWidth(label)) / 2;
  double y = (getHeight(window) - getHeight(label)) / 2;
  setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
  // ball's location
  double x = getX(ball);
  double y = getY(ball);

  // for checking for collisions
  GObject object;

  // check for collision at ball's top-left corner
  object = getGObjectAt(window, x, y);
  if (object != NULL && isVisible(object))
  {
    return object;
  }

  // check for collision at ball's top-right corner
  object = getGObjectAt(window, x + 2 * RADIUS, y);
  if (object != NULL && isVisible(object))
  {
    return object;
  }

  // check for collision at ball's bottom-left corner
  object = getGObjectAt(window, x, y + 2 * RADIUS);
  if (object != NULL && isVisible(object))
  {
    return object;
  }

  // check for collision at ball's bottom-right corner
  object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
  if (object != NULL && isVisible(object))
  {
    return object;
  }

  // no collision
  return NULL;
}
