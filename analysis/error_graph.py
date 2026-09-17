import os
import matplotlib.pyplot as plt
import pandas as pd

# Read the CSV file
df = pd.read_csv(os.environ.get("DRIFT_CSV", "data/adjusted_angular_displacement_Xval.csv"))

# Convert Timestamp column to datetime
df["Timestamp"] = pd.to_datetime(df["Timestamp"])

# Set Timestamp as the index for easier plotting
df.set_index("Timestamp", inplace=True)

# Plot the data
plt.figure(figsize=(10, 5))
plt.plot(df.index, df["Original Angular Displacement (deg)"], label="Org_X_val")
plt.title("Angular Displacement Over Time")
plt.xlabel("Time")
plt.ylabel("Angular Displacement (deg)")
plt.xticks(rotation=45)
plt.legend()
plt.grid()
plt.tight_layout()
plt.show()
