#include <GL/glut.h>

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

int edge[][2] = {
  { 0, 1 }, /* ア (A-B) */
  { 1, 2 }, /* イ (B-C) */
  { 2, 3 }, /* ウ (C-D) */
  { 3, 0 }, /* エ (D-A) */
  { 4, 5 }, /* オ (E-F) */
  { 5, 6 }, /* カ (F-G) */
  { 6, 7 }, /* キ (G-H) */
  { 7, 4 }, /* ク (H-E) */
  { 0, 4 }, /* ケ (A-E) */
  { 1, 5 }, /* コ (B-F) */
  { 2, 6 }, /* サ (C-G) */
  { 3, 7 }  /* シ (D-H) */
};
void display(void)
{
  int i;

  glClear(GL_COLOR_BUFFER_BIT);

  glColor3d(0.0, 0.0, 0.0);
  glBegin(GL_POLYGON);
  for(i = 0; i < 12; i++){
    glVertex3dv(vertex[edge[i][0]]);
    glVertex3dv(vertex[edge[i][1]]);
  }
  glEnd();

  glFlush();
}

void resize(int w, int h)
{
  glViewport(0, 0, w, h);

  glLoadIdentity();
  gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
  glTranslated(0.0, 0.0, -5.0);
}
void init(void)
{
  glClearColor(0.0, 0.0, 1.0, 1.0);
}
int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  init();
  glutMainLoop();
  return 0;
}
