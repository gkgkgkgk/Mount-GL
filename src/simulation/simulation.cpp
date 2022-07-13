#include "simulation.hpp"
#include <iostream>

Simulation::Simulation()
{
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            voxels.push_back(new Voxel(glm::vec3(i * 0.1f, 0, j * 0.1f)));
        }
    }
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