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

#include <random>
#include <vector>
#include <cmath>

#include "biodynamo.h"
#include "diffusion_biology_modules.h"

namespace bdm {

static std::mt19937& mt() {
  // initialize once per thread
  thread_local static std::random_device srd;
  thread_local static std::mt19937 smt(srd());
  return smt;
}

inline int Simulate(int argc, const char** argv) {
  auto set_param = [](Param* param) {
    // Create an artificial bounds for the simulation space
    param->bound_space_ = true;
    param->min_bound_ = -600;
    param->max_bound_ = 600;
  };  // todo can be moved to config file?

  Simulation simulation(argc, argv, set_param);

  /*
    auto construct = [](const Double3& position) {
      int i
      for (i=1;i<=1000;i++)
        Cell* cell = new Cell(position);
      cell->SetDiameter(1);
      cell->SetMass(0.01);

    }
  */

  auto construct = [](const Double3& position) {
    Cell* cell = new Cell(position);
    cell->SetDiameter(0.5);
    cell->SetMass(0.2);
    cell->AddBiologyModule(new Movement());
//    cell->AddBiologyModule(new ViralShedding());

    Double3 position1 = {0, 0, 0};

    if (position == position1) {
      cell->SetDiameter(10);
      cell->SetMass(1);
    }

    return cell;
  };

  std::vector<Double3> positions;
  positions.push_back({0, 0, 0});

  std::uniform_real_distribution<double> distr(0, 1);
  std::lognormal_distribution<double> distr_r(0, 2);
  std::normal_distribution<double> distr_angle(0, 0.05);

  for (int i = 0; i < 10000; i++) {
    double u = distr_angle(mt());
    double v = distr_angle(mt());

    double theta = u * 2 * M_PI;
    double phi = acos(2.0 * v - 1.0);

    double r = 50 * cbrt(distr_r(mt()));

    double x = r * sin(phi) * cos(theta);
    double y = r * sin(phi) * sin(theta);
    double z = r * cos(phi);

    positions.push_back({x, y, z});
  }

  ModelInitializer::CreateCells(positions, construct);

  // Define the substances that cells may secrete
  ModelInitializer::DefineSubstance(kViral, "SARS-CoV-2", 0.4, 0, 50);
  ModelInitializer::DefineSubstance(kDroplets, "SARS-CoV-2-droplets", 0.5, 0.2, 50);

  // Run simulation for N timesteps
  simulation.GetScheduler()->Simulate(200);
  std::cout << "Simulation completed successfully!\n";
  return 0;
}

}  // namespace bdm

#endif  // DEMO_DIFFUSION_MODULE_H_
