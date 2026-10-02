#include <bits/stdc++.h>
using namespace std;

struct Row {
    string date;
    string pair_id;
    double price_y;
    double price_x;
    double z_score;
};

struct PairState {
    double pos_y = 0;
    double pos_x = 0;
    int tranches_long = 0;
    int tranches_short = 0;
    bool is_banned = false;
};

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    ifstream file("pairs_signals.csv");
    string line;
    getline(file, line);

    vector<Row> data;
    while (getline(file, line)) {
        stringstream ss(line);
        string date, pair_id, y_str, x_str, z_str;
        getline(ss, date, ','); getline(ss, pair_id, ',');
        getline(ss, y_str, ','); getline(ss, x_str, ','); getline(ss, z_str, ',');
        data.push_back({date, pair_id, stod(y_str), stod(x_str), stod(z_str)});
    }

    double initial_capital = 100000.0;
    double cash = initial_capital;
    unordered_map<string, PairState> portfolio; 
    
    double tranche_percent = 0.05; 
    double slippage_bps = 5.0 / 10000.0; 

    string current_date = data.empty() ? "" : data[0].date;
    double last_day_equity = initial_capital;
    vector<double> daily_returns;
    double peak_equity = initial_capital;
    double max_drawdown = 0.0;
    int winning_trades = 0;
    int total_trades = 0;
    int stop_losses_hit = 0;

    for (const auto& row : data) {
        PairState& state = portfolio[row.pair_id];
        
        if (state.is_banned) continue; 

        double total_equity = cash;
        for (const auto& [pid, p_state] : portfolio) {
            total_equity += (p_state.pos_y * row.price_y + p_state.pos_x * row.price_x);
        }

        if (row.date != current_date) {
            double daily_ret = (total_equity - last_day_equity) / last_day_equity;
            daily_returns.push_back(daily_ret);
            last_day_equity = total_equity;
            current_date = row.date;
            
            if (total_equity > peak_equity) peak_equity = total_equity;
            double drawdown = (peak_equity - total_equity) / peak_equity;
            if (drawdown > max_drawdown) max_drawdown = drawdown;
        }
        
        double current_tranche_exposure = total_equity * tranche_percent;

        bool mean_reverted = (state.tranches_long > 0 && row.z_score >= 0.0) || 
                             (state.tranches_short > 0 && row.z_score <= 0.0);
                             
        bool catastrophe = (state.tranches_long > 0 && row.z_score < -4.0) || 
                           (state.tranches_short > 0 && row.z_score > 4.0);

        if (mean_reverted || catastrophe) {
            double gross_pnl = (state.pos_y * row.price_y + state.pos_x * row.price_x);
            double slip_cost = (abs(state.pos_y * row.price_y) + abs(state.pos_x * row.price_x)) * slippage_bps;
            
            double net_pnl = gross_pnl - slip_cost;
            cash += net_pnl;
            
            total_trades++;
            if (net_pnl > 0) winning_trades++;
            if (catastrophe) {
                stop_losses_hit++;
                state.is_banned = true; 
            }

            state.pos_y = 0; state.pos_x = 0;
            state.tranches_long = 0; state.tranches_short = 0;
        }

        if (state.is_banned) continue;

        auto execute_tranche = [&](int sign_y, int sign_x, int& tranche_counter) {
            double shares_y = (sign_y * current_tranche_exposure) / row.price_y;
            double shares_x = (sign_x * current_tranche_exposure) / row.price_x;
            
            double slip_cost = (abs(shares_y * row.price_y) + abs(shares_x * row.price_x)) * slippage_bps;
            
            state.pos_y += shares_y; 
            state.pos_x += shares_x;
            cash -= slip_cost; 
            
            tranche_counter++;
        };

        if (state.tranches_short == 0) {
            if (row.z_score < -2.0 && state.tranches_long == 0) execute_tranche(1, -1, state.tranches_long);
            else if (row.z_score < -2.5 && state.tranches_long == 1) execute_tranche(1, -1, state.tranches_long);
            else if (row.z_score < -3.0 && state.tranches_long == 2) execute_tranche(1, -1, state.tranches_long);
        }

        if (state.tranches_long == 0) {
            if (row.z_score > 2.0 && state.tranches_short == 0) execute_tranche(-1, 1, state.tranches_short);
            else if (row.z_score > 2.5 && state.tranches_short == 1) execute_tranche(-1, 1, state.tranches_short);
            else if (row.z_score > 3.0 && state.tranches_short == 2) execute_tranche(-1, 1, state.tranches_short);
        }
    }

    for (const auto& [pid, state] : portfolio) {
        if (state.tranches_long > 0 || state.tranches_short > 0) {
            double last_y = 0, last_x = 0;
            for (auto it = data.rbegin(); it != data.rend(); ++it) {
                if (it->pair_id == pid) {
                    last_y = it->price_y; last_x = it->price_x; break;
                }
            }
            double slip_cost = (abs(state.pos_y * last_y) + abs(state.pos_x * last_x)) * slippage_bps;
            cash += (state.pos_y * last_y + state.pos_x * last_x) - slip_cost;
        }
    }

    double mean_return = 0.0, sum_sq = 0.0;
    for (double r : daily_returns) mean_return += r;
    if (!daily_returns.empty()) mean_return /= daily_returns.size();
    
    for (double r : daily_returns) sum_sq += (r - mean_return) * (r - mean_return);
    double std_dev = daily_returns.size() > 1 ? sqrt(sum_sq / (daily_returns.size() - 1)) : 1.0;
    
    double sharpe_ratio = (std_dev == 0) ? 0 : (mean_return / std_dev) * sqrt(252);
    double win_rate = total_trades > 0 ? (double)winning_trades / total_trades * 100.0 : 0.0;

    cout << fixed << setprecision(2);
    cout << "Final Capital   : $" << cash << "\n";
    cout << "Net Profit      : $" << cash - initial_capital << "\n";
    cout << "Total Return    :  " << ((cash - initial_capital) / initial_capital) * 100.0 << "%\n";
    cout << "Max Drawdown    :  " << max_drawdown * 100.0 << "%\n";
    cout << "Sharpe Ratio    :  " << sharpe_ratio << "\n";
    cout << "Win Rate        :  " << win_rate << "% (" << total_trades << " trades)\n";
    cout << "Stop-Losses Hit :  " << stop_losses_hit << " pairs banned\n";

    return 0;
}