//#include <iostream>
//#include <vector>
//#include <cmath>
//#include <math.h>
//#include <ctime>
//#include <algorithm>
//#include <cstdlib>
//#include <vector>
//#include <string>
//#include <glut.h>
//
//
//#define M_PI 3.14159265358979323846
//#define WINDOW_WIDTH 1024
//#define WINDOW_HEIGHT 768
//#define GAME_DURATION 60.0f
//#define PLAYER_MOVE_SPEED 0.2f
//#define PLAYER_ROTATION_SPEED 5.0f
//#define TARGET_ROTATION_SPEED 45.0f
//#define MAX_DARTS 10
//#define FENCE_BOUND 4.5f
//
//void init();
//void display();
//void update(int value);
//void reshape(int w, int h);
//void keyboard(unsigned char key, int x, int y);
//void keyboardUp(unsigned char key, int x, int y);
//void specialKeyboard(int key, int x, int y);
//void mouse(int x, int y);
//void drawGround();
//void drawSkybox();
//void drawHUD();
//void drawGameOverScreen();
//void drawText(float x, float y, const char* text);
//void drawRect(float x, float y, float width, float height);
//void boundPlayerPosition();
//
//
//struct Vector3 {
//    float x, y, z;
//    Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
//
//    Vector3 operator+(const Vector3& other) const {
//        return Vector3(x + other.x, y + other.y, z + other.z);
//    }
//
//    Vector3 operator-(const Vector3& other) const {
//        return Vector3(x - other.x, y - other.y, z - other.z);
//    }
//
//    Vector3 operator*(float scalar) const {
//        return Vector3(x * scalar, y * scalar, z * scalar);
//    }
//
//    float distance(const Vector3& other) const {
//        float dx = x - other.x;
//        float dy = y - other.y;
//        float dz = z - other.z;
//        return sqrt(dx * dx + dy * dy + dz * dz);
//    }
//
//    void normalize() {
//        float length = sqrt(x * x + y * y + z * z);
//        if (length > 0) {
//            x /= length;
//            y /= length;
//            z /= length;
//        }
//    }
//};
//// Enhanced Camera System
//class Camera {
//public:
//    Vector3 position;
//    Vector3 target;
//    Vector3 up;
//    float yaw;
//    float pitch;
//
//    Camera() : position(0, 2, 5), up(0, 1, 0), yaw(-90), pitch(0) {
//        updateVectors();
//    }
//
//    void updateVectors() {
//        float yawRad = yaw * M_PI / 180.0f;
//        float pitchRad = pitch * M_PI / 180.0f;
//
//        target.x = cos(yawRad) * cos(pitchRad);
//        target.y = sin(pitchRad);
//        target.z = sin(yawRad) * cos(pitchRad);
//        target.normalize();
//    }
//
//    void moveForward(float speed) {
//        Vector3 newPos = position;
//        newPos.x += target.x * speed;
//        newPos.z += target.z * speed;
//
//        // Only update if within bounds
//        if (isWithinBounds(newPos)) {
//            position = newPos;
//        }
//    }
//
//    void moveBack(float speed) {
//        Vector3 newPos = position;
//        newPos.x -= target.x * speed;
//        newPos.z -= target.z * speed;
//
//        // Only update if within bounds
//        if (isWithinBounds(newPos)) {
//            position = newPos;
//        }
//    }
//
//    void moveRight(float speed) {
//        Vector3 newPos = position;
//        Vector3 right = {
//            static_cast<float>(sin(yaw * M_PI / 180.0f - M_PI / 2)),
//            0.0f,
//            static_cast<float>(cos(yaw * M_PI / 180.0f - M_PI / 2))
//        };
//        newPos.x += right.x * speed;
//        newPos.z += right.z * speed;
//
//        // Only update if within bounds
//        if (isWithinBounds(newPos)) {
//            position = newPos;
//        }
//    }
//
//    void moveLeft(float speed) {
//        Vector3 newPos = position;
//        Vector3 right = {
//            static_cast<float>(sin(yaw * M_PI / 180.0f - M_PI / 2)),
//            0.0f,
//            static_cast<float>(cos(yaw * M_PI / 180.0f - M_PI / 2))
//        };
//        newPos.x -= right.x * speed;
//        newPos.z -= right.z * speed;
//
//        // Only update if within bounds
//        if (isWithinBounds(newPos)) {
//            position = newPos;
//        }
//    }
//
//    bool isWithinBounds(const Vector3& pos) const {
//        const float BOUND = FENCE_BOUND - 0.5f; // Add small buffer from fence
//        return pos.x >= -BOUND && pos.x <= BOUND &&
//            pos.z >= -BOUND && pos.z <= BOUND;
//    }
//
//    void rotate(float deltaYaw, float deltaPitch) {
//        yaw += deltaYaw;
//        pitch += deltaPitch;
//
//        // Limit pitch to avoid flipping
//        if (pitch > 89.0f) pitch = 89.0f;
//        if (pitch < -89.0f) pitch = -89.0f;
//
//        updateVectors();
//    }
//
//    void applyView() {
//        gluLookAt(position.x, position.y, position.z,
//            position.x + target.x, position.y + target.y, position.z + target.z,
//            up.x, up.y, up.z);
//    }
//};
//// Projectile class for darts
//class Dart {
//public:
//    Vector3 position;
//    Vector3 velocity;
//    bool active;
//    float rotation;
//    Vector3 direction;
//
//    Dart() : active(false), rotation(0) {}
//
//    void update(float deltaTime) {
//        if (!active) return;
//
//        // Apply gravity
//        velocity.y -= 9.81f * deltaTime;
//        position = position + velocity * deltaTime;
//
//        // Rotate dart based on movement
//        rotation += 360.0f * deltaTime;
//
//        // Check floor collision
//        if (position.y < 0.1f) {
//            active = false;
//        }
//    }
//
//    void draw() {
//        if (!active) return;
//
//        glPushMatrix();
//        glTranslatef(position.x, position.y, position.z);
//
//        // Align dart with velocity direction
//        Vector3 up(0, 1, 0);
//        Vector3 right = { velocity.z, 0, -velocity.x };
//        right.normalize();
//
//        // Create rotation matrix to align dart with velocity
//        GLfloat rotMatrix[16] = {
//            right.x, right.y, right.z, 0,
//            up.x, up.y, up.z, 0,
//            -velocity.x, -velocity.y, -velocity.z, 0,
//            0, 0, 0, 1
//        };
//        glMultMatrixf(rotMatrix);
//
//        // Dart body
//        glColor3f(0.7f, 0.7f, 0.7f);
//        GLUquadricObj* cylinder = gluNewQuadric();
//        glRotatef(90, 0, 1, 0);
//        gluCylinder(cylinder, 0.02, 0.02, 0.2, 10, 1);
//
//        // Dart tip
//        glColor3f(0.9f, 0.1f, 0.1f);
//        glTranslatef(0, 0, 0.2);
//        glutSolidCone(0.03, 0.1, 10, 10);
//
//        // Dart fins
//        glColor3f(0.2f, 0.2f, 0.8f);
//        for (int i = 0; i < 3; i++) {
//            glRotatef(120, 0, 0, 1);
//            glBegin(GL_TRIANGLES);
//            glVertex3f(0, 0, -0.1);
//            glVertex3f(0, 0.05, -0.15);
//            glVertex3f(0, 0, -0.2);
//            glEnd();
//        }
//
//        glPopMatrix();
//        gluDeleteQuadric(cylinder);
//    }
//};
//// Enhanced Player Class
//class Player {
//public:
//    Vector3 position;
//    Vector3 direction;
//    float angle;
//    float health;
//    float rotationSpeed;
//    std::vector<Dart> darts;
//    int score;
//    float bobPhase;
//    bool isMoving;
//    bool isAiming;
//    float aimAngle;
//
//    Player() : position(0, 1.7f, 0),
//        direction(0, 0, 1),
//        angle(0),
//        health(100),
//        score(0),
//        bobPhase(0),
//        rotationSpeed(5.0f),
//        isMoving(false),
//        isAiming(false),
//        aimAngle(0) {
//        darts.resize(10);
//        updateDirection();
//    }
//    bool isWithinBounds(const Vector3& pos) const {
//        const float BOUND = FENCE_BOUND - 0.3f;
//        return pos.x >= -BOUND && pos.x <= BOUND &&
//            pos.z >= -BOUND && pos.z <= BOUND;
//    }
//    void move(float dx, float dz) {
//        Vector3 newPos = position;
//        newPos.x += dx;
//        newPos.z += dz;
//
//        // Check if new position would be within bounds before updating
//        if (isWithinBounds(newPos)) {
//            position = newPos;
//            isMoving = true;
//        }
//        else {
//            // Apply sliding motion along walls
//            if (isWithinBounds(Vector3(position.x + dx, position.y, position.z))) {
//                position.x += dx;
//                isMoving = true;
//            }
//            if (isWithinBounds(Vector3(position.x, position.y, position.z + dz))) {
//                position.z += dz;
//                isMoving = true;
//            }
//        }
//        boundPosition();
//    }
//    void boundPosition() {
//        const float BOUND = FENCE_BOUND - 0.3f; // Add buffer from fence
//        const float BOUNCE_FACTOR = 0.1f;
//
//        // X boundaries with smooth bounce
//        if (position.x < -BOUND) {
//            position.x = -BOUND + BOUNCE_FACTOR;
//            health = std::max(0.0f, health - 1.0f); // Small penalty for hitting fence
//        }
//        else if (position.x > BOUND) {
//            position.x = BOUND - BOUNCE_FACTOR;
//            health = std::max(0.0f, health - 1.0f);
//        }
//
//        // Z boundaries with smooth bounce
//        if (position.z < -BOUND) {
//            position.z = -BOUND + BOUNCE_FACTOR;
//            health = std::max(0.0f, health - 1.0f);
//        }
//        else if (position.z > BOUND) {
//            position.z = BOUND - BOUNCE_FACTOR;
//            health = std::max(0.0f, health - 1.0f);
//        }
//    }
//    void rotate(float degrees) {
//        angle += degrees * rotationSpeed;  // Apply rotation speed
//
//        while (angle >= 360.0f) angle -= 360.0f;
//        while (angle < 0.0f) angle += 360.0f;
//
//        updateDirection();
//    }
//
//    void updateDirection() {
//        direction.x = sin(angle * M_PI / 180.0f);
//        direction.z = cos(angle * M_PI / 180.0f);
//    }
//
//    void shoot() {
//        for (auto& dart : darts) {
//            if (!dart.active) {
//                dart.active = true;
//                // Position dart at player's hand
//                dart.position = position + Vector3(
//                    direction.x * 0.5f,
//                    0.2f,  // Hand height
//                    direction.z * 0.5f
//                );
//
//                // Calculate shooting direction with aim
//                float shootAngle = angle + aimAngle;
//                float radians = shootAngle * M_PI / 180.0f;
//                Vector3 shootDir(sin(radians), 0.2f, cos(radians));
//                dart.velocity = shootDir * 15.0f;  // Shooting speed
//                break;
//            }
//        }
//    }
//
//    void update(float deltaTime) {
//        if (isMoving) {
//            bobPhase += deltaTime * 5.0f;
//            if (bobPhase > 2 * M_PI) bobPhase -= 2 * M_PI;
//        }
//
//        updateDirection();
//
//        // Update darts
//        for (auto& dart : darts) {
//            dart.update(deltaTime);
//        }
//    }
//
//    void draw() {
//        glPushMatrix();
//        glTranslatef(position.x, position.y + (isMoving ? 0.05f * sin(bobPhase) : 0), position.z);
//        glRotatef(angle, 0, 1, 0);
//
//        // Body
//        drawBody();
//        drawHead();
//        drawArms();
//        drawLegs();
//
//        glPopMatrix();
//
//        // Draw darts
//        for (auto& dart : darts) {
//            dart.draw();
//        }
//    }
//
//private:
//    void drawBody() {
//        // Enhanced torso with more detail
//        glColor3f(0.2f, 0.3f, 0.8f);
//
//        // Main torso
//        glPushMatrix();
//        glScalef(0.4f, 0.6f, 0.2f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//
//        // Chest armor
//        glPushMatrix();
//        glTranslatef(0, 0.1f, 0.1f);
//        glScalef(0.35f, 0.4f, 0.1f);
//        glColor3f(0.3f, 0.4f, 0.9f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//
//        // Belt
//        glPushMatrix();
//        glTranslatef(0, -0.25f, 0);
//        glScalef(0.42f, 0.05f, 0.22f);
//        glColor3f(0.1f, 0.1f, 0.1f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//    }
//
//    void drawHead() {
//        glPushMatrix();
//        glTranslatef(0, 0.5f, 0);
//
//        // Head tilt while moving
//        if (isMoving) {
//            glRotatef(5 * sin(bobPhase), 1, 0, 0);
//        }
//
//        // Face
//        glColor3f(0.9f, 0.7f, 0.5f);
//        glutSolidSphere(0.15, 20, 20);
//
//        // Eyes
//        glPushMatrix();
//        glTranslatef(0.05f, 0.05f, 0.13f);
//
//        // White of eyes
//        glColor3f(1.0f, 1.0f, 1.0f);
//        glutSolidSphere(0.03, 10, 10);
//
//        // Pupils
//        glColor3f(0.0f, 0.0f, 0.0f);
//        glTranslatef(0, 0, 0.02f);
//        glutSolidSphere(0.015, 8, 8);
//        glPopMatrix();
//
//        // Second eye
//        glPushMatrix();
//        glTranslatef(-0.05f, 0.05f, 0.13f);
//        glColor3f(1.0f, 1.0f, 1.0f);
//        glutSolidSphere(0.03, 10, 10);
//        glColor3f(0.0f, 0.0f, 0.0f);
//        glTranslatef(0, 0, 0.02f);
//        glutSolidSphere(0.015, 8, 8);
//        glPopMatrix();
//
//        // Hat or helmet
//        glColor3f(0.2f, 0.2f, 0.7f);
//        glPushMatrix();
//        glRotatef(-90, 1, 0, 0);
//        glutSolidCone(0.2, 0.2, 20, 20);
//        glPopMatrix();
//
//        glPopMatrix();
//    }
//
//    void drawArms() {
//        float armSwing = isMoving ? 30 * sin(bobPhase) : 0;
//
//        // Left arm
//        glPushMatrix();
//        glColor3f(0.2f, 0.3f, 0.8f);
//        glTranslatef(-0.25f, 0.1f, 0);
//        glRotatef(-20 + armSwing, 1, 0, 0);
//
//        // Upper arm
//        glPushMatrix();
//        glScalef(0.1f, 0.3f, 0.1f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//
//        // Elbow joint
//        glTranslatef(0, -0.15f, 0);
//        glutSolidSphere(0.05, 10, 10);
//
//        // Lower arm
//        glTranslatef(0, -0.15f, 0);
//        glRotatef(20, 1, 0, 0);
//        glScalef(0.08f, 0.25f, 0.08f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//
//        // Right arm (aiming position)
//        glPushMatrix();
//        glColor3f(0.2f, 0.3f, 0.8f);
//        glTranslatef(0.25f, 0.1f, 0);
//
//        // Adjust arm position when aiming
//        if (isAiming) {
//            glRotatef(-90, 1, 0, 0);
//            glRotatef(aimAngle, 0, 1, 0);
//        }
//        else {
//            glRotatef(-20 - armSwing, 1, 0, 0);
//        }
//
//        // Upper arm
//        glPushMatrix();
//        glScalef(0.1f, 0.3f, 0.1f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//
//        // Elbow joint
//        glTranslatef(0, -0.15f, 0);
//        glutSolidSphere(0.05, 10, 10);
//
//        // Lower arm
//        glTranslatef(0, -0.15f, 0);
//        glRotatef(20, 1, 0, 0);
//        glScalef(0.08f, 0.25f, 0.08f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//    }
//
//    void drawLegs() {
//        float legSwing = isMoving ? 30 * sin(bobPhase) : 0;
//
//        // Left leg
//        glPushMatrix();
//        glColor3f(0.1f, 0.1f, 0.5f);
//        glTranslatef(-0.15f, -0.4f, 0);
//        glRotatef(legSwing, 1, 0, 0);
//
//        // Upper leg
//        glPushMatrix();
//        glScalef(0.12f, 0.3f, 0.12f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//
//        // Knee joint
//        glTranslatef(0, -0.15f, 0);
//        glutSolidSphere(0.06, 10, 10);
//
//        // Lower leg
//        glTranslatef(0, -0.15f, 0);
//        glRotatef(-20, 1, 0, 0);
//        glScalef(0.1f, 0.3f, 0.1f);
//        glutSolidCube(1.0);
//
//        // Foot
//        glTranslatef(0, -0.5f, 0.1f);
//        glScalef(1.0f, 0.3f, 1.5f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//
//        // Right leg (mirrored)
//        glPushMatrix();
//        glTranslatef(0.15f, -0.4f, 0);
//        glRotatef(-legSwing, 1, 0, 0);
//
//        // Upper leg
//        glPushMatrix();
//        glScalef(0.12f, 0.3f, 0.12f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//
//        // Knee joint
//        glTranslatef(0, -0.15f, 0);
//        glutSolidSphere(0.06, 10, 10);
//
//        // Lower leg
//        glTranslatef(0, -0.15f, 0);
//        glRotatef(-20, 1, 0, 0);
//        glScalef(0.1f, 0.3f, 0.1f);
//        glutSolidCube(1.0);
//
//        // Foot
//        glTranslatef(0, -0.5f, 0.1f);
//        glScalef(1.0f, 0.3f, 1.5f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//    }
//};
//// Enhanced Target (Dartboard) class
//class Target {
//public:
//    Vector3 position;
//    bool active;
//    float rotation;
//    float distance;
//    float colorPhase;
//    float scale;
//    int points;  // Points value for different sections
//    Player* playerRef;
//
//    Target(float x, float y, float z, Player* player)
//        : position(x, y, z),
//        active(true),
//        rotation(0),
//        distance(0),
//        colorPhase(0),
//        scale(1.0f),
//        points(100),
//        playerRef(player) {
//        updateDistance();
//    }
//
//    void update(float deltaTime) {
//        if (!active) return;
//
//        // Update rotation
//        rotation += 45.0f * deltaTime;  // Rotate 45 degrees per second
//        if (rotation >= 360.0f) rotation -= 360.0f;
//
//        // Update color phase for animation
//        colorPhase += deltaTime * 2.0f;
//        if (colorPhase >= 2 * M_PI) colorPhase -= 2 * M_PI;
//
//        // Update scale for pulsing effect
//        scale = 1.0f + 0.05f * sin(colorPhase);
//
//        // Update distance from player
//        updateDistance();
//    }
//
//    bool checkCollision(const Dart& dart) {
//        if (!active || !dart.active) return false;
//
//        // Calculate distance between dart and target
//        float dx = position.x - dart.position.x;
//        float dy = position.y - dart.position.y;
//        float dz = position.z - dart.position.z;
//        float hitDistance = sqrt(dx * dx + dy * dy + dz * dz);
//
//        // Different scoring zones
//        if (hitDistance <= 0.05f) {          // Bullseye
//            points = 200;
//            return true;
//        }
//        else if (hitDistance <= 0.15f) {   // Inner ring
//            points = 150;
//            return true;
//        }
//        else if (hitDistance <= 0.25f) {   // Middle ring
//            points = 100;
//            return true;
//        }
//        else if (hitDistance <= 0.45f) {   // Outer ring
//            points = 50;
//            return true;
//        }
//
//        return false;
//    }
//
//    void draw() {
//        if (!active) return;
//
//        glPushMatrix();
//        glTranslatef(position.x, position.y, position.z);
//        glRotatef(rotation, 0, 1, 0);
//        glScalef(scale, scale, scale);  // Apply pulsing scale
//
//        // Draw mounting bracket
//        drawMountingBracket();
//
//        // Draw dartboard
//        drawDartboard();
//
//        // Draw distance indicator
//        drawDistanceIndicator();
//
//        glPopMatrix();
//    }
//
//private:
//    void drawMountingBracket() {
//        // Main bracket
//        glColor3f(0.4f, 0.4f, 0.4f);
//        glPushMatrix();
//        glTranslatef(0, 0, -0.1f);
//        glScalef(0.8f, 0.8f, 0.1f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//
//        // Support arms
//        for (int i = 0; i < 2; i++) {
//            glPushMatrix();
//            glTranslatef((i == 0 ? -0.3f : 0.3f), 0, -0.05f);
//            glRotatef(45, 1, 0, 0);
//            glScalef(0.05f, 0.5f, 0.05f);
//            glutSolidCube(1.0);
//            glPopMatrix();
//        }
//    }
//
//    void drawDartboard() {
//        // Board backing
//        glColor3f(0.3f, 0.2f, 0.1f);
//        GLUquadricObj* cylinder = gluNewQuadric();
//        gluDisk(cylinder, 0, 0.5, 32, 4);
//
//        // Draw sections with animated colors
//        const int NUM_SEGMENTS = 20;
//        const float SEGMENT_ANGLE = 360.0f / NUM_SEGMENTS;
//
//        glPushMatrix();
//        glTranslatef(0, 0, 0.01f);
//
//        for (int i = 0; i < NUM_SEGMENTS; i++) {
//            glRotatef(SEGMENT_ANGLE, 0, 0, 1);
//
//            // Animate colors
//            if (i % 2 == 0)
//                glColor3f(0.8f + 0.2f * sin(colorPhase), 0.1f, 0.1f);  // Animated red
//            else
//                glColor3f(0.1f, 0.1f, 0.1f + 0.2f * sin(colorPhase));  // Animated black
//
//            glBegin(GL_TRIANGLES);
//            glVertex3f(0, 0, 0);
//            for (float angle = 0; angle <= SEGMENT_ANGLE; angle += SEGMENT_ANGLE) {
//                float rad = angle * M_PI / 180.0f;
//                glVertex3f(0.45f * cos(rad), 0.45f * sin(rad), 0);
//            }
//            glEnd();
//        }
//
//        // Draw scoring rings with animated colors
//        const int NUM_RINGS = 4;
//        float ringRadii[] = { 0.45f, 0.35f, 0.25f, 0.15f };
//
//        for (int i = 0; i < NUM_RINGS; i++) {
//            if (i % 2 == 0)
//                glColor3f(0.8f + 0.2f * sin(colorPhase),
//                    0.8f + 0.2f * sin(colorPhase),
//                    0.0f);  // Animated gold
//            else
//                glColor3f(0.7f + 0.3f * sin(colorPhase),
//                    0.0f,
//                    0.0f);  // Animated dark red
//
//            glutSolidTorus(0.01f, ringRadii[i], 32, 32);
//        }
//
//        // Animated bullseye
//        glColor3f(0.7f + 0.3f * sin(colorPhase), 0.0f, 0.0f);
//        glutSolidTorus(0.02f, 0.05f, 20, 20);
//
//        glColor3f(0.0f, 0.7f + 0.3f * sin(colorPhase), 0.0f);
//        gluDisk(cylinder, 0, 0.03f, 20, 2);
//
//        gluDeleteQuadric(cylinder);
//        glPopMatrix();
//    }
//
//    void drawDistanceIndicator() {
//        updateDistance();  // Update distance before drawing
//
//        if (distance < 10.0f) {
//            // Convert to screen space for consistent text size
//            GLdouble modelview[16];
//            GLdouble projection[16];
//            GLint viewport[4];
//
//            glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
//            glGetDoublev(GL_PROJECTION_MATRIX, projection);
//            glGetIntegerv(GL_VIEWPORT, viewport);
//
//            GLdouble screenX, screenY, screenZ;
//            gluProject(0, 0.6, 0,
//                modelview, projection, viewport,
//                &screenX, &screenY, &screenZ);
//
//            // Save current matrices
//            glMatrixMode(GL_PROJECTION);
//            glPushMatrix();
//            glLoadIdentity();
//            gluOrtho2D(0, viewport[2], 0, viewport[3]);
//
//            glMatrixMode(GL_MODELVIEW);
//            glPushMatrix();
//            glLoadIdentity();
//
//            // Draw distance text
//            char distText[32];
//            sprintf(distText, "%.1f m", distance);
//            glColor3f(1.0f, 1.0f, 1.0f);
//            glRasterPos2f(screenX - 20, screenY);
//            for (char* c = distText; *c != '\0'; c++) {
//                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
//            }
//
//            // Restore matrices
//            glMatrixMode(GL_PROJECTION);
//            glPopMatrix();
//            glMatrixMode(GL_MODELVIEW);
//            glPopMatrix();
//        }
//    }
//
//    void updateDistance() {
//        if (playerRef) {
//            distance = position.distance(playerRef->position);
//        }
//    }
//};
//// Scene Objects
//class SceneObject {
//public:
//    Vector3 position;
//    float rotation;
//    float scale;
//    int type;
//    bool animating;
//    float animationPhase;
//
//    SceneObject(float x, float y, float z, int t)
//        : position(x, y, z), rotation(0), scale(1.0f), type(t),
//        animating(false), animationPhase(0) {}
//
//    void draw() {
//        glPushMatrix();
//        glTranslatef(position.x, position.y, position.z);
//        glRotatef(rotation, 0, 1, 0);
//        glScalef(scale, scale, scale);
//
//        switch (type) {
//        case 0: drawTable(); break;
//        case 1: drawChair(); break;
//        case 2: drawTrophy(); break;
//        case 3: drawLamp(); break;
//        case 4: drawPlant(); break;
//        }
//
//        glPopMatrix();
//    }
//
//    void update(float deltaTime) {
//        if (animating) {
//            animationPhase += deltaTime;
//
//            switch (type) {
//            case 0: // Table rotates
//                rotation = 360.0f * sin(animationPhase * 0.5f);
//                break;
//            case 1: // Chair scales
//                scale = 1.0f + 0.2f * sin(animationPhase * 2.0f);
//                break;
//            case 2: // Trophy changes color and rotates
//                rotation += 90.0f * deltaTime;
//                break;
//            case 3: // Lamp translates up/down
//                position.y = 1.0f + 0.5f * sin(animationPhase);
//                break;
//            case 4: // Plant scales and rotates
//                scale = 1.0f + 0.2f * sin(animationPhase * 1.5f);
//                rotation += 45.0f * deltaTime;
//                break;
//            }
//        }
//    }
//
//private:
//    void drawTable() {
//        // Table top with wood grain texture effect
//        glColor3f(0.6f, 0.4f, 0.2f);
//        glPushMatrix();
//        glScalef(1.5f, 0.1f, 1.0f);
//        glutSolidCube(1.0);
//
//        // Wood grain effect
//        glColor3f(0.5f, 0.3f, 0.1f);
//        glBegin(GL_LINES);
//        for (float i = -0.5f; i < 0.5f; i += 0.05f) {
//            glVertex3f(-0.5f, 0.51f, i);
//            glVertex3f(0.5f, 0.51f, i);
//        }
//        glEnd();
//        glPopMatrix();
//
//        // Table legs with decorative elements
//        glColor3f(0.5f, 0.35f, 0.15f);
//        for (int i = 0; i < 4; i++) {
//            glPushMatrix();
//            float x = ((i & 1) ? 0.6f : -0.6f);
//            float z = ((i & 2) ? 0.4f : -0.4f);
//            glTranslatef(x, -0.5f, z);
//
//            // Main leg
//            glPushMatrix();
//            glScalef(0.1f, 1.0f, 0.1f);
//            glutSolidCube(1.0);
//            glPopMatrix();
//
//            // Decorative top
//            glTranslatef(0, 0.4f, 0);
//            glutSolidSphere(0.06f, 8, 8);
//
//            // Decorative bottom
//            glTranslatef(0, -0.8f, 0);
//            glutSolidSphere(0.06f, 8, 8);
//            glPopMatrix();
//        }
//
//        // Table drawer
//        glColor3f(0.55f, 0.35f, 0.15f);
//        glPushMatrix();
//        glTranslatef(0, -0.1f, 0.3f);
//        glScalef(0.8f, 0.2f, 0.1f);
//        glutSolidCube(1.0);
//
//        // Drawer handle
//        glColor3f(0.3f, 0.2f, 0.1f);
//        glTranslatef(0, 0, 0.6f);
//        glScalef(0.3f, 0.3f, 0.5f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//    }
//
//    void drawChair() {
//        // Seat
//        glColor3f(0.4f, 0.2f, 0.1f);
//        glPushMatrix();
//        glScalef(0.6f, 0.1f, 0.6f);
//        glutSolidCube(1.0);
//
//        // Seat cushion
//        glColor3f(0.5f, 0.3f, 0.15f);
//        glTranslatef(0, 0.6f, 0);
//        glScalef(0.9f, 0.2f, 0.9f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//
//        // Back support
//        glPushMatrix();
//        glTranslatef(0, 0.5f, -0.25f);
//
//        // Main back
//        glColor3f(0.4f, 0.2f, 0.1f);
//        glScalef(0.6f, 1.0f, 0.1f);
//        glutSolidCube(1.0);
//
//        // Decorative back slats
//        glColor3f(0.35f, 0.15f, 0.05f);
//        for (int i = -2; i <= 2; i++) {
//            glPushMatrix();
//            glTranslatef(i * 0.15f, 0, 0.2f);
//            glScalef(0.08f, 0.9f, 0.1f);
//            glutSolidCube(1.0);
//            glPopMatrix();
//        }
//        glPopMatrix();
//
//        // Legs
//        glColor3f(0.3f, 0.15f, 0.05f);
//        for (int i = 0; i < 4; i++) {
//            glPushMatrix();
//            float x = ((i & 1) ? 0.25f : -0.25f);
//            float z = ((i & 2) ? 0.25f : -0.25f);
//            glTranslatef(x, -0.5f, z);
//
//            // Main leg
//            glScalef(0.05f, 1.0f, 0.05f);
//            glutSolidCube(1.0);
//
//            // Foot pad
//            glTranslatef(0, -0.45f, 0);
//            glScalef(1.5f, 0.1f, 1.5f);
//            glutSolidCube(1.0);
//            glPopMatrix();
//        }
//
//        // Support bars between legs
//        glPushMatrix();
//        glTranslatef(0, -0.3f, 0);
//        for (int i = 0; i < 4; i++) {
//            glRotatef(90 * i, 0, 1, 0);
//            glPushMatrix();
//            glTranslatef(0, 0, 0.25f);
//            glScalef(0.5f, 0.05f, 0.05f);
//            glutSolidCube(1.0);
//            glPopMatrix();
//        }
//        glPopMatrix();
//    }
//
//    void drawLamp() {
//        // Base
//        glColor3f(0.2f, 0.2f, 0.2f);
//        glPushMatrix();
//        glutSolidTorus(0.05f, 0.2f, 20, 20);
//
//        // Stem
//        GLUquadricObj* cylinder = gluNewQuadric();
//        glColor3f(0.3f, 0.3f, 0.3f);
//        glRotatef(90, 1, 0, 0);
//        gluCylinder(cylinder, 0.03, 0.02, 0.8, 10, 1);
//
//        // Joint spheres
//        glColor3f(0.4f, 0.4f, 0.4f);
//        for (float h = 0.2f; h < 0.8f; h += 0.2f) {
//            glPushMatrix();
//            glTranslatef(0, 0, h);
//            glutSolidSphere(0.04, 10, 10);
//            glPopMatrix();
//        }
//
//        // Lampshade
//        glColor3f(0.9f, 0.9f, 0.8f);
//        glPushMatrix();
//        glTranslatef(0, 0, 0.8);
//
//        // Top ring
//        glutSolidTorus(0.02, 0.15, 20, 20);
//
//        // Shade surface
//        glBegin(GL_QUAD_STRIP);
//        for (int i = 0; i <= 360; i += 10) {
//            float angle = i * M_PI / 180.0f;
//            float c = cos(angle);
//            float s = sin(angle);
//            glVertex3f(0.15f * c, 0.15f * s, 0);
//            glVertex3f(0.25f * c, 0.25f * s, -0.2f);
//        }
//        glEnd();
//
//        // Bottom ring
//        glTranslatef(0, 0, -0.2);
//        glutSolidTorus(0.02, 0.25, 20, 20);
//        glPopMatrix();
//
//        gluDeleteQuadric(cylinder);
//        glPopMatrix();
//    }
//
//    void drawPlant() {
//        // Pot
//        GLUquadricObj* cylinder = gluNewQuadric();
//        glColor3f(0.6f, 0.3f, 0.1f);
//
//        // Bottom of pot
//        glPushMatrix();
//        glRotatef(-90, 1, 0, 0);
//        gluDisk(cylinder, 0, 0.2, 20, 1);
//
//        // Pot walls
//        gluCylinder(cylinder, 0.2, 0.25, 0.3, 20, 4);
//
//        // Pot rim
//        glTranslatef(0, 0, 0.3);
//        glutSolidTorus(0.02, 0.25, 20, 20);
//        glPopMatrix();
//
//        // Soil
//        glColor3f(0.3f, 0.2f, 0.1f);
//        glPushMatrix();
//        glTranslatef(0, 0.28f, 0);
//        glRotatef(-90, 1, 0, 0);
//        gluDisk(cylinder, 0, 0.23, 20, 1);
//        glPopMatrix();
//
//        // Plant stems and leaves
//        glColor3f(0.0f, 0.6f, 0.0f);
//        srand(1234); // Fixed seed for consistent plant
//
//        for (int i = 0; i < 8; i++) {
//            glPushMatrix();
//            float angle = (float)i * 45.0f;
//            float height = 0.3f + (rand() % 20) / 100.0f;
//            float bendAngle = 10.0f + (rand() % 20);
//
//            glRotatef(angle, 0, 1, 0);
//            glTranslatef(0.1f, 0.3f, 0);
//            glRotatef(bendAngle, 1, 0, 0);
//
//            // Stem
//            glPushMatrix();
//            glRotatef(-90, 1, 0, 0);
//            gluCylinder(cylinder, 0.01, 0.005, height, 8, 1);
//            glPopMatrix();
//
//            // Leaf
//            glTranslatef(0, height, 0);
//            glRotatef(45, 0, 0, 1);
//            glScalef(0.1f, 0.2f, 0.05f);
//            glColor3f(0.0f, 0.5f + (rand() % 30) / 100.0f, 0.0f);
//            glutSolidSphere(1.0, 10, 10);
//            glPopMatrix();
//        }
//
//        gluDeleteQuadric(cylinder);
//    }
//
//    void drawTrophy() {
//        // Base - multiple layers with gradient
//        for (int i = 0; i < 3; i++) {
//            glColor3f(0.8f + i * 0.1f, 0.7f + i * 0.1f, 0.0f);
//            glPushMatrix();
//            glTranslatef(0, i * 0.03f, 0);
//            glScalef(0.4f - i * 0.05f, 0.03f, 0.4f - i * 0.05f);
//            glutSolidCube(1.0);
//            glPopMatrix();
//        }
//
//        // Main stem with decorative elements
//        GLUquadricObj* cylinder = gluNewQuadric();
//        glColor3f(0.9f, 0.8f, 0.0f);
//
//        glPushMatrix();
//        glTranslatef(0, 0.09f, 0);
//
//        // Stem base decoration
//        glutSolidTorus(0.03, 0.1, 16, 16);
//
//        // Main stem
//        glRotatef(90, 1, 0, 0);
//        gluCylinder(cylinder, 0.05, 0.04, 0.4, 16, 4);
//
//        // Decorative rings along stem
//        for (int i = 1; i < 4; i++) {
//            glColor3f(1.0f, 0.9f, 0.0f);
//            glutSolidTorus(0.01, 0.06 - i * 0.005f, 12, 16);
//            glTranslatef(0, 0, 0.1f);
//        }
//        glPopMatrix();
//
//        // Trophy cup
//        glPushMatrix();
//        glTranslatef(0, 0.5f, 0);
//
//        // Cup base
//        glColor3f(1.0f, 0.9f, 0.0f);
//        glutSolidSphere(0.08, 16, 16);
//
//        // Main cup body
//        glRotatef(-90, 1, 0, 0);
//        gluCylinder(cylinder, 0.1, 0.2, 0.3, 16, 4);
//
//        // Cup rim
//        glTranslatef(0, 0, 0.3);
//        glutSolidTorus(0.02, 0.2, 16, 20);
//
//        // Interior
//        glColor3f(0.9f, 0.8f, 0.0f);
//        gluCylinder(cylinder, 0.18, 0.18, 0.05, 16, 2);
//        glTranslatef(0, 0, 0.05);
//        gluDisk(cylinder, 0, 0.18, 16, 1);
//        glPopMatrix();
//
//        // Handles
//        for (int i = 0; i < 2; i++) {
//            glPushMatrix();
//            glTranslatef(0, 0.65f, 0);
//            glRotatef(90.0f + i * 180.0f, 0, 1, 0);
//
//            // Handle curve
//            glColor3f(1.0f, 0.9f, 0.0f);
//            glutSolidTorus(0.02, 0.1, 16, 8);
//            glPopMatrix();
//        }
//
//        gluDeleteQuadric(cylinder);
//    }
//};
//// Enhanced Fence System
//class Fence {
//public:
//    void draw() {
//        drawFencePosts();
//        drawFenceRails();
//        drawDecorations();
//    }
//
//private:
//    void drawFencePosts() {
//        GLUquadricObj* cylinder = gluNewQuadric();
//        float postSpacing = 1.0f;
//
//        // Draw posts along three sides
//        for (float x = -5; x <= 5; x += postSpacing) {
//            // Back fence
//            drawPost(x, -5, cylinder);
//            // Side fences
//            if (x <= -3 || x >= 3) {  // Gap for player movement
//                drawPost(-5, x, cylinder);
//                drawPost(5, x, cylinder);
//            }
//        }
//        gluDeleteQuadric(cylinder);
//    }
//
//    void drawPost(float x, float z, GLUquadricObj* cylinder) {
//        glPushMatrix();
//        glTranslatef(x, 0, z);
//
//        // Post base
//        glColor3f(0.4f, 0.2f, 0.1f);
//        glPushMatrix();
//        glRotatef(-90, 1, 0, 0);
//        gluCylinder(cylinder, 0.1, 0.1, 1.5, 8, 4);
//
//        // Post cap
//        glTranslatef(0, 0, 1.5);
//        glColor3f(0.3f, 0.15f, 0.05f);
//        glutSolidCone(0.15, 0.2, 8, 4);
//        glPopMatrix();
//
//        glPopMatrix();
//    }
//
//    void drawFenceRails() {
//        // Draw horizontal rails
//        for (int i = 0; i < 3; i++) {
//            float height = 0.3f + i * 0.5f;
//
//            // Back fence
//            drawRail(-5, -5, 10, 0, height);
//
//            // Side fences (with gaps)
//            drawRail(-5, -5, 2, 90, height);  // Left front section
//            drawRail(-5, 3, 2, 90, height);   // Left back section
//            drawRail(5, -5, 2, 90, height);   // Right front section
//            drawRail(5, 3, 2, 90, height);    // Right back section
//        }
//    }
//
//    void drawRail(float startX, float startZ, float length, float angle, float height) {
//        glPushMatrix();
//        glTranslatef(startX, height, startZ);
//        glRotatef(angle, 0, 1, 0);
//
//        glColor3f(0.45f, 0.25f, 0.15f);
//        glPushMatrix();
//        glScalef(length, 0.1f, 0.05f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//
//        glPopMatrix();
//    }
//
//    void drawDecorations() {
//        float postSpacing = 1.0f;
//
//        for (float x = -5; x <= 5; x += postSpacing) {
//            // Back fence decorations
//            if (fabs(x) > 1.0f) {  // Skip center section
//                drawDecoration(x, -5);
//            }
//
//            // Side fence decorations
//            if (x <= -3 || x >= 3) {
//                drawDecoration(-5, x);
//                drawDecoration(5, x);
//            }
//        }
//    }
//
//    void drawDecoration(float x, float z) {
//        glPushMatrix();
//        glTranslatef(x, 1.6f, z);
//
//        // Decorative finial
//        glColor3f(0.35f, 0.2f, 0.1f);
//        glutSolidSphere(0.08f, 8, 8);
//
//        // Small cross piece
//        glPushMatrix();
//        glRotatef(45, 0, 1, 0);
//        glScalef(0.2f, 0.02f, 0.02f);
//        glutSolidCube(1.0);
//        glPopMatrix();
//
//        glPopMatrix();
//    }
//};
//
//// Game state and global variables
//Camera camera;
//Player player;
//std::vector<Target> targets;
//std::vector<SceneObject> sceneObjects;
//Fence fence;
//float elapsedTime = 60.0f;
//bool gameWon = false;
//bool gameLost = false;
//int currentView = 1;
//float wallColorPhase = 0.0f;
//float lastMouseX = 0;
//float lastMouseY = 0;
//bool firstMouse = true;
//
//void display() {
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glLoadIdentity();
//
//    // Update camera view based on view mode
//    switch (currentView) {
//    case 1: // Top view
//        camera.position = player.position + Vector3(0, 10, 0);
//        camera.pitch = -90;
//        camera.yaw = player.angle;
//        break;
//    case 2: // Side view
//        camera.position = player.position + Vector3(5, 2, 0);
//        camera.pitch = 0;
//        camera.yaw = player.angle - 90;
//        break;
//    case 3: // First person
//        camera.position = player.position + Vector3(0, 1.7f, 0);
//        break;
//    }
//
//    camera.updateVectors();
//    camera.applyView();
//
//    // Draw scene
//    drawGround();
//    fence.draw();
//    drawSkybox();
//
//    // Draw scene objects
//    for (auto& obj : sceneObjects) {
//        obj.draw();
//    }
//
//    // Draw targets
//    for (auto& target : targets) {
//        target.draw();
//    }
//
//    // Draw player (except in first-person view)
//    if (currentView != 3) {
//        player.draw();
//    }
//
//    // Draw HUD
//    drawHUD();
//
//    // Draw game over screen if applicable
//    if (gameWon || gameLost) {
//        drawGameOverScreen();
//    }
//
//    glutSwapBuffers();
//}
//
//void drawGround() {
//    glEnable(GL_TEXTURE_2D);
//    glBegin(GL_QUADS);
//    for (float x = -5; x < 5; x += 0.5f) {
//        for (float z = -5; z < 5; z += 0.5f) {
//            float checker = (int(x * 2) + int(z * 2)) % 2;
//            glColor3f(0.3f + checker * 0.1f,
//                0.2f + checker * 0.1f,
//                0.1f + checker * 0.1f);
//
//            glVertex3f(x, 0, z);
//            glVertex3f(x + 0.5f, 0, z);
//            glVertex3f(x + 0.5f, 0, z + 0.5f);
//            glVertex3f(x, 0, z + 0.5f);
//        }
//    }
//    glEnd();
//    glDisable(GL_TEXTURE_2D);
//}
//
//void drawSkybox() {
//    glPushMatrix();
//    glTranslatef(camera.position.x, camera.position.y, camera.position.z);
//
//    float size = 50.0f;
//    glDisable(GL_LIGHTING);
//
//    // Sky (gradient)
//    glBegin(GL_QUADS);
//    // Top
//    glColor3f(0.2f, 0.5f, 1.0f);
//    glVertex3f(-size, size, -size);
//    glVertex3f(size, size, -size);
//    glVertex3f(size, size, size);
//    glVertex3f(-size, size, size);
//
//    // Sides (with gradient)
//    glColor3f(0.2f, 0.5f, 1.0f);
//    glVertex3f(-size, size, -size);
//    glColor3f(0.6f, 0.8f, 1.0f);
//    glVertex3f(-size, 0, -size);
//    glVertex3f(size, 0, -size);
//    glColor3f(0.2f, 0.5f, 1.0f);
//    glVertex3f(size, size, -size);
//    glEnd();
//
//    glEnable(GL_LIGHTING);
//    glPopMatrix();
//}
//
//void update(int value) {
//    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
//    static float lastTime = currentTime;
//    float deltaTime = currentTime - lastTime;
//    lastTime = currentTime;
//
//    if (!gameWon && !gameLost) {
//        elapsedTime -= deltaTime;
//        if (elapsedTime <= 0) {
//            gameLost = true;
//        }
//
//        // Update game objects
//        player.update(deltaTime);
//
//        for (auto& target : targets) {
//            target.update(deltaTime);
//
//            // Check dart collisions
//            for (auto& dart : player.darts) {
//                if (target.checkCollision(dart)) {
//                    target.active = false;
//                    dart.active = false;
//                    player.score += target.points;
//                }
//            }
//        }
//
//        for (auto& obj : sceneObjects) {
//            obj.update(deltaTime);
//        }
//
//        // Check win condition
//        bool allTargetsHit = true;
//        for (const auto& target : targets) {
//            if (target.active) {
//                allTargetsHit = false;
//                break;
//            }
//        }
//        if (allTargetsHit) {
//            gameWon = true;
//        }
//    }
//
//    glutPostRedisplay();
//    glutTimerFunc(16, update, 0);
//}
//
//void boundPlayerPosition() {
//    // X-axis boundaries (left and right)
//    if (player.position.x < -FENCE_BOUND) {
//        player.position.x = -FENCE_BOUND;
//    }
//    else if (player.position.x > FENCE_BOUND) {
//        player.position.x = FENCE_BOUND;
//    }
//
//    // Z-axis boundaries (forward and backward)
//    if (player.position.z < -FENCE_BOUND) {
//        player.position.z = -FENCE_BOUND;
//    }
//    else if (player.position.z > FENCE_BOUND) {
//        player.position.z = FENCE_BOUND;
//    }
//}
//
//// Input handling functions
//void keyboard(unsigned char key, int x, int y) {
//    float moveSpeed = PLAYER_MOVE_SPEED;
//
//    switch (key) {
//    case 'w': // Move forward
//        player.position.x += sin(player.angle * M_PI / 180.0f) * moveSpeed;
//        player.position.z += cos(player.angle * M_PI / 180.0f) * moveSpeed;
//        player.isMoving = true;
//        break;
//    case 's': // Move backward
//        player.position.x -= sin(player.angle * M_PI / 180.0f) * moveSpeed;
//        player.position.z -= cos(player.angle * M_PI / 180.0f) * moveSpeed;
//        player.isMoving = true;
//        break;
//    case 'a': // Strafe left
//        player.position.x -= cos(player.angle * M_PI / 180.0f) * moveSpeed;
//        player.position.z += sin(player.angle * M_PI / 180.0f) * moveSpeed;
//        player.isMoving = true;
//        break;
//    case 'd': // Strafe right
//        player.position.x += cos(player.angle * M_PI / 180.0f) * moveSpeed;
//        player.position.z -= sin(player.angle * M_PI / 180.0f) * moveSpeed;
//        player.isMoving = true;
//        break;
//    case ' ':
//        player.shoot();
//        break;
//    case '1': currentView = 1; break;
//    case '2': currentView = 2; break;
//    case '3': currentView = 3; break;
//    case 'q':
//        for (auto& obj : sceneObjects) {
//            obj.animating = !obj.animating;
//        }
//        break;
//    case 27: // ESC
//        exit(0);
//        break;
//    }
//
//    // Bound player within fence
//    player.position.x = std::max(-4.5f, std::min(4.5f, player.position.x));
//    player.position.z = std::max(-4.5f, std::min(4.5f, player.position.z));
//    player.boundPosition();
//    //boundPlayerPosition();
//
//    glutPostRedisplay();
//}
//
//void keyboardUp(unsigned char key, int x, int y) {
//    switch (key) {
//    case 'w':
//    case 's':
//    case 'a':
//    case 'd':
//        player.isMoving = false;
//        break;
//    }
//}
//
//void specialKeyboard(int key, int x, int y) {
//    switch (key) {
//    case GLUT_KEY_LEFT:
//        player.rotate(5.0f);
//        break;
//    case GLUT_KEY_RIGHT:
//        player.rotate(-5.0f);
//        break;
//    }
//    glutPostRedisplay();
//}
//
//void mouse(int x, int y) {
//    if (firstMouse) {
//        lastMouseX = x;
//        lastMouseY = y;
//        firstMouse = false;
//        return;
//    }
//
//    float sensitivity = 0.1f;
//    float deltaX = (x - lastMouseX) * sensitivity;
//    float deltaY = (lastMouseY - y) * sensitivity;
//    lastMouseX = x;
//    lastMouseY = y;
//
//    if (currentView == 3) {
//        player.rotate(-deltaX);
//        camera.pitch = std::max(-89.0f, std::min(89.0f, camera.pitch + deltaY));
//    }
//}
//
//// Initialize game
//void init() {
//    // Enable OpenGL features
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    glEnable(GL_COLOR_MATERIAL);
//    glEnable(GL_NORMALIZE);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//    // Setup lighting
//    GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
//    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
//    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//
//    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
//
//    // Initialize targets
//    targets.emplace_back(-4, 2, -4, &player);
//    targets.emplace_back(4, 2, -4, &player);
//    targets.emplace_back(-4, 2, 4, &player);
//    targets.emplace_back(4, 2, 4, &player);
//    targets.emplace_back(0, 3, -4.5, &player);
//
//    // Initialize scene objects
//    sceneObjects.emplace_back(-3, 0, -3, 0); // Table
//    sceneObjects.emplace_back(-2, 0, -3, 1); // Chair
//    sceneObjects.emplace_back(3, 0, -3, 2);  // Trophy
//    sceneObjects.emplace_back(3, 0, 3, 3);   // Lamp
//    sceneObjects.emplace_back(-3, 0, 3, 4);  // Plant
//
//    // Initialize random seed
//    srand(time(nullptr));
//}
//
//int main(int argc, char** argv) {
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ALPHA);
//    glutInitWindowSize(1024, 768);
//    glutCreateWindow("Enhanced 3D Dart Game");
//    // Continue from previous main function
//    init();
//
//    // Register callbacks
//    glutDisplayFunc(display);
//    glutReshapeFunc(reshape);
//    glutKeyboardFunc(keyboard);
//    glutKeyboardUpFunc(keyboardUp);
//    glutSpecialFunc(specialKeyboard);
//    glutPassiveMotionFunc(mouse);
//    glutTimerFunc(16, update, 0);
//
//    // Print game instructions
//    std::cout << "Enhanced 3D Dart Game Instructions:" << std::endl;
//    std::cout << "-----------------------------------" << std::endl;
//    std::cout << "Movement Controls:" << std::endl;
//    std::cout << "  W/A/S/D - Move player" << std::endl;
//    std::cout << "  Mouse/Arrow Keys - Rotate view" << std::endl;
//    std::cout << "  Space - Shoot dart" << std::endl;
//    std::cout << "\nCamera Controls:" << std::endl;
//    std::cout << "  1 - Top View" << std::endl;
//    std::cout << "  2 - Side View" << std::endl;
//    std::cout << "  3 - First Person View" << std::endl;
//    std::cout << "\nGame Controls:" << std::endl;
//    std::cout << "  Q - Toggle object animations" << std::endl;
//    std::cout << "  ESC - Exit game" << std::endl;
//    std::cout << "-----------------------------------" << std::endl;
//    std::cout << "Objective: Hit all targets before time runs out!" << std::endl;
//    std::cout << "Distance to target is shown when close." << std::endl;
//
//    glutMainLoop();
//    return 0;
//}
//
//void reshape(int w, int h) {
//    if (h == 0) h = 1;
//    float ratio = w * 1.0f / h;
//
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glViewport(0, 0, w, h);
//    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
//    glMatrixMode(GL_MODELVIEW);
//}
//
//void drawHUD() {
//    glDisable(GL_LIGHTING);
//    glDisable(GL_DEPTH_TEST);
//
//    // Switch to 2D orthographic projection
//    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
//    glLoadIdentity();
//    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
//
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//
//    // Draw score
//    char scoreText[32];
//    sprintf(scoreText, "Score: %d", player.score);
//    glColor3f(1.0f, 1.0f, 1.0f);
//    drawText(10, glutGet(GLUT_WINDOW_HEIGHT) - 30, scoreText);
//
//    // Draw timer
//    char timerText[32];
//    sprintf(timerText, "Time: %.1f", elapsedTime);
//    drawText(10, glutGet(GLUT_WINDOW_HEIGHT) - 50, timerText);
//
//    // Draw health bar
//    float healthWidth = 200.0f * (player.health / 100.0f);
//    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
//    drawRect(10, glutGet(GLUT_WINDOW_HEIGHT) - 70, 200, 20);
//    glColor4f(1.0f, 0.0f, 0.0f, 0.8f);
//    drawRect(10, glutGet(GLUT_WINDOW_HEIGHT) - 70, healthWidth, 20);
//
//    // Draw crosshair in first-person view
//    if (currentView == 3) {
//        int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
//        int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
//        glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
//
//        // Crosshair lines
//        glBegin(GL_LINES);
//        glVertex2f(centerX - 10, centerY);
//        glVertex2f(centerX + 10, centerY);
//        glVertex2f(centerX, centerY - 10);
//        glVertex2f(centerX, centerY + 10);
//        glEnd();
//    }
//
//    // Restore 3D projection
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
//    glMatrixMode(GL_MODELVIEW);
//
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_LIGHTING);
//}
//
//void drawGameOverScreen() {
//    glDisable(GL_LIGHTING);
//    glDisable(GL_DEPTH_TEST);
//
//    // Setup 2D projection
//    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
//    glLoadIdentity();
//    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
//
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//
//    // Semi-transparent background
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
//    drawRect(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
//
//    // Game result text
//    const char* resultText = gameWon ? "VICTORY!" : "GAME OVER";
//    glColor3f(gameWon ? 0.0f : 1.0f, gameWon ? 1.0f : 0.0f, 0.0f);
//
//    // Center the text
//    int textWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)resultText);
//    drawText((glutGet(GLUT_WINDOW_WIDTH) - textWidth) / 2,
//        glutGet(GLUT_WINDOW_HEIGHT) / 2 + 40,
//        resultText);
//
//    // Score
//    char scoreText[32];
//    sprintf(scoreText, "Final Score: %d", player.score);
//    textWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)scoreText);
//    drawText((glutGet(GLUT_WINDOW_WIDTH) - textWidth) / 2,
//        glutGet(GLUT_WINDOW_HEIGHT) / 2,
//        scoreText);
//
//    // Instructions
//    const char* instructions = "Press ESC to exit";
//    textWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)instructions);
//    drawText((glutGet(GLUT_WINDOW_WIDTH) - textWidth) / 2,
//        glutGet(GLUT_WINDOW_HEIGHT) / 2 - 40,
//        instructions);
//
//    // Restore 3D projection
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
//    glMatrixMode(GL_MODELVIEW);
//
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_LIGHTING);
//    glDisable(GL_BLEND);
//}
//
//// Utility functions
//void drawText(float x, float y, const char* text) {
//    glRasterPos2f(x, y);
//    for (const char* c = text; *c != '\0'; c++) {
//        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
//    }
//}
//
//void drawRect(float x, float y, float width, float height) {
//    glBegin(GL_QUADS);
//    glVertex2f(x, y);
//    glVertex2f(x + width, y);
//    glVertex2f(x + width, y + height);
//    glVertex2f(x, y + height);
//    glEnd();
//}
