---

### WRO Season 2025 Future Engineering Challenge — Self-Driving Cars

Our team, Red Torque, proudly presents **RT-X7**, an autonomous RoboCar developed for the 2025 World Robot Olympiad (WRO) Future Engineering Challenge. This international competition requires teams to design, build, and program fully autonomous vehicles capable of line following, cube recognition, obstacle avoidance, and precise parking — all performed autonomously under competition conditions.

---

### Team Members and Roles

* **Coach:** Akhila R Gomez
* **Assistant Coach:** Yashin M Afsal
* **Team Members:**
    * **Adithya Sree Sivaramanand:** Hardware Design and Motor Control. A passionate BCA student with a love for AI, robotics, and electronics. I enjoy building smart solutions, exploring new tech, and always carry a screwdriver.
    * **Albin Binu K:** Software Design & GitHub Documentation. I'm currently pursuing my BCA and always looking for ways to apply what I learn in real-world projects. I enjoy taking initiative and learning beyond the classroom and am interested in exploring startup plans rather than sticking to a company job for the long term.
    * **Nasnida AN:** Testing & Documentation. I am currently pursuing a BCA in my 5th semester, with a strong interest in software development and emerging technologies. I enjoy exploring creative tech-based solutions to everyday problems.

Together, we combined hardware engineering, software development, and iterative testing to deliver RT-X7 as a robust competition entry.

---

### Technical Solution Design

RT-X7 was engineered with an emphasis on modularity, stability, and real-time performance. The platform integrates mechanical design, embedded control, and computer vision into a cohesive system designed for repeatable, competition-grade performance.

**Key design choices:**

* **Computer:** A **Raspberry Pi 5 (8 GB)** acts as the main high-level controller responsible for vision, decision logic, and AI. A dedicated **ESP32** handles real-time motor control and ultrasonic sensor processing to keep low-latency tasks separate from vision.
* **Drive & Steering:** Four **DC gear motors** provide locomotion and a dedicated steering motor controls direction. Motor power and direction are managed through an **L298N motor driver**.
* **Chassis:** A compact, stable **~30 cm chassis** provides a solid foundation.
* **Power & Regulation:** A main battery pack (competition-grade power) powers the motors; buck converters provide stable 5 V and 3.3 V rails for the Raspberry Pi and sensors.
* **Cooling & Reliability:** Cooling measures (fans/heatsinks) were used to keep the Raspberry Pi stable under continuous processing loads.

These choices split compute and control responsibilities so vision processing never blocks critical motor/sensor loops — a design decision that improved reliability during multi-run testing.

---

### Hardware and Sensors

RT-X7’s perception and navigation rely on a carefully chosen and integrated sensor suite:

* **Camera:** A **USB webcam** (upgraded from the Pi Camera) provides a higher resolution feed with a wider field of view. The webcam is mounted with an adjustable bracket for quick tuning of angle and height during testing.
* **Ultrasonic Sensing:** Ultrasonic sensors are mounted around the vehicle to cover the front and sides. On power-up, RT-X7 performs a 1-second calibration that averages sensor readings to reduce noise and obtain a reliable baseline. This improves obstacle detection and parking decisions.
* **Storage & Data:** A **256 GB SD card** hosts the OS, runtime logs, and trained models — ensuring adequate space for data collection, model updates, and long test sessions.

All electrical wiring, connectors, and mounts were arranged to minimize vibration and electromagnetic interference, improving sensor reliability during runs.

---

### Software and Algorithms

The RT-X7 software stack is written primarily in **Python** and uses **OpenCV** for vision processing. The code is stored on the Raspberry Pi’s SD card and organized so modules can be updated independently.

**Core software components and responsibilities:**

* **Vision & Perception:** An OpenCV-based pipeline handles cube recognition and line detection.
* **Motor Control & Sensing:** The ESP32 runs low-latency loops to read ultrasonic sensors and command motors via PWM, while the Raspberry Pi sends high-level movement decisions.
* **Algorithms:**
    * **Line Following:** Continuous lane detection and PID-style steering adjustments for smooth tracking.
    * **Cube Detection + Distance Estimation:** Real-time identification of red/green cubes and approximate distance calculation for task logic.
    * **Adaptive Speed:** Speed and steering parameters adapt dynamically based on line curvature and obstacle proximity.
    * **Parallel Parking:** A calibrated sequence of backward/forward maneuvers; the exit direction is chosen dynamically depending on which side presents sufficient clearance.
    * **Startup Calibration:** On boot, RT-X7 performs calibration of ultrasonic sensors (1-second averaging) and zeroes motor controllers to reduce variability between runs.

---

### AI Training Process

Our perception and steering policies were developed with a data-driven approach:

* **Data Collection:** Manual driving sessions on test tracks were recorded to capture camera frames paired with steering/throttle commands and timestamps. This dataset forms the basis for supervised learning.
* **Transfer & Training:** Data was transferred from the Pi to development machines (e.g., via SFTP). A **Convolutional Neural Network (CNN)**, trained in **TensorFlow**, maps image input to steering/throttle in a regression setting.
* **Iterative Improvement:** Models were iteratively improved: collect → train → evaluate → adjust data/augmentation → repeat. Each training iteration was validated on the car and further refined with targeted data (sharp turns, obstacle approaches, low light).
* **Deployment:** Trained models are uploaded back to the Raspberry Pi and executed within the main control scripts for freerun and obstacle tasks.

---

### Power and Thermal Management

Reliable power and sensible energy distribution were vital for repeatable performance:

* **Motor Power:** A dedicated battery pack powers the motors, ensuring consistent torque and speed.
* **Electronics Power:** A separate regulated supply (via buck converters) provides stable 5 V to the Raspberry Pi and sensors.
* **PWM Control:** PWM is used for both throttle and steering control to tune responsiveness and conserve power during runs.
* **Thermal Management:** Cooling measures (fan/heatsinks) keep the Raspberry Pi within safe operating temperatures during long sessions.

---

### Iterative Improvements

RT-X7’s final form was achieved through iterative testing and targeted improvements:

* **Camera Upgrade:** We transitioned from the Pi Camera to a USB webcam for improved resolution and field of view, which significantly improved cube recognition and line detection robustness.
* **Storage Upgrade:** We moved to a 256 GB SD card to prevent I/O bottlenecks and support extensive data logging and model storage.
* **Sensor Calibration:** We implemented a startup calibration (1-second averaging) for ultrasonic sensors to reduce noise and improve repeatability.
* **Major Technical Challenge & Solution:** The single biggest challenge was inconsistent motor performance when the camera and other high-load processes were running. The motors would occasionally stutter or stop entirely. We solved this by implementing a split compute/control architecture. By offloading real-time motor and sensor control to a dedicated ESP32, we ensured that even under heavy vision processing loads on the Raspberry Pi, the motor commands were executed with consistent, low latency. This design decision completely eliminated the issue and significantly improved the car's overall reliability.

---

### Conclusion

RT-X7 embodies our team’s engineering approach to the WRO Future Engineering Challenge 2025: purposeful hardware choices, a split compute/control architecture, and iterative software refinement. By focusing on stability, sensor fusion, and repeatable calibration, RT-X7 performs line following, cube detection, obstacle avoidance, and parallel parking reliably under competition conditions.

This project taught Team Red Torque valuable lessons in embedded systems, computer vision, sensor integration, and collaborative problem-solving. We are proud to submit RT-X7 as our competition-ready entry for WRO 2025 and invite judges and teams to view our implementation and test results.
