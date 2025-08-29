from picamera2 import Picamera2
import cv2
import numpy as np
import time

# --- Configuration ---
real_cube_height_mm = 50
focal_length_px = 500
min_cube_area = 2000   # ignore small red/green blobs
min_line_area = 800    # ignore small blue/orange blobs

# HSV color ranges (tighter)
red_lower1 = np.array([0, 150, 100])
red_upper1 = np.array([10, 255, 255])
red_lower2 = np.array([170, 150, 100])
red_upper2 = np.array([180, 255, 255])

green_lower = np.array([45, 100, 70])
green_upper = np.array([75, 255, 255])

blue_lower = np.array([100, 150, 50])
blue_upper = np.array([130, 255, 255])

orange_lower = np.array([10, 150, 100])
orange_upper = np.array([25, 255, 255])

def calculate_distance(focal_length, real_height, pixel_height):
    return (real_height * focal_length) / pixel_height if pixel_height != 0 else 0

# --- Initialize camera ---
picam2 = Picamera2()
preview_config = picam2.create_preview_configuration(main={"format":"XRGB8888","size":(640,480)})
picam2.configure(preview_config)
picam2.start()
time.sleep(2)  # camera warm-up

def process_mask(mask, min_area):
    # Remove noise
    kernel = np.ones((5,5), np.uint8)
    mask = cv2.erode(mask, kernel, iterations=1)
    mask = cv2.dilate(mask, kernel, iterations=2)
    # Find contours
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    filtered = [cnt for cnt in contours if cv2.contourArea(cnt) > min_area]
    return filtered

while True:
    frame = picam2.capture_array()

    # Flip camera upside down
    frame = cv2.flip(frame, -1)  # 180° rotation

    # Brightness/contrast adjustment
    lab = cv2.cvtColor(frame, cv2.COLOR_BGR2LAB)
    l, a, b = cv2.split(lab)
    l = cv2.equalizeHist(l)
    frame = cv2.cvtColor(cv2.merge((l,a,b)), cv2.COLOR_LAB2BGR)

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # --- Red cubes ---
    mask_red = cv2.bitwise_or(cv2.inRange(hsv, red_lower1, red_upper1),
                              cv2.inRange(hsv, red_lower2, red_upper2))
    for cnt in process_mask(mask_red, min_cube_area):
        x,y,w,h = cv2.boundingRect(cnt)
        distance = calculate_distance(focal_length_px, real_cube_height_mm, h)
        cv2.rectangle(frame, (x,y), (x+w, y+h), (0,0,255), 2)
        cv2.putText(frame, f"Red Cube: {distance:.1f}mm", (x,y-10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,0,255),2)

    # --- Green cubes ---
    mask_green = cv2.inRange(hsv, green_lower, green_upper)
    for cnt in process_mask(mask_green, min_cube_area):
        x,y,w,h = cv2.boundingRect(cnt)
        distance = calculate_distance(focal_length_px, real_cube_height_mm, h)
        cv2.rectangle(frame, (x,y), (x+w, y+h), (0,255,0), 2)
        cv2.putText(frame, f"Green Cube: {distance:.1f}mm", (x,y-10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,255,0),2)

    # --- Blue lines ---
    mask_blue = cv2.inRange(hsv, blue_lower, blue_upper)
    for cnt in process_mask(mask_blue, min_line_area):
        x,y,w,h = cv2.boundingRect(cnt)
        aspect_ratio = w/h
        if aspect_ratio > 2:  # only long horizontal lines
            cv2.rectangle(frame, (x,y), (x+w, y+h), (255,0,0), 2)
            cv2.putText(frame, "Blue Line", (x,y-10), cv2.FONT_HERSHEY_SIMPLEX,0.6,(255,0,0),2)

    # --- Orange lines ---
    mask_orange = cv2.inRange(hsv, orange_lower, orange_upper)
    for cnt in process_mask(mask_orange, min_line_area):
        x,y,w,h = cv2.boundingRect(cnt)
        aspect_ratio = w/h
        if aspect_ratio > 2:  # only long horizontal lines
            cv2.rectangle(frame, (x,y), (x+w, y+h), (0,165,255), 2)
            cv2.putText(frame, "Orange Line", (x,y-10), cv2.FONT_HERSHEY_SIMPLEX,0.6,(0,165,255),2)

    cv2.imshow("Cube and Line Detection", frame)
    if cv2.waitKey(1) & 0xFF == 27:  # ESC key
        break

cv2.destroyAllWindows()
picam2.stop()
 
