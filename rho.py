import numpy as np
import matplotlib.pyplot as plt

# ===== Dati =====
R = np.array([
    27.25, 47, 99.91, 219.68, 329.54, 389.52, 469.38,
    558.38, 680.06, 747.14, 1002.1, 1206.3, 1497.1, 1797.5, 2203
])

rho = np.array([
    1.000, 1.001, 1.001, 0.999, 1.000, 0.999, 1.000,
    1.001, 1.000, 0.999, 1.000, 1.001, 0.999, 0.994, 1.002
])

rho_err = np.array([
    0.003, 0.002, 0.001, 0.001, 0.001, 0.001, 0.001,
    0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001
])

# ===== Media di rho e deviazione percentuale =====
rho_mean = np.mean(rho)
bias_percent = (rho_mean - 1.0) / 1.0 * 100.0
print(f"Media rho          = {rho_mean:.6f}")
print(f"Bias sistematico   = {bias_percent:.4f} %")

# ===== Grafico =====
fig, ax = plt.subplots(figsize=(7, 6))

# Punti sperimentali con barre di errore
ax.errorbar(
    R, rho, yerr=rho_err,
    fmt='o',
    color='black',
    markersize=4,
    linewidth=0,
    elinewidth=1.2,
    capsize=3,
    label=r'$\mathrm{R_1/R_2}$'
)

# Linea rossa continua a y = 1
ax.axhline(y=1.0, color='red', linestyle='-', linewidth=1.2,
           label=r'$\rho = 1$')

# Linea viola tratteggiata alla media di rho
ax.axhline(y=rho_mean, color='purple', linestyle='--', linewidth=1.5,
           label=rf'$\langle\rho\rangle = {rho_mean:.4f}$')

# ===== Freccia e annotazione bias =====
# Posizionata a x=1600, zona senza dati vicini, tra le due linee
# ===== Freccia e annotazione bias =====
# ===== Freccia e annotazione bias =====
arrow_x    = 1600
arrow_y0   = 1.0 - (rho_mean - 1.0) * 2.0     # testo e base freccia (in basso)
arrow_y1   = rho_mean                           # punta freccia (sulla media)

ax.annotate(
    '',
    xy     = (arrow_x, arrow_y0),              # punta in basso
    xytext = (arrow_x, arrow_y1),              # base sulla media
    arrowprops=dict(
        arrowstyle='->', color='purple', lw=1.5
    )
)

# Testo del bias sotto la freccia
ax.text(
    arrow_x + 40, arrow_y0,
    rf'$\delta_{{sys}} = {bias_percent:+.4f}\%$',
    color='purple',
    fontsize=13,
    va='center'
)

# ===== Assi e stile =====
ax.set_xlabel(r'$\mathrm{R_i \ (\Omega) \ METRIX}$', fontsize=13)
ax.set_ylabel(r'$\mathrm{R_1/R_2}$',                 fontsize=13)

ax.tick_params(axis='both', which='both', direction='in',
               top=True, right=True, labelsize=11)

ax.set_xlim(0, 2400)

# Legenda
ax.legend(fontsize=11, frameon=True)

plt.tight_layout()
plt.show()