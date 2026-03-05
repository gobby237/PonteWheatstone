import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import AutoMinorLocator

# --- 1. DATASET ---
resistenze = np.array([
    27.25, 47, 99.91, 219.68, 329.54, 389.52, 469.38, 558.38, 680.06, 747.14, 
    1.0021, 1.2063, 1.4971, 1.7975, 2.203
])

# k^-1 (Asse Y)
k_inv = np.array([
    0.60, 0.74, 0.87, 0.93, 0.92, 0.91, 0.89, 0.87, 0.84, 0.82, 
    0.77, 0.73, 0.67, 0.62, 0.56
])

# --- 2. ELABORAZIONE ---
resistenze[-5:] = resistenze[-5:] * 1000

# --- 3. CREAZIONE DEL GRAFICO ---
plt.style.use('seaborn-v0_8-white')
fig, ax = plt.subplots(figsize=(12, 8))

# Disegno dei punti (ingranditi a s=80)
ax.scatter(resistenze, k_inv, color='black', s=80, alpha=1.0, 
           label='Dati sperimentali', zorder=2)

# Personalizzazione font (Aumentati di diverse unità)
ax.set_title(r"Distribuzione di $k^{-1}$ in funzione della Resistenza", 
             fontsize=24, pad=25, fontweight='bold')
ax.set_xlabel(r"$R_{A,B}$ ($\Omega$)", fontsize=22, labelpad=10)
ax.set_ylabel(r"$k^{-1}$ ($\mu A / \Omega$)", fontsize=22, labelpad=10)

# --- CONFIGURAZIONE TICKS (Più lunghi e spessi) ---
# Ticks principali
ax.tick_params(axis='both', which='major', direction='in', 
               length=12, width=2.0, labelsize=18, top=True, right=True)

# Ticks secondari
ax.tick_params(axis='both', which='minor', direction='in', 
               length=6, width=1.5, top=True, right=True)

# Locator per tacche secondarie
ax.xaxis.set_minor_locator(AutoMinorLocator())
ax.yaxis.set_minor_locator(AutoMinorLocator())

# Ispessimento dei bordi del grafico (Spine)
for spine in ax.spines.values():
    spine.set_linewidth(2.0)

# Legenda aumentata
ax.legend(fontsize=18, frameon=True, shadow=True, loc='best')

plt.tight_layout()
plt.show()
