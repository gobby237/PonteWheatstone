import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import pearsonr

# --- 1. DATASET AGGIORNATO ---
x = np.array([
    2014.40, 1914.40, 1814.40, 1714.40, 1614.40, 1514.40, 1414.40,
    1404.40, 1394.40, 1384.40, 1374.40, 1364.40, 1354.40, 1344.40,
    1334.40, 1324.40, 1314.40, 1264.40, 1454.40, 1424.40, 1414.40,
    1284.40, 1267.40, 1467.40, 1407.40, 1367.40, 1437.40, 1404.40,
    1364.40, 1354.40, 1374.40, 1379.40, 1369.40, 1370.40, 1376.40,
    1373.40, 1359.40, 1357.40, 1354.40
])

y = np.array([
    -838.23, -667.48, -515.42, -379.68, -257.44, -146.92, -46.34,
    -36.81, -27.35, -17.96, -8.72, 1.67, 10.94, 19.95,
    28.83, 37.88, 46.70, 89.69, -83.05, -53.00, -44.00,
    73.70, 88.10, -96.12, -37.37, 0.06, -66.30, -34.50,
    2.75, 11.89, -6.47, -11.90, -2.63, -3.54, -7.23,
    -4.59, 6.33, 8.15, 10.86
])

# --- 2. PARAMETRI DI CONTROLLO ---
# Range per la regressione (modifica questi)
x_fit_min = 1300 
x_fit_max = 1450 

# --- 3. CALCOLO REGRESSIONE E CORRELAZIONE ---
mask = (x >= x_fit_min) & (x <= x_fit_max)
x_fit_data = x[mask]
y_fit_data = y[mask]

if len(x_fit_data) > 1:
    # Regressione lineare
    m, q = np.polyfit(x_fit_data, y_fit_data, 1)
    
    # Coefficiente di correlazione di Pearson (rho)
    rho, _ = pearsonr(x_fit_data, y_fit_data)
    
    # Generazione punti retta SOLO nell'intervallo scelto
    x_line = np.linspace(min(x_fit_data), max(x_fit_data), 100)
    y_line = m * x_line + q
    
    fit_label = rf'Regressione ($\rho = {rho:.4f}$)'
else:
    m, q, rho = 0, 0, 0
    fit_label = "Dati insufficienti per il fit"

# --- 4. GRAFICO ---
plt.style.use('seaborn-v0_8-whitegrid') # Stile pulito
plt.figure(figsize=(12, 8))

# Disegno di tutti i punti sperimentali
plt.scatter(x, y, color='#2c3e50', s=20, alpha=0.7, label='Dati sperimentali', edgecolors='white', linewidth=0.5)

# Disegno del segmento di regressione (Rosso fine)
if len(x_fit_data) > 1:
    plt.plot(x_line, y_line, color='red', linewidth=1.5, label=fit_label, zorder=10)
    # Opzionale: evidenzia i punti usati per il fit
    plt.scatter(x_fit_data, y_fit_data, color='red', s=20, zorder=11, label='Punti nel range di fit')

# Personalizzazione assi e titoli
plt.title("Intervallo di sensibilità del ponte", fontsize=18, fontweight='bold', pad=20)
plt.xlabel(r"$R_n$ ($\Omega$)", fontsize=14, labelpad=10)
plt.ylabel(r"$i$ ($\mu A$)", fontsize=14, labelpad=10)

# Linea dello zero per facilitare la lettura dell'equilibrio
plt.axhline(0, color='black', linewidth=0.8, linestyle='-', alpha=0.3)

# Miglioramento della griglia e dei tick
plt.xticks(fontsize=12)
plt.yticks(fontsize=12)
plt.grid(True, linestyle='--', alpha=0.6)

# Legenda
plt.legend(fontsize=12, frameon=True, shadow=True, loc='best')

plt.tight_layout()
plt.show()
