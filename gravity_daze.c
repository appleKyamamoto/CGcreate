/*
gcc -lglut -lGLU -o gravity_daze gravity_daze.c -lm
*/

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define FLOOR 50 /* 地面の１辺の大きさ */
#define MOVESPEED 0.6 /* 移動速度 */
#define RAD M_PI / 180 /* rad変換 */

#define TEXWIDTH 256 /* テクスチャ幅 */
#define TEXHEIGHT 256 /* テクスチャの高さ */
static const char texture1[] = "renga_256x256.raw"; /* テクスチャファイル名 */

double h[1000]; /* 建物の高さの乱数 */

time_t t;
int t_flag = -1; /*-1:無効化 1:有効化 */

double eye[3] = { FLOOR, 30.0, FLOOR }; /* 視点位置 */
double eyed[3] = { 1.0, 0.0, 0.0 }; /* 目標位置ベクトル(視点位置を中心に単位球)*/
double c_up = 1.0; /* カメラ上向き */
double theta = -29.0; /* y軸と視線方向のなす角 */
double phi = 230.0; /* x軸と視線方向ベクトルのxz平面成分のなす角 */

GLdouble vertex[][3] = {
  { 0.0, 0.0, 0.0 }, /*A*/
  { 1.0, 0.0, 0.0 }, /*B*/
  { 1.0, 1.0, 0.0 }, /*C*/
  { 0.0, 1.0, 0.0 }, /*D*/
  { 0.0, 0.0, 1.0 }, /*E*/
  { 1.0, 0.0, 1.0 }, /*F*/
  { 1.0, 1.0, 1.0 }, /*G*/
  { 0.0, 1.0, 1.0 }  /*H*/
}; /* 立方体 */

GLdouble texcoord[][2] = {
  { 0.0, 2.0 },
  { 2.0, 2.0 },
  { 2.0, 0.0 },
  { 0.0, 0.0 }
}; /* textureの座標 */

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

GLfloat light0pos[] = { 5.0, 3.0, 0.0, 1.0 };
GLfloat light1pos[] = { 5.0, 3.0, 0.0, 1.0 };

/* 色配列 */
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat blue[] = { 0.2, 0.2, 0.8, 1.0 };
GLfloat brown[] = { 0.85, 0.46, 0.14, 1.0};
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0};
GLfloat gray[] = { 0.5, 0.5, 0.5, 1.0};

void cube(void){
  int i;
  int j;

  /* 材質の設定 */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brown);

  /* アルファテスト開始 */
  glEnable(GL_ALPHA_TEST);
  
  /* テクスチャマッピング開始 */
  glEnable(GL_TEXTURE_2D);

  glBegin(GL_QUADS);
  for(j = 0; j < 6; ++j){
    glNormal3dv(normal[j]);
    for(i = 0; i < 4; ++i){
      glTexCoord2dv(texcoord[i]);
      glVertex3dv(vertex[face[j][i]]);
    }
  }
  glEnd();

  /* テクスチャマッピング終了 */
  glDisable(GL_TEXTURE_2D);

  /* アルファテスト終了 */
  glDisable(GL_ALPHA_TEST);
}

void Ground(void){
  double i,j;
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, gray);

  /* アルファテスト開始 */
  glEnable(GL_ALPHA_TEST);
  
  /* テクスチャマッピング開始 */
  glEnable(GL_TEXTURE_2D);

  glBegin(GL_QUADS);
  glNormal3dv(normal[5]);
  for(i = 0; i < FLOOR; i++){
    for(j = 0; j < FLOOR; j++){
      glTexCoord2d(0.0, 2.0);
        glVertex3d(i, 0.0, j + 1);
      glTexCoord2d(2.0, 2.0);
        glVertex3d(i + 1.0, 0.0, j + 1.0);
      glTexCoord2d(2.0, 0.0);
        glVertex3d(i + 1.0, 0.0, j);
      glTexCoord2d(0.0, 0.0);
        glVertex3d(i, 0.0, j);
    }
  }
  glEnd();

  /* テクスチャマッピング終了 */
  glDisable(GL_TEXTURE_2D);

  /* アルファテスト終了 */
  glDisable(GL_ALPHA_TEST);
}


double direction(double target[3], double a, double b){
  target[0] = cos(a * RAD) * cos(b * RAD);
  target[1] = sin(a * RAD);
  target[2] = cos(a * RAD) * sin(b * RAD);
} /* 目標位置設定 */

void angle(double v, double h){
  static int tr = 1.0;
  if((theta + tr * v) > 90 || (theta + tr * v) < -90){ tr *= -1; c_up *= -1;}
  theta += tr * v;
  phi += h;
} /* 角度の増減計算 */ 

void Move(char c){
  double d;
  int i;
  double n[3]; /* 進行方向ベクトル */
  double dir = 1.0; /* 方向による補正 1は順方向 */
    switch(c){
    case 'w':
    case 'W':
    case 's':
    case 'S':
      direction(n,theta,phi);
      if(c == 's' || c == 'S') dir *= -1; /* 後退 */
      break;
    case 'a':
    case 'A':
    case 'd':
    case 'D':
      direction(n,theta,phi + 90);
      if(c == 'a' || c == 'A') dir *= -1; /* 左 */
      break;
    case 'q':
    case 'Q':
    case 'e':
    case 'E':
      direction(n,theta + 90,phi);
      if(c == 'e' || c == 'E') dir *= -1; /* 下降 */
      break;
    default:
      break;
    }
    for(i = 0; i < 3; i++){
      d = dir * n[i] * MOVESPEED; /* 移動量計算 */
      eye[i] += d;
    }
} /* 移動 */

