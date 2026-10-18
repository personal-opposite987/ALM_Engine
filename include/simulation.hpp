
#pragma once

#include "models.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

struct ScenarioResult {
    double surplus;
    double loss;
};

ScenarioResult simulate_scenario(
    const ModelParameters& parameters,
    const std::vector<Cohort>& cohorts,
    std::uint64_t base_seed,
    std::uint64_t scenario_id
);

std::vector<ScenarioResult> run_serial_simulation(
    const ModelParameters& parameters,
    const std::vector<Cohort>& cohorts,
    std::size_t num_scenarios,
    std::uint64_t base_seed
);
