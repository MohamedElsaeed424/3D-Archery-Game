#include <cmath>
#include <vector>
#include <ctime>
#include <string>
#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#include <chrono>
#include <glut.h>

#define M_PI 3.14159265358979323846
// Global Variables
float playerX = 0.0f, playerY = 0.0f, playerZ = 8.0f;
float playerRotation = 0.0f;
bool isMoving = false;
float shootingPower = 1.0f;
bool isPowerCharging = false;
const float MAX_POWER = 2.0f;
const float POWER_INCREASE_RATE = 0.1f;

float cameraX = 0.0f, cameraY = 4.0f, cameraZ = 20.0f;
int cameraView = 0; // 0: Free, 1: Top, 2: Side, 3: Front
float cameraSpeed = 0.5f;
float cameraRotationX = 0.0f;
float cameraRotationY = 0.0f;

bool mouseRotating = false;
int lastMouseX = 0;
int lastMouseY = 0;
float mouseSensitivity = 0.2f;
float yaw = 0.0f;   // Left/right rotation
float pitch = 0.0f;  // Up/down rotation

bool gameWon = false;
bool gameLost = false;

int timeRemaining = 60; // 60 seconds game time
bool targetHit = false;
float targetX = 0.0f, targetY = 2.0f, targetZ = -8.0f;
float targetRotation = 0.0f;
int targerHitCount = 0;

//  for light control
GLfloat light1_position[] = { -8.0f, 8.0f, -7.0f, 1.0f };
GLfloat light1_direction[] = { 0.0f, -1.0f, 0.5f };
GLfloat light1_diffuse[] = { 1.0f, 0.95f, 0.8f, 1.0f }; // Warm light color
GLfloat light1_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat light1_specular[] = { 1.0f, 0.95f, 0.8f, 1.0f };

bool isBackgroundMusicPlaying = false;

// Animation flags and variables
bool treeAnimation = false;
bool benchAnimation = false;
bool flagAnimation = false;
bool lightAnimation = false;
bool bannerAnimation = false;
float legAnimation = 0.0f;
float armAnimation = 0.0f;
float shootingArmAngle = 0.0f;  // For smooth arm movement during shooting
float drawbackAmount = 0.0f;    // How far the arm is pulled back
const float MAX_DRAWBACK = 45.0f; // Maximum angle for drawing back the bow
float animationAngles[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };


float wallColorR = 0.545f;  // Saddle brown R component
float wallColorG = 0.271f;  // Saddle brown G component
float wallColorB = 0.075f;  // Saddle brown B component

struct Arrow {
    float x, y, z;       // Position
    float dx, dy, dz;    // Direction
    bool active;         // Is arrow currently flying?
    float speed;         // Arrow speed
};
std::vector<Arrow> arrows;  // Store multiple arrows

const int NUM_AUDIENCE = 100;  // Number of audience members
struct AudienceMember {
    float x, z;              // Position
    float baseY;            // Base height for bobbing animation
    float animationOffset;  // Individual animation timing offset
    float scale;           // Individual size variation
    float rotation;        // Rotation angle
    int colorVariation;    // Color variation for clothes
};
std::vector<AudienceMember> audience;



// Function prototypes
LPCWSTR convertToLPCWSTR(const char* charArray);
void playBackgroundMusic();
void stopBackgroundMusic();
void playSound(const char* filename, bool loop);
void drawPlayer();
void updatePlayerAnimations();
void drawDisk(float outerRadius, float height, int slices);
void drawGoldCup();
void drawTarget();
void drawArrow();
void drawWalls();
void drawGround();
void drawTree();
void drawBench();
void drawFlag();
void drawLight();
void drawBanner();
void drawAudienceMember(const AudienceMember& member, float animationTime);
void drawAudience();
void drawText(float x, float y, std::string text);
void glutSolidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks);
void checkCollisions();
void updateAnimations();
void updateArrows();
void drawGameEndScreen();
void reshape(int w, int h);
void init();
void initAudience();
void updateLights();
void display();
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void timer(int value);



LPCWSTR convertToLPCWSTR(const char* charArray) {
    size_t newSize = strlen(charArray) + 1;
    wchar_t* wString = new wchar_t[newSize];
    mbstowcs(wString, charArray, newSize); // Convert to wide character
    return wString;
}
void playBackgroundMusic() {
    mciSendString(L"open \"D:\\GUC\\Semester 7\\Graphics\\3DGame\\sounds\\background.wav\" type mpegvideo alias bgMusic", NULL, 0, NULL);
    mciSendString(L"play bgMusic repeat", NULL, 0, NULL);
}
void stopBackgroundMusic() {
    // Stop the music associated with the alias "bgMusic"
    mciSendString(L"stop bgMusic", NULL, 0, NULL);

    // Close the music resource
    mciSendString(L"close bgMusic", NULL, 0, NULL);
}
void playSound(const char* filename, bool loop) {
    LPCWSTR wideFilename = convertToLPCWSTR(filename);

    if (loop && !isBackgroundMusicPlaying) {
        // Play the background music in loop if it's not already playing
        PlaySound(wideFilename, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        isBackgroundMusicPlaying = true; // Set the flag that music is playing
    }
    else if (!loop) {
        // Play the sound effect without looping
        PlaySound(wideFilename, NULL, SND_FILENAME | SND_ASYNC);
    }

    delete[] wideFilename; // Clean up memory
}

void glutSolidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks) {
    const double PI = 3.14159265358979323846;
    GLfloat x, y, z;
    GLfloat angle;
    GLfloat angleStep = 2.0f * PI / slices;
    GLfloat stackStep = height / stacks;

    // Draw the tube
    glPushMatrix();
    glBegin(GL_QUADS);
    for (int i = 0; i < slices; i++) {
        angle = i * angleStep;
        GLfloat nextAngle = angle + angleStep;

        x = radius * cos(angle);
        y = radius * sin(angle);
        GLfloat nextX = radius * cos(nextAngle);
        GLfloat nextY = radius * sin(nextAngle);

        // Calculate normal for proper lighting
        GLfloat nx = cos(angle);
        GLfloat ny = sin(angle);
        GLfloat nextNx = cos(nextAngle);
        GLfloat nextNy = sin(nextAngle);

        for (int j = 0; j < stacks; j++) {
            z = j * stackStep;
            GLfloat nextZ = (j + 1) * stackStep;

            // First vertex
            glNormal3f(nx, ny, 0.0f);
            glVertex3f(x, y, z);

            // Second vertex
            glNormal3f(nextNx, nextNy, 0.0f);
            glVertex3f(nextX, nextY, z);

            // Third vertex
            glNormal3f(nextNx, nextNy, 0.0f);
            glVertex3f(nextX, nextY, nextZ);

            // Fourth vertex
            glNormal3f(nx, ny, 0.0f);
            glVertex3f(x, y, nextZ);
        }
    }
    glEnd();

    // Draw bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);  // Center
    for (int i = 0; i <= slices; i++) {
        angle = i * angleStep;
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y, 0.0f);
    }
    glEnd();

    // Draw top cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, height);  // Center
    for (int i = slices; i >= 0; i--) {
        angle = i * angleStep;
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y, height);
    }
    glEnd();
    glPopMatrix();
}

