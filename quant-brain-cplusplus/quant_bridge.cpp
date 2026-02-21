#include "pch.h"

extern "C" __declspec(dllexport)
double __stdcall MonteCarloRisk(double meanReturn, double stdDev, int nTrades)
{
    if (nTrades <= 0) return 0.01;

    double risk = meanReturn / (stdDev + 1e-6);
    double lot = risk * 0.1;   // simple placeholder logic

    if (lot < 0.01) lot = 0.01;
    if (lot > 5.0) lot = 5.0;

    return lot;
}
