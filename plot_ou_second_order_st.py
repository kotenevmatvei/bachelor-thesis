import matplotlib.pyplot as plt
import numpy as np

# Simulation parameters
D = 0.1
gamma = 1.0
delta_t = 0.01

# Initialize the figure with a 2x2 grid
fig, axes = plt.subplots(2, 2, figsize=(10, 8))
plt.subplots_adjust(wspace=0.3, hspace=0.3)
(ax_traj, ax_fft), (ax_corr, ax_ps) = axes

# ==========================================
# (a) Trajectory
# ==========================================
# loadtxt automatically handles the space-separated values
traj = np.loadtxt("data/trajectory.txt")
time_traj = np.linspace(0, 1000, len(traj))

ax_traj.plot(time_traj, traj, color='black', linewidth=0.5)
ax_traj.set_xlabel("Time", fontsize=12)
ax_traj.set_xlim(0, 1000)
ax_traj.set_ylim(-2, 2)
ax_traj.text(0.95, 0.9, "(a)", transform=ax_traj.transAxes, fontsize=16, fontweight='bold', va='top', ha='right')
ax_traj.text(0.5, 0.1, "Trajectory", transform=ax_traj.transAxes, fontsize=12, fontweight='bold', va='bottom', ha='center')

# ==========================================
# (b) Fourier Transform
# ==========================================
# Load the first 10,000 points and multiply by delta_t to approximate the continuous integral
fft_data = np.loadtxt("data/fft.txt")[:10000] * delta_t
freq_fft = np.linspace(0, 10, len(fft_data))

ax_fft.plot(freq_fft, fft_data, color='black', linewidth=0.5)
ax_fft.set_xlabel("Frequency", fontsize=12)
ax_fft.set_xlim(0, 10)
ax_fft.set_ylim(-30, 30)
ax_fft.text(0.95, 0.9, "(b)", transform=ax_fft.transAxes, fontsize=16, fontweight='bold', va='top', ha='right')
ax_fft.text(0.5, 0.1, "Fourier transform", transform=ax_fft.transAxes, fontsize=12, fontweight='bold', va='bottom', ha='center')

# ==========================================
# (c) Correlation Function
# ==========================================
# loadtxt parses the two columns into a 2D array
corr_data = np.loadtxt("data/corr.txt")
tau = corr_data[:, 0]
C_sim = corr_data[:, 1]

# Theoretical curve: C(tau) = (D / gamma) * exp(-gamma * tau)
C_theory = (D / gamma) * np.exp(-gamma * tau)

ax_corr.plot(tau, C_theory, '-', color='black', label="Theory")
# Plot every 20th simulation point as an open circle, matching the book's style
ax_corr.plot(tau[::20], C_sim[::20], 'o', fillstyle='none', color='black', markersize=4, label="Sims")
ax_corr.set_xlabel("Time delay", fontsize=12)
ax_corr.set_xlim(0, 10)
ax_corr.set_ylim(0, 0.1)
ax_corr.legend(loc='center right', frameon=True)
ax_corr.text(0.95, 0.9, "(c)", transform=ax_corr.transAxes, fontsize=16, fontweight='bold', va='top', ha='right')
ax_corr.text(0.5, 0.3, "Correlation function", transform=ax_corr.transAxes, fontsize=12, fontweight='bold', va='bottom', ha='center')

# ==========================================
# (d) Power Spectrum
# ==========================================
ps_data = np.loadtxt("data/ps.txt")
f_ps = ps_data[:, 0]
S_sim = ps_data[:, 1]

# Theoretical curve: S(f) = 2D / (gamma^2 + (2*pi*f)^2)
S_theory = (4 * D) / (gamma**2 + (2 * np.pi * f_ps)**2)

ax_ps.semilogy(f_ps, S_theory, '-', color='black', label="Theory")
# Plot every 200th simulation point as an open circle
ax_ps.semilogy(f_ps[::200], S_sim[::200], 'o', fillstyle='none', color='black', markersize=4)
ax_ps.set_xlabel("Frequency", fontsize=12)
ax_ps.set_xlim(0, 10)
ax_ps.set_ylim(5e-5, 2e-2)
ax_ps.text(0.95, 0.9, "(d)", transform=ax_ps.transAxes, fontsize=16, fontweight='bold', va='top', ha='right')
ax_ps.text(0.6, 0.5, "Power spectrum", transform=ax_ps.transAxes, fontsize=12, fontweight='bold', va='bottom', ha='center')

# Final layout adjustments and save
plt.tight_layout()
plt.savefig("figures/second_order.png", dpi=300)
plt.show()