void drawPlayer() {
    glPushMatrix();
    glTranslatef(playerX, playerY, playerZ);
    glRotatef(playerRotation, 0.0f, 1.0f, 0.0f);

    // Material properties
    GLfloat skin_ambient[] = { 0.4f, 0.3f, 0.25f, 1.0f };
    GLfloat skin_diffuse[] = { 0.8f, 0.65f, 0.55f, 1.0f };
    GLfloat skin_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat skin_shininess = 20.0f;

    // Fancy athletic shirt material (shiny sports fabric)
    GLfloat shirt_ambient[] = { 0.1f, 0.1f, 0.3f, 1.0f };
    GLfloat shirt_diffuse[] = { 0.2f, 0.2f, 0.8f, 1.0f };
    GLfloat shirt_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat shirt_shininess = 80.0f;

    // Head and Face
    glMaterialfv(GL_FRONT, GL_AMBIENT, skin_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, skin_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, skin_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, skin_shininess);

    // Head
    glPushMatrix();
    glTranslatef(0.0f, 1.6f, 0.0f);
    glColor3f(0.8f, 0.65f, 0.55f);
    glutSolidSphere(0.15f, 16, 16);

    // Hair with style
    glColor3f(0.1f, 0.05f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.08f, 0.0f);
    // Base hair
    glPushMatrix();
    glScalef(0.17f, 0.1f, 0.17f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Hair strands
    for (int i = 0; i < 8; i++) {
        glPushMatrix();
        glRotatef(45.0f * i, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.08f, 0.0f, 0.0f);
        glScalef(0.1f, 0.05f, 0.02f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    glPopMatrix();

    // Eyes with expression
    glColor3f(1.0f, 1.0f, 1.0f);
    // Left eye
    glPushMatrix();
    glTranslatef(-0.05f, 0.0f, 0.12f);
    glutSolidSphere(0.025f, 8, 8);
    // Iris
    glColor3f(0.2f, 0.4f, 0.8f);
    glTranslatef(0.0f, 0.0f, 0.01f);
    glutSolidSphere(0.015f, 8, 8);
    // Pupil
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, 0.005f);
    glutSolidSphere(0.008f, 8, 8);
    glPopMatrix();

    // Right eye (similar to left)
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.05f, 0.0f, 0.12f);
    glutSolidSphere(0.025f, 8, 8);
    glColor3f(0.2f, 0.4f, 0.8f);
    glTranslatef(0.0f, 0.0f, 0.01f);
    glutSolidSphere(0.015f, 8, 8);
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, 0.005f);
    glutSolidSphere(0.008f, 8, 8);
    glPopMatrix();

    // Expressive mouth (changes with shooting)
    glColor3f(0.8f, 0.4f, 0.4f);
    glPushMatrix();
    glTranslatef(0.0f, -0.05f, 0.12f);
    if (isPowerCharging) {
        // Concentrated expression
        glScalef(0.04f, 0.01f, 0.02f);
        glutSolidCube(1.0f);
    }
    else {
        // Relaxed smile
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glutSolidTorus(0.005f, 0.02f, 8, 16);
    }
    glPopMatrix();
    glPopMatrix();

    // Athletic Shirt
    glMaterialfv(GL_FRONT, GL_AMBIENT, shirt_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, shirt_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, shirt_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shirt_shininess);

    glColor3f(0.2f, 0.2f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, 1.1f, 0.0f);
    // Form-fitting shirt
    glScalef(0.25f, 0.4f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Arms with archery animation
    float archeryArmAngle = isPowerCharging ? 90.0f : 0.0f;
    float bowArmAngle = isPowerCharging ? 90.0f : 0.0f;

    // Bow arm (left)
    glPushMatrix();
    glTranslatef(-0.25f, 1.3f, 0.0f);
    glRotatef(bowArmAngle, 1.0f, 0.0f, 0.0f);

    // Upper arm
    glColor3f(0.2f, 0.2f, 0.8f);
    glPushMatrix();
    glScalef(0.08f, 0.25f, 0.08f);
    glutSolidCube(1.0f);
    glPopMatrix();

    if (isPowerCharging) {
        // Bow body
        glColor3f(0.4f, 0.2f, 0.0f);
        glPushMatrix();
        glTranslatef(0.0f, -0.3f, 0.1f);

        // Main bow curve
        GLUquadricObj* quadObj = gluNewQuadric();
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        for (int i = -30; i <= 30; i++) {
            glPushMatrix();
            glRotatef(i * 2, 0.0f, 1.0f, 0.0f);
            gluCylinder(quadObj, 0.02f, 0.02f, 0.02f, 8, 2);
            glPopMatrix();
        }

        // Bowstring
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.3f, 0.0f);
        glVertex3f(0.0f, -0.3f, 0.0f);
        glEnd();

        glPopMatrix();
        gluDeleteQuadric(quadObj);
    }
    glPopMatrix();

    // Drawing arm (right)
    glPushMatrix();
    glTranslatef(0.25f, 1.3f, 0.0f);
    glRotatef(archeryArmAngle, 1.0f, 0.0f, 0.0f);

    // Upper arm
    glColor3f(0.2f, 0.2f, 0.8f);
    glPushMatrix();
    glScalef(0.08f, 0.25f, 0.08f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw arrow when charging
    if (isPowerCharging) {
        glColor3f(0.6f, 0.3f, 0.0f);
        glPushMatrix();
        glTranslatef(0.0f, -0.3f, 0.1f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

        // Arrow shaft
        GLUquadricObj* quadObj = gluNewQuadric();
        gluCylinder(quadObj, 0.01f, 0.01f, 0.4f, 8, 1);

        // Arrow head
        glTranslatef(0.0f, 0.0f, 0.4f);
        glutSolidCone(0.02f, 0.05f, 8, 1);

        // Arrow fletching
        glTranslatef(0.0f, 0.0f, -0.4f);
        for (int i = 0; i < 3; i++) {
            glRotatef(120.0f, 0.0f, 0.0f, 1.0f);
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_TRIANGLES);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.03f, 0.0f, 0.1f);
            glVertex3f(0.0f, 0.0f, 0.1f);
            glEnd();
        }

        gluDeleteQuadric(quadObj);
        glPopMatrix();
    }
    glPopMatrix();


    // Legs with animation
    float legSwing = isMoving ? sin(legAnimation) * 20.0f : 0.0f;

    // Pants color
    glColor3f(0.2f, 0.2f, 0.2f);  // Dark pants

    // Left leg
    glPushMatrix();
    glTranslatef(-0.1f, 0.7f, 0.0f);
    glRotatef(legSwing, 1.0f, 0.0f, 0.0f);
    glScalef(0.1f, 0.4f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Right leg
    glPushMatrix();
    glTranslatef(0.1f, 0.7f, 0.0f);
    glRotatef(-legSwing, 1.0f, 0.0f, 0.0f);
    glScalef(0.1f, 0.4f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Simple shoes
    glColor3f(0.1f, 0.1f, 0.1f);
    // Left shoe
    glPushMatrix();
    glTranslatef(-0.1f, 0.4f, 0.0f);
    glScalef(0.12f, 0.1f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Right shoe
    glPushMatrix();
    glTranslatef(0.1f, 0.4f, 0.0f);
    glScalef(0.12f, 0.1f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}

//helper
void drawDisk(float outerRadius, float height, int slices) {
    // Draw a solid disk using quads
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f);  // Center
    for (int i = 0; i <= slices; i++) {
        float angle = 2.0f * M_PI * i / slices;
        float x = outerRadius * cos(angle);
        float y = outerRadius * sin(angle);
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
}

void drawGoldCup() {
    // Gold material properties
    GLfloat gold_ambient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
    GLfloat gold_diffuse[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
    GLfloat gold_specular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
    GLfloat gold_shininess = 51.2f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, gold_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, gold_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, gold_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, gold_shininess);

    glPushMatrix();

    // Base of trophy
    glPushMatrix();
    glScalef(1.0f, 1.0f, 0.2f);
    glutSolidCube(0.8f);
    glPopMatrix();

    // Second layer of base
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.2f);
    glScalef(0.7f, 0.7f, 0.15f);
    glutSolidCube(0.8f);
    glPopMatrix();

    // Stem
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.35f);
    for (int i = 0; i < 20; i++) {
        float scale = 0.2f - (i * 0.002f);
        glTranslatef(0.0f, 0.0f, 0.05f);
        glutSolidCube(scale);
    }
    glPopMatrix();

    // Main cup body
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 1.4f);

    // Bottom part of cup (inverted cone)
    for (int i = 0; i < 15; i++) {
        float scale = 0.2f + (i * 0.04f);
        glTranslatef(0.0f, 0.0f, 0.05f);
        glutSolidTorus(0.02f, scale, 16, 16);
    }

    // Top part of cup
    glTranslatef(0.0f, 0.0f, 0.05f);
    glutSolidTorus(0.04f, 0.8f, 20, 20);

    // Lip of the cup
    glTranslatef(0.0f, 0.0f, 0.1f);
    glutSolidTorus(0.05f, 0.85f, 20, 20);
    glPopMatrix();

    // Handles
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * 0.8f, 0.0f, 1.8f);

        // Main handle curve
        glPushMatrix();
        glScalef(0.2f, 0.15f, 0.4f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

        // Draw handle using multiple toruses
        for (int i = 0; i < 180; i += 30) {
            glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
            glutSolidTorus(0.1f, 1.0f, 8, 16);
        }
        glPopMatrix();
        glPopMatrix();
    }

    // Decorative bands
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 1.6f + (i * 0.2f));
        for (int j = 0; j < 8; j++) {
            glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
            glBegin(GL_TRIANGLES);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.1f, 0.3f, 0.0f);
            glVertex3f(-0.1f, 0.3f, 0.0f);
            glEnd();
        }
        glPopMatrix();
    }

    // Top ornament
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 2.3f);
    glutSolidSphere(0.1f, 16, 16);

    // Star on top
    for (int i = 0; i < 8; i++) {
        glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 0.0f, 0.15f);  // Top point
        glVertex3f(0.05f, 0.0f, 0.05f);  // Right point
        glVertex3f(-0.05f, 0.0f, 0.05f); // Left point
        glEnd();
    }
    glPopMatrix();

    glPopMatrix();
}

