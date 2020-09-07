// -----------------------------------------------------------------------------
//
// Copyright (C) The BioDynaMo Project.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// See the LICENSE file distributed with this work for details.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership.
//
// -----------------------------------------------------------------------------
//
// This model creates 8 cells at each corner of a cube, and one in the middle.
// The cell in the middle secretes a substance. The cells are modeled to move
// according to the extracellular gradient; in this case to the middle.
//

#ifndef DEMO_DIFFUSION_MODULE_H_
#define DEMO_DIFFUSION_MODULE_H_

#include <vector>

#include "biodynamo.h"
#include "diffusion_biology_modules.h"

namespace bdm {

inline int Simulate(int argc, const char** argv) {
  auto set_param = [](Param* param) {
    // Create an artificial bounds for the simulation space
    param->bound_space_ = true;
    param->min_bound_ = -600;
    param->max_bound_ = 600;
  };  // todo can be moved to config file?

  Simulation simulation(argc, argv, set_param);

  auto construct = [](const Double3& position) {
    Cell* cell = new Cell(position);
    cell->SetDiameter(30);
    cell->SetMass(1.0);
    cell->AddBiologyModule(new Movement());
    cell->AddBiologyModule(new ViralShedding());

    Double3 position1 = {0, 0, 0};

    if (!(position == position1)) {
      cell->SetDiameter(25);  // todo more sane way to discriminate
    }

    return cell;
  };
  std::vector<Double3> positions;

  positions.push_back({100, 100, 0});

  // The cell responsible for secretion
  positions.push_back({0, 0, 0});
  ModelInitializer::CreateCells(positions, construct);

  // Define the substances that cells may secrete
  ModelInitializer::DefineSubstance(kViral, "SARS-CoV-2", 0.4, 0, 50);

  // Run simulation for N timesteps
  simulation.GetScheduler()->Simulate(3000);
  std::cout << "Simulation completed successfully!\n";
  return 0;
}

}  // namespace bdm

#endif  // DEMO_DIFFUSION_MODULE_H_
