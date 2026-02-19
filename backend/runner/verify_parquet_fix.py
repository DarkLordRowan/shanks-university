
import pandas as pd
import pyarrow.parquet as pq
from pathlib import Path

def verify():
    parquet_path = Path("output/results_parquet/accelerations")
    if not parquet_path.exists():
        print(f"Directory {parquet_path} not found. Run the runner first.")
        return

    # Read the entire dataset directory; pandas handles Hive partitioning automatically
    try:
        df = pd.read_parquet(parquet_path)
    except Exception as e:
        print(f"Error reading parquet: {e}")
        return

    print("Columns found in accelerations table:", df.columns.tolist())
    
    if "series_args" not in df.columns:
        print("FAIL: series_args column missing from accelerations table.")
        return
    else:
        print("SUCCESS: series_args column found.")

    # Check for duplicates based on series_id AND accel_name AND m_value AND series_args
    # Since series_args is a dict/struct, we might need to stringify it for grouping if pandas doesn't handle it
    df['series_args_str'] = df['series_args'].apply(str)
    
    # Each row in the user's report had different values but shared IDs.
    # Now each should have a unique ID if they represent different x.
    
    counts = df.groupby(['series_id', 'series_args_str', 'accel_name', 'm_value']).size()
    duplicates = counts[counts > 1]
    
    if not duplicates.empty:
        print("FAIL: Still found duplicates with same ID and arguments!")
        print(duplicates)
    else:
        print("SUCCESS: No duplicates found for unique parameter combinations.")

    print("\nSample records:")
    print(df[['series_id', 'series_args_str', 'accel_name', 'm_value']].head(10))

if __name__ == "__main__":
    verify()
