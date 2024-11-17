import socket
import csv
from datetime import datetime
from linregonang import slope, intercept  # Import regression parameters
print(slope, intercept)
'''
# Server details
HOST = "0.0.0.0"  # Listen on all network interfaces
PORT = 23456      # Port to match the ESP8266

# CSV file configuration
CSV_FILENAME = "adjusted_angular_displacement_Xval.csv"

def initialize_csv(filename):
    """Initialize the CSV file with headers if it doesn't exist."""
    try:
        with open(filename, mode='x', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(["Timestamp", "Original Angular Displacement (deg)", "Adjusted Angular Displacement (deg)"])
    except FileExistsError:
        pass  # File already exists, no action needed

def log_to_csv(filename, timestamp, original_value, adjusted_value):
    """Log the timestamp and angular displacements to the CSV file."""
    with open(filename, mode='a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([timestamp, original_value, adjusted_value])

def calculate_adjusted_displacement(original_displacement, time_elapsed):
    """Subtract the drift function (calculated using regression) from the angular displacement."""
    drift = slope * time_elapsed + intercept  # Regression function
    return original_displacement - drift

def start_server():
    initialize_csv(CSV_FILENAME)
    while True:
        try:
            # Create a socket and bind to the specified host and port
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
                server_socket.bind((HOST, PORT))
                server_socket.listen(1)
                print(f"Listening on {HOST}:{PORT}...")
                
                # Wait for a connection from the ESP8266
                conn, addr = server_socket.accept()
                print(f"Connected by {addr}")
                
                # Handle incoming data
                with conn:
                    start_time = datetime.now()
                    while True:
                        data = conn.recv(1024).decode('utf-8').strip()
                        if not data:
                            print("Disconnected. Waiting for reconnection...")
                            break  # Exit the inner loop to start listening again
                        
                        # Parse and process data
                        try:
                            gyro_z, angular_displacement = map(float, data.split(","))
                        
                            # Calculate elapsed time since the server started
                            elapsed_time = (datetime.now() - start_time).total_seconds()
                            
                            # Adjust for drift using the regression function
                            adjusted_value = calculate_adjusted_displacement(angular_displacement, elapsed_time)
                            
                            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                            print(f"Timestamp: {timestamp}, Original: {angular_displacement} deg, Adjusted: {adjusted_value} deg")
                            
                            # Log to CSV
                            log_to_csv(CSV_FILENAME, timestamp, angular_displacement, adjusted_value)
                        except ValueError:
                            print(f"Invalid data received: {data}")
        except Exception as e:
            print(f"Error occurred: {e}. Restarting server...")
            continue  # Restart the server in case of any error

if __name__ == "__main__":
    start_server()
'''