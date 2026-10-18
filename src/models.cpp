
#include "models.hpp"

#include <cmath>

namespace {
constexpr double DT = 1.0 / 12.0;
}

double vasicek_step(
    double rate,
    const ModelParameters& parameters,
    double normal_sample
) {
    return rate
        + parameters.mean_reversion
            * (parameters.long_run_rate - rate) * DT
        + parameters.volatility
            * std::sqrt(DT) * normal_sample;
}

double mortality_hazard(
    double age,
    const ModelParameters& parameters
) {
    return parameters.alpha
        + parameters.beta * std::pow(parameters.c, age);
}
