#ifndef DIFFUSION_BIOLOGY_MODULES_H_
#define DIFFUSION_BIOLOGY_MODULES_H_

#include <iostream>
#include "biodynamo.h"
#include "turbulence.h"

namespace bdm {

// List the extracellular substances
enum Substances { kViral, kDroplets };

// Define displacement behavior:
struct Movement : public BaseBiologyModule {
  BDM_STATELESS_BM_HEADER(Movement, BaseBiologyModule, 1);

  Movement() : BaseBiologyModule(gAllEventIds) {}

  void Run(SimObject* so) override {
    if (auto* cell = dynamic_cast<Cell*>(so)) {

      auto* sim = Simulation::GetActive();
      double time = static_cast<double>(sim->GetScheduler()->GetSimulatedSteps());

      double mass = cell->GetMass();
      Double3 step = {0, 0, -0.5 * mass};

      Double3 gravity_step;
      Double3 current = cell->GetTractorForce();

      if (cell->GetDiameter() < 1) {
        Double3 g = {0, -0.05, 0};
        gravity_step = current + g;
      } else {
        gravity_step = {0, 0, 0};
      }

      Double3 turbulence_step = turbulence_force
          cell->GetPosition(), cell->GetMass(), time);

      Double3 empty = {0, 0, 0};

      Double3 displacement = cell->GetPosition()[1] > -10
                                 ? step + random_step + gravity_step + turbulence_step
                                 : empty;

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
