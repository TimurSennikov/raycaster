#include <stdio.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define SCREENWIDTH 1920
#define SCREENHEIGHT 1080
#define MAPWIDTH 24
#define MAPHEIGHT 24
#define ROTSPEED 0.01
#define MOVESPEED 1

float boxPositionX = MAPWIDTH;
float boxPositionY = MAPHEIGHT;
float boxSize = 5.0f;

double posX = 3, posY = 3;  //x and y start position
double dirX = -1, dirY = 0; //initial direction vector
double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane

/*
double time = 0; //time of current frame
double oldTime = 0; //time of previous frame
*/

int calcul = 0;

int worldMap[MAPWIDTH][MAPHEIGHT]=
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
    glVertex2f(boxPositionX, boxPositionY);
    glVertex2f(boxPositionX + boxSize, boxPositionY);
    glVertex2f(boxPositionX + boxSize, boxPositionY + boxSize);
    glVertex2f(boxPositionX, boxPositionY + boxSize);
    glEnd();
}

void update() {
for(int x = 0; x < MAPWIDTH * MAPHEIGHT; x++){
      double cameraX = 2 * x / SCREENWIDTH - 1;
      double rayDirX = dirX + planeX * cameraX * 3;
      double rayDirY = dirY + planeY * cameraX;

      int mapX = posX;
      int mapY = posY;
      double sideDistX;
      double sideDistY;

       //length of ray from one x or y-side to next x or y-side
      double deltaDistX = (rayDirX == 0);
      double deltaDistY = (rayDirY == 0);
      if (deltaDistX == 0){deltaDistX=1e30;}
      if (deltaDistY == 0){deltaDistY=1e30;}

      double perpWallDist;

      //what direction to step in x or y-direction (either +1 or -1)
      int stepX;
      int stepY;

      int hit = 0; //was there a wall hit?
      int side; //was a NS or a EW wall hit?
	
	if (rayDirX < 0)
      {
        stepX = -1;
        sideDistX = (posX - mapX) * deltaDistX;
      }
      else
      {
        stepX = 1;
        sideDistX = (mapX + 1.0 - posX) * deltaDistX;
      }
      if (rayDirY < 0)
      {
        stepY = -1;
        sideDistY = (posY - mapY) * deltaDistY;
      }
      else
      {
        stepY = 1;
        sideDistY = (mapY + 1.0 - posY) * deltaDistY;
      }
	
	while (hit == 0)
      {
        //jump to next map square, either in x-direction, or in y-direction
        if (sideDistX < sideDistY)
        {
          sideDistX += deltaDistX;
          mapX += stepX;
          side = 0;
        }
        else
        {
          sideDistY += deltaDistY;
          mapY += stepY;
          side = 1;
        }

	 if (worldMap[mapX][mapY] > 0) hit = 1;
	
	        if (side == 0) {
            perpWallDist = fabs((mapX - posX + (1 - stepX) / 2) / rayDirX);
        } else {
            perpWallDist = fabs((mapY - posY + (1 - stepY) / 2) / rayDirY);
        }

}
	


	while (hit == 1) {
	
	calcul += 1;

	if(calcul==SCREENWIDTH)calcul = 0;

        // Calculate line height
        int lineHeight = SCREENHEIGHT / perpWallDist;

        // Calculate draw start and draw end
        int drawStart = -lineHeight / 2 + SCREENHEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + SCREENHEIGHT / 2;
        if (drawEnd >= SCREENHEIGHT) drawEnd = SCREENHEIGHT - 1;
	
	if (worldMap[mapX][mapY] == 1) glColor3f(1.0f, 0.0f, 0.0f);
	if (worldMap[mapX][mapY] == 2) glColor3f(0.0f, 1.0f, 0.0f);
	if (worldMap[mapX][mapY] == 3) glColor3f(0.0f, 0.0f, 1.0f);
	if (worldMap[mapX][mapY] == 4) glColor3f(0.5f, 0.5f, 0.5f);
        // Draw the line
        glBegin(GL_LINES);
        glVertex2f(calcul+1, drawStart); // Start point
        glVertex2f(calcul+1, drawEnd);   // End point
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f );
//	glClearColor(1.0f, 0.0f, 0.0f, 1.0f );
//	glClearColor(0.0f, 1.0f, 0.0f, 1.0f );
//	glClearColor(0.0f, 0.0f, 1.0f, 1.0f );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnd();
	
	sleep(0.5);

	hit = 0;
		}
	
	}

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*
	if (key == GLFW_KEY_W){
	      if(worldMap[posX + dirX * MOVESPEED][posY] == 0) posX += dirX * MOVESPEED;
	      if(worldMap[posX][posY + dirY * MOVESPEED] == 0) posY += dirY * MOVESPEED;
	}
	
	if (key == GLFW_KEY_W){
              if(worldMap[posX + dirX * MOVESPEED][posY] == 0) posX -= dirX * MOVESPEED;
              if(worldMap[posX][posY + dirY * MOVESPEED] == 0) posY -= dirY * MOVESPEED;
        }

	*/

	if (key == GLFW_KEY_A && action == GLFW_PRESS){
      double oldDirX = dirX;
      dirX = dirX * cos(-ROTSPEED) - dirY * sin(-ROTSPEED);
      dirY = oldDirX * sin(-ROTSPEED) + dirY * cos(-ROTSPEED);
      double oldPlaneX = planeX;
      planeX = planeX * cos(-ROTSPEED) - planeY * sin(-ROTSPEED);
      planeY = oldPlaneX * sin(-ROTSPEED) + planeY * cos(-ROTSPEED);
	}
	
	if (key == GLFW_KEY_D && action == GLFW_PRESS){
      double oldDirX = dirX;
      dirX = dirX * cos(-ROTSPEED) + dirY * sin(-ROTSPEED);
      dirY = oldDirX * sin(-ROTSPEED) - dirY * cos(-ROTSPEED);
      double oldPlaneX = planeX;
      planeX = planeX * cos(-ROTSPEED) + planeY * sin(-ROTSPEED);
      planeY = oldPlaneX * sin(-ROTSPEED) - planeY * cos(-ROTSPEED);
        }


}

int main() {

if (!glfwInit()) {
       	fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Raycaster", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
    glMatrixMode(GL_PROJECTION);
    glfwSetKeyCallback(window, key_callback);
    glLoadIdentity();
    glOrtho(0, SCREENWIDTH, SCREENHEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

      while (!glfwWindowShouldClose(window)) {
        update();

	glfwSwapBuffers(window);
        glfwPollEvents();
	}

}
