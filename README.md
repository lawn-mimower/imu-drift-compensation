# IMU drift compensation

A gyroscope integrated over time drifts. This measures that drift on an MPU6050, fits it,
and subtracts it live — so the correction comes from the sensor's own measured behaviour
rather than a datasheet constant.

## The loop

```
MPU6050 ──► NodeMCU ──WiFi/TCP──► Python server ──► CSV
 gyro Z      integrate              log + correct       │
 100 Hz      to angle                    ▲              │
                                         │              ▼
                                    slope, intercept ◄── linear regression
```

1. `firmware/Gyro_thetacalc.ino` reads the Z-axis gyro at 100 Hz, integrates rate into
   angular displacement, streams it over TCP.
2. `server/ANGLE_DRIFT_SERV.py` accepts the connection and logs timestamped readings.
3. `server/linregonang.py` fits `sklearn.LinearRegression` to displacement against time.
   With the sensor stationary, any slope *is* the drift.
4. The server imports that fitted `slope` and `intercept` back and subtracts
   `slope·t + intercept` from each incoming reading.

Both the raw and corrected values are logged side by side, so the correction can be
checked rather than assumed.

## Why regression rather than a fixed bias

Gyro bias isn't a single number — it varies per unit and with temperature. Fitting it
against a stationary recording measures what *this* sensor does under *these* conditions.
Recalibrating means re-running the fit, not editing a constant.

## Layout

```
firmware/
  Gyro_thetacalc.ino       MPU6050 read, integrate, stream
  Angle_drift_server.ino   variant
server/
  ANGLE_DRIFT_SERV.py      socket server, live correction, CSV log
  linregonang.py           the fit — exports slope/intercept
analysis/
  error_graph.py           residual plot
```

## Running it

```bash
# firmware: set WIFI_SSID / WIFI_PASSWORD / host IP, flash to the NodeMCU
pip install pandas numpy scikit-learn matplotlib
python server/ANGLE_DRIFT_SERV.py          # listens on :23456
```

Point `DRIFT_CSV` at your recording; it defaults to
`data/adjusted_angular_displacement_Xval.csv`.

To calibrate: leave the sensor still, record a few minutes, run the fit, then restart the
server so it picks up the new coefficients.

## Caveats

- Z-axis only. Full 3-axis orientation needs the other two and a fusion filter.
- The fit is linear. Real drift has a temperature-dependent component this won't capture.
- Recorded data is not in this repo — the CSVs are gitignored.

Hardware: MPU6050 (I²C) + NodeMCU/ESP8266, sampling at 100 Hz.