void drawTarget() {
    if (!gameWon) {
        glPushMatrix();
        glTranslatef(targetX, targetY, targetZ);
        glRotatef(targetRotation, 0.0f, 1.0f, 0.0f);

        // Material properties for metallic parts (chrome-like finish for the wire separators)
        GLfloat metal_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
        GLfloat metal_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
        GLfloat metal_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
        GLfloat metal_shininess = 76.8f;

        // Draw target board backing (solid disk - dark sisal fiber color)
        glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT, metal_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, metal_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, metal_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, metal_shininess);

        glColor3f(0.15f, 0.15f, 0.15f);  // Darker background for sisal fiber
        glTranslatef(0.0f, 0.0f, -0.1f);
        drawDisk(1.1f, 0.1f, 32);  // Background disk
        glPopMatrix();

        // Target rings with realistic dart board colors
        const int NUM_RINGS = 5;
        float ringColors[NUM_RINGS][3] = {
            {0.2f, 0.2f, 0.2f},    // Outer (Black)
            {0.95f, 0.95f, 0.95f}, // White
            {0.2f, 0.2f, 0.2f},    // Black
            {0.95f, 0.95f, 0.95f}, // White
            {0.2f, 0.2f, 0.2f}     // Inner (Black)
        };
        float ringRadii[NUM_RINGS] = { 1.0f, 0.8f, 0.6f, 0.4f, 0.2f };

        for (int i = 0; i < NUM_RINGS; i++) {
            // Set material properties for each ring
            GLfloat ring_ambient[] = { ringColors[i][0] * 0.3f, ringColors[i][1] * 0.3f, ringColors[i][2] * 0.3f, 1.0f };
            GLfloat ring_diffuse[] = { ringColors[i][0], ringColors[i][1], ringColors[i][2], 1.0f };
            GLfloat ring_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
            GLfloat ring_shininess = 25.0f;

            glMaterialfv(GL_FRONT, GL_AMBIENT, ring_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, ring_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, ring_specular);
            glMaterialf(GL_FRONT, GL_SHININESS, ring_shininess);

            glColor3fv(ringColors[i]);
            glutSolidTorus(0.09f, ringRadii[i], 32, 32);
        }

        // Double bullseye (outer red, inner green)
        GLfloat outer_bullseye_ambient[] = { 0.2f, 0.0f, 0.0f, 1.0f };
        GLfloat outer_bullseye_diffuse[] = { 0.8f, 0.0f, 0.0f, 1.0f };
        GLfloat outer_bullseye_specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat outer_bullseye_shininess = 50.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, outer_bullseye_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, outer_bullseye_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, outer_bullseye_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, outer_bullseye_shininess);

        glColor3f(0.8f, 0.0f, 0.0f);  // Dark red for outer bullseye
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.01f);
        glutSolidSphere(0.1f, 32, 32);
        glPopMatrix();

        // Enhanced stand with realistic dark wood color
        GLfloat wood_ambient[] = { 0.15f, 0.09f, 0.05f, 1.0f };
        GLfloat wood_diffuse[] = { 0.3f, 0.18f, 0.1f, 1.0f };
        GLfloat wood_specular[] = { 0.05f, 0.05f, 0.05f, 1.0f };
        GLfloat wood_shininess = 5.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, wood_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, wood_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, wood_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, wood_shininess);

        // Main stand pole
        glColor3f(0.3f, 0.18f, 0.1f);
        glPushMatrix();
        glTranslatef(0.0f, -1.0f, 0.0f);
        glScalef(0.2f, 2.0f, 0.2f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Base support
        glPushMatrix();
        glTranslatef(0.0f, -2.0f, 0.0f);
        // Cross base
        for (int i = 0; i < 4; i++) {
            glPushMatrix();
            glRotatef(i * 90.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(0.4f, 0.0f, 0.0f);
            glScalef(0.8f, 0.1f, 0.1f);
            glutSolidCube(1.0f);
            glPopMatrix();
        }
        glPopMatrix();

        // Add thickness to the target (side wall)
        glColor3f(0.15f, 0.15f, 0.15f);  // Darker edge color
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= 32; i++) {
            float angle = 2.0f * M_PI * i / 32;
            float x = 1.1f * cos(angle);
            float y = 1.1f * sin(angle);
            glVertex3f(x, y, 0.0f);
            glVertex3f(x, y, -0.1f);
        }
        glEnd();

        glPopMatrix();
    }
}

