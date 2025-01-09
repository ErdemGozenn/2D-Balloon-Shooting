//Shooting Balloon Game 
/*Erdem Gozen 22103098 164-04
When press the F1, the game stop/reset 
when press space bar, the game fire the gun and move triger
when press the up and down key, the gun moving in Y cordinates
*/
#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define TIMER_PERIOD   20 
#define TIMER_ON        1 // 0:disable timer, 1:enable timer
#define D2R 0.01745329252
#define PI  3.14159265358

#define MAX_FIRE 1  // Maximum number of fires at a time
#define TARGET_RADIUS 45
#define MAX_BALLOON 5

#define WEAPON_SIZE 35
#define FIRE_SIZE 8

// Global variables
int winWidth, winHeight; // Current window width and height
bool spacebar = false;
bool F1 = false;
bool game_over = false;
bool paused = false;
int score = 0;
int last_hit_point = 0;
int remaining_time = 35 * 1000; // 35 seconds
int total_balloon_passed = 0;
int total_balloon_shot = 0;
int balloon_on_screen = 0; // Count of balloon
int shot = 0;
//balloon color
double k = (double)rand() / RAND_MAX; // Random value between 0 and 1 
double l = (double)rand() / RAND_MAX;
double m = (double)rand() / RAND_MAX;
// User defined types and variables
typedef struct {
	float x, y;
} point_t;

typedef struct {
	point_t pos;
	float speed;
} balloon_t;

typedef struct {
	point_t pos;
	float angle;
	float size;
	bool active;
} fire_t;

typedef struct {
	point_t pos;
	float speed;
	float size;
	float r, g, b;
} weapon_t;

balloon_t balloon[MAX_BALLOON];
fire_t fire[MAX_FIRE];
weapon_t weapon = { {-350, 0}, 5, WEAPON_SIZE, 0.5,0.5, 0.5 }; // Weapon's initial position, speed, size, and color

// Functions for drawing
void circle(int x, int y, int r) {
	float angle;
	
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++) {
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char* string, void* font) {
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, string[i]);
}

void line(point_t p1, point_t p2) {
	glBegin(GL_LINES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glEnd();
}

void triangle(point_t p1, point_t p2, point_t p3) {
	glBegin(GL_TRIANGLES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glVertex2f(p3.x, p3.y);
	glEnd();
}


void drawBalloon(balloon_t balloon) {
	point_t top = { balloon.pos.x, balloon.pos.y };
	point_t rope_top = { balloon.pos.x, balloon.pos.y - 30 };
	point_t rope_bottom = { balloon.pos.x, balloon.pos.y - 60 };

	// Draw rope
	line(rope_top, rope_bottom);
	// Draw balloon
	
	glColor3f(k, l, m);
	circle(top.x, top.y, 30);
}

void drawGradient(int x1, int y1, int w, int h, float r, float g, float b) {
	glBegin(GL_QUADS);
	glColor3f(r, g, b);
	glVertex2f(x1, y1);
	glVertex2f(x1 + w, y1);
	glColor3f(r + 0.4, g + 0.4, b + 0.4);
	glVertex2f(x1 + w, y1 - h);
	glVertex2f(x1, y1 - h);
	glEnd();

	glColor3f(0.1, 0.1, 0.1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, y1);
	glVertex2f(x1 + w, y1);
	glVertex2f(x1 + w, y1 - h);
	glVertex2f(x1, y1 - h);
	glEnd();
}

void drawCloud(int x, int y) {
	glColor3f(1, 1, 1); // White
	circle(0 + x, 0 + y, 30);
	circle(-25 + x, y, 20);
	circle(25 + x, -2 + y, 20); double k = (double)rand() / RAND_MAX; // Random value between 0 and 1 
	double l = (double)rand() / RAND_MAX;
	double m = (double)rand() / RAND_MAX;
	circle(21 + x, -19 + y, 10);
}

void drawBackground() {

	drawGradient(-400, 300, 800, 600, 0.5, 0.5, 0.9); // Sky
	drawGradient(-400, -200, 800, 100, 0.4, 1, 0.7);  // Green bottom
	//Grass
	for (int i = -320; i < 800; i += 5) {
		glBegin(GL_LINES);
		glColor3f(0, 1, 0); // Green
		glVertex2f(-410+i,-190 );
		glVertex2f(-400+i,-200 );
		glEnd();
	}
	// Info message
	glColor3f(0, 0, 0); //bLACK
	print(-370, -215, "SpaceBar: Fire               F1: Pause/Restrart              Up KEY / Down KEY = Gun movements", GLUT_BITMAP_8_BY_13);
	// Draw 3 clouds
	drawCloud(-250, 180);
	drawCloud(250, 100);
	drawCloud(0, 200);
}

void drawFire(fire_t fire) {
	glColor3f(0.5, 0.5, 0.5); // grey
	circle(fire.pos.x, fire.pos.y, 5);
}

void drawGun(weapon_t weapon) {
	// Set gun color
	glColor3f(0.5f, 0.5f, 0.5f); // Gray color for gun body

	// Draw gun body
	glBegin(GL_POLYGON);
	glVertex2f(weapon.pos.x - 20, weapon.pos.y - 5); // Bottom left
	glVertex2f(weapon.pos.x + 40, weapon.pos.y - 5); // Bottom right
	glVertex2f(weapon.pos.x + 40, weapon.pos.y + 5); // Top right
	glVertex2f(weapon.pos.x - 20, weapon.pos.y + 5); // Top left
	glEnd();

	// Set color for the barrel
	glColor3f(0.3f, 0.3f, 0.3f); // Dark gray for barrel

	// Draw barrel
	glBegin(GL_POLYGON);
	glVertex2f(weapon.pos.x + 35, weapon.pos.y - 3); // Bottom right
	glVertex2f(weapon.pos.x + 100, weapon.pos.y - 3); // Bottom right extended
	glVertex2f(weapon.pos.x + 100, weapon.pos.y + 3); // Top right extended
	glVertex2f(weapon.pos.x + 35, weapon.pos.y + 3); // Top right
	glEnd();

	// Set color for the magazine
	glColor3f(0.3f, 0.3f, 0.3f); // Dark gray for magazine

	// Draw magazine
	glBegin(GL_POLYGON);
	glVertex2f(weapon.pos.x - 20, weapon.pos.y - 5); // Bottom left
	glVertex2f(weapon.pos.x - 20, weapon.pos.y - 30); // Top left
	glVertex2f(weapon.pos.x, weapon.pos.y - 30); // Top right
	glVertex2f(weapon.pos.x, weapon.pos.y - 5); // Bottom right
	glEnd();

	// Set color for the sights
	glColor3f(0.3f, 0.3f, 0.3f); // Dark gray for sights

	// Draw sights
	glBegin(GL_TRIANGLES);
	glVertex2f(weapon.pos.x + 30, weapon.pos.y + 3); // Top sight
	glVertex2f(weapon.pos.x + 30, weapon.pos.y - 3); // Bottom left sight
	glVertex2f(weapon.pos.x + 25, weapon.pos.y);    // Bottom right sight
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(weapon.pos.x +10, weapon.pos.y-2);
	glVertex2f(weapon.pos.x+15-shot, weapon.pos.y-15);
	glEnd();

	
}



void drawScore(int score) {
	glColor3f(1, 1, 1); // White
	char scoreStr[20];
	sprintf(scoreStr, "Score: %d / %d", total_balloon_shot, total_balloon_passed);
	glRasterPos2f(-WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - 40);
	int len = strlen(scoreStr);
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreStr[i]);
	}
}

