/*******************************************
  TARIK ANAFARTA

  Additional Functions:

  Timer gets redder as time passes.
  The longer you hold the spacebar, the faster the dart becomes.
  Adds points between 1-5 based on accuracy AND based on how fast you shot the balloon.
  Up arrow key to move the kid upwards.
  Down arrow key to move the kid downwards.
  If the player presses F2 key, moon appears and the sky becomes darker which indicates that the day became night.
*******************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600

#define TIMER_ON 1          // To disable the timer: 0, to enable the timer: 1.
#define TIMER_PERIOD 10    // Period for the timer.
#define DURATION 20       // The game continues for 20 seconds.

#define D2R 0.0174532
#define PI 3.1415

double timeCounter = DURATION;

int randomCoordinate_X() {
    int x = -105;
    return x + (rand() % 100 * 3);
}

int randomCoordinate_Y() {
    int y = -290;
    return y + ((rand() % 100) * -7.5);
}

// Global Variables
double green = 1, red = 0;
bool stop = true, gameState = true, up = false, down = false, right = false, left = false, timer = true, shoot = false, hit = false, night=false;
double power = 1;
int winWidth, winHeight, playerX = -425, playerY = 0, balloonX[5], balloonY[5], dartX = playerX + 25, dartY = playerY - 40, lastPoint = 0, totalPoints = 0, balloonCount = 0, shootCount = 0;

// to draw circle, center at (x,y) radius r
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void customCircle(float x, float y, float r, float start, float end) 
{
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    //I examined the circle function and came up with this function. It takes two angles as start and end points and draws the shape between them.

    for (float angle = start; angle < end; angle += 10)
        glVertex2f(r * cos(angle * D2R) + x, r * sin(angle * D2R) + y);

    glVertex2f(r * cos(end * D2R) + x, r * sin(end * D2R) + y);
    glEnd();
}

void playerDisplay(int playerX, int playerY)
{
    //body
    glColor3ub(255, 150, 0);
    glRectf(playerX - 10, playerY + 30, playerX + 10, playerY - 30);

    //belly
    circle(playerX, playerY - 5, 15);
    circle(playerX, playerY - 10, 15);

    //arm
    glRectf(playerX + 10, playerY + 30, playerX + 40, playerY + 20);
    circle(playerX + 40, playerY + 25, 5);

    glColor3ub(165, 42, 42);

    //legs
    glBegin(GL_POLYGON);
    glVertex2f(playerX, playerY - 30);
    glVertex2f(playerX + 10, playerY - 30);
    glVertex2f(playerX + 20, playerY - 60);
    glVertex2f(playerX + 10, playerY - 60);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(playerX, playerY - 30);
    glVertex2f(playerX - 10, playerY - 30);
    glVertex2f(playerX - 20, playerY - 60);
    glVertex2f(playerX - 10, playerY - 60);
    glEnd();

    //head
    glColor3ub(0, 0, 0);
    circle(playerX, playerY + 50, 20);

    //hat
    glColor3ub(0, 0, 255);
    customCircle(playerX, playerY + 50, 20, 0, 180);
    glRectf(playerX, playerY + 55, playerX + 40, playerY + 50);
    glColor3ub(210, 180, 140);
    customCircle(playerX, playerY + 50, 20, 270, 360);
}

void balloon(int balloonX, int balloonY)
{
    int radiusB = 50; //Balloon's initial radius

    glLineWidth(3);
    glColor3f(1, 1, 0); // Rope: Yellow
    glBegin(GL_LINES);
    glVertex2f(balloonX, balloonY);
    glVertex2f(balloonX, balloonY - 90);
    glEnd();

    glLineWidth(1);
    glColor3ub(123, 176, 128);// Body: Dark lime green
    circle(balloonX, balloonY, 50);

    glBegin(GL_TRIANGLES); // Tip of the body
    glVertex2f(balloonX, balloonY - 33);
    glVertex2f(balloonX - 10, balloonY - 60);
    glVertex2f(balloonX + 10, balloonY - 60);
    glEnd();

    // Light Reflection
    glColor3ub(230, 230, 230);
    circle(balloonX + 15, balloonY + 15, 15);
}

void dart(int dartX, int* dartY) {

    if (!shoot)
        *dartY = playerY + 25;

    glColor3ub(146, 146, 146); // Body of the dart: White
    glRectf(dartX + 20, *dartY - 5, dartX + 60, *dartY + 5); // This is a useful function I found on the internet. (It makes drawing rectangles easier.)

    glBegin(GL_TRIANGLES);
    glColor3ub(255, 0, 0); // Tip of the dart: Red
    glVertex2f(dartX + 60, *dartY + 5);
    glVertex2f(dartX + 70, *dartY);
    glVertex2f(dartX + 60, *dartY - 5);

    glColor3ub(0, 0, 0); // Bottom of the dart: Black
    glVertex2f(dartX + 20, *dartY + 5);
    glVertex2f(dartX + 20, *dartY);
    glVertex2f(dartX + 10, *dartY + 10);
    glVertex2f(dartX + 20, *dartY + 5);
    glVertex2f(dartX + 20, *dartY - 5);
    glVertex2f(dartX + 5, *dartY);
    glVertex2f(dartX + 20, *dartY - 5);
    glVertex2f(dartX + 20, *dartY);
    glVertex2f(dartX + 10, *dartY - 10);
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables. vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
        glutBitmapCharacter(font, str[i]);
}

void displayTimer()
{
    glColor3f(1, 1, 1);
    vprint(360, 270, GLUT_BITMAP_9_BY_15, "Remaining");
    vprint(385, 250, GLUT_BITMAP_9_BY_15, "Time");

    glColor3ub(red, green, 0);
    vprint(378, 220, GLUT_BITMAP_TIMES_ROMAN_24, "%04.2f", timeCounter);
}

// To display onto window using OpenGL commands
void display()
{
    // Clear window to Dark Blue
    glClearColor(103 / 255., 167/ 255., 236 / 255., 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw background
    if (night)
    {        
        glBegin(GL_QUADS);
        glColor3ub(5, 14,43);
        glVertex2f(-450, 300);
        glVertex2f(-450, -300);
        glVertex2f(450, -300);
        glVertex2f(450, 300);
        glEnd();
        glColor3f(1, 1, 1); // Moon
        circle(-330, 200, 80);
    }
    else
    {
        glColor3f(1, 1, 0); // Sun
        circle(-330, 200, 80);
    }
    // Dart
    dart(dartX, &dartY);

    // Balloons
    for (int i = 0; i < 5; i++)
        balloon(balloonX[i], balloonY[i]);
        
    // Draw kid
    playerDisplay(playerX, playerY);

    // Gaming area:

    // Bottom part
    glColor3ub(0, 0, 0);
    glRectf(-500, -270, 500, -300);

    // Right part
    glColor3ub(0, 0, 0);
    glRectf(350, 300, 500, -300);

    // Gaming area texts
    glColor3ub(255, 255, 0);
    vprint(-430, -290, GLUT_BITMAP_9_BY_15, "<SpaceBar> to Fire, <F1> Pause/Restart, <F2> Day/Night, Up and Down keys to move the player.");

    // Total balloon count
    glColor3ub(255, 255, 255);
    vprint(380, 160, GLUT_BITMAP_9_BY_15, "Total");
    vprint(370, 140, GLUT_BITMAP_9_BY_15, "Balloon");
    glColor3ub(255, 255, 0);
    vprint(392, 110, GLUT_BITMAP_TIMES_ROMAN_24, "%d", balloonCount);

    // Popped balloon count
    glColor3ub(255, 255, 255);
    vprint(375, 50, GLUT_BITMAP_9_BY_15, "Popped");
    vprint(370, 30, GLUT_BITMAP_9_BY_15, "Balloon");
    glColor3ub(255, 255, 0);
    vprint(397, 0, GLUT_BITMAP_TIMES_ROMAN_24, "%d", shootCount);

    // Last earned points
    glColor3ub(255, 255, 255);
    vprint(385, -70, GLUT_BITMAP_9_BY_15, "Last");
    vprint(375, -90, GLUT_BITMAP_9_BY_15, "Points");
    glColor3ub(255, 255, 0);
    vprint(397, -120, GLUT_BITMAP_TIMES_ROMAN_24, "%d", lastPoint);

    // Total points
    glColor3ub(255, 255, 255);
    vprint(380, -200, GLUT_BITMAP_9_BY_15, "Total");
    vprint(375, -220, GLUT_BITMAP_9_BY_15, "Points");
    glColor3ub(255, 0, 0);
    vprint(397, -250, GLUT_BITMAP_TIMES_ROMAN_24, "%d", totalPoints);

    displayTimer();
    glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y)
{
    if (key == 27) // Exit when ESC is pressed.
        exit(0);

    if (key == ' ') // Increase the speed of the dart.
        power += 0.5;

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ')
        shoot = true;

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void restart()
{
    //reset all the variables.
    timeCounter = DURATION;

    stop = true;
    gameState = true;
    timer = true;

    up = false;
    right = false;
    left = false;
    down = false;

    hit = false;
    shoot = false;

    red = 0;
    green = 1;

    playerY = 0;
    playerX = -425;
    power = 1;

    dartX = playerX + 30;
    dartY = playerY + 10;

    balloonCount = 0;
    shootCount = 0;

    totalPoints = 0;
    lastPoint = 0;

    for (int i = 0; i < 5; i++)
    {
        balloonX[i] = 400 + (rand() % 10) * 170;
        balloonY[i] = (rand() % 11) * 33 - (300 - 110);
    }
}

// Special Key like GLUT_KEY_F1, F2, F3,... Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP:
        up = true;
        break;
    case GLUT_KEY_DOWN:
        down = true;
        break;
    case GLUT_KEY_F1:
        if (gameState == true)
            stop = !stop;
        if (gameState == false)
        {
            restart();
            gameState = true;
        }
        break;
    case GLUT_KEY_F2:
        night = !night;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
void onSpecialKeyUp(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        up = false;
        break;
    case GLUT_KEY_DOWN:
        down = false;
        break;
    case GLUT_KEY_LEFT:
        left = false;
        break;
    case GLUT_KEY_RIGHT:
        right = false;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);

    if (gameState == true && stop == true) {

        red += 0.06; // red color code for the timer
        green -= 0.12; // green color code for the timer
        if (shoot) {
            if (370 < dartX)
            {
                power = 1;
                shoot = false;
                dartY = playerY + 10;
                dartX = playerX + 30;
            }
            else
                dartX += 7 * power;

            for (int i = 0; i < 5; i++)
                if (balloonX[i] - 50 < dartX + 50 && dartX + 50 < balloonX[i] + 50 && balloonY[i] - 50 < dartY && dartY < balloonY[i] + 50)
                {

                    lastPoint = fabs(balloonY[i] - dartY);
                    lastPoint -= 50;
                    lastPoint /= -10;
                    lastPoint *= power;
                    lastPoint++;

                    totalPoints += lastPoint; //the faster the player shoots, the more points he/she gets.
                    power = 1;

                    shoot = false;
                    shootCount++;
                    balloonCount++;

                    balloonX[i] = randomCoordinate_X();
                    balloonY[i] = randomCoordinate_Y();

                    dartY = playerY + 10;
                    dartX = playerX + 30;
                }
        }

        for (int i = 0; i < 5; i++)
            if (balloonY[i] < 350)
                balloonY[i] += 3;
            else
            {
                balloonCount++;
                balloonX[i] = randomCoordinate_X();
                balloonY[i] = randomCoordinate_Y();
            }

        if (up && playerY < winHeight / 2. - 60)
            playerY += 3;
        if (down && playerY > winHeight / -2. + 110)
            playerY -= 3;

        timeCounter -= 0.01;
        if (timeCounter < 0.1)
        {
            timeCounter = 0;
            gameState = !gameState;
        }
    }

    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()
}
#endif

void Init() // Smoothing shapes
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char* argv[])
{
    // Initialize random generator
    srand(time(NULL));

    for (int i = 0; i < 5; i++)
    {
        balloonCount++;
        balloonY[i] = randomCoordinate_Y();
        balloonX[i] = randomCoordinate_X();
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(500, -100); // Window's Initial Position
    glutCreateWindow("KID POPPING BALLOONS by Tarik Anafarta"); // Meaningful Window Title

    // Window Events
    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    // Keyboard Events
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);
    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

#if  TIMER_ON == 1
    // if (TIMER_ON) - Timer Event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();
    glutMainLoop();

    return 0;
}

/*
    GLUT to OpenGL coordinate conversion:
    x2 = x1 - winWidth / 2
    y2 = winHeight / 2 - y1
*/