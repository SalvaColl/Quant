Write-Host "`n[1/3] Running Python Statistical Model..." -ForegroundColor Yellow
..\..\.venv\Scripts\python.exe generate_signals.py

Write-Host "`n[2/3] Compiling C++ Execution Engine..." -ForegroundColor Yellow
g++ -O3 backtester.cpp -o backtester.exe

if ($LASTEXITCODE -ne 0) {
    Write-Host "Compilation Failed. Exiting." -ForegroundColor Red
    exit
}

Write-Host "`n[3/3] Running C++ Backtest..." -ForegroundColor Yellow
.\backtester.exe

Write-Host "`nExecution Complete." -ForegroundColor Green