void drawLastHitPoint(int last_hit_point) {
	glColor3f(1, 1, 1); // White
	char pointStr[20];
	sprintf(pointStr, "Last Point: %d", last_hit_point);
	glRasterPos2f(-WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - 60);
	int len = strlen(pointStr);
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, pointStr[i]);
	}
}

// Initialization
void init() {
	srand(time(NULL));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2, WINDOW_HEIGHT / 2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Initialize balloon
	
	for (int i = 0; i < MAX_BALLOON; i++) {
		balloon[i].pos.x = rand() % 251; // random x position between 0 and 250
		balloon[i].pos.y =-300; // random y position between WINDOW_HEIGHT / 2 and WINDOW_HEIGHT
		balloon[i].speed = 1.5 + (rand() % 3) * 0.5; // random speed between 1.5 and 3, slower than before
	}
}

void resetBalloon(balloon_t *bal) {
	bal->pos.x = rand() % 251; // random x position between 0 and 250
	bal->pos.y = -300; // random y position between WINDOW_HEIGHT / 2 and WINDOW_HEIGHT
	bal->speed = 1.5 + (rand() % 3) * 0.5; // random speed between 1.5 and 3, slower than before
}

// Timer function
void onTimer(int v) {
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	if (F1) {
		if (game_over) {
			game_over = false;
			score = 0;
			remaining_time = 20 * 1000;
			total_balloon_passed = 0;
			total_balloon_shot = 0;
			init();
		}
		else {
			paused = !paused;
		}
		F1 = false;
	}

	if (!paused && !game_over) {
		remaining_time -= TIMER_PERIOD;
		if (remaining_time <= 0) {
			game_over = true;
			remaining_time = 0;
		}

		if (spacebar) {
			shot = 5;
			int availFire = -1;
			for (int i = 0; i < MAX_FIRE; i++) {
				if (!fire[i].active) {
					availFire = i;
					break;
				}
			}
			if (availFire != -1) {
				fire[availFire].pos.x = weapon.pos.x; // Weapon's x position
				fire[availFire].pos.y = weapon.pos.y; // Weapon's y position
				fire[availFire].active = true;
			}
		}
		if (!spacebar)
			shot = 0;

		for (int i = 0; i < MAX_FIRE; i++) {
			if (fire[i].active) {
				fire[i].pos.x += 10; // Move fire towards right
				if (fire[i].pos.x > WINDOW_WIDTH / 2)
				{
					fire[i].active = false;
				}
			}
		}
		
	
		for (int i = 0; i < MAX_BALLOON; i++) {
			balloon[i].pos.y += balloon[i].speed; // Move ballon downward
			if (balloon[i].pos.y > WINDOW_HEIGHT / 2) { // If balloon goes out of screen
				balloon[i].pos.y = -300; // Reset its y position
				balloon[i].pos.x = rand() % 251; // Reset its x position
				total_balloon_passed++;
	
			}
			else if (balloon[i].pos.y > -WINDOW_HEIGHT / 2 && balloon[i].pos.y < WINDOW_HEIGHT / 2 && balloon_on_screen < MAX_BALLOON) {
				balloon_on_screen++;
				//to calculate balloon count in the screen
			}
		}


		for (int i = 0; i < MAX_FIRE; i++) {
			if (fire[i].active) {
				for (int j = 0; j < MAX_BALLOON; j++) {
					float distance = sqrt(pow(fire[i].pos.x - balloon[j].pos.x, 2) + pow(fire[i].pos.y - balloon[j].pos.y, 2));
					if (distance < TARGET_RADIUS) {
						balloon[j].pos.y = WINDOW_HEIGHT / 2 + rand() % (WINDOW_HEIGHT / 2); // Reset balloon's position
						balloon[j].pos.x = rand() %200+50; // Reset balloon's x position with random angle
						balloon[j].pos.y = -320; // Reset baloon's y position with random angle
						// Calculate the hit point based on the distance between the hit point and the center of the target
						if (distance < 5) {
							last_hit_point += 10;
						}
						else if (distance < 20) {
							last_hit_point += 8;
						}
						else if (distance < 30) {
							last_hit_point += 5;
						}
						else if (distance < 45) {
							last_hit_point += 2;
						}
						else {
							last_hit_point += 1;
						}
						score += last_hit_point; // increase score based on the hit point
						total_balloon_shot++;
						balloon_on_screen--;// when hitten the balloon decrease to new one occur
						total_balloon_passed++;
					}
					
				}
			}
		}


	}
	

	glutPostRedisplay();
}


