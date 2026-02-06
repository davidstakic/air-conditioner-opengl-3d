#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Util.h"
#include "Cube.h"
#include "Sphere.h"
#include "HollowCylinder.h"
#include "SolidCylinder.h"
#include "TexturedQuad.h"
#include "model.hpp"
#include "shader.hpp"

const float TARGET_FPS = 75.0;
const float TARGET_FRAME_TIME = 1.0 / TARGET_FPS;

const float lookThreshold = 0.995f;
const float flapSpeed = 0.0005f;
const float flapMaxOffset = 0.03f;
const float dropSpeed = 0.003f;
const float maxWaterHeight = 0.18f;

bool useTex = false;
bool transparent = false;
bool firstMouse = true;
bool isOn = false;
bool prevLookingAtLamp = false;
bool basinFilled = false;
bool basinPicked = false;
bool basinEmptied = false;
bool mouseClicked = false;
bool spacePressedLastFrame = false;
bool spacePressed = false;
bool basinStartingToFill = false;

float lastX, lastY = 500.0f;
float yaw = -90.0f, pitch = 0.0f;
float fov = 45.0f;
float flapOffset = 0.0f;
float waterHeight = 0.0f;
float dropY1 = 0.05f;
float dropY2 = 0.10f;
float dropY3 = 0.15f;

unsigned signature;

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 basinOriginalPos;
glm::vec3 acWorldPos = glm::vec3(0.0f, 0.0f, 0.0f);

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        mouseClicked = true;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

bool lookingTowardsAC(const glm::vec3& cameraPos)
{
    glm::vec3 toAC = glm::normalize(acWorldPos - cameraPos);
    float dot = glm::dot(cameraFront, toAC);
    return dot > 0.8f;
}

bool lookingAwayFromAC(const glm::vec3& cameraPos)
{
    glm::vec3 toAC = glm::normalize(acWorldPos - cameraPos);
    float dot = glm::dot(cameraFront, toAC);
    return dot < -0.8f;
}

