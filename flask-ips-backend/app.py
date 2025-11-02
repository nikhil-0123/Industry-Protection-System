from flask import Flask, request, jsonify  # Importing Flask for API creation
import psycopg2  # PostgreSQL database connection library
from psycopg2.extras import RealDictCursor  # To return query results as dictionaries
from dotenv import load_dotenv  # To load environment variables from .env file
import os  # For accessing environment variables

# Load environment variables from .env file
load_dotenv()

# Create a Flask web application
app = Flask(__name__)

# Database configuration 
DATABASE_CONFIG = {
    'dbname': os.getenv('DB_DATABASE'),  # Database name
    'user': os.getenv('DB_USER'),  # Database username
    'password': os.getenv('DB_PASSWORD'),  # Database password
    'host': os.getenv('DB_HOST'),  # Database host 
    'port': os.getenv('DB_PORT')  # Database port 
}

# Function to create and return a connection to the database
def get_db_connection():
    conn = psycopg2.connect(
        dbname=DATABASE_CONFIG['dbname'],
        user=DATABASE_CONFIG['user'],
        password=DATABASE_CONFIG['password'],
        host=DATABASE_CONFIG['host'],
        port=DATABASE_CONFIG['port']
    )
    return conn


# ---------------------------- LOGIN ENDPOINT API ---------------------------- #
@app.route('/login', methods=['POST'])
def login():
    data = request.get_json()  # Get JSON data from request
    username = data.get('username')  # Extract username
    password = data.get('password')  # Extract password

    conn = get_db_connection()  # Connect to the database
    cursor = conn.cursor(cursor_factory=RealDictCursor)  # Create a cursor that returns dictionary results

    # Query to check if the user exists with the given credentials
    cursor.execute("SELECT * FROM users WHERE username = %s AND password = %s", (username, password))
    user = cursor.fetchone()  # Fetch the user details (if exists)
    conn.close()  # Close database connection

    if user:
        print("User logged in successfully")  # Print success message
        return jsonify({"message": "Login successful", "user_id": user['id']})  # Send response
    else:
        print("Error: Invalid User")  # Print error message
        return jsonify({"message": "Invalid credentials"}), 401  # Send error response


# ---------------------------- GET LATEST SENSOR DATA FROM DB ENDPOINT API ---------------------------- #
@app.route('/sensor-data', methods=['GET'])
def get_sensor_data():
    conn = get_db_connection()  # Connect to the database
    cursor = conn.cursor(cursor_factory=RealDictCursor)  # Create a dictionary-based cursor

    # Query to get the most recent sensor data
    cursor.execute("SELECT * FROM sensor_data ORDER BY timestamp DESC LIMIT 1")
    latest_data = cursor.fetchone()  # Fetch the latest record
    conn.close()  # Close the connection

    if latest_data:
        print("Data sent:", latest_data)  # Print the fetched data
        return jsonify(latest_data)  # Send the data as a JSON response
    else:
        print("Error: No data available")  # Print error if no data found
        return jsonify({"message": "No data available"}), 404  # Send error response


# ---------------------------- UPLOAD SENSOR ESP8266 DATA ENDPOINT API ---------------------------- #
@app.route('/upload_data', methods=['POST'])
def upload_data():
    data = request.json  # Get the JSON data sent from ESP8266

    print("Received data:", data)  # Print received data for debugging

    # List of required keys in the JSON request
    required_keys = ['temperature', 'gas_level', 'light_intensity', 'fire_detected', 'fan_status', 'led_status']
    
    # Check if all required keys are present
    for key in required_keys:
        if key not in data:
            print(f"Error: '{key}' is required")  # Print error if a key is missing
            return jsonify({"error": f"'{key}' is required"}), 400  # Send error response

    try:
        conn = get_db_connection()  # Connect to the database
        cursor = conn.cursor()  # Create a cursor object

        # SQL Query to insert sensor data into the database
        query = """
            INSERT INTO sensor_data (temperature, gas_level, light_intensity, fire_detected, fan_status, led_status)
            VALUES (%s, %s, %s, %s, %s, %s)
        """
        cursor.execute(query, (
            data['temperature'],  # Temperature value
            data['gas_level'],  # Gas level value
            data['light_intensity'],  # Light intensity value
            data['fire_detected'],  # Fire detection status (True/False)
            data['fan_status'],  # Fan status (On/Off)
            data['led_status']  # LED status (On/Off)
        ))

        conn.commit()  # Save changes to the database
        print("Data uploaded successfully")  # Print success message
        return jsonify({"message": "Data uploaded successfully"}), 201  # Send success response

    except Exception as e:
        print("Error:", e)  # Print any error that occurs
        return jsonify({"error": str(e)}), 500  # Send error response

    finally:
        cursor.close()  # Close cursor
        conn.close()  # Close database connection


# Run the Flask application
if __name__ == '__main__':
    app.run(debug=True) 