void drawArrow() {
    for (const auto& arrow : arrows) {
        if (arrow.active) {
            glPushMatrix();
            glTranslatef(arrow.x, arrow.y, arrow.z);

            // Calculate rotation based on arrow direction
            float angle = atan2(arrow.dx, arrow.dz) * 180.0f / M_PI;
            float pitch = atan2(arrow.dy, sqrt(arrow.dx * arrow.dx + arrow.dz * arrow.dz)) * 180.0f / M_PI;

            glRotatef(-angle, 0.0f, 1.0f, 0.0f);
            glRotatef(pitch, 1.0f, 0.0f, 0.0f);

            // Set material properties for metallic look
            GLfloat shaft_material_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
            GLfloat shaft_material_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
            GLfloat shaft_material_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
            GLfloat shaft_shininess = 76.8f;

            // Arrow shaft - metallic silver color
            glMaterialfv(GL_FRONT, GL_AMBIENT, shaft_material_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, shaft_material_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, shaft_material_specular);
            glMaterialf(GL_FRONT, GL_SHININESS, shaft_shininess);

            // Draw shaft with metallic effect
            glPushMatrix();
            glColor3f(0.75f, 0.75f, 0.75f); // Bright silver
            glScalef(0.05f, 0.05f, 0.5f);
            glutSolidCube(1.0f);
            glPopMatrix();

            // Decorative rings on shaft
            glPushMatrix();
            glColor3f(0.8f, 0.1f, 0.1f); // Bright red rings
            glTranslatef(0.0f, 0.0f, -0.1f);
            glutSolidTorus(0.02f, 0.05f, 8, 8);
            glTranslatef(0.0f, 0.0f, 0.2f);
            glutSolidTorus(0.02f, 0.05f, 8, 8);
            glPopMatrix();

            // Arrow head - golden color
            glPushMatrix();
            GLfloat head_material_ambient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
            GLfloat head_material_diffuse[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
            GLfloat head_material_specular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
            GLfloat head_shininess = 51.2f;

            glMaterialfv(GL_FRONT, GL_AMBIENT, head_material_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, head_material_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, head_material_specular);
            glMaterialf(GL_FRONT, GL_SHININESS, head_shininess);

            glColor3f(0.85f, 0.65f, 0.13f); // Golden color
            glTranslatef(0.0f, 0.0f, 0.25f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidCone(0.06f, 0.15f, 12, 12); // Slightly larger and more detailed cone
            glPopMatrix();

            // Feathers (fletching)
            glPushMatrix();
            glColor3f(0.2f, 0.8f, 0.2f); // Bright green feathers

            // Draw three feathers at 120-degree intervals
            for (int i = 0; i < 3; i++) {
                glRotatef(120.0f, 0.0f, 0.0f, 1.0f);
                glBegin(GL_TRIANGLES);
                glVertex3f(0.0f, 0.0f, -0.2f);
                glVertex3f(0.0f, 0.1f, -0.1f);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glEnd();
            }
            glPopMatrix();

            glPopMatrix();
        }
    }
}

void drawWalls() {
    // Fence parameters aligned with collision boundaries
    float fenceHeight = 1.2f;
    float postWidth = 0.15f;
    float fenceLength = 19.0f;  // Set to match collision bounds (-9.5 to 9.5)
    float postSpacing = 2.0f;
    float fenceBoundary = 9.5f; // Match collision boundary

    // Set material properties for metal fence
    GLfloat metal_ambient[] = { 0.19225f, 0.19225f, 0.19225f, 1.0f };
    GLfloat metal_diffuse[] = { 0.50754f, 0.50754f, 0.50754f, 1.0f };
    GLfloat metal_specular[] = { 0.508273f, 0.508273f, 0.508273f, 1.0f };
    float shininess = 51.2f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, metal_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, metal_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, metal_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    // Draw fence sections
    for (int side = 0; side < 4; side++) {
        glPushMatrix();
        glRotatef(90.0f * side, 0.0f, 1.0f, 0.0f);
        glTranslatef(-fenceBoundary, 0.0f, -fenceBoundary);

        // Draw vertical posts
        for (float x = 0; x <= fenceLength; x += postSpacing) {
            glPushMatrix();
            glTranslatef(x, fenceHeight / 2, 0.0f);

            // Main post
            glColor3f(0.3f, 0.3f, 0.3f);
            glPushMatrix();
            glScalef(postWidth, fenceHeight, postWidth);
            glutSolidCube(1.0f);
            glPopMatrix();

            // Decorative top
            glColor3f(0.4f, 0.4f, 0.4f);
            glPushMatrix();
            glTranslatef(0.0f, fenceHeight / 2, 0.0f);
            glutSolidSphere(postWidth / 1.5f, 8, 8);
            glPopMatrix();

            glPopMatrix();
        }

        // Draw horizontal rails (three levels)
        glColor3f(0.35f, 0.35f, 0.35f);
        float railHeights[] = { 0.3f, 0.6f, 0.9f };
        for (int rail = 0; rail < 3; rail++) {
            glPushMatrix();
            glTranslatef(fenceBoundary, railHeights[rail], 0.0f);
            glPushMatrix();
            glScalef(fenceLength, 0.05f, 0.05f);
            glutSolidCube(1.0f);
            glPopMatrix();
            glPopMatrix();
        }

        // Draw small vertical bars between posts
        for (float x = 0; x < fenceLength; x += postSpacing / 4) {
            if (fmod(x, postSpacing) > 0.1f) {  // Skip positions where main posts are
                glPushMatrix();
                glTranslatef(x, fenceHeight / 2, 0.0f);
                glScalef(0.03f, fenceHeight - 0.2f, 0.03f);
                glutSolidCube(1.0f);
                glPopMatrix();
            }
        }

        // Draw decorative diagonal cross bars
        glColor3f(0.4f, 0.4f, 0.4f);
        for (float x = 0; x < fenceLength; x += postSpacing) {
            glPushMatrix();
            glTranslatef(x + postSpacing / 2, fenceHeight / 2, 0.0f);

            glBegin(GL_LINES);
            // Cross pattern
            glVertex3f(-postSpacing / 2 + postWidth / 2, fenceHeight / 3, 0.0f);
            glVertex3f(postSpacing / 2 - postWidth / 2, -fenceHeight / 3, 0.0f);

            glVertex3f(-postSpacing / 2 + postWidth / 2, -fenceHeight / 3, 0.0f);
            glVertex3f(postSpacing / 2 - postWidth / 2, fenceHeight / 3, 0.0f);
            glEnd();

            glPopMatrix();
        }

        glPopMatrix();
    }

    // Add corner posts at exact collision boundaries
    glColor3f(0.3f, 0.3f, 0.3f);
    float cornerPostWidth = postWidth * 1.5f;
    float cornerPostHeight = fenceHeight * 1.2f;

    for (int corner = 0; corner < 4; corner++) {
        glPushMatrix();
        float cornerX = ((corner == 1 || corner == 2) ? fenceBoundary : -fenceBoundary);
        float cornerZ = ((corner == 2 || corner == 3) ? fenceBoundary : -fenceBoundary);

        glTranslatef(cornerX, cornerPostHeight / 2, cornerZ);

        // Corner post
        glPushMatrix();
        glScalef(cornerPostWidth, cornerPostHeight, cornerPostWidth);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Decorative top
        glColor3f(0.4f, 0.4f, 0.4f);
        glPushMatrix();
        glTranslatef(0.0f, cornerPostHeight / 2, 0.0f);
        glutSolidSphere(cornerPostWidth / 1.5f, 12, 12);
        glPopMatrix();

        glPopMatrix();
    }

    // Add subtle shadow under the fence
    glColor4f(0.2f, 0.2f, 0.2f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, 0.01f, 0.0f);  // Slightly above ground
    glBegin(GL_QUADS);
    glVertex3f(-fenceBoundary, 0.0f, -fenceBoundary);
    glVertex3f(-fenceBoundary, 0.0f, fenceBoundary);
    glVertex3f(fenceBoundary, 0.0f, fenceBoundary);
    glVertex3f(fenceBoundary, 0.0f, -fenceBoundary);
    glEnd();
    glPopMatrix();
}

void drawGround() {
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);

    // Enhanced material properties for better visibility
    GLfloat ground_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };  // Increased ambient
    GLfloat ground_diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };  // Increased diffuse
    GLfloat ground_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // Increased specular
    GLfloat ground_shininess = 10.0f;                        // Increased shininess

    glMaterialfv(GL_FRONT, GL_AMBIENT, ground_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ground_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ground_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, ground_shininess);

    // Main ground surface - made larger and more vibrant
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.6f, 0.2f);  // Brighter green for better visibility
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-20.0f, 0.0f, -20.0f);  // Increased size from 10 to 20
    glVertex3f(20.0f, 0.0f, -20.0f);
    glVertex3f(20.0f, 0.0f, 20.0f);
    glVertex3f(-20.0f, 0.0f, 20.0f);
    glEnd();

    // Draw shooting lane - made more visible
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);  // Pure white for better contrast
    glVertex3f(-1.5f, 0.02f, -9.5f);  // Slightly raised to prevent z-fighting
    glVertex3f(1.5f, 0.02f, -9.5f);
    glVertex3f(1.5f, 0.02f, 9.5f);
    glVertex3f(-1.5f, 0.02f, 9.5f);
    glEnd();

    // Draw distance markers with better contrast
    float markerPositions[] = { -8.0f, -6.0f, -4.0f, -2.0f, 0.0f, 2.0f, 4.0f, 6.0f, 8.0f };
    for (float pos : markerPositions) {
        glBegin(GL_QUADS);  // Changed from lines to quads for better visibility
        glColor3f(0.3f, 0.3f, 0.3f);  // Darker gray for better contrast
        glVertex3f(-1.4f, 0.03f, pos - 0.1f);
        glVertex3f(1.4f, 0.03f, pos - 0.1f);
        glVertex3f(1.4f, 0.03f, pos + 0.1f);
        glVertex3f(-1.4f, 0.03f, pos + 0.1f);
        glEnd();
    }

    // Olympic rings with enhanced visibility
    float ringRadius = 0.5f;
    float ringSpacing = 1.1f;
    float ringY = 0.03f;  // Raised slightly
    float ringThickness = 0.12f;  // Increased thickness

    // Brighter Olympic ring colors
    float ringColors[5][3] = {
        {0.0f, 0.0f, 1.0f},    // Bright Blue
        {0.1f, 0.1f, 0.1f},    // Deep Black
        {1.0f, 0.0f, 0.0f},    // Bright Red
        {1.0f, 0.8f, 0.0f},    // Bright Yellow
        {0.0f, 1.0f, 0.0f}     // Bright Green
    };

    glPushMatrix();
    glTranslatef(0.0f, ringY, 0.0f);

    // Draw each ring with enhanced visibility
    for (int i = 0; i < 5; i++) {
        float xOffset = (i - 2) * ringSpacing;
        float zOffset = (i % 2 == 0) ? 0.0f : ringRadius * 0.5f;

        glPushMatrix();
        glTranslatef(xOffset, 0.0f, zOffset);
        glColor3f(ringColors[i][0], ringColors[i][1], ringColors[i][2]);

        // Draw ring using triangle strip for smoother appearance
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= 360; j += 5) {  // Increased segments for smoother circles
            float angle = j * 3.14159f / 180.0f;
            float outerX = (ringRadius + ringThickness) * cos(angle);
            float outerZ = (ringRadius + ringThickness) * sin(angle);
            float innerX = (ringRadius - ringThickness) * cos(angle);
            float innerZ = (ringRadius - ringThickness) * sin(angle);

            glVertex3f(outerX, 0.0f, outerZ);
            glVertex3f(innerX, 0.0f, innerZ);
        }
        glEnd();

        glPopMatrix();
    }
    glPopMatrix();

    // Enhanced shooting positions
    float positions[] = { -2.0f, 0.0f, 2.0f };
    for (float pos : positions) {
        glPushMatrix();
        glTranslatef(pos, 0.025f, 8.0f);  // Raised more for better visibility

        // Position marker with stronger contrast
        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(-0.3f, 0.0f, -0.3f);
        glVertex3f(0.3f, 0.0f, -0.3f);
        glVertex3f(0.3f, 0.0f, 0.3f);
        glVertex3f(-0.3f, 0.0f, 0.3f);
        glEnd();

        // Position border with better visibility
        glLineWidth(2.0f);  // Thicker lines
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        glVertex3f(-0.2f, 0.001f, -0.2f);
        glVertex3f(0.2f, 0.001f, -0.2f);
        glVertex3f(0.2f, 0.001f, 0.2f);
        glVertex3f(-0.2f, 0.001f, 0.2f);
        glEnd();
        glLineWidth(1.0f);  // Reset line width

        glPopMatrix();
    }

    // Enhanced corner markings
    glLineWidth(2.0f);  // Thicker lines for better visibility
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glRotatef(90.0f * i, 0.0f, 1.0f, 0.0f);
        glTranslatef(-19.5f, 0.02f, -19.5f);  // Adjusted for larger ground

        glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(2.0f, 0.0f, 0.0f);  // Longer lines
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 2.0f);  // Longer lines
        glEnd();

        glPopMatrix();
    }
    glLineWidth(1.0f);  // Reset line width

    glPopMatrix();
}

