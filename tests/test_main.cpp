
#include "models.hpp"
#include "simulation.hpp"
#include "risk_metrics.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

bool approximately_equal(
    double actual,
    double expected,
    double tolerance = 1e-9
) {
    return std::abs(actual - expected) <= tolerance;
}

int main() {
    // Test 1: Vasicek update with zero random shock.
    ModelParameters p;

    const double next_rate =
        vasicek_step(0.04, p, 0.0);

    const double expected_rate =
        0.04
        + 0.50 * (0.05 - 0.04) / 12.0;

    assert(
        approximately_equal(next_rate, expected_rate)
    );

    // Test 2: Gompertz-Makeham hazard.
    const double hazard = mortality_hazard(0.0, p);

    assert(
        approximately_equal(hazard, 0.00052)
    );

    // Test 3: Known VaR and Expected Shortfall.
    std::vector<double> losses;

    for (int i = 1; i <= 1000; ++i) {
        losses.push_back(static_cast<double>(i));
    }

    const RiskMetrics metrics =
        calculate_risk_metrics(losses);

    assert(approximately_equal(metrics.mean_loss, 500.5));
    assert(approximately_equal(metrics.var_995, 995.0));
    assert(
        approximately_equal(
            metrics.expected_shortfall_995, 998.0
        )
    );

    // Test 4: Reject an empty loss distribution.
    bool empty_rejected = false;

    try {
        calculate_risk_metrics({});
    }
    catch (const std::invalid_argument&) {
        empty_rejected = true;
    }

    assert(empty_rejected);

    // Test 5: Reproducible scenario generation.
    const std::vector<Cohort> cohorts = {
        {100, 35.0, 12000.0, 500000.0}
    };

    const ScenarioResult first =
        simulate_scenario(p, cohorts, 42, 7);

    const ScenarioResult second =
        simulate_scenario(p, cohorts, 42, 7);

    assert(
        approximately_equal(
            first.surplus, second.surplus
        )
    );

    assert(
        approximately_equal(
            first.loss, second.loss
        )
    );

    assert(
        approximately_equal(
            first.loss, -first.surplus
        )
    );

    std::cout << "All tests passed.\n";

    return 0;
}
