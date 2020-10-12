#ifndef DIFFUSION_TURBULENCE_H
#define DIFFUSION_TURBULENCE_H

namespace bdm {

static std::mt19937& mt() {
  // initialize once per thread
  thread_local static std::random_device srd;
  thread_local static std::mt19937 smt(srd());
  return smt;
}

static std::normal_distribution<double> distr =
    std::normal_distribution<double>(0, 0.1);

Double3 turbulence_force(Double3 coords, double mass, double time) {

  // Replace with the CFD code calculating the turbulence

  double rx = this->distr(mt());
  double ry = this->distr(mt());
  double rz = this->distr(mt());

  return {rx, ry, rz};
}

}  // namespace bdm

#endif  // DIFFUSION_TURBULENCE_H