void drawTree() {
    glPushMatrix();
    glTranslatef(-5.0f, 0.0f, -5.0f);

    // Base rotation for wind animation
    if (treeAnimation) {
        glRotatef(sin(animationAngles[0] * 3.14159f / 180.0f) * 5.0f, 0.0f, 1.0f, 0.0f);
    }

    // Set material properties for trunk
    GLfloat trunk_ambient[] = { 0.25f, 0.148f, 0.06475f, 1.0f };
    GLfloat trunk_diffuse[] = { 0.4f, 0.2368f, 0.1036f, 1.0f };
    GLfloat trunk_specular[] = { 0.774597f, 0.458561f, 0.200621f, 1.0f };
    GLfloat trunk_shininess = 76.8f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, trunk_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, trunk_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, trunk_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, trunk_shininess);

    // Main trunk with texture-like effect
    glColor3f(0.4f, 0.2f, 0.1f);
    for (int i = 0; i < 8; i++) {
        glPushMatrix();
        glRotatef(45.0f * i, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 2.0f, 0.25f);
        glScalef(0.2f, 4.0f, 0.05f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // Trunk base (wider)
    glPushMatrix();
    glColor3f(0.35f, 0.16f, 0.08f);
    glScalef(0.7f, 0.5f, 0.7f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Root system
    for (int i = 0; i < 6; i++) {
        glPushMatrix();
        glRotatef(60.0f * i, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.4f, 0.1f, 0.0f);
        glColor3f(0.35f, 0.16f, 0.08f);
        glScalef(0.8f, 0.1f, 0.2f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // Set material properties for leaves
    GLfloat leaf_ambient[] = { 0.0f, 0.3f, 0.0f, 1.0f };
    GLfloat leaf_diffuse[] = { 0.0f, 0.8f, 0.0f, 1.0f };
    GLfloat leaf_specular[] = { 0.2f, 0.4f, 0.2f, 1.0f };
    GLfloat leaf_shininess = 32.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, leaf_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, leaf_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, leaf_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, leaf_shininess);

    // Main branches
    for (int i = 0; i < 5; i++) {
        float height = 1.5f + i * 0.8f;
        float scale = 1.0f - (i * 0.15f);

        glPushMatrix();
        glTranslatef(0.0f, height, 0.0f);

        // Branch layer with animation
        if (treeAnimation) {
            glRotatef(sin(animationAngles[0] * 3.14159f / 180.0f + i) * (3.0f + i),
                sin(i * 1.1f), 1.0f, cos(i * 1.1f));
        }

        // Draw branches
        for (int j = 0; j < 6; j++) {
            glPushMatrix();
            glRotatef(60.0f * j, 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, 0.2f);

            // Branch
            glColor3f(0.35f, 0.16f, 0.08f);
            glPushMatrix();
            glScalef(0.1f * scale, 0.1f * scale, 1.0f * scale);
            glutSolidCube(1.0f);
            glPopMatrix();

            // Leaf clusters
            glColor3f(0.0f, 0.5f + (i * 0.1f), 0.0f);
            for (int k = 0; k < 3; k++) {
                glPushMatrix();
                glTranslatef(0.0f, 0.1f, 0.3f + k * 0.3f);
                if (treeAnimation) {
                    glRotatef(sin(animationAngles[0] * 3.14159f / 180.0f + k) * 10.0f,
                        1.0f, 0.0f, 1.0f);
                }
                glutSolidSphere(0.2f * scale, 8, 8);
                glPopMatrix();
            }
            glPopMatrix();
        }
        glPopMatrix();
    }

    // Top foliage layers
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        float height = 4.0f + i * 0.5f;
        float scale = 1.0f - (i * 0.2f);

        glTranslatef(0.0f, height, 0.0f);
        if (treeAnimation) {
            glRotatef(sin(animationAngles[0] * 3.14159f / 180.0f + i) * (2.0f + i),
                0.0f, 1.0f, 0.0f);
        }

        // Layered foliage
        glColor3f(0.0f, 0.6f + (i * 0.1f), 0.0f);
        glPushMatrix();
        glScalef(1.5f * scale, 0.4f, 1.5f * scale);
        glutSolidSphere(1.0f, 16, 16);
        glPopMatrix();
        glPopMatrix();
    }

    glPopMatrix();
}

void drawBench() {
    glPushMatrix();
    glTranslatef(5.0f, 0.5f, 5.0f);
    if (benchAnimation) {
        glScalef(1.0f + sin(animationAngles[1] * 3.14159 / 180.0f) * 0.2f,
            1.0f + sin(animationAngles[1] * 3.14159 / 180.0f) * 0.2f,
            1.0f + sin(animationAngles[1] * 3.14159 / 180.0f) * 0.2f);
    }

    // Bench material (wooden)
// Rich wooden material properties (mahogany-like finish)
    GLfloat wood_ambient[] = { 0.4f, 0.2f, 0.07f, 1.0f };      // Darker brown ambient
    GLfloat wood_diffuse[] = { 0.6f, 0.3f, 0.1f, 1.0f };       // Rich brown color
    GLfloat wood_specular[] = { 0.05f, 0.05f, 0.05f, 1.0f };   // Very subtle shine
    GLfloat wood_shininess = 5.0f;                             // Less shiny for wood

    glColor3f(0.6f, 0.3f, 0.1f);                              // Base wood color
    glMaterialfv(GL_FRONT, GL_AMBIENT, wood_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, wood_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, wood_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, wood_shininess);
    // Seat
    glPushMatrix();
    glScalef(2.0f, 0.2f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Legs
    glPushMatrix();
    glTranslatef(-0.8f, -0.4f, 0.4f);
    glScalef(0.2f, 0.8f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.8f, -0.4f, 0.4f);
    glScalef(0.2f, 0.8f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.8f, -0.4f, -0.4f);
    glScalef(0.2f, 0.8f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.8f, -0.4f, -0.4f);
    glScalef(0.2f, 0.8f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // True gold material properties for the cup
    GLfloat gold_ambient[] = { 0.33f, 0.22f, 0.03f, 1.0f };    // Rich gold ambient
    GLfloat gold_diffuse[] = { 1.0f, 0.84f, 0.0f, 1.0f };      // Pure gold color
    GLfloat gold_specular[] = { 1.0f, 0.94f, 0.23f, 1.0f };    // Bright gold shine
    GLfloat gold_shininess = 128.0f;                           // Very shiny

    glColor3f(1.0f, 0.84f, 0.0f);                             // Pure gold base color
    glMaterialfv(GL_FRONT, GL_AMBIENT, gold_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, gold_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, gold_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, gold_shininess);

    // Add the gold cup above the bench
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.0f);      // Lifted higher above the bench
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);  // Rotate to make it upright
    glScalef(1.0f, 1.0f, 1.0f);          // Made significantly bigger

    if (benchAnimation) {
        glRotatef(animationAngles[1], 0.0f, 0.0f, 1.0f);
    }

    drawGoldCup();
    glPopMatrix();

    glPopMatrix();
}

void drawFlag() {
    glPushMatrix();
    glTranslatef(8.0f, 0.0f, -8.0f);

    // Base animation for the whole flag structure
    if (flagAnimation) {
        glRotatef(sin(animationAngles[2] * 3.14159 / 180.0f) * 10.0f, 0.0f, 1.0f, 0.0f);
    }

    // Draw fancy base (multi-layered)
    glPushMatrix();
    // Bottom layer (wider)
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glScalef(0.6f, 0.1f, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Middle layer
    glColor3f(0.4f, 0.4f, 0.4f);
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.0f);
    glScalef(0.4f, 0.1f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Top layer
    glColor3f(0.5f, 0.5f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, 0.2f, 0.0f);
    glScalef(0.2f, 0.1f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    // Main pole with metallic effect
    GLfloat pole_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat pole_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat pole_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat pole_shininess = 100.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, pole_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, pole_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, pole_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, pole_shininess);

    // Main pole segments (for visual interest)
    for (float h = 0.0f; h < 7.0f; h += 0.5f) {
        glPushMatrix();
        glTranslatef(0.0f, h, 0.0f);
        if (int(h * 2) % 2 == 0)
            glColor3f(0.8f, 0.8f, 0.8f);
        else
            glColor3f(0.9f, 0.9f, 0.9f);
        glPushMatrix();
        glScalef(0.1f, 0.5f, 0.1f);
        glutSolidCube(1.0f);
        glPopMatrix();
        glPopMatrix();
    }

    // Decorative top sphere with gold finish
    GLfloat gold_ambient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
    GLfloat gold_diffuse[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
    GLfloat gold_specular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
    GLfloat gold_shininess = 51.2f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, gold_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, gold_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, gold_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, gold_shininess);

    glPushMatrix();
    glTranslatef(0.0f, 7.0f, 0.0f);
    glColor3f(1.0f, 0.843f, 0.0f);  // Gold color
    glutSolidSphere(0.15f, 16, 16);
    glPopMatrix();

    // Flag with enhanced wave effect
    float flagHeight = 5.5f;  // Raised height
    float flagWidth = 3.0f;   // Wider flag
    float waveAmplitude = 0.15f;
    float waveFrequency = 3.0f;
    int segments = 30;  // More segments for smoother wave

    glPushMatrix();
    glTranslatef(0.6f, flagHeight, 0.0f);

    // White background of the flag with better material
    GLfloat flag_ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat flag_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat flag_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat flag_shininess = 32.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, flag_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, flag_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, flag_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, flag_shininess);

    // Draw flag segments with double-sided material
    for (int i = 0; i < segments; i++) {
        float x = (float)i / segments * flagWidth;
        float nextX = (float)(i + 1) / segments * flagWidth;

        float wave = sin(animationAngles[2] * 3.14159f / 180.0f + x * waveFrequency) * waveAmplitude;
        float nextWave = sin(animationAngles[2] * 3.14159f / 180.0f + nextX * waveFrequency) * waveAmplitude;

        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        // Front face
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(x, wave, 0.0f);
        glVertex3f(nextX, nextWave, 0.0f);
        glVertex3f(nextX, -1.5f + nextWave, 0.0f);
        glVertex3f(x, -1.5f + wave, 0.0f);
        // Back face
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(x, wave, -0.02f);
        glVertex3f(nextX, nextWave, -0.02f);
        glVertex3f(nextX, -1.5f + nextWave, -0.02f);
        glVertex3f(x, -1.5f + wave, -0.02f);
        glEnd();
    }

    // Olympic rings with enhanced visibility
    float ringRadius = 0.2f;
    float ringSpacing = 0.45f;
    float ringY = -0.7f;
    float ringThickness = 0.04f;

    float ringColors[5][3] = {
        {0.0f, 0.0f, 1.0f},    // Bright Blue
        {0.0f, 0.0f, 0.0f},    // Deep Black
        {1.0f, 0.0f, 0.0f},    // Bright Red
        {1.0f, 0.84f, 0.0f},   // Golden Yellow
        {0.0f, 0.8f, 0.0f}     // Bright Green
    };

    glPushMatrix();
    glTranslatef(0.8f, ringY, 0.02f);

    // Draw each Olympic ring
    for (int i = 0; i < 5; i++) {
        float xOffset = (i - 2) * ringSpacing;
        float yOffset = (i % 2 == 0) ? 0.0f : 0.12f;

        glPushMatrix();
        glTranslatef(xOffset, yOffset, 0.0f);
        glColor3f(ringColors[i][0], ringColors[i][1], ringColors[i][2]);

        // Draw ring using quad strip for better 3D effect
        const int ringSegments = 32;
        float ringAngle = 2.0f * 3.14159f / ringSegments;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= ringSegments; j++) {
            float angle = j * ringAngle;
            float cosa = cos(angle);
            float sina = sin(angle);

            glVertex3f((ringRadius + ringThickness) * cosa,
                (ringRadius + ringThickness) * sina, 0.03f);
            glVertex3f((ringRadius - ringThickness) * cosa,
                (ringRadius - ringThickness) * sina, 0.03f);
        }
        glEnd();

        glPopMatrix();
    }

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

void drawLight() {
    glPushMatrix();
    glTranslatef(-8.0f, 0.0f, -8.0f);
    glPushAttrib(GL_LIGHTING_BIT);

    float currentRotation = lightAnimation ? animationAngles[3] : 0.0f;
    glRotatef(currentRotation, 0.0f, 1.0f, 0.0f);

    // Update light position with smooth animation
    if (lightAnimation) {
        light1_position[0] = -8.0f + 2.0f * sin(animationAngles[3] * M_PI / 180.0f);
        light1_position[2] = -7.0f + 2.0f * cos(animationAngles[3] * M_PI / 180.0f);
    }

    // Enhanced metal material for pole
    GLfloat pole_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat pole_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat pole_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
    GLfloat pole_shininess = 76.8f;

    // Base of the pole
    glMaterialfv(GL_FRONT, GL_AMBIENT, pole_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, pole_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, pole_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, pole_shininess);

    // Draw decorative base
    glPushMatrix();
    glColor3f(0.4f, 0.4f, 0.4f);
    glTranslatef(0.0f, 0.0f, 0.0f);

    // Bottom plate
    glPushMatrix();
    glScalef(1.0f, 0.1f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Decorative rings at base
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(0.0f, 0.1f + i * 0.1f, 0.0f);
        glutSolidTorus(0.05f, 0.4f - i * 0.1f, 16, 32);
        glPopMatrix();
    }
    glPopMatrix();

    // Main pole with segments
    glPushMatrix();
    glTranslatef(0.0f, 0.4f, 0.0f);
    for (int i = 0; i < 8; i++) {
        // Pole segment
        glPushMatrix();
        glTranslatef(0.0f, i * 1.0f, 0.0f);
        glScalef(0.3f, 1.0f, 0.3f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Decorative ring between segments
        if (i < 7) {
            glPushMatrix();
            glTranslatef(0.0f, i * 1.0f + 0.5f, 0.0f);
            glutSolidTorus(0.02f, 0.2f, 12, 24);
            glPopMatrix();
        }
    }
    glPopMatrix();

    // Enhanced light fixture
    glPushMatrix();
    glTranslatef(0.0f, 8.0f, 1.0f);
    glRotatef(45.0f, 1.0f, 0.0f, 0.0f);

    // Light housing with improved materials
    GLfloat housing_ambient[] = { 0.15f, 0.15f, 0.15f, 1.0f };
    GLfloat housing_diffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat housing_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat housing_shininess = 50.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, housing_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, housing_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, housing_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, housing_shininess);

    // Main housing
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glScalef(1.0f, 0.5f, 1.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Decorative frame
    glPushMatrix();
    glTranslatef(0.0f, -0.25f, 0.0f);
    glScalef(1.1f, 0.1f, 1.6f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Light bulb with dynamic emission
    float time = animationAngles[3] * M_PI / 180.0f;
    float flickerIntensity = 0.95f + 0.05f * sin(time * 10.0f);  // Fast subtle flicker
    float baseIntensity = (sin(time) + 1.0f) / 2.0f;  // Slow intensity variation
    float finalIntensity = baseIntensity * flickerIntensity;

    GLfloat bulb_ambient[] = { 0.4f * finalIntensity, 0.4f * finalIntensity, 0.3f * finalIntensity, 1.0f };
    GLfloat bulb_diffuse[] = { 1.0f * finalIntensity, 0.95f * finalIntensity, 0.8f * finalIntensity, 1.0f };
    GLfloat bulb_specular[] = { 1.0f * finalIntensity, 1.0f * finalIntensity, 0.9f * finalIntensity, 1.0f };
    GLfloat bulb_emission[] = { 0.8f * finalIntensity, 0.8f * finalIntensity, 0.6f * finalIntensity, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, bulb_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, bulb_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, bulb_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, bulb_emission);

    // Enhanced bulb with glass effect
    glPushMatrix();
    glTranslatef(0.0f, -0.2f, 0.0f);
    glutSolidSphere(0.3f, 32, 32);

    // Glare effect
    glPushMatrix();
    glScalef(1.1f, 1.1f, 1.1f);
    glColor4f(1.0f, 1.0f, 0.8f, 0.3f);
    glutSolidSphere(0.3f, 16, 16);
    glPopMatrix();
    glPopMatrix();

    // Reset emission
    GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
    glPopMatrix();

    // Enhanced light properties
    if (lightAnimation) {
        GLfloat animated_diffuse[] = {
            light1_diffuse[0] * finalIntensity,
            light1_diffuse[1] * finalIntensity,
            light1_diffuse[2] * finalIntensity,
            1.0f
        };
        GLfloat animated_specular[] = {
            light1_specular[0] * finalIntensity,
            light1_specular[1] * finalIntensity,
            light1_specular[2] * finalIntensity,
            1.0f
        };
        glLightfv(GL_LIGHT1, GL_DIFFUSE, animated_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, animated_specular);
    }

    // Update light properties
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0f);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0f);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.045f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0075f);

    glPopAttrib();
    glPopMatrix();
}

void drawBanner() {
    glPushMatrix();
    glTranslatef(0.0f, 7.0f, -9.8f);
    if (bannerAnimation) {
        float scale = 1.0f + sin(animationAngles[4] * 3.14159 / 180.0f) * 0.1f;
        glScalef(scale, scale, 1.0f);
    }

    // Banner background
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glScalef(6.0f, 1.5f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Olympic rings
    float ringColors[5][3] = {
        {0.0f, 0.0f, 1.0f}, // Blue
        {0.0f, 0.0f, 0.0f}, // Black
        {1.0f, 0.0f, 0.0f}, // Red
        {1.0f, 1.0f, 0.0f}, // Yellow
        {0.0f, 1.0f, 0.0f}  // Green
    };

    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glTranslatef(-2.0f + i * 1.0f, 0.0f, 0.1f);
        glColor3f(ringColors[i][0], ringColors[i][1], ringColors[i][2]);
        glutSolidTorus(0.1f, 0.4f, 20, 20);
        glPopMatrix();
    }

    glPopMatrix();
}

void drawGameEndScreen() {
    // Switch to 2D orthographic projection for text rendering
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Clear screen with background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set text color
    glColor3f(1.0f, 1.0f, 1.0f);

    // Position text in center of screen
    glRasterPos2i(glutGet(GLUT_WINDOW_WIDTH) / 2 - 50, glutGet(GLUT_WINDOW_HEIGHT) / 2);

    // Display appropriate message
    std::string message;
    if (gameWon) {
        message = "GAME WIN!";
        glColor3f(0.0f, 1.0f, 0.0f); // Green for win
    }
    else {
        message = "GAME LOSE!";
        glColor3f(1.0f, 0.0f, 0.0f); // Red for lose
    }

    // Render each character
    for (char c : message) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Display score and time
    std::string scoreText = "Time remaining: " + std::to_string(timeRemaining) + " seconds";
    glRasterPos2i(glutGet(GLUT_WINDOW_WIDTH) / 2 - 70, glutGet(GLUT_WINDOW_HEIGHT) / 2 - 30);
    for (char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    // Restore 3D projection
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawAudienceMember(const AudienceMember& member, float animationTime) {
    glPushMatrix();
    float bobHeight = sin(animationTime * 2.0f + member.animationOffset) * 0.1f;
    glTranslatef(member.x, member.baseY + bobHeight, member.z);
    glRotatef(member.rotation, 0.0f, 1.0f, 0.0f);
    glScalef(member.scale, member.scale, member.scale);

    // Body colors based on variation
    float bodyColors[5][3] = {
        {0.8f, 0.2f, 0.2f},  // Red shirt
        {0.2f, 0.2f, 0.8f},  // Blue shirt
        {0.2f, 0.8f, 0.2f},  // Green shirt
        {0.8f, 0.8f, 0.2f},  // Yellow shirt
        {0.8f, 0.2f, 0.8f}   // Purple shirt
    };

    // Head
    glColor3f(0.95f, 0.85f, 0.7f);
    glPushMatrix();
    glTranslatef(0.0f, 1.6f, 0.0f);
    glutSolidSphere(0.15f, 8, 8);
    glPopMatrix();

    // Body
    glColor3fv(bodyColors[member.colorVariation]);
    glPushMatrix();
    glTranslatef(0.0f, 1.1f, 0.0f);
    glScalef(0.3f, 0.5f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Arms (animated waving)
    float armWave = sin(animationTime * 3.0f + member.animationOffset) * 30.0f;

    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * 0.2f, 1.3f, 0.0f);
        glRotatef(armWave * side, 1.0f, 0.0f, 0.0f);

        glColor3fv(bodyColors[member.colorVariation]);
        glPushMatrix();
        glScalef(0.1f, 0.4f, 0.1f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Hands
        glColor3f(0.95f, 0.85f, 0.7f);
        glPushMatrix();
        glTranslatef(0.0f, -0.25f, 0.0f);
        glutSolidSphere(0.06f, 8, 8);
        glPopMatrix();

        glPopMatrix();
    }

    // Legs
    glColor3f(0.2f, 0.2f, 0.2f);  // Dark pants
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * 0.1f, 0.5f, 0.0f);
        glScalef(0.1f, 0.6f, 0.1f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    glPopMatrix();
}

void drawAudience() {
    static float lastTime = 0;
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    // Enable alpha blending for crowd depth effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (const auto& member : audience) {
        // Calculate distance-based alpha for crowd depth effect
        float distanceToCamera = sqrt(
            pow(member.x - cameraX, 2) +
            pow(member.z - cameraZ, 2)
        );

        // Draw the audience member with animation
        drawAudienceMember(member, currentTime);
    }

    glDisable(GL_BLEND);
}

void drawText(float x, float y, std::string text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void checkCollisions() {
    // Calculate distance between player and target
    float dx = playerX - targetX;
    float dz = playerZ - targetZ;
    float distance = sqrt(dx * dx + dz * dz);


    // Check collision with walls
    if (playerX < -9.5f) playerX = -9.5f;
    if (playerX > 9.5f) playerX = 9.5f;
    if (playerZ < -9.5f) playerZ = -9.5f;
    if (playerZ > 9.5f) playerZ = 9.5f;
}

void updateLights() {
    // Update street lamp light position and properties
    GLfloat street_light_position[] = { -8.0f, 8.0f, -7.0f, 1.0f };
    GLfloat street_light_direction[] = { 0.0f, -1.0f, 0.5f };

    glLightfv(GL_LIGHT1, GL_POSITION, street_light_position);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0f);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0f);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, street_light_direction);

    // Add light attenuation
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01f);
}

