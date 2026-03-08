// Лабораторная работа №7: создание движения через аффинные преобразования

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include <glm.hpp>                   // glm/glm.hpp
#include <gtc/matrix_transform.hpp>  // glm/gtc/matrix_transform.hpp
#include <gtc/type_ptr.hpp>          // glm/gtc/type_ptr.hpp

#include "Shader.h"
#include "Model.h"

static const unsigned int SCR_WIDTH = 1024;
static const unsigned int SCR_HEIGHT = 768;

// =================== Камера (позиция/направление/вверх) ====================

glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// ========================== Мышь: углы yaw/pitch ===========================

float yaw = -90.0f;
float pitch = -10.0f;  // стартовый наклон камеры чуть вниз и перерасчет cameraFront в main()

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool  firstMouse = true;
float sensitivity = 0.1f;

// ========================= Время кадра (deltaTime) ==========================

float deltaTime = 0.0f;
float lastFrame = 0.0f;


static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// ============================= Источник света ===============================

// Позиция точки света в мировых координатах (для расчёта Фонга)
glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, 2.0f);

// ======================== Параметры движения модели =========================

// 1-я степень: каретка едет вдоль балки по X
float carriageOffsetX = 0.0f;
const float carriageMinOffsetX = -1.3f;
const float carriageMaxOffsetX = 1.3f;

// 2-я степень: наклон бокса манипулятора
float manipulatorAngleDeg = 0.0f;
const float manipulatorMinAngleDeg = 0.0f;
const float manipulatorMaxAngleDeg = 22.0f;

// 3-я степень: руки перемещаются вдоль направляющей
// Не абсолютная координата, а смещение от стартового положения
float armsOffsetZ = 0.0f;
const float armsMinOffsetZ = -0.38881f; // 1.2 - 1.58881
const float armsMaxOffsetZ = 0.11119f;  // 1.7 - 1.58881

// Скорости движения (в единицах в секунду / градусах в секунду)
const float carriageSpeed = 1.2f;
const float manipulatorRotateSpeed = 35.0f;
const float armsSpeed = 0.5f;

// ============================================================================
// Обработка клавиатуры: управление камерой и моделью
// ============================================================================

void processInput(GLFWwindow* window)
{
    const float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // ============================= Камера: WASD =============================

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= right * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += right * cameraSpeed;

    // ============================ Движение модели ============================

    // 1) Каретка: стрелки Left / Right
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        carriageOffsetX -= carriageSpeed * deltaTime;

        if (carriageOffsetX < carriageMinOffsetX)
            carriageOffsetX = carriageMinOffsetX;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        carriageOffsetX += carriageSpeed * deltaTime;

        if (carriageOffsetX > carriageMaxOffsetX)
            carriageOffsetX = carriageMaxOffsetX;
    }

    // 2) Бокс манипулятора: клавиши R / T
    // R - наклон, T - возврат назад
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        manipulatorAngleDeg += manipulatorRotateSpeed * deltaTime;

        if (manipulatorAngleDeg > manipulatorMaxAngleDeg)
            manipulatorAngleDeg = manipulatorMaxAngleDeg;
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        manipulatorAngleDeg -= manipulatorRotateSpeed * deltaTime;

        if (manipulatorAngleDeg < manipulatorMinAngleDeg)
            manipulatorAngleDeg = manipulatorMinAngleDeg;
    }

    // 3) Руки: стрелки Up / Down
    // Up - поднять, Down - опустить 
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        armsOffsetZ += armsSpeed * deltaTime;

        if (armsOffsetZ > armsMaxOffsetZ)
            armsOffsetZ = armsMaxOffsetZ;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        armsOffsetZ -= armsSpeed * deltaTime;

        if (armsOffsetZ < armsMinOffsetZ)
            armsOffsetZ = armsMinOffsetZ;
    }
}

// ============================================================================
// Callback мыши: обновляет yaw/pitch и пересчитывает cameraFront
// ============================================================================

void mouse_callback(GLFWwindow* /*window*/, double xposIn, double yposIn)
{
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    front.y = sinf(glm::radians(pitch));
    front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));

    cameraFront = glm::normalize(front);
}

// Отрисовка одного меша с его собственной model-матрицей
void DrawMeshPart(const Model& model, int meshIndex, const glm::mat4& modelMat, const Shader& shader)
{
    // Матрица нормалей нужна для корректного освещения после поворотов/масштабирования
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMat)));

    // Передаём в шейдер матрицу модели и матрицу нормалей
    shader.setMat4("model", glm::value_ptr(modelMat));
    shader.setMat3("normalMatrix", glm::value_ptr(normalMatrix));

    // Рисуем только один нужный меш
    model.DrawMesh((unsigned int)meshIndex);
}

// ============================================================================
// main(): создание окна, настройка OpenGL, загрузка модели и рендер-цикл
// ============================================================================

