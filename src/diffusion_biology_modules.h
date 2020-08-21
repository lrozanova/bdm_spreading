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

namespace bdm {

// List the extracellular substances
enum Substances { kViral };

// Define displacement behavior:
struct Movement : public BaseBiologyModule {
  BDM_STATELESS_BM_HEADER(Movement, BaseBiologyModule, 1);

  Movement() : BaseBiologyModule(gAllEventIds) {}

  void Run(SimObject* so) override {

    if (auto* cell = dynamic_cast<Cell*>(so)) {
      Double3 step;

      if (cell->GetDiameter() > 27) {
          step = {0.1, 0, 0};
      } else {
          step = {0.02, -0.05, 0};
      }

      cell->UpdatePosition(step);
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
    double amount = 4;
    kDg->IncreaseConcentrationBy(so->GetPosition(), amount);
  }
};

}  // namespace bdm

#endif  // DIFFUSION_BIOLOGY_MODULES_H_
