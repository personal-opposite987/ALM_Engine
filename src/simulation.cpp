/* each scenario gets a base seed, becuase we want to combine this seed with
 with the scenario number or id and get a unique id cuz scenario number will
 not be able to identify uniquely as the scenarios will be distributed among
 gpus */

 
#include "simulation.hpp"

#include <cmath>
#include <random>
#include <stdexcept>

namespace {
constexpr int MONTHS = 360;
constexpr double DT = 1.0 / 12.0;

// Produce a deterministic seed for each scenario.
// This allows scenarios to be computed independently.
std::uint64_t scenario_seed(
    std::uint64_t base_seed,
    std::uint64_t scenario_id
) {
    std::uint64_t z =
        base_seed + scenario_id
        + 0x9E3779B97F4A7C15ULL;

    z = (z ^ (z >> 30))
        * 0xBF58476D1CE4E5B9ULL;

    z = (z ^ (z >> 27))
        * 0x94D049BB133111EBULL;

    return z ^ (z >> 31);
}
}

ScenarioResult simulate_scenario(
    const ModelParameters& parameters,
    const std::vector<Cohort>& cohorts,
    std::uint64_t base_seed,
    std::uint64_t scenario_id
) {
    if (parameters.volatility < 0.0) {
        throw std::invalid_argument(
            "Volatility cannot be negative."
        );
    }

    std::mt19937_64 rng(
        scenario_seed(base_seed, scenario_id)
    );

    std::normal_distribution<double> normal(0.0, 1.0);

    double rate = parameters.initial_rate;
    double cumulative_rate = 0.0;
    double surplus = 0.0;

    std::vector<double> survival(cohorts.size(), 1.0);

    for (int month = 0; month < MONTHS; ++month) {
        rate = vasicek_step(
            rate, parameters, normal(rng)
        );

        cumulative_rate += rate * DT;
        const double discount_factor =
            std::exp(-cumulative_rate);

        double premium_cash_flow = 0.0;
        double claim_cash_flow = 0.0;

        for (std::size_t j = 0; j < cohorts.size(); ++j) {
            const Cohort& cohort = cohorts[j];

            const double age =
                cohort.initial_age + month / 12.0;

            const double hazard =
                mortality_hazard(age, parameters);

            const double death_probability =
                1.0 - std::exp(-hazard * DT);

            const double expected_alive =
                cohort.initial_policies * survival[j];

            const double expected_deaths =
                expected_alive * death_probability;

            premium_cash_flow +=
                expected_alive
                * cohort.annual_premium_per_policy / 12.0;

            claim_cash_flow +=
                expected_deaths
                * cohort.death_benefit_per_policy;

            survival[j] *= 1.0 - death_probability;
        }

        surplus +=
            (premium_cash_flow - claim_cash_flow)
            * discount_factor;
    }

    return {surplus, -surplus};
}

std::vector<ScenarioResult> run_serial_simulation(
    const ModelParameters& parameters,
    const std::vector<Cohort>& cohorts,
    std::size_t num_scenarios,
    std::uint64_t base_seed
) {
    if (num_scenarios == 0) {
        throw std::invalid_argument(
            "Scenario count must be positive."
        );
    }

    std::vector<ScenarioResult> results;
    results.reserve(num_scenarios);

    for (std::size_t s = 0; s < num_scenarios; ++s) {
        results.push_back(
            simulate_scenario(
                parameters, cohorts, base_seed,
                static_cast<std::uint64_t>(s)
            )
        );
    }

    return results;
}
