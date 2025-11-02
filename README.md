# Industry Protection System (IoT + Software)

This repository contains an end-to-end Industry Protection System project that monitors temperature, gas levels, light intensity and fire detection using Arduino + sensors and uploads telemetry to a PostgreSQL database via an ESP WiFi module and a Flask backend. A companion Android app (Java / Android Studio) displays live data and daily reports.

## Contents

- `flask-ips-backend/` — Flask backend that exposes endpoints to upload and fetch sensor data (PostgreSQL used for storage).
- `arduino_code.ino`, `Wifi_Model_Code.ino` — Arduino sketches used on the microcontroller and ESP WiFi module.
- `Institute_Protection_System/` — Android app (Android Studio, Java) to view live data and reports.

## Quick start

1. Copy the repository locally.
2. Create a PostgreSQL database (see DB schema below).
3. Create a `.env` file in `flask-ips-backend/` with the required environment variables (see `.env.example`).
4. Start the Flask backend and the Android app.

### Recommended workflow (PowerShell)

Run these commands from the repository root (PowerShell):

```powershell
# create a python virtual environment and activate it
python -m venv .venv; .\.venv\Scripts\Activate.ps1

# install backend requirements
pip install -r flask-ips-backend\requirements.txt

# create the DB (use pgcli/psql or a GUI). See db_schema.sql for the table definitions.

# run the backend
cd flask-ips-backend; flask run
```

## Backend (flask-ips-backend)

- Language: Python + Flask
- DB: PostgreSQL (psycopg2 used in code)

Setup notes:

- Copy `flask-ips-backend/.env.example` -> `flask-ips-backend/.env` and fill in your DB credentials.
- The backend expects `sensor_data` and `users` tables; you can initialize them using `db_schema.sql` included in the repo.

API endpoints (high level):

- `POST /upload_data` — Called by the ESP to upload telemetry. Expects JSON with: `temperature`, `gas_level`, `light_intensity`, `fire_detected`, `fan_status`, `led_status`.
- `GET /sensor-data` — Returns the latest sensor reading.
- `POST /login` — Basic username/password login (see code). Consider improving authentication for production.

Security note: Do not commit real credentials to the repository. Use the `.env` file or environment variables.

## Arduino & ESP

Hardware used:

- LM35 — temperature sensor
- Gas sensor (e.g., MQ-series) — gas level
- LDR — light / darkness detection
- Flame sensor — fire detection
- Relay or transistor to drive fan and lights (auto control)

Wiring and sketch notes:

- See `arduino_code.ino` and `Wifi_Model_Code.ino` for how sensors are read and how JSON is sent to the Flask backend via HTTP (ESP module).
- Typical flow: Arduino reads sensors -> composition of JSON payload -> ESP module performs HTTP POST to backend `/upload_data`.

## Android App

- Open `Institute_Protection_System` in Android Studio.
- Build: Gradle wrapper is included. Use a compatible Java / Android SDK per the project's `build.gradle.kts`.
- The app reads from the Flask backend (ensure backend is reachable from the device/emulator — if using emulator, map correctly or use the host IP).

## DB schema

See `db_schema.sql` for CREATE TABLE statements for `users` and `sensor_data`.

## .env variables

Use `flask-ips-backend/.env.example` as a reference. Typical variables:

- `DB_DATABASE`
- `DB_USER`
- `DB_PASSWORD`
- `DB_HOST`
- `DB_PORT`

## How to publish to GitHub (PowerShell)

1. Ensure you have a `.gitignore` and `.env` is excluded.
2. Initialize git, commit, create a GitHub repo and push.

Example commands (PowerShell):

```powershell
cd path\to\Industry-Protection-System
git init
git add .
git commit -m "Initial commit: Industry Protection System"
# create a repo on GitHub via web UI or gh CLI, then add remote and push
git remote add origin https://github.com/<your-username>/<repo-name>.git
git branch -M main
git push -u origin main
```

If you use the GitHub CLI, you can create the repo with:

```powershell
gh repo create <your-username>/<repo-name> --public --source=. --remote=origin
git push -u origin main
```

## License

This repository is provided under the MIT License. See `LICENSE`.

## Next steps (optional)

- Add unit tests for the Flask endpoints.
- Add CI (GitHub Actions) to run linting and tests.

If you'd like, I can also:

- Create a more detailed wiring diagram and a README section per sensor.
- Add a small script to seed the DB with test data.
 
## Project images

I have added a dedicated folder for project images: `assets/images/`.

Please upload your five images there using these recommended filenames (PNG or JPG):

- `login_page.png` — screenshot of the login screen
- `dashboard.png` — main dashboard showing live readings
- `daily_report.png` — daily report screen
- `architecture-diagram.png` — system architecture diagram
- `ips_iot.png` — photo of the working IoT model

Embed images in this `README.md` with relative paths, for example:

```markdown
![Login page](assets/images/login_page.png)
![Dashboard](assets/images/dashboard.png)
```

Tips:

- Resize large photos to ~1280px width to keep repo size small.
- Avoid committing sensitive images that contain private information.

I added `assets/images/README.md` with the same guidance and a `.gitkeep` file so the folder is tracked by git — drop your images there and commit.

---
End of README
