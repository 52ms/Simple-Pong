#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "shader.h"

#include <stdio.h>

// TO COMPILE: gcc main.c glad.c -o main -lglfw -lGL -ldl && ./main

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

void processInput(GLFWwindow* window);

Shader shader;

struct Block {
    float x;
    float y;
    float width;
    float height;
} LPaddle, RPaddle, Ball;

void init_blocks() {
    LPaddle.x = -0.9f;
    LPaddle.y = 0.0f;
    LPaddle.width = 0.02f;
    LPaddle.height = 0.2f;

    RPaddle.x = 0.9f;
    RPaddle.y = 0.0f;
    RPaddle.width = 0.02f;
    RPaddle.height = 0.2f;

    Ball.x = -0.02;
    Ball.y = -0.02;
    Ball.width = 0.04;
    Ball.height = 0.04;
}

float RoffsetY = 0.0f;
float LoffsetY = 0.0f;

float ballOffsetX = 0.0f, ballOffsetY = 0.0f;

const float paddleSpeed = 0.015f;
float ballSpeedX = 0.007f;
float ballSpeedY = 0.01f;
void update_ball_pos();

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    init_blocks();

    shader = init_shader("vertexShader.glsl", "fragmentShader.glsl");

    float vertices[] = {
        // LPaddle
        LPaddle.x, LPaddle.y,
        LPaddle.x + LPaddle.width, LPaddle.y,
        LPaddle.x, LPaddle.y + LPaddle.height,
        LPaddle.x + LPaddle.width, LPaddle.y + LPaddle.height,

        // RPaddle
        RPaddle.x, RPaddle.y,
        RPaddle.x + RPaddle.width, RPaddle.y,
        RPaddle.x, RPaddle.y + RPaddle.height,
        RPaddle.x + RPaddle.width, RPaddle.y + RPaddle.height,

        // Ball
        Ball.x, Ball.y,
        Ball.x + Ball.width, Ball.y,
        Ball.x, Ball.y + Ball.height,
        Ball.x + Ball.width, Ball.y + Ball.height,
    };

    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    shader.use();
    glBindVertexArray(VAO);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw Left Paddle
        shader.set_float("offsetX", 0.0f);
        shader.set_float("offsetY", LoffsetY);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Draw Right Paddle
        shader.set_float("offsetX", 0.0f);
        shader.set_float("offsetY", RoffsetY);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

        // Draw Ball
        shader.set_float("offsetX", ballOffsetX);
        shader.set_float("offsetY", ballOffsetY);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);

        update_ball_pos();

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    shader.delete();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (RoffsetY + RPaddle.y + RPaddle.height < 1.0f) {
            RoffsetY += paddleSpeed;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (RoffsetY + RPaddle.y > -1.0f) {
            RoffsetY -= paddleSpeed;
        }
    }
}

void update_ball_pos() {
    static int revX = 0, revY = 0;

    if (ballOffsetY + 0.02f > 1) revY = !revY;
    if (ballOffsetY - 0.02f < -1) revY = !revY;

    if (ballOffsetX - 0.02f <= LPaddle.x + LPaddle.width && ballOffsetY >= LPaddle.y + LoffsetY && ballOffsetY <= LPaddle.y + LoffsetY + LPaddle.height) {
        revX = !revX;
        ballOffsetX += ballSpeedX;
    }
    if (ballOffsetX + 0.02f >= RPaddle.x && ballOffsetY >= RPaddle.y + RoffsetY && ballOffsetY <= RPaddle.y + RoffsetY + RPaddle.height) {
        revX = !revX;
        ballOffsetX -= ballSpeedX;
    }

    if (ballOffsetX + 0.02f > 1 || ballOffsetX - 0.02f < -1) {
        ballOffsetX = 0.0f;
        ballOffsetY = 0.0f;
        revX = !revX;
    }

    ballOffsetX += (!revX) ? ballSpeedX : -ballSpeedX;
    ballOffsetY += (!revY) ? ballSpeedY : -ballSpeedY;
    

    // Conputer Pos
    if (ballOffsetX < 0.75f) {
        if (ballOffsetY > LoffsetY + LPaddle.y + LPaddle.height / 2) {
            LoffsetY += ballSpeedY * 0.95;
            if (LoffsetY > 1.0 - LPaddle.height) LoffsetY = 1.0 - LPaddle.height;
        } else if (ballOffsetY < LoffsetY + LPaddle.y) {
            LoffsetY -= ballSpeedY * 0.95;
        }
    }
}