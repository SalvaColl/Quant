#include <bits/stdc++.h>
using namespace std;

// Standard Normal cumulative distribution function
double norm_cdf(double x) {
    return 0.5 * erfc(-x * M_SQRT1_2);
}

// Black-Scholes Pricing Formula (for a fast baseline)
double bs_price(double S, double K, double T, double r, double sigma, bool is_call) {
    double d1 = (log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
    double d2 = d1 - sigma * sqrt(T);
    
    if (is_call) {
        return S * norm_cdf(d1) - K * exp(-r * T) * norm_cdf(d2);
    } else {
        return K * exp(-r * T) * norm_cdf(-d2) - S * norm_cdf(-d1);
    }
}

signed main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    
    // Contract Parameters
    double S = 100.0;           // Stock Price
    double K = 100.0;           // Strike Price
    double T = 30.0 / 365.0;    // 30 days to expiration
    double r = 0.05;            // 5% risk-free rate
    double sigma = 0.20;        // 20% implied volatility
    bool is_call = true;
    
    // Calculate Base Price
    double base_price = bs_price(S, K, T, r, sigma, is_call);
    
    // Finite Difference Bump Sizes
    double dS = 0.01;       // 1 cent bump in stock price
    double dVol = 0.001;    // 0.1% bump in volatility
    double dT = 1.0 / 365.0;// 1 day bump in time
    double dR = 0.0001;     // 1 basis point bump in rate
    
    // DELTA: First derivative with respect to Price
    double price_up_S = bs_price(S + dS, K, T, r, sigma, is_call);
    double price_down_S = bs_price(S - dS, K, T, r, sigma, is_call);
    double delta = (price_up_S - price_down_S) / (2.0 * dS);
    
    // GAMMA: Second derivative with respect to Price
    double gamma = (price_up_S - 2.0 * base_price + price_down_S) / (dS * dS);
    
    // VEGA: First derivative with respect to Volatility (scaled to 1%)
    double price_up_vol = bs_price(S, K, T, r, sigma + dVol, is_call);
    double price_down_vol = bs_price(S, K, T, r, sigma - dVol, is_call);
    double vega = ((price_up_vol - price_down_vol) / (2.0 * dVol)) / 100.0; 
    
    // THETA: 1-day time decay (Forward difference because time only moves forward)
    double price_down_T = bs_price(S, K, T - dT, r, sigma, is_call);
    double theta = (price_down_T - base_price); 
    
    // RHO: First derivative with respect to Interest Rate (scaled to 1%)
    double price_up_R = bs_price(S, K, T, r + dR, sigma, is_call);
    double price_down_R = bs_price(S, K, T, r - dR, sigma, is_call);
    double rho = ((price_up_R - price_down_R) / (2.0 * dR)) / 100.0;
    
    // Results
    cout << fixed << setprecision(4);
    cout << "--- Option Pricing & Greeks (Finite Difference) ---\n";
    cout << "Contract         : 30-Day Call (Strike: $" << K << ")\n";
    cout << "Fair Price       : $" << base_price << "\n";
    cout << "---------------------------------------------------\n";
    cout << "Delta (Direction):  " << delta << "  (Gains $" << delta << " per $1 stock rise)\n";
    cout << "Gamma (Accel.)   :  " << gamma << "  (Delta increases by " << gamma << " per $1 rise)\n";
    cout << "Theta (Time)     : " << theta << " (Bleeds $" << abs(theta) << " in value tomorrow)\n";
    cout << "Vega  (Vol)      :  " << vega << "  (Gains $" << vega << " per 1% vol spike)\n";
    cout << "Rho   (Rates)    :  " << rho << "  (Gains $" << rho << " per 1% rate hike)\n";
    
    return 0;
}