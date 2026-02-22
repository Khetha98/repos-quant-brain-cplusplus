#include "pch.h"   // MUST BE FIRST
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>

extern "C" __declspec(dllexport)
int MonteCarlo_Drawdown(double* returns, int n, double* out_max_dd_p95)
{
    if (n < 10) return 0;

    std::vector<double> sims;
    std::mt19937 rng(42);

    for (int k = 0; k < 1000; k++)
    {
        std::vector<double> shuffled(returns, returns + n);
        std::shuffle(shuffled.begin(), shuffled.end(), rng);

        double equity = 0;
        double peak = 0;
        double worst_dd = 0;

        for (double r : shuffled)
        {
            equity += r;
            peak = std::max(peak, equity);
            worst_dd = std::min(worst_dd, equity - peak);
        }

        sims.push_back(worst_dd);
    }

    std::sort(sims.begin(), sims.end());
    int idx = (int)(0.95 * sims.size());
    *out_max_dd_p95 = sims[idx];

    return 1;
}

extern "C" __declspec(dllexport)
double WalkForward_Score(double* returns, int n)
{
    if (n < 5) return 0;

    int window = n / 5;
    double total = 0;

    for (int i = 0; i < 4; i++)
    {
        double mu = 0, sigma = 0;
        for (int j = i * window; j < (i + 1) * window; j++)
            mu += returns[j];

        mu /= window;

        for (int j = (i + 1) * window; j < (i + 2) * window; j++)
            sigma += pow(returns[j] - mu, 2);

        sigma = sqrt(sigma / window + 1e-6);
        total += mu / sigma;
    }

    return total / 4.0;
}

extern "C" __declspec(dllexport)
double Regime_PositionSize(int regime, double balance, double drawdown)
{
    double risk = 0.01;

    if (regime == 0) risk *= 1.2;      // RANGE
    else if (regime == 1) risk *= 1.5; // TREND
    else if (regime == 2) risk *= 0.5; // VOLATILE

    if (drawdown > 0.05 * balance)
        risk *= 0.5;

    double lot = balance * risk / 1000.0;
    return std::max(lot, 0.01);
}

// ✅ Wrap this in a function
extern "C" __declspec(dllexport)
void FillReturnsAndCalculateDrawdown(double* out_returns, int* out_count, double* out_max_dd_p95)
{
    int count = 0;

    // You cannot call MQL5-specific functions like HistoryDealsTotal() here
    // This would need to be called from MQL5 side and pass the returns array to DLL
    // Example assumes returns are already filled by MQL5

    // Just for demonstration: simulate some returns
    for (int i = 0; i < 50; i++)
    {
        out_returns[i] = (rand() % 2000 - 1000) / 10000.0; // random -0.1..0.1
        count++;
    }

    *out_count = count;

    MonteCarlo_Drawdown(out_returns, count, out_max_dd_p95);
}