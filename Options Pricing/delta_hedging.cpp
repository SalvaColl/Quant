#include <bits/stdc++.h>
using namespace std;

double norm_cdf(double x) {
    return 0.5 * erfc(-x * M_SQRT1_2);
}

// Pricing baseline to calculate upfront premium
double bs_price(double S, double K, double T, double r, double sigma) {
    if (T <= 0.0) return max(0.0, S - K);
    double d1 = (log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
    double d2 = d1 - sigma * sqrt(T);
    return S * norm_cdf(d1) - K * exp(-r * T) * norm_cdf(d2);
}

// Delta calculator for daily rebalancing
double bs_delta(double S, double K, double T, double r, double sigma) {
    if (T <= 0.0) return (S > K) ? 1.0 : 0.0;
    double d1 = (log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
    return norm_cdf(d1); 
}

signed main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    
    double S0 = 100.0;
    double K = 100.0;
    double r = 0.05;
    double sigma = 0.20;
    int days_to_expiry = 30;
    int options_sold = 1000;
    
    int num_simulations = 10000;
    
    random_device rd;
    mt19937_64 rng(rd());
    normal_distribution<double> norm_dist(0.0, 1.0);
    
    double dt = 1.0 / 365.0; 
    double daily_drift = (r - 0.5 * sigma * sigma) * dt;
    double daily_vol = sigma * sqrt(dt);
    
    // Collect premium from the buyer
    double initial_option_price = bs_price(S0, K, days_to_expiry / 365.0, r, sigma);
    double total_premium_collected = initial_option_price * options_sold;
    
    vector<double> pnl_results;
    double sum_pnl = 0.0;
    
    for(int sim = 0; sim < num_simulations; sim++) {
        double S = S0;
        double current_shares = 0.0;
        double bank_balance = total_premium_collected; // Start with the cash we collected
        
        // Simulate the 30-day life of the option
        for (int day = 0; day <= days_to_expiry; day++) {
            double T = (days_to_expiry - day) / 365.0;
            
            // Rebalance Portfolio
            double current_delta = bs_delta(S, K, T, r, sigma);
            double target_shares = current_delta * options_sold;
            double shares_to_trade = target_shares - current_shares;
            double trade_cost = shares_to_trade * S;
            current_shares = target_shares;
            
            // Update Bank
            double daily_interest = bank_balance * (exp(r * dt) - 1.0);
            bank_balance += daily_interest;
            bank_balance -= trade_cost; 
            
            // Market Shock (skip on expiration day)
            if (day < days_to_expiry) {
                double Z = norm_dist(rng);
                S = S * exp(daily_drift + daily_vol * Z);
            }
        }
        
        // Expiration Settlement
        double final_portfolio_value = (current_shares * S) + bank_balance;
        double liability = options_sold * max(0.0, S - K); // What we owe the buyer if ITM
        
        double net_pnl = final_portfolio_value - liability;
        
        pnl_results.push_back(net_pnl);
        sum_pnl += net_pnl;
    }
    
    double mean_pnl = sum_pnl / num_simulations;
    double variance = 0.0, max_loss = 0.0, max_gain = 0.0;
    
    for(double pnl : pnl_results) {
        variance += (pnl - mean_pnl) * (pnl - mean_pnl);
        if(pnl < max_loss) max_loss = pnl;
        if(pnl > max_gain) max_gain = pnl;
    }
    variance /= num_simulations;
    double std_dev = sqrt(variance);
    
    cout << fixed << setprecision(2);
    cout << "--- Institutional Delta Hedging Engine (" << num_simulations << " Paths) ---\n";
    cout << "Total Premium Collected : $" << total_premium_collected << "\n";
    cout << "--------------------------------------------------------\n";
    cout << "Average Net PnL         : $" << mean_pnl << " (Should be near $0.00)\n";
    cout << "Tracking Error (StdDev) : $" << std_dev << " (Cost of discrete daily hedging)\n";
    cout << "Maximum Profit Path     : $" << max_gain << "\n";
    cout << "Maximum Loss Path       : $" << max_loss << "\n";
    
    return 0;
}