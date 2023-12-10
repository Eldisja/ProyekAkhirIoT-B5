import cv2
import time
import matplotlib.pyplot as plt
import easyocr
from IPython.display import Image
from wifi_communicator import WiFiCommunicator
import re

harcascade = "model/haarcascade_russian_plate_number.xml"
plate = None
plateMatch = False
registeredPlate = "13954"

def keep_alphanumeric(input_string):
    if input_string is not None:
        # Use a regular expression to keep only alphanumeric characters
        cleaned_string = re.sub(r'[^a-zA-Z0-9]', '', input_string)
        return cleaned_string
    else:
        return None

# Jalankan program
def main():
    global plate
    wifi_communicator = WiFiCommunicator(max_buffer_sz=1024, plate_match_ref=plateMatch)
    while True:
        try:
            correctCount = 0
            # Dapatkan pesan dari antrian pesan masuk
            message = wifi_communicator.get_message().data
            print(message)
            if (message == "Denied!"):
                print("Running plate recognition")
                if(wifi_communicator._plate_match_ref != True):     
                    plate = plateDetection()
                    plate = keep_alphanumeric(plate)
                    print("Parse : " + plate)
                if message is not None:
                    # Periksa apakah plat nomor sesuai dengan yang diinginkan
                    for i in range(len(registeredPlate)):
                        try:
                            if plate[i] == registeredPlate[i]:
                                correctCount += 1
                        except IndexError:
                            # Handle the IndexError (e.g., print a message or take appropriate action)
                            print("Index out of bound. Skipping comparison for index:", i)
                            break  # Break out of the for loop and reloop the entire main function
                    # Calculate correct percentage only if there are elements in both plates
                    if len(plate) > 0 and len(registeredPlate) > 0:
                        correctPercentage = correctCount / len(plate)
                        print("Correct percent:", correctPercentage)
                    else:
                        print("No elements to compare.")
                    if correctCount / len(plate) >= 0.6:
                        print("Correct percent: " + str(correctCount / len(plate)))
                        wifi_communicator._plate_match_ref = True
                        print("Plate Match: " + str(wifi_communicator._plate_match_ref))
        except KeyboardInterrupt:
            # Tangkap sinyal keyboard interrupt dan hentikan program
            wifi_communicator.destroy()
        time.sleep(1)

def plateDetection():
    cap = cv2.VideoCapture(0)

    cap.set(3, 640)  # width
    cap.set(4, 480)  # height

    min_area = 500
    plateFound = False
    img_roi = None

    while not plateFound:
        success, img = cap.read()

        plate_cascade = cv2.CascadeClassifier(harcascade)
        img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        plates = plate_cascade.detectMultiScale(img_gray, 1.1, 4)

        for (x, y, w, h) in plates:
            area = w * h

            if area > min_area:
                cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)
                cv2.putText(img, "Number Plate", (x, y - 5), cv2.FONT_HERSHEY_COMPLEX_SMALL, 1, (255, 0, 255), 2)

                img_roi = img[y: y + h, x:x + w]
                cv2.imshow("ROI", img_roi)

                timestamp = time.time()
                print(timestamp)
                time.sleep(0.2)
                cv2.imwrite("plates/" + str(timestamp) + ".jpg", img_roi)
                cv2.rectangle(img, (0, 200), (640, 300), (0, 255, 0), cv2.FILLED)
                cv2.putText(img, "Plate Saved", (150, 265), cv2.FONT_HERSHEY_COMPLEX_SMALL, 2, (0, 0, 255), 2)
                cv2.imshow("Results", img)
                cv2.waitKey(500)
                Image('plates/' + str(timestamp) + '.jpg')
                time.sleep(0.5)
                reader = easyocr.Reader(['en'])
                output = reader.readtext('plates/' + str(timestamp) + '.jpg')

                if output:
                    plate = output[0][1]
                    print("Plate found:", plate)
                    plateFound = True
                    break  # Break the inner loop

        cv2.imshow("Result", img)

        key = cv2.waitKey(1) & 0xFF  # Check for key press (wait for 1 ms)

        if key == ord('q'):  # Quit if 'q' is pressed
            break

    cap.release()  # Release the video capture
    cv2.destroyAllWindows()  # Close all OpenCV windows
    return plate

if __name__ == '__main__':
    main()
