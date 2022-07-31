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
private:
    float _moveSpeed;
};

