import yfinance as yf
import pandas as pd
import numpy as np
import statsmodels.api as sm
from statsmodels.tsa.stattools import adfuller
import itertools

def kalman_filter(y, x):
    """Calculates dynamic hedge ratio and intercept using a Kalman Filter."""
    state_mean = np.zeros(2)
    state_cov = np.ones((2, 2))
    
    delta = 1e-4
    trans_cov = delta / (1 - delta) * np.eye(2)
    obs_var = 1e-3
    
    means = []
    for i in range(len(y)):
        H = np.array([1, x.iloc[i]])
        
        state_cov = state_cov + trans_cov
        
        error = y.iloc[i] - np.dot(H, state_mean)
        S = np.dot(np.dot(H, state_cov), H.T) + obs_var
        K = np.dot(state_cov, H.T) / S
        
        state_mean = state_mean + K * error
        state_cov = state_cov - np.outer(K, np.dot(H, state_cov))
        
        means.append(state_mean.copy())
        
    means = np.array(means)
    return means[:, 0], means[:, 1] 

tickers = ['V', 'MA', 'AXP', 'PYPL', 'COF', 'JPM', 'BAC']
print(f"Downloading data for universe: {tickers}...")
data = yf.download(tickers, start='2022-01-01', end='2026-08-01', progress=False)['Close']
data.dropna(axis=1, how='all', inplace=True)
data.dropna(axis=0, how='any', inplace=True)
valid_tickers = data.columns.tolist()

print(f"Scanning {len(valid_tickers)} valid stocks for cointegrated pairs...")
pairs = list(itertools.combinations(valid_tickers, 2))
all_signals = []

for asset_y, asset_x in pairs:
    y = data[asset_y]
    x = data[asset_x]
    
    x_with_constant = sm.add_constant(x)
    model = sm.OLS(y, x_with_constant).fit()
    static_spread = y - model.predict(x_with_constant)
    p_value = adfuller(static_spread)[1]
    
    if p_value < 0.05:
        print(f"[APPROVED] {asset_y} vs {asset_x} | P-Value: {p_value:.4f}")
        
        kf_intercept, kf_hedge_ratio = kalman_filter(y, x)
        
        dynamic_spread = y - (kf_hedge_ratio * x + kf_intercept)
        rolling_mean = dynamic_spread.rolling(window=30).mean()
        rolling_std = dynamic_spread.rolling(window=30).std()
        z_score = (dynamic_spread - rolling_mean) / rolling_std
        
        pair_df = pd.DataFrame({
            'Date': data.index,
            'Pair_ID': f"{asset_y}_{asset_x}",
            'Price_Y': y,
            'Price_X': x,
            'Z_Score': z_score
        })
        all_signals.append(pair_df)

if not all_signals:
    print("CRITICAL: No statistically significant pairs found.")
    exit(1)

final_export = pd.concat(all_signals).dropna()
final_export.reset_index(drop=True, inplace=True) 
final_export.sort_values('Date', inplace=True)
final_export.to_csv('pairs_signals.csv', index=False)
print(f"Exported {len(final_export)} signals across {len(all_signals)} active pairs.")