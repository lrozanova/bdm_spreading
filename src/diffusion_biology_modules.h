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

#ifndef DIFFUSION_BIOLOGY_MODULES_H_
#define DIFFUSION_BIOLOGY_MODULES_H_

#include "biodynamo.h"
#include <iostream>

namespace bdm {

// List the extracellular substances
enum Substances { kViral, kDroplets };

// Define displacement behavior:
struct Movement : public BaseBiologyModule {
  BDM_STATELESS_BM_HEADER(Movement, BaseBiologyModule, 1);

  Movement() : BaseBiologyModule(gAllEventIds) {}

  static std::mt19937& mt() {
    // initialize once per thread
    thread_local static std::random_device srd;
    thread_local static std::mt19937 smt(srd());
    return smt;
  }

  std::normal_distribution<double> distr = std::normal_distribution<double>(0, 0.1);

  void Run(SimObject* so) override {
    if (auto* cell = dynamic_cast<Cell*>(so)) {
      double mass = cell->GetMass();
      Double3 step = {0, 0, -0.5 * mass};
      double rx = this->distr(mt());
      double ry = this->distr(mt());
      double rz = this->distr(mt());

      Double3 gravity_step;
      Double3 current = cell->GetTractorForce();

      if (cell->GetDiameter() < 1) {
        Double3 g = {0, -0.05, 0};
        gravity_step = current + g;
      } else {
        gravity_step = {0, 0, 0};
      }

      Double3 random_step = {rx, ry, rz};
      Double3 empty = {0, 0, 0};
      Double3 displacement = cell->GetPosition()[1] > -10 ? step + random_step + gravity_step : empty;

//      Double3 position = cell->GetPosition();
//
//      if (position[1] < 30) {
//        displacement = {0, 0, 0};
//      }

      cell->SetTractorForce(gravity_step);
      cell->UpdatePosition(displacement);
    }
  }
};

struct ViralShedding : public BaseBiologyModule {
  BDM_STATELESS_BM_HEADER(ViralShedding, BaseBiologyModule, 1);

  ViralShedding() : BaseBiologyModule() {}

  void Run(SimObject* so) override {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    static auto* kDg = rm->GetDiffusionGrid(kViral);
    static auto* kDgDrop = rm->GetDiffusionGrid(kDroplets);

    double amount = 40;
    double amountDroplets = 20;

    if (auto* cell = dynamic_cast<Cell*>(so)) {
      auto position = so->GetPosition();
      auto vector = cell->GetTractorForce();
      auto displacement = vector * 400;
      auto forwardPoint = position + displacement;

      kDg->IncreaseConcentrationBy(forwardPoint, amount);
      kDgDrop->IncreaseConcentrationBy(forwardPoint, amountDroplets);
      // todo increase concentration in the cone towards the direction
//      std::cout << "Force: " << cell->GetTractorForce() << std::endl;
    }

  }
};

}  // namespace bdm

#endif  // DIFFUSION_BIOLOGY_MODULES_H_
