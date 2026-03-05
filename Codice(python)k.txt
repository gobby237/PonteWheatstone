import matplotlib.pyplot as plt
import numpy as np

# --- Inserimento Dati ---
resistenze = np.array([
    27.25, 47, 99.91, 219.68, 329.54, 389.52, 469.38, 558.38, 680.06, 747.14, 
    1.0021, 1.2063, 1.4971, 1.7975, 2.203
])

# k^-1 (Asse Y)
k_inv = np.array([
    0.60, 0.74, 0.87, 0.93, 0.92, 0.91, 0.89, 0.87, 0.84, 0.82, 
    0.77, 0.73, 0.67, 0.62, 0.56
])

# --- Elaborazione ---
resistenze[-5:] = resistenze[-5:] * 1000

# --- Creazione del Grafico ---
plt.figure(figsize=(10, 6))

plt.scatter(resistenze, k_inv, color='red', s=40, label='Dati sperimentali')

# Personalizzazione estetica con LaTeX corretto
plt.title('Distribuzione di $k^{-1}$ in funzione della Resistenza', fontsize=14, fontweight='bold')
plt.xlabel('$R_{A,B}$ [$\Omega$]', fontsize=12)

# Correzione Unità di Misura: \mu per micro e \Omega per Ohm
plt.ylabel('$k^{-1}$ [$\mu A / \Omega$]', fontsize=12)

plt.grid(True, linestyle=':', alpha=0.6)
plt.legend()

# Visualizzazione
plt.show()