int main(void)
{
    // 1) GLFW
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab 7 - Moving", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    // Захватываем курсор мыши внутри окна (иначе он будет улетать)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Подписываемся на движение мыши
    glfwSetCursorPosCallback(window, mouse_callback);

    // Пересчёт cameraFront по стартовым yaw/pitch 
    // (так как изменила float pitch в глобальных переменных) 
    {
        glm::vec3 front;
        front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
        front.y = sinf(glm::radians(pitch));
        front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // чтобы при изменении размера окна корректно обновлялся viewport

    // 2) GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "ERROR: could not start GLEW\n");
        glfwTerminate();
        return 1;
    }

    // 3) Depth test для 3D
    glEnable(GL_DEPTH_TEST);

    // 4) Шейдер для модели
    Shader shader("shaders/light.vert", "shaders/light.frag");
    if (shader.ID == 0) {
        fprintf(stderr, "ERROR: shader program not created\n");
        glfwTerminate();
        return 1;
    }

    // 5) Загружаем модель 

    // тестовая модель: куб-заглушка в папке assets
    // Model model("../assets/cube_zaglushka/Cube.obj");

    // своя модель из Blender
    Model model("../assets/blender_model_M20C/Lab3_V10_M20C.obj");

    // Проверка: вывести индексы и имена мешей
    model.PrintMeshInfo();

    // Ищем индексы мешей по их именам из экспортированной модели
    int beamIndex = model.FindMeshIndexByName("Beam_Cube.002");
    int carriageIndex = model.FindMeshIndexByName("Carriage_Cube.007");
    int manipulatorBoxIndex = model.FindMeshIndexByName("ManipulatorBox_Cylinder.015");
    int armsIndex = model.FindMeshIndexByName("Arms_Cylinder.001");

    // Проверка: все ли меши найдены
    if (beamIndex == -1 || carriageIndex == -1 || manipulatorBoxIndex == -1 || armsIndex == -1)
    {
        fprintf(stderr, "ERROR: not all mesh names were found in the model\n");
        glfwTerminate();
        return 1;
    }

    while (!glfwWindowShouldClose(window))
    {
        // Время кадра (для движения с deltaTime)
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Обработка клавиатуры
        processInput(window);

        // Фон (цвет - графит)
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Активируем шейдер с освещением
        shader.use();

        // Матрицы сцены: projection (перспектива) и view (камера)
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Параметры источника света
        shader.setVec3("light.position", lightPos.x, lightPos.y, lightPos.z);
        shader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);   // базовая подсветка, чтобы не было полной темноты
        shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);   // рассеянный свет (зависит от угла к нормали)
        shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);  // блики на "глянцевой" поверхности

        // Параметры материала: как поверхность реагирует на свет
        shader.setVec3("material.ambient", 0.8f, 0.8f, 0.8f);
        shader.setVec3("material.diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("material.specular", 0.9f, 0.9f, 0.9f);  // цвет/сила блика
        shader.setFloat("material.shininess", 16.0f);           // "резкость" блика

        // Позиция камеры: нужна для расчёта бликов (specular)
        shader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

       // Эти матрицы одинаковы для всех частей модели в текущем кадре,
       // поэтому передаём их в шейдер один раз перед отрисовкой всех мешей:
        shader.setMat4("projection", glm::value_ptr(projection));
        shader.setMat4("view", glm::value_ptr(view));

        // Координаты шарнира после перевода осей из Blender в систему OpenGL/OBJ:
        // X' = X, Y' = Z, Z' = -Y
        glm::vec3 hingePoint(0.0f, 1.41064f, 0.370484f);

        // --------------------- Матрицы движения частей -----------------------

        // Балка неподвижна
        glm::mat4 beamMat = glm::mat4(1.0f);

        // 1) Каретка: линейное перемещение вдоль балки
        glm::mat4 carriageMat = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(carriageOffsetX, 0.0f, 0.0f)
        );

        // 2) Бокс манипулятора:
        // вращение вокруг точки шарнира
        glm::mat4 manipulatorRotateMat =
            glm::translate(glm::mat4(1.0f), hingePoint) *
            glm::rotate(
                glm::mat4(1.0f),
                glm::radians(manipulatorAngleDeg),
                glm::vec3(1.0f, 0.0f, 0.0f)
            ) *
            glm::translate(glm::mat4(1.0f), -hingePoint);

        // Бокс наследует движение каретки и свой поворот в шарнире
        glm::mat4 manipulatorBoxMat = carriageMat * manipulatorRotateMat;

        // 3) Руки:
        // сначала наследуют движение каретки и поворот бокса и потом двигаются вверх-вниз
        // в Blender руки двигаются по локальной оси Z,после экспорта эта ось соответствует Y
        glm::mat4 armsLocalMoveMat = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, armsOffsetZ, 0.0f)
        );

        glm::mat4 armsMat = carriageMat * manipulatorRotateMat * armsLocalMoveMat;

        // ------------------------- Отрисовка частей --------------------------

        DrawMeshPart(model, beamIndex, beamMat, shader);
        DrawMeshPart(model, carriageIndex, carriageMat, shader);
        DrawMeshPart(model, manipulatorBoxIndex, manipulatorBoxMat, shader);
        DrawMeshPart(model, armsIndex, armsMat, shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Завершение работы GLFW
    glfwTerminate();
    return 0;
}
