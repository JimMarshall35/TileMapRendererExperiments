#pragma once
#include "Camera2D.h"
#include "BasicTypedefs.h"
struct EditorCameraInitializationSettings {
    float moveSpeed;
};
class EditorCamera :
    public Camera2D
{
public:
    EditorCamera(const EditorCameraInitializationSettings& init);
    void UpdatePosition(Directions direction, float deltaT);
    void StartDrag(double x, double y);
    void StopDrag();
    void OnMouseMove(double lastX, double lastY, int WindowW, int WindowH, float deltaTime);
    void OnUpdate(float deltaTime);

private:
    bool _isBeingDragged = false;
    float _moveSpeed;
    float _maxScalar = 0;
    glm::dvec2 _dragStartScreenPixelCoords;
    glm::dvec2 _cameraMoveVector;
};

