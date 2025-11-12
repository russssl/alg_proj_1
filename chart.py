#!/usr/bin/env python3
"""Plot a bar chart from a two-column CSV.

The script treats the first column as categorical x labels (strings) and the
second column as numeric y values. This handles cases where the left column
contains non-numeric labels like "1 2 33 23 r234 23".

Usage:
  python3 binary_search_chart_generator.py input.csv --output chart.png
"""

import csv
import argparse
import matplotlib.pyplot as plt


def generate_chart(filename='example.csv', save_path=None, show=True):
  xs = []
  ys = []
  header = []
  with open(filename, newline='') as f:
    reader = csv.reader(f)
    header = next(reader, None)
    for row in reader:
      if len(row) < 2:
        continue
      x = row[0].strip()
      y_str = row[1].strip()
      try:
        y = float(y_str)
      except Exception:
        # skip rows where y is not numeric
        continue
      xs.append(x)
      ys.append(y)

  if not xs:
    raise SystemExit(f'No valid data found in {filename}')

  # create bar chart with categorical x labels
  n_items = len(xs)
  # auto-scale figure width but cap it to avoid huge images
  fig_width = min(max(8, n_items / 50), 48)
  fig, ax = plt.subplots(figsize=(fig_width, 6))
  indices = list(range(n_items))
  ax.bar(indices, ys, color='C0')

  # By default, don't show all x labels if there are many bars.
  # Show at most `max_labels` labels (configurable via CLI). If there
  # are more items than max_labels, we pick a step to display a subset.
  max_labels = getattr(generate_chart, '_max_labels', 60)
  label_step = 1
  if n_items > max_labels > 0:
    # choose step so that roughly max_labels are shown
    label_step = int((n_items + max_labels - 1) // max_labels)

  tick_positions = indices[::label_step]
  tick_labels = [xs[i] for i in tick_positions]
  ax.set_xticks(tick_positions)
  ax.set_xticklabels(tick_labels, rotation=45, ha='right', fontsize=getattr(generate_chart, '_fontsize', 8))

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
  # parser = argparse.ArgumentParser(description='Generate bar chart from two-column CSV')
  # parser.add_argument('input', nargs='?', default='example.csv', help='input CSV file')
  # parser.add_argument('--output', '-o', default=None, help='output image file (PNG)')
  # parser.add_argument('--no-show', dest='show', action='store_false', help="don't call plt.show()")
  # args = parser.parse_args()
  # generate_chart(args.input, save_path=args.output, show=args.show)
  generate_chart('shell_sort_results.csv', save_path=None, show=True)