// Display function
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	drawBackground();

	// Draw balloon
	for (int i = 0; i < MAX_BALLOON; i++) {
        // Generate random color components (R, G, B)
      
        drawBalloon(balloon[i]);
    }

	// Draw fires
	for (int i = 0; i < MAX_FIRE; i++) {
		if (fire[i].active)
			drawFire(fire[i]);
	}

	// Draw weapon
	drawGun(weapon);

	// Draw score
	drawScore(score);

	// Draw last hit point
	drawLastHitPoint(last_hit_point);

	// Draw remaining time
	char timeStr[20];
	sprintf(timeStr, "Time: %d", remaining_time / 1000);
	glColor3f(1, 1, 1); // White
	glRasterPos2f(-WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - 20);
	int len = strlen(timeStr);
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timeStr[i]);
	}

	// Draw game over message
	if (game_over) {
		glColor3f(1, 1, 1); // White
		char gameOverStr[] = "Game Over! Press F1 to restart";
		glRasterPos2f(-80, 0);
		int len = strlen(gameOverStr);
		for (int i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, gameOverStr[i]);
		}

		char scoreStr[20];
		sprintf(scoreStr, "Score: %d\n", total_balloon_shot);
		sprintf(scoreStr, "Total Ballon: %d", total_balloon_passed);
		glRasterPos2f(-50, -40);
		len = strlen(scoreStr);
		for (int i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreStr[i]);
		}
		score = 0;
	}

	glutSwapBuffers();
}



// Keyboard down function
void onKeyDown(unsigned char key, int x, int y) {
	if (key == 32) // Spacebar
		spacebar = true;
}
// Keyboard up function
void onKeyUp(unsigned char key, int x, int y) {
	if (key == 32) // Spacebar
		spacebar = false;
}

// keys and gun movements
void onSpecialKeyDown(int key, int x, int y) {
	if (key == GLUT_KEY_F1) // F1 key
		F1 = true;
	if (key == GLUT_KEY_UP && weapon.pos.y < WINDOW_HEIGHT / 2) // Up arrow key
		weapon.pos.y += weapon.speed;
	if (key == GLUT_KEY_DOWN && weapon.pos.y > -WINDOW_HEIGHT / 2) // Down arrow key
		weapon.pos.y -= weapon.speed;
}

// Resize function
void onResize(int w, int h) {
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-w / 2, w / 2, -h / 2, h / 2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Shoting Balloon Game");

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialFunc(onSpecialKeyDown);
	glutReshapeFunc(onResize);

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	glutMainLoop();
	return 0;
}