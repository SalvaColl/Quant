#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    
    // European option
    double S0 = 100.0;       // Current stock price
    double K = 105.0;        // Strike price
    double T = 1.0;          // Time to maturity (1 year)
    double r = 0.05;         // Risk-free interest rate
    double sigma = 0.20;     // Implied volatility
    int num_simulations = 5000000; 
    
    random_device rd;
    mt19937_64 rng(rd()); 
    normal_distribution<double> norm_dist(0.0, 1.0);
    
    double drift = (r - 0.5 * sigma * sigma) * T;
    double vol = sigma * sqrt(T);
    double discount_factor = exp(-r * T);
    
    double sum_call_payoffs = 0.0;
    double sum_call_payoffs_squared = 0.0;
    double sum_put_payoffs = 0.0;

    int iterations = num_simulations / 2;
    
    for(int i = 0; i < iterations; i++) {
        double Z = norm_dist(rng);
        
        double S_T1 = S0 * exp(drift + vol * Z);
        double call_payoff1 = max(0.0, S_T1 - K);
        double put_payoff1 = max(0.0, K - S_T1);
        
        double S_T2 = S0 * exp(drift + vol * -Z);
        double call_payoff2 = max(0.0, S_T2 - K);
        double put_payoff2 = max(0.0, K - S_T2);

        double avg_call_payoff = (call_payoff1 + call_payoff2) / 2.0;
        double avg_put_payoff = (put_payoff1 + put_payoff2) / 2.0;
        
        sum_call_payoffs += avg_call_payoff;
        sum_call_payoffs_squared += (avg_call_payoff * avg_call_payoff);
        sum_put_payoffs += avg_put_payoff;
    }
    
    double expected_call = sum_call_payoffs / iterations;
    double expected_put = sum_put_payoffs / iterations;
    
    double call_price = discount_factor * expected_call;
    double put_price = discount_factor * expected_put;

    double variance = (sum_call_payoffs_squared / iterations) - (expected_call * expected_call);
    double standard_error = discount_factor * sqrt(variance / iterations);
    
    cout << fixed << setprecision(4);
    cout << "--- Institutional Monte Carlo Engine ---\n";
    cout << "Paths Simulated  : " << num_simulations << " (Antithetic Pairs)\n";
    cout << "Underlying Price : $" << S0 << "\n";
    cout << "Strike Price     : $" << K << "\n";
    cout << "----------------------------------------\n";
    cout << "Fair Call Price  : $" << call_price << "  (+-$" << standard_error * 1.96 << " at 95% confidence)\n";
    cout << "Fair Put Price   : $" << put_price << "\n";
    cout << "Standard Error   :  " << standard_error << "\n";
    
    return 0;
}