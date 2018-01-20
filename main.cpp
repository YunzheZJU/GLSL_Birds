#include "main.h"

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("GPU Based Rendering: Project");

    glutDisplayFunc(Redraw);
    glutReshapeFunc(Reshape);
    glutMotionFunc(ProcessMouseMoving);
    glutPassiveMotionFunc(ProcessMouseMoving);
    glutEntryFunc(ProcessFocus);
    glutKeyboardFunc(ProcessNormalKey);
    glutSpecialFunc(ProcessSpecialKey);
    glutIdleFunc(Idle);

    init();

    glutMainLoop();

    return 0;
}