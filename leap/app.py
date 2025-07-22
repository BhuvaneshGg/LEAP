from flask import Flask, render_template, jsonify
import requests
import os
from dotenv import load_dotenv
from twilio.rest import Client
import time

# ==== Load Environment Variables ==== #
load_dotenv()

app = Flask(__name__)

ESP32_URL = os.getenv("ESP32_IP") + "/data"
HEART_LIMIT = int(os.getenv("HEART_RATE_LIMIT"))
TEMP_LIMIT = float(os.getenv("TEMP_LIMIT"))

last_alert_time = 0
ALERT_COOLDOWN = 300  # 5 minutes

twilio_client = Client(os.getenv("TWILIO_SID"), os.getenv("TWILIO_AUTH"))

# ==== Helper Functions ==== #
def get_sensor_data():
    try:
        response = requests.get(ESP32_URL, timeout=3)
        data = response.json()
        print("[ESP32] Data Received:", data)
        return data
    except Exception as e:
        print("[ERROR] ESP32 fetch failed:", e)
        return {"temperature": None, "heart_rate": None}

def send_sms_alert(temp, hr):
    global last_alert_time
    now = time.time()
    if now - last_alert_time < ALERT_COOLDOWN:
        return  # Avoid spamming
    msg = f"ALERT! Temp: {temp} °C or Heart Rate: {hr} bpm is abnormal."
    twilio_client.messages.create(
        body=msg,
        from_=os.getenv("TWILIO_FROM"),
        to=os.getenv("TWILIO_TO")
    )
    print("[ALERT] SMS sent")
    last_alert_time = now

# ==== Routes ==== #
@app.route("/")
def dashboard():
    return render_template("index.html")

@app.route("/api/data")
def api_data():
    retries = 5
    for _ in range(retries):
        data = get_sensor_data()
        temp = data.get("temperature")
        hr = data.get("heart_rate")

        if hr and hr >= 61:  # Active threshold
            if temp and (temp > TEMP_LIMIT or hr > HEART_LIMIT):
                send_sms_alert(temp, hr)
            return jsonify(data)

        time.sleep(1)  # Wait 1 sec and retry

    return jsonify({"temperature": temp, "heart_rate": None})

# ==== Run App ==== #
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
