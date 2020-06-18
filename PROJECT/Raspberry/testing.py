#Library Import

import time
import RPi.GPIO as GPIO
from picamera import PiCamera
import os, re, glob 
import cv2  
import numpy as np 
from keras.models import load_model 
 

#기본 세팅 ###############################################
#분류할 카테고리 지정
categories = ["good posture", "curved posture"] #좋은(허리를 핀)자세와 나쁜(허리를 구부린)자세
picam = PiCamera() #Picamera 세팅

#testing하기전 resizing해 줄 크기들
image_w = 28
image_h = 28

#촬영될 image file name과 testing image가 담길 list(추후 np배열로 바뀜)
name = "test.jpg"
test = [0] 
test_image_path = "./" #테스트 이미지를 불러올 경로

i = 0

#무한 루프를 위한 flag
flag = 1

#GPIO 초기 세팅; pin 16, 18을 사용하며 default는 LOW.
GPIO.setmode(GPIO.BOARD)
GPIO.setup(16, GPIO.OUT)
GPIO.setup(18, GPIO.OUT)
GPIO.output(16, GPIO.LOW)
GPIO.output(18, GPIO.LOW)


print("READY")
        
        
model = load_model('cnnModel_jase.h5') #이미 학습된 자세 분석 데이터를 불러옴
#기본 세팅 ###############################################



#무한루프; 이 루프 안의 자세 판독 작업은 계속 돌아감
while (flag):

    picam.capture('test.jpg')   #자세 이미지 촬영
    time.sleep(.1)
    for file in os.listdir(test_image_path):    # 촬영된 이미지를 찾기

        if file.endswith('.jpg'):       #폴더에 테스트 파일 외의 jpg 파일은 제거 해야함
            print(file)
            print(test_image_path + file)   #디버깅용 정보 표시
            print(i)
            
            img = cv2.imread(test_image_path + file)    #경로에서 image 불러오기
            resized = cv2.resize(img, dsize = (image_w,image_h))    #지정한 크기로 resize
            test[0] = resized #test list로 resize된 이미지 이동
            
            i = i+1
    test = np.array(test) #input data로 사용할 수 있도록 np.array 형태로 변형
    
    predict = model.predict_classes(test)   #단순 판단 결과
    predict2 = 100*model.predict(test)  #판단결과 확률
     
     
    #디버깅용 모니터 출력: 현재 파일의 이름과 예측 결과, 그리고 각 예측 항목에 대한 확률 모두 표시
    print(name + " : "+ str(categories[predict[0]]))
    print("Specifics : Good: " + str(round(predict2[0][0],2)) +
              "%, Bad: "+ str(round(predict2[0][1],2)) + "%\n")
    #################

    #Arduino 로 결과 전송
    if (predict2[0][0] + predict2[0][1] <= 95):     #판단 결과합이 95퍼가 안넘는 오류
        GPIO.output(16, GPIO.LOW)   #아두이노로 01을 전달한다
        GPIO.output(18, GPIO.HIGH)
        print("Posture data corrupted; relocate your camera")
    elif (predict[0] == 0):         #판단결과 올바른 자세
        GPIO.output(16, GPIO.HIGH)  #아두이노로 11을 전달한다
        GPIO.output(18, GPIO.HIGH)
        print("Good posture sent to arduino")
    elif (predict[0] == 1):         #판단결과 허리가 굽은 자세
        GPIO.output(16, GPIO.HIGH)  #아두이노로 10을 전달한다
        GPIO.output(18, GPIO.LOW)
        print("bad posture sent to arduino")
    else:                               #라즈베리파이 준비중
        GPIO.output(16, GPIO.LOW)   #아두이노로 00을 전달한다
        GPIO.output(18, GPIO.LOW)
        print("READY")
