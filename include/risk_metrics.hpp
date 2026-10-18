
#pragma once

#include <vector>

struct RiskMetrics {
    double mean_loss;
    double var_995;
    double expected_shortfall_995;
};

RiskMetrics calculate_risk_metrics(
    std::vector<double> losses
);
