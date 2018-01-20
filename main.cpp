#include "main.h"

int main(int argc, char *argv[]) {
    string stringBaseOfBirds;
    cout << "Enter the base number of birds you want to create (like 2, 4, 8, 16 or 32)" << endl;
    getline(cin, stringBaseOfBirds);
    int decision = string2int(stringBaseOfBirds);
    if (decision != 2 && decision != 4 && decision != 8 && decision != 16 && decision != 32) {
        cout << "Invalid input: " << stringBaseOfBirds << "." << endl;
        cout << "Default base will be used: " << DEFAULT_BASE_OF_BIRDS << ". " << endl;
        base = DEFAULT_BASE_OF_BIRDS;
    } else {
        cout << "Your decision: " << decision << "." << endl;
        base = decision;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(1280, 720);
    int windowHandle = glutCreateWindow("GPU Based Rendering: Project");

    // Set the background color - white
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);

    glutDisplayFunc(Redraw);
    glutReshapeFunc(Reshape);
    glutMouseFunc(ProcessMouseClick);
    glutMotionFunc(ProcessMouseMoving);
    glutPassiveMotionFunc(ProcessMouseMoving);
    glutEntryFunc(ProcessFocus);
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

    setupShader();
    initVBO();
    setupTexture();
    setupFBO();
    setupVAO();

    glutMainLoop();

    return 0;
}