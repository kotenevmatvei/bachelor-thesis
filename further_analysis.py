#!/usr/bin/env python3
"""Snapshot-wise analysis of the central B-species peak on [-1, 1]."""
from collections import deque
from pathlib import Path
import csv
import numpy as np
import matplotlib.pyplot as plt

BIN_WIDTH = 0.02
N_PARTICLES_PER_SPECIES = 100000
N_LAST_SNAPSHOTS = 70
ORDER_SCALE = 50.0  # Preserve the original definition; not normalized to [0, 1].
C_VALUES = [3, 4, 5, 5.2, 5.3, 5.33, 5.5, 5.6, 6, 7]

# Known mixed runs from the original analysis, not an estimated critical coupling.
KNOWN_MIXED = {3, 4}
MIN_SEGMENT_BINS = 3
INTERFACE_GUARD_BINS = 1
MIN_HIGH_MIDDLE_GAP = 0.20
MIN_MIDDLE_LOW_GAP = 0.03
FIGURES = Path('figures')
RESULTS = Path('analysis_results')


def analytical_levels(c, ell):
    if c <= 0 or not np.isfinite(ell) or ell <= 0:
        return np.nan, np.nan, np.nan
    h = ell + 1.0 / (c * ell)
    energy = h * (1.0 + 4.0 * c * ell**2)
    m = np.sinh(np.arcsinh(3.0 * energy * np.sqrt(3.0*c)) / 3.0) / np.sqrt(3.0*c)
    w = (1.0 - 2.0*m) / (h + 2.0*ell - 3.0*m)
    return h, m, w


def order_parameter(density):
    return np.sum((0.5 - density)**2) / ORDER_SCALE


def load_snapshots(run_name):
    filename = Path('runs') / run_name / 'data' / f'cnts_{run_name}.txt'
    with filename.open() as stream:
        tail = list(deque((line for line in stream if line.strip()),
                          maxlen=3*N_LAST_SNAPSHOTS))
    if not tail or len(tail) % 3:
        raise ValueError(f'{filename}: expected complete A/B/C triples.')
    rows = [np.fromstring(line, sep=' ', dtype=float) for line in tail]
    if len({len(row) for row in rows}) != 1 or len(rows[0]) < 2:
        raise ValueError(f'{filename}: inconsistent or empty rows.')
    data = np.stack(rows).reshape(-1, 3, len(rows[0]))
    if not np.all(data[:, :, 0] == data[:, :1, 0]):
        raise ValueError(f'{filename}: A/B/C timestep indices do not match.')
    counts = data[:, :, 1:]
    if np.any(counts < 0) or not np.all(np.isfinite(counts)):
        raise ValueError(f'{filename}: invalid counts.')
    n_bins = counts.shape[-1]
    if not np.isclose(n_bins*BIN_WIDTH, 2.0):
        raise ValueError('BIN_WIDTH and number of bins must describe [-1, 1].')
    densities = counts / (N_PARTICLES_PER_SPECIES * BIN_WIDTH)
    return data[:, 0, 0], densities


def fit_five_segments(y):
    """Global minimum-SSE piecewise-constant fit, with five nonempty segments.

    Boundaries are bin edges. No stationarity equation is imposed in this fit.
    """
    y = np.asarray(y, dtype=float)
    n, k, minimum = len(y), 5, MIN_SEGMENT_BINS
    if n < k*minimum or not np.all(np.isfinite(y)):
        raise ValueError('Too few bins or non-finite data.')
    sums = np.r_[0.0, np.cumsum(y)]
    squares = np.r_[0.0, np.cumsum(y*y)]
    cost = np.full((k+1, n+1), np.inf)
    previous = np.full((k+1, n+1), -1, dtype=int)
    cost[0, 0] = 0.0
    for segments in range(1, k+1):
        for end in range(segments*minimum, n+1):
            starts = np.arange((segments-1)*minimum, end-minimum+1)
            lengths = end-starts
            segment_sums = sums[end]-sums[starts]
            errors = squares[end]-squares[starts]-segment_sums**2/lengths
            candidates = cost[segments-1, starts] + np.maximum(errors, 0.0)
            best = int(np.argmin(candidates))
            cost[segments, end] = candidates[best]
            previous[segments, end] = starts[best]
    edges = [n]
    end = n
    for segments in range(k, 0, -1):
        end = previous[segments, end]
        if end < 0:
            raise ValueError('Segmentation failed.')
        edges.append(end)
    return np.array(edges[::-1], dtype=int)


