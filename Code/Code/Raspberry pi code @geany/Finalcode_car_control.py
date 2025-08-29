import cv2
import serial
import time

# Serial connection
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
time.sleep(2)

def send_cmd(cmd):
    ser.write((cmd + "\n").encode())
    print("Sent:", cmd)

def main():
    print("🚗📷 Car + Vision started")

    # --- Step 1: Exit parallel parking ---
    send_cmd("E")

    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("❌ Cannot open camera")
        return

    orange_count = 0
    blue_count = 0

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        # --- Red Cube ---
        red1 = cv2.inRange(hsv, (0, 120, 100), (10, 255, 255))
        red2 = cv2.inRange(hsv, (170, 120, 100), (180, 255, 255))
        red_mask = cv2.bitwise_or(red1, red2)

        # --- Green Cube ---
        green_mask = cv2.inRange(hsv, (40, 70, 70), (80, 255, 255))

        # --- Orange Line ---
        orange_mask = cv2.inRange(hsv, (10, 100, 150), (25, 255, 255))

        # --- Blue Line ---
        blue_mask = cv2.inRange(hsv, (90, 100, 100), (130, 255, 255))

        # --- Decisions ---
        if cv2.countNonZero(green_mask) > 5000:
            send_cmd("AL")  # arc left
        elif cv2.countNonZero(red_mask) > 5000:
            send_cmd("AR")  # arc right
        elif cv2.countNonZero(orange_mask) > 5000 and cv2.countNonZero(blue_mask) > 5000:
            send_cmd("NL")  # normal left
            orange_count += 1
            blue_count += 1
        elif cv2.countNonZero(orange_mask) > 5000:
            send_cmd("NR")  # normal right
            orange_count += 1
        elif cv2.countNonZero(blue_mask) > 5000:
            send_cmd("NL")  # normal left
            blue_count += 1
        else:
            send_cmd("F")   # forward

        # --- Stop condition ---
        if orange_count >= 12 and blue_count >= 12:
            send_cmd("STOP")
            print("✅ Completed 12+12 lines, car stopped.")
            break

        cv2.imshow("Feed", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
