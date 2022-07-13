#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "../rendering/render_model.h"
#include "voxel.hpp"

#define MAX_OCCLUDERS 4

class Simulation
{
private:
    std::vector<Voxel *> voxels;

public:
    Simulation();
    ~Simulation();

    std::vector<Voxel *> *GetVoxels();
    void tick(double deltaTime);
};
