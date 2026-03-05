import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import pearsonr
from matplotlib.ticker import AutoMinorLocator

# --- 1. DATASET AGGIORNATO (NUOVI DATI MODIFICATI) ---
x = np.array([
    2199.40, 2099.40, 1979.40, 1969.40, 1939.40, 1929.40, 
    1919.40, 1909.40, 1899.40, 1849.40, 2039.40, 2009.40, 
    1999.40, 1869.40, 1852.40, 2052.40, 1992.40, 1952.40, 
    2022.40, 1989.40, 1949.40, 1959.40, 1964.40, 1954.40, 
    1955.40, 1961.40, 1958.40, 1944.40, 1942.40, 1939.40
])

y = np.array([
    -257.44, -146.92, -27.35, -17.96, 10.94, 19.95, 
    28.83, 37.88, 46.70, 89.69, -83.05, -53.00, 
    -44.00, 73.70, 88.10, -96.12, -37.37, 0.06, 
    -66.30, -34.50, 2.75, -6.47, -11.90, -2.63, 
    -3.54, -7.23, -4.59, 6.33, 8.15, 10.86
])

# --- 2. PARAMETRI DI CONTROLLO E LIMITI DEL GRAFICO ---
# Adattati ai nuovi valori di Rn (traslati)
x_lim_min = 1800
x_lim_max = 2250
y_lim_min = -300
y_lim_max = 120

# --- 3. CALCOLO AUTOMATICO DELL'INTERVALLO DI FIT ---
indice_zero = np.argmin(np.abs(y))
x_centro = x[indice_zero]

x_fit_min = x_centro - 15
x_fit_max = x_centro + 15

# --- 4. CALCOLO REGRESSIONE E CORRELAZIONE ---
mask = (x >= x_fit_min) & (x <= x_fit_max)
x_fit_data = x[mask]
y_fit_data = y[mask]

if len(x_fit_data) > 1:
    m, q = np.polyfit(x_fit_data, y_fit_data, 1)
    rho, _ = pearsonr(x_fit_data, y_fit_data)
    
    x_line = np.linspace(min(x_fit_data), max(x_fit_data), 100)
    y_line = m * x_line + q
    fit_label = rf'Regressione ($\rho = {rho:.4f}$)'
else:
    m, q, rho = 0, 0, 0
    fit_label = "Dati insufficienti per il fit"

# --- 5. GRAFICO ---
plt.style.use('seaborn-v0_8-white')
fig, ax = plt.subplots(figsize=(12, 8))

# Disegno dei punti esclusi dal fit
ax.scatter(x[~mask], y[~mask], color='black', s=35, alpha=1.0, 
           label='Dati esclusi dal fit', zorder=2)

# Disegno del segmento di regressione
if len(x_fit_data) > 1:
    ax.plot(x_line, y_line, color='red', linewidth=2.5, label=fit_label, zorder=10)
    
    # Evidenzia i punti usati per il fit (bordo rosso, interno nero)
    ax.scatter(x_fit_data, y_fit_data, facecolors='black', edgecolors='red', s=45, linewidth=1.5,
               label=r'Punti del fit ($\pm 15\,\Omega$)', zorder=11)

    # --- Segmento indicatore del range ---
    y_bracket = max(y_fit_data) + 30
    
    ax.plot([x_fit_min, x_fit_max], [y_bracket, y_bracket], color='darkred', 
             marker='|', markersize=12, linewidth=1.5, zorder=12)
    
    ax.text(x_centro, y_bracket + 10, r'$\pm$ 15 $\Omega$', color='darkred', 
             ha='center', va='bottom', fontsize=17, fontweight='bold')

# Personalizzazione estetica
ax.set_title("Intervallo di linearità del ponte", fontsize=20, pad=20, fontweight='bold')
ax.set_xlabel(r"$R_n$ ($\Omega$)", fontsize=18)
ax.set_ylabel(r"$i$ ($\mu A$)", fontsize=18)

# Assi guida centrali
ax.axhline(0, color='red', linewidth=1.2, linestyle='--', alpha=0.7) 
ax.axvline(x_centro, color='red', linewidth=1.2, linestyle='--', alpha=0.7, label=f'Centro $R_x$ ({x_centro} $\Omega$)')

# --- CONFIGURAZIONE TICKS IN STILE ROOT ---
ax.tick_params(axis='both', which='major', direction='in', length=8, width=1.2, labelsize=14, top=True, right=True)
ax.tick_params(axis='both', which='minor', direction='in', length=4, width=1.0, top=True, right=True)
ax.xaxis.set_minor_locator(AutoMinorLocator())
ax.yaxis.set_minor_locator(AutoMinorLocator())

for spine in ax.spines.values():
    spine.set_linewidth(1.5)

# --- APPLICAZIONE DEI LIMITI DEL GRAFICO ---
if x_lim_min is not None and x_lim_max is not None:
    ax.set_xlim(x_lim_min, x_lim_max)
if y_lim_min is not None and y_lim_max is not None:
    ax.set_ylim(y_lim_min, y_lim_max)

ax.legend(fontsize=14, frameon=True, shadow=True, loc='best')

plt.tight_layout()
plt.show()
