-- Minimal DB schema for Industry Protection System

-- users table (simple example for login used by the Flask app)
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(150) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT now()
);

-- sensor_data table (store telemetry from devices)
CREATE TABLE IF NOT EXISTS sensor_data (
    id SERIAL PRIMARY KEY,
    temperature REAL,
    gas_level REAL,
    light_intensity REAL,
    fire_detected BOOLEAN,
    fan_status BOOLEAN,
    led_status BOOLEAN,
    timestamp TIMESTAMP WITH TIME ZONE DEFAULT now()
);
