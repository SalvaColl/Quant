#include <bits/stdc++.h>
using namespace std;

// Intrinsic value of a call option
long double payoff(long double s, long double k) {
    return max(0.0L, s - k);
}

long double price(long double s0, long double sigma, long double r, long double k, long double expiry, int steps, int type, int see) {
    // Matrices to store the value of the action (s) and option (v) at each step
    vector<vector<long double>> s(steps + 1, vector<long double> (steps + 1));
    vector<vector<long double>> v(steps + 1, vector<long double> (steps + 1));
    s[0][0] = s0;
    
    long double dt = expiry / steps; // Time delta
    long double df = exp(-r * dt);   // Discount factor to bring the future money to present value
    long double temp1 = exp((r + sigma * sigma) * dt); 
    long double temp2 = 0.5 * (df + temp1);

    // Magnitude of stock price up and down movements
    long double u = temp2 + sqrt(temp2 * temp2 - 1.0);
    long double d = 1.0 / u;

    // Risk neutral probability of the stock going up
    // The probability we need so that the stock's return is the same as a loan interest rate 
    // to avoid arbitrage
    // Real-world growth is irrelevant to price the option
    long double p = (exp(r * dt) - d) / (u - d);
    
    for (int i = 0; i <= steps; i++){
        // For each step the price drops at the lower nodes
        if (i) s[i][i] = s[i - 1][i - 1] * d;
        
        // And increases at the upper nodes
        for (int j = i + 1; j <= steps; j++){
            s[i][j] = s[i][j - 1] * u;
        }
    }
    
    // Calculate the option value at expiration date
    for (int i = 0; i <= steps; i++){
        v[i][steps] = payoff(s[i][steps], k);
    }
    
    if(!see) {
        cout << "Stock price tree: " << endl;
        for (int i = 0; i <= steps; i++){
            for (int j = i; j <= steps; j++){
                cout << s[i][j] << ' ';
            }
            cout << endl;
        }
        cout << endl;
    }
    
    // Move backwards to calculate the option price today
    for (int i = steps - 1; i >= 0; i--){
        for (int j = 0; j <= i; j++){
            if(!type) {
                // European option: Discount expected value from the next stop
                v[j][i] = df * (p * v[j][i + 1] + (1 - p) * v[j + 1][i + 1]);
            }
            else {
                // American option: Compare discounted value with the payoff of exercising today
                v[j][i] = max(df * (p * v[j][i+1] + (1 - p) * v[j + 1][i + 1]), payoff(s[j][i], k));
            }
        }
    }

    if(!see) {
        cout << "Option price tree: " << endl;
        for (int i = 0; i <= steps; i++){
            for (int j = i; j <= steps; j++){
                cout << v[i][j] << ' ';
            }
            cout << endl;
        }
        cout << endl;
    }

    // Fair option price today
    return v[0][0];
}

int main() {
    long double s0;
    long double sigma;
    long double r;
    long double k;
    long double expiry;
    int steps;
    int type;
    int see;
    
    cout << "Enter" << endl;
    cout << "Stock price: ";
    cin >> s0;
    cout << "Volatility: ";
    cin >> sigma;
    cout << "Risk-free interest rate: ";
    cin >> r;
    cout << "Strike: ";
    cin >> k;
    cout << "Nº of periods to expiry: ";
    cin >> expiry;
    cout << "Nº of steps: ";
    cin >> steps;
    cout << "0 for European options, 1 for American options: ";
    cin >> type;
    cout << "0 to see the binomial tree, 1 otherwise: ";
    cin >> see;
    cout << endl;
    
    long double optionPrice = price(s0, sigma, r, k , expiry, steps, type, see);
    cout << "Option price: " << optionPrice << endl;
}