# Capstone Design Project
SKKU EEE "천지창조"   
[poster] https://github.com/ihansam/Semester7_CapstoneDesign/blob/master/POSTER.png   
[main code] https://github.com/ihansam/Semester7_CapstoneDesign/blob/master/PROJECT/Arduino/main/MAIN.ino

Smart Health Chair
-------------------
[Real-time sitting posture evaluation using AI]
- Sensor control system collecting user's distance, weight, and taking a picture of the side.
- Algorithm that classifying Postures based on sensed data.
- Image processing using OpenCV and Deep Learning (CNN) model using Keras.

[Notification IoT system]
- Notification (LED, LCD) control based on present evaluated posture.
- Provide Posture Analysis Reports based on accumulated user's posture data.
- Detect sitting for a long time and recommend stretching with alarm.
- Communication system among chair(sensors), Arduino(main server, AI model, control system), Raspberry Pi(CNN), and smartphone App.
