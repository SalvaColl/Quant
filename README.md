# Options Pricing & Quantitative Trading Pipelines

This repository contains implementations of institutional-grade option pricing models and automated trading strategies. The primary goals of the project have evolved from basic technical analysis to building high-performance quantitative architecture:

- **Research:** Apply advanced statistical methods (Kalman Filters, Cointegration, Cholesky Decomposition) to financial time series.
- **Develop:** Build scalable, automated trading pipelines using Python for data analysis and C++ for low-latency execution.
- **Backtest:** Evaluate strategies using institutional risk metrics (Sharpe Ratio, Max Drawdown, slippage modeling) rather than just raw profitability.

## Market Making & Risk Management
The `Options Pricing` directory includes engines for analyzing and hedging derivative risk.

**Dynamic Delta Hedging Simulator (C++)**
- **Black-Scholes Replication:** Proves the No-Arbitrage pricing theory by dynamically hedging a short option position with the underlying stock and a risk-free bank loan.
- **Monte Carlo Paths:** Simulates 10,000 independent market lifetimes to analyze the distribution of hedging outcomes.
- **Institutional Metrics:** Calculates Tracking Error (Standard Deviation of discrete daily hedging) and absolute Gamma Bleed. 

**Finite Difference Greeks Engine (C++)**
- **Risk Dimensions:** Calculates Delta ($\Delta$), Gamma ($\Gamma$), Vega ($\nu$), Theta ($\Theta$), and Rho ($\rho$).
- **Computational Engineering:** Uses Central and Forward Finite Difference methods to measure sensitivity by simulating microscopic market bumps, avoiding complex closed-form calculus derivations.

## Options Pricing
The `Options Pricing` directory features both discrete and continuous-time pricing engines:

**Monte Carlo Simulation Engine (C++)**
- **Geometric Brownian Motion (GBM):** Simulates millions of price paths for underlying assets.
- **Variance Reduction:** Implements Antithetic Variates to neutralize RNG drift and halve computational cost.
- **Correlated Basket Options:** Uses Cholesky Decomposition to price derivatives based on multi-asset indices with modeled covariance.
- **Risk Metrics:** Outputs standard error and 95% confidence intervals.

**Binomial Tree Model**
- **European Options:** Pricing with no early exercise.
- **American Options:** Backward induction allowing early exercise decisions at each node.
- **Volatility Calculation & Random Walks:** Lognormal approximations plotted alongside historical stock prices.

## Trading Strategies
The `Trading Strategies` directory contains both basic indicator-based prototypes and a fully automated algorithmic execution pipeline.

**Statistical Arbitrage (Pairs Trading) Pipeline**
- **Python Research Scanner:** Scrapes a universe of correlated assets, drops delisted entities, and uses the Augmented Dickey-Fuller (ADF) test to mathematically prove cointegration.
- **Dynamic Hedge Ratios:** Uses a Kalman Filter to recursively update equilibrium baselines and adapt to market volatility instantly.
- **C++ Multi-Pair Execution Engine:** Ingests Python signals and executes via an `unordered_map` to track concurrent portfolios.
- **Risk Management:** Implements equal-risk portfolio budgeting, 5% tranching to average into volatile drawdowns, simulated bid-ask slippage, and a catastrophe stop-loss that permanently bans pairs whose correlations break.

**Technical Indicator Prototypes**
A framework for testing baseline retail strategies, including:
- Moving Average (MA) & MA Crossovers
- Relative Strength Index (RSI)
- Moving Average Convergence Divergence (MACD)
- Money Flow Index (MFI)

## Installation & Execution

**1. Clone the repository:**
```bash
git clone <repository-url>
cd quant
```

**2. Python Environment (Research & Signals):**
```bash
python -m venv .venv
.venv\Scripts\activate
pip install -r requirements.txt
```

**3. C++ Execution (High-Performance Backtesting & Pricing):**
For maximum performance on Monte Carlo simulations and multi-pair backtesting, compile the C++ engines with the `-O3` optimization flag:
```bash
g++ -O3 greeks.cpp -o greeks.exe
.\greeks.exe

g++ -O3 delta_hedging.cpp -o delta_hedging.exe
.\delta_hedging.exe
```