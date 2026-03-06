#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// Матрица для корректного преобразования нормалей
uniform mat3 normalMatrix;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);

// Позиция вершины в мировых координатах (нужна для расчёта света во фрагментном шейдере)
FragPos = worldPos.xyz;

// Нормаль после преобразования (normalMatrix нужна при масштабировании/поворотах модели)
Normal = normalMatrix * aNormal;

// Итоговая позиция в пространстве после проекции (clip space)
gl_Position = projection * view * worldPos;

}
