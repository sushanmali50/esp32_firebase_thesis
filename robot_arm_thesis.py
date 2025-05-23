import time
import datetime
import firebase_admin
from firebase_admin import credentials, db
from adafruit_servokit import ServoKit

# === PCA9685 Initialization ===
# Setup for 16-channel PWM servo controller
nbPCAServo = 16
pca = ServoKit(channels=nbPCAServo)

# Configure pulse width range for all servos (in microseconds)
MIN_IMP = [500] * nbPCAServo
MAX_IMP = [2500] * nbPCAServo
for i in range(nbPCAServo):
    pca.servo[i].set_pulse_width_range(MIN_IMP[i], MAX_IMP[i])

# === Firebase Initialization ===
# Load credentials and initialize Firebase connection
cred = credentials.Certificate("serviceAccountKey.json")
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://esp32-thesis-9627d-default-rtdb.europe-west1.firebasedatabase.app/'
})

# Get references to specific Firebase Realtime Database nodes
servo_ref = db.reference("arm/servos")
status_ref = db.reference("robot/status")

# Define default servo positions (in degrees) for reset
default_position = [90, 90, 90, 90, 90, 100]

# === Servo Motion Helper ===
# Smoothly interpolates a servo's movement from current to target angle
def smooth_single(index, target, steps=20, delay=0.05):
    current = pca.servo[index].angle or 90  # Default to 90 if angle is None
    for step in range(steps + 1):
        interp = current + (target - current) * step / steps
        pca.servo[index].angle = interp
        time.sleep(delay)

# === Task Sequence ===
# Predefined arm movement sequence for picking and placing an object
def perform_task_sequence():
    print("▶️ Starting predefined movement sequence")
    start = datetime.datetime.now()

    # Initial movement to approach object
    smooth_single(1, 50)
    smooth_single(0, 160)
    smooth_single(1, 90)
    smooth_single(2, 70)

    # Simulate grabbing the object with claw (servo 5)
    print("🧲 Grabbing object (servo 5)...")
    pca.servo[5].angle = 180
    time.sleep(1.0)

    # Movement to drop-off location
    smooth_single(1, 60)
    smooth_single(0, 80)
    smooth_single(1, 90)

    # Release the object
    print("👐 Releasing object...")
    pca.servo[5].angle = 100
    time.sleep(0.4)

    # Notify the robot that the task is complete
    status_ref.set("task_done")
    print("✅ Task completed and status set.")

    # Log task duration
    end = datetime.datetime.now()
    duration = (end - start).total_seconds()
    print(f"🕒 Task Duration (excluding reset): {duration:.3f} seconds")

    # Reset all servos to default positions
    print("🔁 Returning to default angles...")
    for i in range(6):
        smooth_single(i, default_position[i])

# === Firebase Listener ===
# Reacts to changes in the robot status in Firebase
def status_listener(event):
    status = str(event.data).lower()
    if status == "waiting":
        print("📍 Checkpoint reached - starting arm task sequence...")
        perform_task_sequence()

# === Initialization ===
# Moves all servos to their default startup angles
def initialize():
    print("Initializing to default angles...")
    for i in range(6):
        pca.servo[i].angle = default_position[i]
    time.sleep(1)

# Run initialization
initialize()

# Setup Firebase listeners
servo_ref.listen(lambda e: None)          # Optional: future use for servo control
status_ref.listen(status_listener)       # Watch for "waiting" status to trigger task

print("🔥 Listening to Firebase for status: 'waiting'...")

# === Main Loop ===
# Keeps the program running to allow Firebase listening
try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    print("Shutting down...")
    for i in range(6):
        pca.servo[i].angle = 90  # Reset to neutral on shutdown