void updatePlayerAnimations() {
    if (isMoving) {
        legAnimation += 0.1f;
        armAnimation += 0.1f;
        if (legAnimation > 2 * 3.14159f) {
            legAnimation -= 2 * 3.14159f;
        }
        if (armAnimation > 2 * 3.14159f) {
            armAnimation -= 2 * 3.14159f;
        }
    }
}

void updateAnimations() {
    if (treeAnimation) animationAngles[0] += 2.0f;
    if (benchAnimation) animationAngles[1] += 3.0f;
    if (flagAnimation) animationAngles[2] += 5.0f;
    if (lightAnimation) animationAngles[3] += 4.0f;
    if (bannerAnimation) animationAngles[4] += 1.0f;

    // Keep angles in reasonable range
    for (int i = 0; i < 5; i++) {
        if (animationAngles[i] >= 360.0f) {
            animationAngles[i] -= 360.0f;
        }
    }
}

void updateShootingAnimation() {
    const float DRAW_SPEED = 0.05f;
    const float RETURN_SPEED = 0.1f;

    if (isPowerCharging) {
        // Smoothly move arms into shooting position
        shootingArmAngle += (90.0f - shootingArmAngle) * DRAW_SPEED;

        // Update drawback based on power
        float targetDrawback = (shootingPower / MAX_POWER) * MAX_DRAWBACK;
        drawbackAmount += (targetDrawback - drawbackAmount) * DRAW_SPEED;
    }
    else {
        // Return arms to normal position
        shootingArmAngle *= (1.0f - RETURN_SPEED);
        drawbackAmount *= (1.0f - RETURN_SPEED);
    }
}

