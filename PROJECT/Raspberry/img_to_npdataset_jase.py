#라이브러리들
import os, re, glob  
import cv2
import numpy as np 
from sklearn.model_selection import train_test_split 


#변수들 선언 및 제어
#########################################
#자세의 label은 두가지: 바른 자세와 굽은 자세
#label과 이미지 세부 폴더의 이름은 같다.
categories = ["good posture", "curved posture"]
class_num = len(categories)

#데이터 셋이 담긴 경로
image_root = './training_img_set'
image_path = []

#이미지 리사이징 크기
image_w = 28
image_h = 28

#데이터셋 이미지와 label들이 각각 담길 배열
input_images= []
output_labels = []

testing_size = 0.2  #validation data는 전체 데이터의 20%
seed = 42   #data shuffling을 위한 random seed
############################################

#이미지 폴더 찾기
for category in categories: #현재 지정된 이미지 데이터 전체 폴더속 세부 폴더들을 모두 찾음
    image_path.append(image_root + '/' + str(category))
    print("Searching for Image folder " + image_root + '/' + str(category))



ind = 0
#경로 안의 이미지 폴더들 내부의 모든 데이터 이미지 검색
for imagefolder in image_path:  #이미지 전체 폴더속 세부 폴더 모두 조사
    label_element = [0 for i in range(class_num)]
    label_element[ind] = 1 #폴더 이름이 이미지들의 label이므로 해당 index의 label을 1로 변경
    print("\nReading images in folder " + imagefolder)
    
    for file in os.listdir(imagefolder):    #세부 폴더 속 이미지파일 모두 찾기
        if file.endswith('.jpg'):   #이미지 파일 탐지
            print("Reading image : " + file)
            img = cv2.imread(imagefolder + '/' + file)  #이미지를 읽고 리사이징, 크기는 28x28
            resized = cv2.resize(img, dsize = (image_w,image_h), interpolation=cv2.INTER_AREA)

            input_images.append(resized/256)    #이미지 데이터는 인풋 배열로,
            output_labels.append(label_element) #label데이터는 label 배여로            
    ind = ind+1    


#train_test_split으로 training data와 test(validation) 데이터를 나눠준다
X_train, X_test, Y_train, Y_test = train_test_split(np.array(input_images), np.array(output_labels),
                                                    test_size = testing_size,
                                                    random_state = seed)

#준비가 완료된 데이터셋은 npy파일로 저장, 추후 model training시 불러옴
np.save("./testingdataset.npy", (X_train, X_test, Y_train, Y_test))