void preprocessTexture(unsigned& texture, const char* filepath) {
    texture = loadImageToTexture(filepath);
    glBindTexture(GL_TEXTURE_2D, texture);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

int main()
{
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Air Conditioner 3D", monitor, NULL);

    if (!window) return 2;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (glewInit() != GLEW_OK) return 3;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader("basic.vert", "basic.frag");
    Shader overlayShader("overlay.vert", "overlay.frag");

    TexturedQuad signatureQuad(10.0f, 10.0f, 300.0f, 50.0f);
    preprocessTexture(signature, "res/signature.png");

    Cube airConditioner(0.6f, 0.2f, 0.15);
    Cube flap(0.45f, 0.03f, 0.01f);
    Cube flapUnder(0.45f, 0.03f, 0.0001f);
    HollowCylinder basin(0.25f, 0.35f, 0.2f, 64);
    glm::vec3 basinPos(0.0f, -0.7f, 0.0f);
    basinOriginalPos = basinPos;
    SolidCylinder water(0.24f, 0.0f, 64);
    Sphere drop(0.01f, 12, 12);
    glm::vec3 dropOffset2(0.05f, 0.0f, 0.05f);
    glm::vec3 dropOffset3(-0.05f, 0.0f, -0.05f);
    Sphere lamp(0.03f, 16, 16);
    glm::vec3 lampPos(0.55f, -0.16f, 0.14f);

    Model toilet("res/10778_Toilet_V2.obj");
    Model remote("res/10816_ Remote Control_v2_LOD3.obj");
                                                                                                                                                                                                                                                            
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 ortho = glm::ortho(0.0f, (float)mode->width, 0.0f, (float)mode->height);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glCullFace(GL_BACK);

    while (!glfwWindowShouldClose(window))
    {
        double startTime = glfwGetTime();

        glm::vec3 lampWorldPos;

        bool spaceNow = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        spacePressed = spaceNow && !spacePressedLastFrame;
        spacePressedLastFrame = spaceNow;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) glEnable(GL_DEPTH_TEST);
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) glDisable(GL_DEPTH_TEST);
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) glEnable(GL_CULL_FACE);
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) glDisable(GL_CULL_FACE);
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) cameraPos += 0.01f * glm::normalize(glm::vec3(cameraFront.z, 0, -cameraFront.x));
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) cameraPos -= 0.01f * glm::normalize(glm::vec3(cameraFront.z, 0, -cameraFront.x));
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) cameraPos += 0.01f * glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) cameraPos -= 0.01f * glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));

        if (basinPicked && basinFilled && !basinEmptied && spacePressed && lookingAwayFromAC(cameraPos))
        {
            basinEmptied = true;
            basinFilled = false;
            waterHeight = 0.0f;
            water.setHeight(waterHeight);
            basinStartingToFill = false;
        }

        if (basinPicked && basinEmptied && !basinFilled && spacePressed && lookingTowardsAC(cameraPos))
        {
            basinPicked = false;
            basinEmptied = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 proj = glm::perspective(glm::radians(fov), (float)mode->width / mode->height, 0.1f, 100.0f);

        shader.use();
        shader.setMat4("uV", view);
        shader.setMat4("uP", proj);
        shader.setVec3("viewPos", cameraPos);

        shader.setVec3("lightPos", glm::vec3(1.0f, 2.0f, 2.0f));
        shader.setVec3("lightColor", glm::vec3(1.0f));
        shader.setFloat("lightStrength", 1.0f);

        lampWorldPos = glm::vec3(model * glm::vec4(lampPos, 1.0f));
        shader.setBool("lampOn", isOn);
        shader.setVec3("lampPos", lampWorldPos);
        shader.setVec3("lampColor", glm::vec3(1.0f, 0.1f, 0.1f));
        shader.setFloat("lampStrength", 0.5f);

        shader.setMat4("uM", model);
        shader.setBool("useTex", false);
        shader.setInt("useUniformColor", 1);
        shader.setVec4("uColor", 0.95f, 0.95f, 0.95f, 1.0f);
        airConditioner.draw();

        glm::mat4 toiletModel = glm::mat4(1.0f);
        float toiletScale = 0.05f;
        toiletModel = glm::scale(toiletModel, glm::vec3(toiletScale));
        toiletModel = glm::rotate(toiletModel, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        glm::vec3 toiletPos = glm::vec3(0.25f, 0.0f, -1.5f);
        toiletModel = glm::translate(glm::mat4(1.0f), toiletPos) * toiletModel;
        shader.setMat4("uM", toiletModel);
        shader.setInt("useUniformColor", 0);
        shader.setBool("useTex", true);
        toilet.Draw(shader);

        if (!basinPicked)
        {
            float remoteDistance = 0.5f;
            glm::vec3 remoteOffset(0.0f, -0.15f, 0.0f);

            glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));

            glm::vec3 remoteWorldPos = cameraPos + cameraFront * remoteDistance + right * remoteOffset.x + cameraUp * remoteOffset.y;

            glm::mat4 remoteModel = glm::mat4(1.0f);
            remoteModel = glm::translate(remoteModel, remoteWorldPos);

            remoteModel = glm::rotate(remoteModel, glm::radians(-yaw + 270.0f), glm::vec3(0, 1, 0));

            float remoteScale = 0.03f;
            remoteModel = glm::scale(remoteModel, glm::vec3(remoteScale));

            shader.setMat4("uM", remoteModel);
            shader.setInt("useUniformColor", 0);
            shader.setBool("useTex", true);
            remote.Draw(shader);
        }

        if (isOn) {
            if (flapOffset < flapMaxOffset) flapOffset += flapSpeed;
        }
        else {
            if (flapOffset > 0.0f) flapOffset -= flapSpeed;
        }
        glm::vec3 flapPos(0.0f, -0.14f - 0.01f + flapOffset, 0.15f / 2 + 0.05f / 2 + 0.06f);
        glm::mat4 flapModel = model * glm::translate(glm::mat4(1.0f), flapPos);
        shader.setMat4("uM", flapModel);
        shader.setInt("useUniformColor", 1);
        shader.setBool("useTex", false);
        shader.setVec4("uColor", 0.4f, 0.4f, 0.4f, 1.0f);
        flap.draw();

        glm::vec3 flapUnderPos(0.0f, -0.14f - 0.01f, 0.15f / 2 + 0.05f / 2 + 0.05f);
        glm::mat4 flapUnderModel = model * glm::translate(glm::mat4(1.0f), flapUnderPos);
        shader.setMat4("uM", flapUnderModel);
        shader.setInt("useUniformColor", 1);
        shader.setBool("useTex", false);
        shader.setVec4("uColor", 0.2f, 0.2f, 0.2f, 1.0f);
        flapUnder.draw();

        glm::vec3 finalBasinPos = basinPos;
        if (basinPicked)
        {
            finalBasinPos = cameraPos + cameraFront * 0.6f + glm::vec3(0.0f, -0.25f, 0.0f);
        }
        glm::mat4 basinModel = model * glm::translate(glm::mat4(1.0f), finalBasinPos);
        
        shader.setMat4("uM", basinModel);
        shader.setVec4("uColor", 1.0f, 1.0f, 0.7f, 1.0f);
        basin.draw();

        if (isOn && !basinFilled) {
            shader.setVec4("uColor", 0.2f, 0.6f, 1.0f, 0.6f);

            dropY1 -= dropSpeed;
            if (dropY1 <= basinPos.y + waterHeight) dropY1 = 0.05f;
            glm::mat4 dropModel1 = model * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, dropY1, 0.0f));
            shader.setMat4("uM", dropModel1);
            drop.draw();

            dropY2 -= dropSpeed;
            if (dropY2 <= basinPos.y + waterHeight) dropY2 = 0.10f;
            glm::mat4 dropModel2 = model * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, dropY2, 0.0f) + dropOffset2);
            shader.setMat4("uM", dropModel2);
            drop.draw();

            dropY3 -= dropSpeed;
            if (dropY3 <= basinPos.y + waterHeight) dropY3 = 0.15f;
            glm::mat4 dropModel3 = model * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, dropY3, 0.0f) + dropOffset3);
            shader.setMat4("uM", dropModel3);
            drop.draw();
        }

        if (isOn && waterHeight < maxWaterHeight) {
            waterHeight += 0.00015f;
            water.setHeight(waterHeight);
        }
        else if (isOn && waterHeight >= maxWaterHeight) {
            basinFilled = true;
            isOn = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstMouse = true;
        }

        if (isOn) basinStartingToFill = true;

        glm::vec3 waterPos = finalBasinPos + glm::vec3(0.0f, 0.001f, 0.0f);
        glm::mat4 waterModel = model * glm::translate(glm::mat4(1.0f), waterPos);
        shader.setMat4("uM", waterModel);
        shader.setVec4("uColor", 0.2f, 0.6f, 1.0f, 0.4f);
        if (basinStartingToFill) {
            glDepthMask(GL_FALSE);
            glm::vec3 waterPos = finalBasinPos + glm::vec3(0.0f, 0.001f, 0.0f);
            glm::mat4 waterModel = model * glm::translate(glm::mat4(1.0f), waterPos);
            shader.setMat4("uM", waterModel);
            shader.setVec4("uColor", 0.2f, 0.6f, 1.0f, 0.4f);
            water.draw();
            glDepthMask(GL_TRUE);
        }

        glm::mat4 lampModel = model * glm::translate(glm::mat4(1.0f), lampPos);
        shader.setMat4("uM", lampModel);

        if (isOn)
            shader.setVec4("uColor", 1.0f, 0.2f, 0.2f, 1.0f);
        else
            shader.setVec4("uColor", 0.5f, 0.0f, 0.0f, 1.0f);

        lampWorldPos = glm::vec3(model * glm::vec4(lampPos, 1.0f));
        glm::vec3 camToLamp = glm::normalize(lampWorldPos - cameraPos);
        float dot = glm::dot(cameraFront, camToLamp);
        bool currentlyLooking = (dot > lookThreshold);
        if (!prevLookingAtLamp && currentlyLooking && !basinFilled) {
            isOn = !isOn;
        }
        prevLookingAtLamp = currentlyLooking;
        lamp.draw();

        if (mouseClicked && basinFilled)
        {
            glm::vec3 rayOrigin = cameraPos;
            glm::vec3 rayDir = glm::normalize(cameraFront);
            glm::vec3 toLavor = finalBasinPos - rayOrigin;
            float projection = glm::dot(toLavor, rayDir);

            if (projection > 0.0f)
            {
                glm::vec3 closestPoint = rayOrigin + rayDir * projection;
                float distance = glm::length(finalBasinPos - closestPoint);
                float lavorRadius = 0.35f;
                if (distance < lavorRadius)
                {
                    basinPicked = true;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    firstMouse = true;
                }
            }
            mouseClicked = false;
        }

        while (glfwGetTime() - startTime < TARGET_FRAME_TIME) {}

        overlayShader.use();
        overlayShader.setMat4("uMVP", ortho);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, signature);
        overlayShader.setInt("uTexture", 0);
        overlayShader.setFloat("uOpacity", 0.5f);

        signatureQuad.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}