
#include "models.hpp"
#include "simulation.hpp"
#include "risk_metrics.hpp"

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    try {
        std::size_t num_scenarios = 1000;
        std::uint64_t seed = 42;

        if (argc >= 2) {
            num_scenarios = std::stoull(argv[1]);
        }

        if (argc >= 3) {
            seed = std::stoull(argv[2]);
        }

        if (num_scenarios == 0) {
            throw std::invalid_argument(
                "Scenario count must be positive."
            );
        }

        const ModelParameters parameters;

        const std::vector<Cohort> cohorts = {
            {1000, 35.0, 12000.0, 500000.0},
            {800,  50.0, 18000.0, 400000.0},
            {500,  65.0, 24000.0, 300000.0}
        };

        const auto start =
            std::chrono::steady_clock::now();

        const std::vector<ScenarioResult> results =
            run_serial_simulation(
                parameters,
                cohorts,
                num_scenarios,
                seed
            );

        const auto end =
            std::chrono::steady_clock::now();

        const double execution_time =
            std::chrono::duration<double>(
                end - start
            ).count();

        std::vector<double> losses;
        losses.reserve(results.size());

        for (const ScenarioResult& result : results) {
            losses.push_back(result.loss);
        }

        const RiskMetrics metrics =
            calculate_risk_metrics(losses);

        std::ofstream csv("results/losses.csv");

        if (!csv) {
            throw std::runtime_error(
                "Cannot open results/losses.csv. "
                "Run the program from the project root."
            );
        }

        csv << "scenario,surplus,loss\n";
        csv << std::setprecision(12);

        for (std::size_t i = 0; i < results.size(); ++i) {
            csv << (i + 1) << ','
                << results[i].surplus << ','
                << results[i].loss << '\n';
        }

        csv.close();

        std::cout << std::fixed << std::setprecision(6);

        std::cout << "mode=Serial\n";
        std::cout << "scenarios=" << num_scenarios << '\n';
        std::cout << "months=360\n";
        std::cout << "seed=" << seed << '\n';

        std::cout << "execution_time_seconds="
                  << execution_time << '\n';

        std::cout << "mean_loss="
                  << metrics.mean_loss << '\n';

        std::cout << "var_995="
                  << metrics.var_995 << '\n';

        std::cout << "expected_shortfall_995="
                  << metrics.expected_shortfall_995 << '\n';

        std::cout << "results_file=results/losses.csv\n";

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