def interior(y, start, end, left_interface, right_interface):
    guard = INTERFACE_GUARD_BINS
    start += guard if left_interface else 0
    end -= guard if right_interface else 0
    if end <= start:
        raise ValueError('No plateau interior; decrease guard or increase segment size.')
    return y[start:end]


def measure_peak(y):
    edges = fit_five_segments(y)
    plateaus = [interior(y, a, b, i > 0, i < 4)
                for i, (a, b) in enumerate(zip(edges[:-1], edges[1:]))]
    levels = [float(np.mean(p)) for p in plateaus]
    low_left, mid_left, high, mid_right, low_right = levels
    if (high - max(mid_left, mid_right) < MIN_HIGH_MIDDLE_GAP or
        min(mid_left, mid_right) - max(low_left, low_right) < MIN_MIDDLE_LOW_GAP):
        raise ValueError('No accepted low/middle/high/middle/low profile.')
    ell = float(np.mean(np.concatenate([plateaus[0], plateaus[4]])))
    middle = float(np.mean(np.concatenate([plateaus[1], plateaus[3]])))
    widths = np.diff(edges)*BIN_WIDTH
    return dict(ell=ell, m=middle, h=high,
                ell_left=low_left, ell_right=low_right,
                m_left=mid_left, m_right=mid_right,
                peak_width=widths[2], middle_width=widths[1]+widths[3],
                low_width=widths[0]+widths[4]), edges


def mean_std(values):
    values = np.asarray(values, dtype=float)
    values = values[np.isfinite(values)]
    if len(values) == 0:
        return np.nan, np.nan
    return float(np.mean(values)), (float(np.std(values, ddof=1)) if len(values) > 1 else np.nan)


def write_csv(filename, rows):
    if not rows:
        return
    with filename.open('w', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)


def plot_summary(summary, name, ylabel, theory=False):
    c = np.array([r['c'] for r in summary])
    y = np.array([r[name+'_mean'] for r in summary])
    sd = np.array([r[name+'_std'] for r in summary])
    fig, ax = plt.subplots()
    ax.errorbar(c, y, yerr=sd, fmt='o-', capsize=3,
                label='Simulation: mean +/- temporal SD')
    if theory:
        ax.errorbar(c, [r['w_theory_mean'] for r in summary],
                    yerr=[r['w_theory_std'] for r in summary], fmt='s--', capsize=3,
                    label='Theory: per-snapshot measured ell')
        fixed = np.array([analytical_levels(cv, 0.1)[2] for cv in c])
        fixed[(fixed <= 0) | (fixed >= 2/3)] = np.nan
        ax.plot(c, fixed, ':', label='Theory: fixed ell=0.1')
        ax.legend()
    ax.set(xlabel='Coupling strength c', ylabel=ylabel)
    ax.grid(True)
    fig.tight_layout()
    fig.savefig(FIGURES / f'{name}.png', dpi=300)
    plt.close(fig)


