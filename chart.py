#!/usr/bin/env python3
"""Plot CSV data produced by the Shell-sort experiment.

This script auto-detects the CSV shape and chooses a sensible plot:
- If the CSV has more than two numeric columns and the first column is
  numeric, it plots a line for each subsequent column vs the first column
  (ideal for `test_2_combined.csv`).
- If the CSV has exactly two columns and the first column is non-numeric,
  it draws a categorical bar chart (original behaviour).
- If the CSV has exactly two numeric columns, it draws a line plot.

Usage:
  python3 chart.py input.csv --output chart.png

The script uses only matplotlib and the standard csv module.
"""

import csv
import argparse
import math
import matplotlib.pyplot as plt
from typing import List


def _is_float(s: str) -> bool:
  try:
    float(s)
    return True
  except Exception:
    return False


def generate_chart(filename: str = 'example.csv', save_path: str = None, show: bool = True, max_labels: int = 60, fontsize: int = 8):
  header = []
  rows: List[List[str]] = []
  with open(filename, newline='') as f:
    reader = csv.reader(f)
    header = next(reader, None) or []
    for row in reader:
      if not row:
        continue
      # trim trailing empty columns
      rows.append([c.strip() for c in row])

  if not rows:
    raise SystemExit(f'No data rows found in {filename}')

  ncols = max(len(r) for r in rows)

  # Normalize rows to same length by padding with empty strings
  norm_rows = [r + [''] * (ncols - len(r)) for r in rows]

  # Determine if first column is numeric for all rows
  first_col_numeric = all(_is_float(r[0]) for r in norm_rows if r[0] != '')

  # If more than 2 columns and first numeric -> multi-line plot with numeric x
  if ncols >= 2 and first_col_numeric and all(all(_is_float(c) or c == '' for c in r[1:]) for r in norm_rows):
    # build numeric arrays
    xs = [float(r[0]) for r in norm_rows if r[0] != '' and all(_is_float(c) or c == '' for c in r[1:])]
    # for each subsequent column, collect y values where numeric
    ys_list = []
    for col in range(1, ncols):
      ys = []
      for r in norm_rows:
        if r[0] == '':
          continue
        val = r[col] if col < len(r) else ''
        ys.append(float(val) if _is_float(val) else math.nan)
      ys_list.append(ys)

    # plot
    fig, ax = plt.subplots(figsize=(10, 6))
    labels = header[1:ncols] if header and len(header) >= ncols else [f'col{c}' for c in range(1, ncols)]

    # filter out columns that are constant (or entirely NaN) which often
    # indicates an accidental extra column (e.g. trailing ',1' in CSV).
    def _is_constant_or_nan(vals: List[float]) -> bool:
      # collect finite values
      finite = [v for v in vals if not math.isnan(v)]
      if not finite:
        return True
      first = finite[0]
      return all(math.isclose(v, first, rel_tol=1e-9, abs_tol=1e-12) for v in finite)

    filtered_pairs = []
    for ys, lab in zip(ys_list, labels):
      if _is_constant_or_nan(ys):
        # skip constant columns
        continue
      filtered_pairs.append((ys, lab))

    if not filtered_pairs:
      # nothing useful to plot after filtering: fall back to plotting first non-empty column
      for ys, lab in zip(ys_list, labels):
        ax.plot(xs, ys, marker='o', label=lab)
    else:
      for ys, lab in filtered_pairs:
        ax.plot(xs, ys, marker='o', label=lab)

    ax.set_xlabel(header[0] if header else 'n')
    ax.set_ylabel('time(s)')
    ax.set_title(f"{ax.get_ylabel()} vs {ax.get_xlabel()}")
    ax.grid(axis='y', linestyle='--', alpha=0.5)
    ax.legend()
    plt.tight_layout()
    if save_path:
      fig.savefig(save_path, dpi=150)
      print(f'Saved chart to {save_path}')
    if show:
      plt.show()
    return

  # Two-column cases or non-numeric first column -> categorical bar or simple line
  # Try to coerce second column to numeric
  xs = []
  ys = []
  for r in norm_rows:
    if len(r) < 2:
      continue
    x = r[0]
    y = r[1]
    if not _is_float(y):
      continue
    ys.append(float(y))
    xs.append(x)

  if not xs:
    raise SystemExit(f'No valid numeric data found in {filename}')

  # If first column is numeric, draw line plot, otherwise categorical bar chart
  if all(_is_float(x) for x in xs):
    xnums = [float(x) for x in xs]
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(xnums, ys, marker='o')
    ax.set_xlabel(header[0] if header else '')
    ax.set_ylabel(header[1] if header and len(header) > 1 else 'value')
    ax.set_title(f"{ax.get_ylabel()} vs {ax.get_xlabel()}")
    ax.grid(axis='y', linestyle='--', alpha=0.5)
    plt.tight_layout()
    if save_path:
      fig.savefig(save_path, dpi=150)
      print(f'Saved chart to {save_path}')
    if show:
      plt.show()
    return

  # Categorical bar chart
  n_items = len(xs)
  fig_width = min(max(8, n_items / 10), 48)
  fig, ax = plt.subplots(figsize=(fig_width, 6))
  indices = list(range(n_items))
  ax.bar(indices, ys, color='C0')

  # label step
  label_step = 1
  if n_items > max_labels > 0:
    label_step = int((n_items + max_labels - 1) // max_labels)
  tick_positions = indices[::label_step]
  tick_labels = [xs[i] for i in tick_positions]
  ax.set_xticks(tick_positions)
  ax.set_xticklabels(tick_labels, rotation=45, ha='right', fontsize=fontsize)

  ax.set_xlabel(header[0] if header and len(header) > 0 else '')
  ax.set_ylabel(header[1] if header and len(header) > 1 else 'value')
  ax.set_title(f"{ax.get_ylabel()} vs {ax.get_xlabel()}")
  ax.grid(axis='y', linestyle='--', alpha=0.5)
  plt.tight_layout()
  if save_path:
    fig.savefig(save_path, dpi=150)
    print(f'Saved chart to {save_path}')
  if show:
    plt.show()


if __name__ == '__main__':
  p = argparse.ArgumentParser(description='Plot CSV results from Shell sort experiments')
  p.add_argument('input', nargs='?', default='test_2_combined.csv', help='input CSV file (default: test_2_combined.csv)')
  p.add_argument('--output', '-o', default=None, help='output image file (PNG)')
  p.add_argument('--no-show', dest='show', action='store_false', help="don't call plt.show()")
  p.add_argument('--max-labels', type=int, default=60, help='maximum number of x labels to show for categorical plots')
  p.add_argument('--fontsize', type=int, default=8, help='font size for x tick labels')
  args = p.parse_args()
  generate_chart(args.input, save_path=args.output, show=args.show, max_labels=args.max_labels, fontsize=args.fontsize)