void updateArrows() {
    for (auto& arrow : arrows) {
        if (arrow.active) {
            // Update position
            arrow.x += arrow.dx * arrow.speed;
            arrow.y += arrow.dy * arrow.speed;
            arrow.z += arrow.dz * arrow.speed;

            // Apply gravity
            arrow.dy -= 0.01f;

            // Check if arrow hit the ground
            if (arrow.y < 0.0f) {
                arrow.active = false;
            }

            // Check if arrow hit the target
            float dx = arrow.x - targetX;
            float dy = arrow.y - targetY;
            float dz = arrow.z - targetZ;
            float distance = sqrt(dx * dx + dy * dy + dz * dz);

            if (distance < 1.0f ) {
                targetHit = true;
                playSound("sounds/hit.wav", false);
                targerHitCount++;
                if (targerHitCount == 3) {
                    gameWon = true;
                    playSound("sounds/win.wav", false);
                }
                arrow.active = false;
            }
        }
    }
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = w * 1.0f / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    // Enable necessary features
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);  // Main light
    glEnable(GL_LIGHT1);  // Street lamp light
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);  // Automatically normalize normals

    // Set the sky blue background
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);

    // Main light (sunlight) settings
    GLfloat main_light_position[] = { 100.0f, 100.0f, 100.0f, 1.0f };  // Positioned high and to the side
    GLfloat main_light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };         // Brighter ambient
    GLfloat main_light_diffuse[] = { 1.0f, 1.0f, 0.9f, 1.0f };         // Slightly warm sunlight
    GLfloat main_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, main_light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, main_light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, main_light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, main_light_specular);

    //// Street lamp light settings (warm light)
    //GLfloat street_light_ambient[] = { 0.1f, 0.1f, 0.05f, 1.0f };
    //GLfloat street_light_diffuse[] = { 1.0f, 0.95f, 0.8f, 1.0f };    // Warm color
    //GLfloat street_light_specular[] = { 0.5f, 0.5f, 0.4f, 1.0f };

    //glLightfv(GL_LIGHT1, GL_AMBIENT, street_light_ambient);
    //glLightfv(GL_LIGHT1, GL_DIFFUSE, street_light_diffuse);
    //glLightfv(GL_LIGHT1, GL_SPECULAR, street_light_specular);

    // Set up material properties
    //glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    initAudience();
    // Enable smooth shading
    glShadeModel(GL_SMOOTH);

}

