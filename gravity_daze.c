#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define FLOOR 7.0
#define MOVESPEED 0.2

double eye[3] = { 15.0, 10.0, 15.0 }; /* 視点位置 */
double eyed[3] ={ 0.0, 0.0, 0.0 }; /* 目標位置 */
double h[9];

GLdouble vertex[][3] = {
  { 0.0, 0.0, 0.0 }, /*A*/
  { 1.0, 0.0, 0.0 }, /*B*/
  { 1.0, 1.0, 0.0 }, /*C*/
  { 0.0, 1.0, 0.0 }, /*D*/
  { 0.0, 0.0, 1.0 }, /*E*/
  { 1.0, 0.0, 1.0 }, /*F*/
  { 1.0, 1.0, 1.0 }, /*G*/
  { 0.0, 1.0, 1.0 }  /**/
};

int face[][4] = {
  { 0, 1, 2, 3 },
  { 1, 5, 6, 2 },
  { 5, 4, 7, 6 },
  { 4, 0, 3, 7 },
  { 4, 5, 1, 0 },
  { 3, 2, 6, 7 }
};

GLdouble normal[][3] = {
  { 0.0, 0.0,-1.0 },
  { 1.0, 0.0, 0.0 },
  { 0.0, 0.0, 1.0 },
  {-1.0, 0.0, 0.0 },
  { 0.0,-1.0, 0.0 },
  { 0.0, 1.0, 0.0 }
};

GLdouble ground[][3] = {
  {  0.0,  0.0, 0.0 }, 
  { FLOOR,  0.0, 0.0 },
  { FLOOR,  0.0, FLOOR },
  {  0.0,  0.0, FLOOR },
};

GLfloat light0pos[] = { 5.0, 3.0, 0.0, 1.0 };
GLfloat light1pos[] = { 5.0, 3.0, 0.0, 1.0 };

GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat blue[] = { 0.2, 0.2, 0.8, 1.0 };
GLfloat brown[] = { 0.85, 0.46, 0.14, 1.0};
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0};

void cube(void){
  int i;
  int j;

  glBegin(GL_QUADS);
  for(j = 0; j < 6; ++j){
    glNormal3dv(normal[j]);
    for(i = 0; i < 4; ++i){
      glVertex3dv(vertex[face[j][i]]);
    }
  }
  glEnd();
}

void Ground(void){
  int i;
  
  glBegin(GL_QUADS);
  for(i = 0; i < 4; i++){
    glVertex3dv(ground[i]);
  }
  glEnd();
}

void direction(void){
  glBegin(GL_LINES);
  glColor3d(1.0, 0.0, 0.0);
  glVertex3d( 0.0, 0.0, 0.0); //x
  glVertex3d( 1.0, 0.0, 0.0);
  glColor3d(0.0, 1.0, 0.0);
  glVertex3d( 0.0, 0.0, 0.0); //y
  glVertex3d( 0.0, 1.0, 0.0);
  glColor3d(0.0, 0.0, 1.0);
  glVertex3d( 0.0, 0.0, 0.0); //z
  glVertex3d( 0.0, 0.0, 1.0);
  glEnd();
}
void idle(void){
  glutPostRedisplay();
}

void display(void)
{
  int i;
  int j;
  static int r = 0; /* 回転角 */

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  /* 視点位置と視線方向 */
  gluLookAt(eye[0], eye[1], eye[2], eyed[0], eyed[1], eyed[2], 0.0, 1.0, 0.0);

  /* 方向補助 */
  glPushMatrix();
  glTranslated(0.0, 4.0, 0.0);
  direction();
  glPopMatrix();
  
  /* 地面の描画 */
  glPushMatrix();
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brown);
  Ground();

  /* 図形の描画 */
  for(j = 0;j < 3; j++){
    glPushMatrix();
    glTranslated( 0.0, 0.0, (2 * j) + 1.0);
    for(i = 0;i < 3; i++){
      glPushMatrix();
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
      glTranslated((2 * i) + 1.0, 0.0, 0.0);
      glScaled(1.0, 2.0 + h[j * 3 + i], 1.0);
      cube();
      glPopMatrix();
    }
    glPopMatrix();
  }

  /* モデルビュー変換行列の復帰 */
  glPopMatrix();

  glutSwapBuffers();

  /* 一周回ったら回転角を 0 に戻す */
  if (++r >= 360) r = 0;
}

