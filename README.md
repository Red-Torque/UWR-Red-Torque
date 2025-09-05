# RT-X7: Autonomous RoboCar for WRO 2025 Future Engineering Challenge 

RT-X7 is a fully autonomous RoboCar developed by Team Red Torque for the 2025 World Robot Olympiad (WRO) Future Engineering Challenge. This project showcases a robust autonomous system capable of line following, real-time cube recognition, obstacle avoidance, and precise parallel parking under competition conditions.

---

###  Team Members

* **Adithya Sree Sivaramanand:** Hardware Design and Motor Control.
* **Albin Binu K:** Software Design & GitHub Documentation.
* **Nasnida AN:** Testing & Documentation.
* **Coach:** Akhila R Gomez
* **Assistant Coach:** Yashin M Afsal

---

###  Technical Design

Our solution is built on a modular architecture that separates high-level processing from real-time control, ensuring reliability and performance.

* **Dual-Processor Architecture:** A **Raspberry Pi 5 (8 GB)** handles high-level vision, decision logic, and AI. A dedicated **ESP32** manages low-latency motor control and ultrasonic sensor processing.
* **Drive & Steering:** Four DC gear motors are managed by an **L298N motor driver** for locomotion, with a separate motor for steering.
* **Power & Regulation:** A competition-grade battery powers the motors, while buck converters provide stable 5V and 3.3V rails for the Pi and sensors.
* **Cooling:** Active cooling (fans/heatsinks) ensures the Raspberry Pi remains stable during long, continuous processing runs.

---

###  Perception & Sensors

RT-X7 navigates using a combination of vision and ultrasonic sensing.

* **Camera:** A high-resolution **USB webcam** provides a wide field of view for robust line and cube detection. The adjustable mount allows for quick tuning.
* **Ultrasonic Sensors:** Mounted on the front and sides, these sensors are used for obstacle avoidance and precise parking. A 1-second startup calibration reduces noise and improves detection reliability.

---

###  Software & Algorithms

The software stack, primarily in Python and C++, is organized for modularity and easy updates.

* **Vision & Perception:** We use **OpenCV** on the Raspberry Pi for the core vision pipeline, handling line and cube detection.
* **Motor Control:** The ESP32 executes low-latency PWM loops for motor and steering control based on commands from the Raspberry Pi.
* **Core Algorithms:**
    * **Line Following:** A PID-style control system provides smooth, continuous tracking.
    * **Cube Detection:** Real-time object detection and distance estimation for task logic.
    * **Parallel Parking:** A calibrated sequence of forward and backward maneuvers based on dynamic clearance detection.

---

###  AI & Training

Our steering and perception models were developed using a data-driven, iterative process.

* **Data Collection:** We recorded manual driving sessions on our test track to create a dataset of camera frames paired with steering commands.
* **Training:** A **Convolutional Neural Network (CNN)** was trained in **TensorFlow** to map image input directly to steering commands.
* **Iterative Refinement:** We followed a cycle of `collect → train → evaluate → repeat` to continuously improve model performance with targeted data.

---

###  Key Improvements & Challenges

Our final design is the result of multiple iterations and key upgrades.

* **USB Webcam Upgrade:** Switching from the Pi Camera to a USB webcam significantly improved image quality and field of view, making cube recognition more robust.
* **Split Compute/Control:** Our biggest challenge was motor inconsistency under heavy camera processing loads. We solved this by separating the tasks: the Raspberry Pi handles complex vision, while the ESP32 manages critical, low-latency motor control. This design decision completely eliminated the issue and was vital to our success.

---

###  Conclusion

RT-X7 is a culmination of our team's work in embedded systems, computer vision, and collaborative problem-solving. By focusing on a stable architecture and iterative refinement, our RoboCar reliably executes all tasks for the WRO 2025 Future Engineering Challenge. We are proud of this project and the valuable lessons it has taught us.