void initAudience() {
    audience.clear();
    float fenceBoundary = 9.5f;
    float spacing = 1.0f;

    // Create audience around the perimeter
    for (int i = 0; i < NUM_AUDIENCE; i++) {
        AudienceMember member;
        member.animationOffset = (float)rand() / RAND_MAX * 360.0f;
        member.scale = 0.8f + ((float)rand() / RAND_MAX * 0.4f);
        member.rotation = (float)(rand() % 360);
        member.colorVariation = rand() % 5;
        member.baseY = 0.0f;

        // Randomly position around the fence
        float side = rand() % 4;
        float offset = -9.0f + (float)(rand() % 180) / 10.0f;

        if (side == 0) {      // North side
            member.x = offset;
            member.z = -11.0f - (float)(rand() % 30) / 10.0f;
        }
        else if (side == 1) { // South side
            member.x = offset;
            member.z = 11.0f + (float)(rand() % 30) / 10.0f;
        }
        else if (side == 2) { // East side
            member.x = 11.0f + (float)(rand() % 30) / 10.0f;
            member.z = offset;
        }
        else {              // West side
            member.x = -11.0f - (float)(rand() % 30) / 10.0f;
            member.z = offset;
        }

        audience.push_back(member);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    if (gameWon || gameLost) {
        stopBackgroundMusic();
        drawGameEndScreen();
    }
    else {
        switch (cameraView) {
        case 1: // Top view
            gluLookAt(0.0f, 22.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
            break;
        case 2: // Side view
            gluLookAt(20.0f, 5.0f, 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f);
            break;
        case 3: // Front view
            gluLookAt(0.0f, 5.0f, 20.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f);
            break;
        default: // Free camera
            float lookAtX = cameraX + cos(pitch * M_PI / 180.0f) * sin(yaw * M_PI / 180.0f);
            float lookAtY = cameraY + sin(pitch * M_PI / 180.0f);
            float lookAtZ = cameraZ - cos(pitch * M_PI / 180.0f) * cos(yaw * M_PI / 180.0f);

            gluLookAt(cameraX, cameraY, cameraZ,          // Camera position
                lookAtX, lookAtY, lookAtZ,          // Look-at point
                0.0f, 1.0f, 0.0f);                 // Up vector
            break;
        }
        playBackgroundMusic();
        // Setup lighting
        GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        drawGround();
        drawWalls();
        drawAudience();
        drawPlayer();
        if (!gameWon) {
            drawTarget();
        }
        drawTree();
        drawBench();
        //drawGoldCup();
        drawFlag();
        drawLight();
        drawBanner();
        drawArrow();

        if (isPowerCharging) {
            // Draw power meter in 2D overlay
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_QUADS);
            glVertex2f(10, 10);
            glVertex2f(10 + (shootingPower / MAX_POWER) * 100, 10);
            glVertex2f(10 + (shootingPower / MAX_POWER) * 100, 30);
            glVertex2f(10, 30);
            glEnd();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
        }

        updateAnimations();
        updateArrows();
        updateLights();
		updatePlayerAnimations();
        updateShootingAnimation();
        checkCollisions();

        // Save current matrices
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Disable lighting for text
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        // Draw text
        glColor3f(1.0f, 1.0f, 0.0f);  // Yellow color for text

        // Draw target hits
        glRasterPos2i(glutGet(GLUT_WINDOW_WIDTH) - 150, glutGet(GLUT_WINDOW_HEIGHT) - 30);
        std::string hitsText = "Target Hits: " + std::to_string(targerHitCount);
        for (char c : hitsText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        // Draw time remaining
        glRasterPos2i(glutGet(GLUT_WINDOW_WIDTH) / 2 - 50, glutGet(GLUT_WINDOW_HEIGHT) - 30);
        std::string timeText = "Time: " + std::to_string((int)timeRemaining);
        for (char c : timeText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        // Re-enable lighting and depth testing
        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);

        // Restore matrices
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
    glutSwapBuffers();
    glutPostRedisplay(); // Ensure continuous rendering
}

void keyboard(unsigned char key, int x, int y) {
    float moveSpeed = 0.1f;
    float oldX = playerX;
    float oldZ = playerZ;
	isMoving = false;

    // Calculate movement direction based on camera rotation
    float forwardX = sin(yaw * M_PI / 180.0f);
    float forwardZ = -cos(yaw * M_PI / 180.0f);
    float rightX = sin((yaw + 90.0f) * M_PI / 180.0f);
    float rightZ = -cos((yaw + 90.0f) * M_PI / 180.0f);

    switch (key) {
    case 'w':
        playerZ -= moveSpeed;
        playerRotation = 0.0f;  // Looking forward
        isMoving = true;
        cameraX += forwardX * moveSpeed;
        cameraZ += forwardZ * moveSpeed;
        break;
    case 's':
        playerZ += moveSpeed;
        playerRotation = 180.0f;  // Looking backward
        isMoving = true;
        cameraX -= forwardX * moveSpeed;
        cameraZ -= forwardZ * moveSpeed;
        break;
    case 'a':
        playerX -= moveSpeed;
        playerRotation = 270.0f;  // Looking left
        isMoving = true;
        cameraX -= rightX * moveSpeed;
        cameraZ -= rightZ * moveSpeed;
        break;
    case 'd':
        playerX += moveSpeed;
        playerRotation = 90.0f;  // Looking right
        isMoving = true;
        cameraX += rightX * moveSpeed;
        cameraZ += rightZ * moveSpeed;
        break;
    case 'q': // Move up
        cameraY += moveSpeed;
        break;
    case 'e': // Move down
        cameraY -= moveSpeed;
        break;
    case ' ': // Space bar to start charging shot
        if (!isPowerCharging) {
            isPowerCharging = true;
            shootingPower = 0.5f;
        }
        else {
            playSound("sounds/shoot", false);
        }
        break;
    case '2': cameraView = 1; break;
    case '3': cameraView = 2; break;
    case '4': cameraView = 3; break;
    case '1': cameraView = 0; break;
    case 'z': treeAnimation = !treeAnimation; break;
    case 'x': benchAnimation = !benchAnimation; break;
    case 'c': flagAnimation = !flagAnimation; break;
    case 'v': lightAnimation = !lightAnimation; break;
    case 'b': bannerAnimation = !bannerAnimation; break;
    case 27: exit(0); break;
    }

    // Smooth rotation when changing direction
    static float targetRotation = playerRotation;
    float rotationDiff = targetRotation - playerRotation;

    // Normalize the rotation difference to be between -180 and 180 degrees
    if (rotationDiff > 180) rotationDiff -= 360;
    if (rotationDiff < -180) rotationDiff += 360;

    // Smoothly interpolate the rotation
    //playerRotation += rotationDiff * 0.2f;

    // Keep rotation in 0-360 range
    if (playerRotation >= 360.0f) playerRotation -= 360.0f;
    if (playerRotation < 0.0f) playerRotation += 360.0f;

    // Check collisions after movement 
    float dx = playerX - oldX;
    float dz = playerZ - oldZ;
    if (dx != 0 || dz != 0) {
        // Normalize movement vector
        float len = sqrt(dx * dx + dz * dz);
        if (len > 0) {
            dx /= len;
            dz /= len;
        }
    }

    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (cameraView == 0) { // Only move camera in free view mode
        switch (key) {
        case GLUT_KEY_UP:
            // Move camera forward in the direction it's looking
            cameraX += cameraSpeed * sin(cameraRotationY * M_PI / 180.0f);
            cameraZ -= cameraSpeed * cos(cameraRotationY * M_PI / 180.0f);
            break;
        case GLUT_KEY_DOWN:
            // Move camera backward in the direction it's looking
            cameraX -= cameraSpeed * sin(cameraRotationY * M_PI / 180.0f);
            cameraZ += cameraSpeed * cos(cameraRotationY * M_PI / 180.0f);
            break;
        case GLUT_KEY_LEFT:
            // Rotate camera left
            cameraRotationY -= 5.0f;
            break;
        case GLUT_KEY_RIGHT:
            // Rotate camera right
            cameraRotationY += 5.0f;
            break;
        }

        // Keep rotation in 0-360 range
        if (cameraRotationY >= 360.0f) cameraRotationY -= 360.0f;
        if (cameraRotationY < 0.0f) cameraRotationY += 360.0f;

        glutPostRedisplay();
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    if (key == ' ' && isPowerCharging) { // Space bar released - shoot arrow
        isPowerCharging = false;

        Arrow newArrow;
        newArrow.x = playerX;
        newArrow.y = playerY + 0.5f; // Shoot from player's chest height
        newArrow.z = playerZ;

        // Calculate shooting direction based on player's rotation
        float angle = playerRotation * M_PI / 180.0f;
        newArrow.dx = -sin(angle) * shootingPower;
        newArrow.dz = -cos(angle) * shootingPower;
        newArrow.dy = 0.2f * shootingPower; // Add some upward trajectory

        newArrow.active = true;
        newArrow.speed = 0.5f;

        arrows.push_back(newArrow);
        shootingPower = 1.0f; // Reset power for next shot


    }
    
	if (key == 'w' || key == 's' || key == 'a' || key == 'd') {
	    isMoving = false;
	}
}

void mouse(int button, int state, int x, int y) {
    if (cameraView == 0) { // Only in free camera mode
        if (button == GLUT_LEFT_BUTTON) {
            if (state == GLUT_DOWN) {
                mouseRotating = true;
                lastMouseX = x;
                lastMouseY = y;
            }
            else if (state == GLUT_UP) {
                mouseRotating = false;
            }
        }
    }
}

void mouseMotion(int x, int y) {
    if (mouseRotating && cameraView == 0) {
        // Calculate change in mouse position
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        // Update camera rotation
        yaw += deltaX * mouseSensitivity;
        pitch -= deltaY * mouseSensitivity;  // Inverted for natural feel

        // Limit pitch to prevent camera flipping
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        // Convert to radians for calculation
        float yawRad = yaw * M_PI / 180.0f;
        float pitchRad = pitch * M_PI / 180.0f;

        // Calculate new look-at point
        cameraRotationY = yaw;  // Store for other functions
        cameraRotationX = pitch;

        // Update last mouse position
        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay();
    }
}

void timer(int value) {
    // Update wall colors
    //wallColorR = (float)rand() / RAND_MAX;
    //wallColorG = (float)rand() / RAND_MAX;
    //wallColorB = (float)rand() / RAND_MAX;
    if (isPowerCharging && shootingPower < MAX_POWER) {
        shootingPower += POWER_INCREASE_RATE;
        if (shootingPower > MAX_POWER) shootingPower = MAX_POWER;
    }

    // Update target rotation
    targetRotation += 2.0f;
    if (targetRotation >= 360.0f) targetRotation = 0.0f;

    // Update time
    if (!gameWon && !gameLost) {
        timeRemaining--;
        if (timeRemaining <= 0) {
            gameLost = true;
			playSound("sounds/lose.wav", false);
        }
    }

    glutPostRedisplay();
    glutTimerFunc(1000, timer, 0);
}

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Olympic Archery Game");

    // Initialize OpenGL
    init();

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutTimerFunc(1000, timer, 0);
    yaw = -90.0f;  // Start looking along negative Z
    pitch = 0.0f;
    // Enter the main loop
    glutMainLoop();
    return 0;
}