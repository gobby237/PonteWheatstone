import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import AutoMinorLocator
from scipy.stats import linregress

# Dati originali
x = np.array([2199.40, 2099.40, 1979.40, 1969.40, 1939.40, 1929.40, 1919.40, 1909.40, 1899.40, 1849.40, 2039.40, 2009.40, 1999.40, 1869.40, 1852.40, 2052.40, 1992.40, 1952.40, 2022.40, 1989.40, 1949.40, 1959.40, 1964.40, 1954.40, 1955.40, 1961.40, 1958.40, 1944.40, 1942.40])
y = np.array([-257.44, -146.92, -27.35, -17.96, 10.94, 19.95, 28.83, 37.88, 46.70, 89.69, -83.05, -53.00, -44.00, 73.70, 88.10, -96.12, -37.37, 0.06, -66.30, -34.50, 2.75, -6.47, -11.90, -2.63, -3.54, -7.23, -4.59, 6.33, 8.15])

# Filtro i dati: centro 1952.4 +/- 15
centro_x = 1952.4
mask = (x >= centro_x - 15) & (x <= centro_x + 15)
x_filtered = x[mask]
y_filtered = y[mask]

# Fit lineare per calcolare i valori attesi
res = linregress(x_filtered, y_filtered)
slope = res.slope
intercept = res.intercept

# Calcolo dei residui (Dato Reale - Dato dal Fit)
y_fit = slope * x_filtered + intercept
residui = y_filtered - y_fit

# Creazione della figura
fig, ax = plt.subplots(figsize=(9, 6))

# Stile degli assi e delle tacche (ticks) per matchare la tua immagine
ax.tick_params(which='major', direction='in', length=7, width=1.2, top=True, right=True, labelsize=12)
ax.tick_params(which='minor', direction='in', length=4, width=1, top=True, right=True)
ax.xaxis.set_minor_locator(AutoMinorLocator(5))
ax.yaxis.set_minor_locator(AutoMinorLocator(5))

# Spessore del bordo del grafico
for spine in ax.spines.values():
    spine.set_linewidth(1.2)

# Plot della retta di riferimento a 0 (che rappresenta il fit nel grafico dei residui) e del centro
ax.axhline(0, color='darkblue', linestyle='-', linewidth=1.2, label='Fit (Residui = 0)', zorder=2)
ax.axvline(centro_x, color='gray', linestyle='--', linewidth=1.2, label=rf'Centro $R_x$ ({centro_x} $\Omega$)', zorder=1)

# Plot dei residui con il colore richiesto
ax.scatter(x_filtered, residui, color='#bc51a1', s=70, label='Residui', zorder=3)

# Etichette degli assi
ax.set_xlabel(r'$R_n$ ($\Omega$)', fontsize=14, labelpad=8)
ax.set_ylabel(r'Residui $\Delta i$ ($\mu$A)', fontsize=14, labelpad=8)

# Limiti degli assi per centrare l'immagine
ax.set_xlim(centro_x - 15.5, centro_x + 14.5)
# Rendo l'asse y simmetrico rispetto allo zero per visualizzare meglio i residui
y_max = np.max(np.abs(residui)) * 1.5
ax.set_ylim(-y_max, y_max)

# Configurazione della legenda
handles, labels = ax.get_legend_handles_labels()
# Ordiniamo gli elementi: prima i punti, poi la retta zero, poi la linea verticale
order = [2, 0, 1] 
ax.legend([handles[idx] for idx in order], [labels[idx] for idx in order], 
          loc='upper right', frameon=True, shadow=True, fancybox=True, 
          fontsize=12, borderpad=0.8)

plt.tight_layout()
plt.show()
