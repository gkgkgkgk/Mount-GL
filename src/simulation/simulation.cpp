#include "simulation.hpp"
#include <iostream>

Simulation::Simulation()
{
    voxels.push_back(new Voxel(glm::vec3(1, 0, 0)));
}

Simulation::~Simulation()
{
    for (int i = 0; i < Simulation::voxels.size(); i++)
    {
        delete voxels.at(i);
    }
}

std::vector<Voxel *> *Simulation::GetVoxels()
{
    return &voxels;
}

void Simulation::tick(double deltaTime)
{
}