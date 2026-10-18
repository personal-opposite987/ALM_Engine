
#include "risk_metrics.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

RiskMetrics calculate_risk_metrics(
    std::vector<double> losses
) {
    if (losses.empty()) {
        throw std::invalid_argument(
            "Loss distribution cannot be empty."
        );
    }

    for (double loss : losses) {
        if (!std::isfinite(loss)) {
            throw std::invalid_argument(
                "Losses must be finite numbers."
            );
        }
    }

    const std::size_t n = losses.size();

    double sum = 0.0;

    for (double loss : losses) {
        sum += loss;
    }

    const double mean_loss = sum / n;

    std::sort(losses.begin(), losses.end());

    // Empirical nearest-rank VaR at 99.5%.
    const std::size_t var_rank =
        static_cast<std::size_t>(
            std::ceil(0.995 * n)
        );

    const double var_995 = losses[var_rank - 1];

    // Average the worst ceil(0.5% of scenarios).
    const std::size_t tail_count =
        std::max<std::size_t>(
            1,
            static_cast<std::size_t>(
                std::ceil(0.005 * n)
            )
        );

    double tail_sum = 0.0;

    for (std::size_t i = n - tail_count; i < n; ++i) {
        tail_sum += losses[i];
    }

    const double expected_shortfall_995 =
        tail_sum / tail_count;

    return {
        mean_loss,
        var_995,
        expected_shortfall_995
    };
}
