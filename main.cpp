#include "main.h"

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(1280, 720);
    int windowHandle = glutCreateWindow("GPU Based Rendering: A1");

    // Set the background color - white
    glClearColor(0.2, 0.2, 0.2, 1.0);
//    glColor3f(0.1, 0.3, 0.8);

    glutDisplayFunc(Redraw);
    glutReshapeFunc(Reshape);
//    glutMouseFunc(ProcessMouseClick);
//    glutPassiveMotionFunc(ProcessMouseMove);
//    glutEntryFunc(ProcessFocus);
    glutKeyboardFunc(ProcessNormalKey);
    glutSpecialFunc(ProcessSpecialKey);
    glutIdleFunc(Idle);

    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        cerr << "Error occurred when initializing GLEW: " << glewGetErrorString(glewErr) << endl;
        exit(1);
    }
    if (!glewIsSupported("GL_VERSION_4_3")) {
        cerr << "OpenGL 4.3 is not supported" << endl;
        exit(1);
    }

    try {
        birdShader.compileShader("bird.vert");
        birdShader.compileShader("bird.frag");
        birdShader.link();
        computeShader.compileShader("compute.vert");
        computeShader.compileShader("compute.frag");
        computeShader.link();
    } catch (GLSLProgramException &e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    initVBO();
    setupTexture();
    setupFBO();
    setupVAO();
//    setShader();

    glutMainLoop();

    return 0;
}