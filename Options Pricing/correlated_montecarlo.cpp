#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    
    double S1 = 100.0, S2 = 100.0;      
    double vol1 = 0.20, vol2 = 0.25;     
    double rho = 0.80;                   
    
    double K = 100.0;                    
    double T = 1.0;                      
    double r = 0.05;                     
    int num_simulations = 5000000; 

    int iterations = num_simulations / 2;
    
    random_device rd;
    mt19937_64 rng(rd()); 
    normal_distribution<double> norm_dist(0.0, 1.0);
    
    double drift1 = (r - 0.5 * vol1 * vol1) * T;
    double drift2 = (r - 0.5 * vol2 * vol2) * T;
    double v1_T = vol1 * sqrt(T);
    double v2_T = vol2 * sqrt(T);
    double discount_factor = exp(-r * T);
    
    // Cholesky Decomposition matrix to enforce correlation
    double chol21 = rho;
    double chol22 = sqrt(1.0 - rho * rho);
    
    double payoff_sum = 0.0;
    
    for(int i = 0; i < iterations; i++) {
        // Generate two independent random stocks
        double Z1 = norm_dist(rng);
        double Z2 = norm_dist(rng);
        // Make them correlated
        double W1_pos = Z1;
        double W2_pos = chol21 * Z1 + chol22 * Z2;
        // Simulate prices
        double ST1_pos = S1 * exp(drift1 + v1_T * W1_pos);
        double ST2_pos = S2 * exp(drift2 + v2_T * W2_pos);
        // Avg for basket
        double basket_pos = (ST1_pos + ST2_pos) / 2.0;
        double payoff_pos = max(0.0, basket_pos - K);
        
        // Second simulation (opposite)
        double W1_neg = -Z1;
        double W2_neg = chol21 * (-Z1) + chol22 * (-Z2);
        
        double ST1_neg = S1 * exp(drift1 + v1_T * W1_neg);
        double ST2_neg = S2 * exp(drift2 + v2_T * W2_neg);
        
        double basket_neg = (ST1_neg + ST2_neg) / 2.0;
        double payoff_neg = max(0.0, basket_neg - K);
        
        // Average results
        payoff_sum += (payoff_pos + payoff_neg) / 2.0;
    }
    
    // Average everything and bring it back to present value
    double expected_payoff = payoff_sum / num_simulations;
    double basket_call_price = discount_factor * expected_payoff;
    
    cout << fixed << setprecision(4);
    cout << "--- Correlated Basket Monte Carlo ---\n";
    cout << "Asset 1 Volatility : " << vol1 * 100 << "%\n";
    cout << "Asset 2 Volatility : " << vol2 * 100 << "%\n";
    cout << "Covariance (Rho)   : " << rho << "\n";
    cout << "-------------------------------------\n";
    cout << "Fair Basket Price  : $" << basket_call_price << "\n";
    
    return 0;
}