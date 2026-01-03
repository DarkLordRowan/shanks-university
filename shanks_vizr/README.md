# Vizr

## Description

Vizr is a visualization tool designed to analyze and display the convergence properties of mathematical series and their acceleration methods. It loads data Parquet data and provides an interactive dashboard using PyQt6 and PyQtGraph.

## Installation

To install the package and its dependencies:

```bash
pip install .
```

## Usage

To run the visualizer, provide the path to the directory containing the `series` and `accelerations` parquet folders:

```bash
python -m vizr.main PATH_TO_PARQUET_DATA
```

## Project Structure

- `vizr/main.py`: The entry point and UI implementation.
- `vizr/data_loader.py`: Handles loading and processing of Parquet data using `polars`.
- `vizr/symlog.py`: Implements the Symmetric Logarithmic logic and high-precision number handling.