void resize(int w, int h)
{
  glViewport(0, 0, w, h);

  /* 透視変換行列の設定 */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

  /* モデルビュー変換行列の設定 */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye[0], eye[1], eye[2], eyed[0], eyed[1], eyed[2], 0.0, 1.0, 0.0);
}

void mouse(int button, int state, int x,int y){
  switch(button){
  case GLUT_LEFT_BUTTON:
    if(state == GLUT_DOWN){
      glutIdleFunc(idle);
    }
    else{
      /* アニメーション停止 */
      glutIdleFunc(0);
    }
    break;
  case GLUT_RIGHT_BUTTON:
    if(state == GLUT_DOWN){
      glutIdleFunc(idle);
     }
    break;
  default:
    break;
  }
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':
  case 'Q':
    eye[1] += MOVESPEED; eyed[1] += MOVESPEED;
    glutIdleFunc(idle); /* 上昇 */
    break;
  case 'e':
  case 'E':
    eye[1] -= MOVESPEED; eyed[1] -= MOVESPEED;
    glutIdleFunc(idle); /* 下降 */
    break;
  case '\033':  /* '\033' は ESC の ASCII コード */
    exit(0);
  case 'w':
  case 'W':
    eye[0] -= MOVESPEED; eyed[0] -= MOVESPEED;
    eye[2] -= MOVESPEED; eyed[2] -= MOVESPEED;
    glutIdleFunc(idle); /* 前進 */
    break;
  case 'a':
  case 'A':
    eye[0] -= MOVESPEED; eyed[0] -= MOVESPEED;
    eye[2] += MOVESPEED; eyed[2] += MOVESPEED;
    glutIdleFunc(idle); /* 左移動 */
    break;
  case 's':
  case 'S':
    eye[0] += MOVESPEED; eye[0] += MOVESPEED;
    eye[2] += MOVESPEED; eye[2] += MOVESPEED;
    glutIdleFunc(idle); /* 後退 */
    break;
  case 'd':
  case 'D':
    eye[0] += MOVESPEED; eye[0] += MOVESPEED;
    eye[2] -= MOVESPEED; eye[2] -= MOVESPEED;
    glutIdleFunc(idle); /* 右移動 */
    break;
  case 'r':
  case 'R':
    eye[0] = 15.0; eye[1] = 10.0; eye[2] = 15.0;
    eyed[0] = 0.0; eyed[1] = 0.0; eyed[2] = 0.0;
    glutPostRedisplay();
    break;
  default:
    break;
  }
}

void specialkeyboard(int key, int x, int y)
{
  switch(key){
  case GLUT_KEY_UP:
    eyed[1] += MOVESPEED;
    glutIdleFunc(idle);
    break;
  case GLUT_KEY_DOWN:
    eyed[1] -= MOVESPEED;
    glutIdleFunc(idle);
    break;
  case GLUT_KEY_LEFT:
    eyed[0] -= MOVESPEED; eyed[2] += MOVESPEED;
    glutIdleFunc(idle);
    break;
  case GLUT_KEY_RIGHT:
    eyed[0] += MOVESPEED; eyed[2] -= MOVESPEED;
    glutIdleFunc(idle);
    break;
  default:
    break;
  }
}
void init(void)
{
  glClearColor(1.0, 1.0, 1.0, 1.0);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  /* 光源の位置設定 */
  glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white);
  
  int i;
  srand((unsigned)time(NULL));
  for(i = 0; i < 9; i++){
    h[i] = (double)(rand()%10) / 10;
  }
}
int main(int argc, char *argv[])
{
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(640,480);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutMouseFunc(mouse);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialkeyboard);
  init();
  glutMainLoop();
  return 0;
}
