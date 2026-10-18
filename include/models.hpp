
#pragma once

struct ModelParameters {
    double initial_rate = 0.04;
    double mean_reversion = 0.50;
    double long_run_rate = 0.05;
    double volatility = 0.02;

    // Annual Gompertz-Makeham hazard parameters.
    double alpha = 0.0005;
    double beta = 0.00002;
    double c = 1.09;
};

struct Cohort {
    int initial_policies;
    double initial_age;
    double annual_premium_per_policy;
    double death_benefit_per_policy;
};

double vasicek_step(
    double rate,
    const ModelParameters& parameters,
    double normal_sample
);

double mortality_hazard(
    double age,
    const ModelParameters& parameters
);
