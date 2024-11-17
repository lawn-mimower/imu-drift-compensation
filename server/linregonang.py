import os
import pandas as pd
import numpy as np
from sklearn.linear_model import LinearRegression
import matplotlib.pyplot as plt

# Load data
df = pd.read_csv(os.environ.get("DRIFT_CSV", "data/adjusted_angular_displacement_Xval.csv"))

# Convert Timestamp to datetime
df["Timestamp"] = pd.to_datetime(df["Timestamp"])

# Convert Timestamp to numerical values (seconds since start)
#df["Seconds"] = (df["Timestamp"] - df["Timestamp"].iloc[0]).dt.total_seconds()

# Independent (X) and dependent (y) variables
X = df["Timestamp"].values.reshape(-1, 1)  # Reshape for sklearn
y = df["Original Angular Displacement (deg)"].values

# Fit a linear regression model
model = LinearRegression()
model.fit(X, y)

# Extract slope and intercept
slope = model.coef_[0]  # m
intercept = model.intercept_  # b

print(f"Regression Function: f(x) = {slope:.10f} * x + {intercept:.10f}")

# Plot the original data and the regression line
plt.figure(figsize=(10, 5))
plt.scatter(X, y, label="Original Data", alpha=0.7)
plt.plot(df["Timestamp"], model.predict(X), color="red", label="Regression Line")
plt.title("Linear Regression on Angular Displacement")
plt.xlabel("Time (seconds)")
plt.ylabel("Angular Displacement (deg)")
plt.legend()
plt.grid()
plt.show()