void drop(){
  time_t now = time(NULL);
  double d = 0.5 * 9.8 * difftime(now, t) * difftime(now, t) * 0.04;
  int i;
  for( i = 0;i < 3; i++){
    eye[i] += d * eyed[i];
  }
} /* 自由落下 */

void idle(void){
  if(t_flag>0){
    drop();
  }
  direction(eyed,theta,phi);
  glutPostRedisplay();
}

void scene(void){
  int i,j;

  glPushMatrix();

  /* 地面の描画 */
  Ground();

  /* 図形の描画 */
  for(j = 0;j < (FLOOR / 4); j++){
    glPushMatrix();
    glTranslated( 0.0, 0.0, (4 * j) + 1.0);
    for(i = 0;i < (FLOOR / 4); i++){
      glPushMatrix();

      /* 座標と大きさ指定 */
      glTranslated((4 * i) + 1.0, 0.0, 0.0);
      glScaled(h[j * 3 + i] / 3, h[j * 3 + i], h[j * 3 + i] / 3);
 
      cube();
      
      glPopMatrix();
    }
    glPopMatrix();
  }

  /* モデルビュー変換行列の復帰 */
  glPopMatrix();

  /*glBegin(GL_LINES);
  glVertex3d(eye[0] + eyed[0], eye[1] + eyed[1], eye[2] + eyed[2]);
  glVertex3d(FLOOR / 2, 0, FLOOR / 2);
  glEnd(); */  // 舞台の真ん中
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  /* 視点位置と視線方向 */
  gluLookAt(eye[0], eye[1], eye[2], eye[0] + eyed[0], eye[1] + eyed[1], eye[2] + eyed[2], 0.0, c_up, 0.0);
  /* 問題点 視点を上下回転しているときカメラ上向き方向を固定しているため変になる */

  /* 物体の描画 */
  scene();

  glutSwapBuffers();
}

void resize(int w, int h)
{
  glViewport(0, 0, w, h);

  /* 透視変換行列の設定 */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, (double)w / (double)h, 1.0, 200.0);

  /* モデルビュー変換行列の設定 */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye[0], eye[1], eye[2], eyed[0], eyed[1], eyed[2], 0.0, 1.0, 0.0);
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':
  case 'Q':
  case 'e':
  case 'E':
  case 'w':
  case 'W':
  case 's':
  case 'S':
  case 'a':
  case 'A':
  case 'd':
  case 'D':
    Move(key); /* 移動 */
    break;
  case 'j':
  case 'J':
    printf("x:%f y:%f z:%f theta:%f phi:%f\n",eye[0],eye[1],eye[2],theta,phi);
    break; 
  case 'r':
  case 'R':
    eye[0] = 25.0; eye[1] = 15.0; eye[2] = 35;
    eyed[0] = 1.0; eyed[1] = 0.0; eyed[2] = 0.0;
    theta = -10.0; phi = 37.0;
    glutPostRedisplay();
    break; /* 初期位置に戻る */
  case ' ':
    t_flag *= -1;
    t = time(NULL);
    break;
  case '\033':  /* '\033' は ESC の ASCII コード */
    exit(0);
  default:
    break;
  }
}

void specialkeyboard(int key, int x, int y)
{
  switch(key){
  case GLUT_KEY_UP:
    angle(1,0);
    break;
  case GLUT_KEY_DOWN:
    angle(-1, 0);
    break;
  case GLUT_KEY_LEFT:
    angle(0,-1);
    break;
  case GLUT_KEY_RIGHT:
    angle(0, 1);
    break;
  default:
    break;
  }
}
void init(void)
{
  /* テクスチャの読み込みに使う配列 */
  GLubyte texture[TEXHEIGHT][TEXWIDTH][4];
  FILE *fp;
  
  /* テクスチャ画像の読み込み */
  if ((fp = fopen(texture1, "rb")) != NULL) {
    fread(texture, sizeof texture, 1, fp);
    fclose(fp);
  }
  else {
    perror(texture1);
  }
 
  /* テクスチャ画像はワード単位に詰め込まれている */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  
  /* テクスチャの割り当て */
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, TEXWIDTH, TEXHEIGHT,
    GL_RGBA, GL_UNSIGNED_BYTE, texture);
    
  /* テクスチャを拡大・縮小する方法の指定 */
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  /* テクスチャ環境 */
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  /* アルファテストの比較関数 */
  glAlphaFunc(GL_GREATER, 0.5);
  
  /* 初期設定 */
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  /* 光源の初期設定 */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white);
  

  int i;
  srand((unsigned)time(NULL));
  for(i = 0; i < (FLOOR / 4) * (FLOOR / 4); i++){
    h[i] = (double)(rand() % 100) / 10;
  }
  glutIdleFunc(idle);
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
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialkeyboard);
  init();
  glutMainLoop();
  return 0;
}