def main():
    FIGURES.mkdir(exist_ok=True)
    RESULTS.mkdir(exist_ok=True)
    all_rows, summary, visual_profiles = [], [], []
    metrics = ['order', 'peak_width', 'middle_width', 'low_width', 'ell', 'm', 'h',
               'ell_left', 'ell_right', 'm_left', 'm_right', 'w_theory',
               'w_residual', 'R', 'flux_relative_spread', 'normalization_residual',
               'mass_A', 'mass_B', 'mass_C']
    for c in C_VALUES:
        run_name = (f'pow_sym_ref_init-dem_q2_c{c}_dt1e-05_'
                    f'nr100000_rs0_bins100_cnts-step1000')
        times, snapshots = load_snapshots(run_name)
        records, last_edges = [], None
        for timestep, density in zip(times, snapshots):
            A, B, C = density
            record = dict(c=c, timestep=timestep, status='unclassified')
            record.update({name: np.nan for name in metrics})
            record['order'] = order_parameter(B)
            for label, p in zip('ABC', density):
                record['mass_'+label] = float(BIN_WIDTH*np.sum(p))
            if c in KNOWN_MIXED:
                record.update(status='known_mixed', peak_width=0.0,
                              middle_width=2.0, low_width=0.0)
            else:
                try:
                    measured, edges = measure_peak(B)
                    record.update(measured)
                    record['status'] = 'demixed'
                    last_edges = edges
                    ell, m, h = record['ell'], record['m'], record['h']
                    _, _, wt = analytical_levels(c, ell)
                    if np.isfinite(wt) and 0 < wt < 2/3:
                        record['w_theory'] = wt
                        record['w_residual'] = record['peak_width'] - wt
                    record['R'] = c*ell*(h-ell)
                    fluxes = np.array([h*(1+4*c*ell**2), m*(1+4*c*m**2),
                                       ell*(1+c*(h+ell)**2)])
                    record['flux_relative_spread'] = np.ptp(fluxes)/np.mean(fluxes)
                    w = record['peak_width']
                    record['normalization_residual'] = w*(h+2*ell)+(2-3*w)*m-1
                except ValueError as error:
                    record['status'] = 'rejected: '+str(error)
            records.append(record)
        accepted = sum(r['status'] == 'demixed' for r in records)
        rejected = sum(r['status'].startswith('rejected') for r in records)
        row = dict(c=c, n_snapshots=len(records), n_demixed=accepted,
                   n_rejected=rejected,
                   n_theory_valid=sum(np.isfinite(r['w_theory']) for r in records))
        for name in metrics:
            row[name+'_mean'], row[name+'_std'] = mean_std([r[name] for r in records])
        summary.append(row)
        all_rows.extend(records)
        visual_profiles.append((c, snapshots.mean(axis=0)[1]))
        print(f'c={c}: {accepted}/{len(records)} demixed fits, {rejected} rejected; '
              f'w={row["peak_width_mean"]:.5g}, ell={row["ell_mean"]:.5g}, '
              f'w_theory={row["w_theory_mean"]:.5g}')
        if c not in KNOWN_MIXED:
            B = snapshots[-1, 1]
            x = -1 + (np.arange(len(B))+0.5)*BIN_WIDTH
            fig, ax = plt.subplots()
            ax.plot(x, B, '.-', label='Last B snapshot')
            try:
                measured, edges = measure_peak(B)
                fitted_levels = [measured[k] for k in
                                 ['ell_left', 'm_left', 'h', 'm_right', 'ell_right']]
                for a, b, level in zip(edges[:-1], edges[1:], fitted_levels):
                    ax.hlines(level, -1+a*BIN_WIDTH, -1+b*BIN_WIDTH, color='C1')
                for edge in edges[1:-1]:
                    ax.axvline(-1+edge*BIN_WIDTH, color='gray', linestyle=':')
            except ValueError:
                ax.set_title(f'c={c}: last snapshot rejected')
            ax.set(xlabel='x', ylabel='B density')
            ax.legend()
            fig.tight_layout()
            fig.savefig(FIGURES / f'fit_c{c}.png', dpi=200)
            plt.close(fig)
    write_csv(RESULTS / 'snapshot_observables.csv', all_rows)
    write_csv(RESULTS / 'summary.csv', summary)
    for name, label in [('peak_width', 'Central peak width'),
                        ('middle_width', 'Total mixed-interval width'),
                        ('order', 'Order parameter (original scaling)'),
                        ('ell', 'Measured low plateau ell'), ('h', 'High plateau h'),
                        ('m', 'Middle plateau m'), ('R', 'c ell (h-ell)'),
                        ('w_residual', 'Measured width - predicted width')]:
        plot_summary(summary, name, label, theory=(name == 'peak_width'))
    fig, ax = plt.subplots()
    for c, B in visual_profiles:
        x = -1+(np.arange(len(B))+0.5)*BIN_WIDTH
        ax.plot(x, B, label=f'c={c}')
    ax.set(xlabel='x', ylabel='B density', title='Mean profiles: visualization only')
    ax.legend()
    ax.grid(True)
    fig.tight_layout()
    fig.savefig(FIGURES / 'mean_B_profiles.png', dpi=300)
    plt.close(fig)


if __name__ == '__main__':
    main()
