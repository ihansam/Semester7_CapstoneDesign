# 필요한 Library들 import
from keras.models import Sequential 
from keras.layers import Dropout, Activation, Dense 
from keras.layers import Flatten, Conv2D, MaxPooling2D 
from keras.models import load_model
from sklearn.model_selection import train_test_split
import numpy as np 
import cv2  
import matplotlib.pyplot as plt

#사용 변수들 선언 및 값 지정

categories = ["good posture", "curved posture"]
class_num = len(categories)

batch = 32
epoch = 50

#생성한 dataset 불러오
X_train, X_test, Y_train, Y_test = np.load('./testingdataset.npy', allow_pickle = True)


#train set 에서 Validation set을 분리(7:1의 비율)
X_train_2, X_validation, Y_train_2, Y_validation = train_test_split(X_train, Y_train,
                                                                test_size = 0.125, random_state = 42)


#앞에서 만든 데이터셋을 불러온다.

model = Sequential() 


#첫번째 convolution layer
model.add(Conv2D(16, (3, 3), activation='relu', padding='same')) 
#maxpooling -> 픽셀 수 절반으로 감소
model.add(MaxPooling2D(pool_size=(2, 2))) 
#Overfitting 방지용 Dropout
model.add(Dropout(.25)) 


#두번째 convolution layer
model.add(Conv2D(64, (3, 3), activation='relu', padding='same'))
#maxpooling -> 픽셀 수 절반으로 감소
model.add(MaxPooling2D(pool_size=(2, 2)))
#Overfitting 방지용 Dropout
model.add(Dropout(.25)) 

#두번째 convolution layer
model.add(Conv2D(256, (3, 3), activation='relu', padding='same'))
#maxpooling -> 픽셀 수 절반으로 감소
model.add(MaxPooling2D(pool_size=(2, 2)))
#Overfitting 방지용 Dropout
model.add(Dropout(.25)) 


#CNN 데이터 타입을 fully connected neural network 형태로 변경  
model.add(Flatten())

#256 input으로 dense
model.add(Dense(1024, activation = 'relu')) 
model.add(Dropout(.25))

#분류 클라스 개수로 shape
model.add(Dense(class_num,activation = 'softmax')) 



#모델 컴파일 및 학습 
model.compile(loss='binary_crossentropy',optimizer='Adam',metrics=['accuracy']) 
hist = model.fit(X_train_2, Y_train_2, batch_size=batch, epochs=epoch,
          shuffle=True, validation_data = (X_validation, Y_validation)) 


model.summary()




#validation accuracy/loss와 training loss/accuracy를 그래프로 출력#######
y_loss = hist.history['loss']
y_acc = hist.history['accuracy']
y_vloss = hist.history['val_loss']
y_vacc = hist.history['val_accuracy']

x_len = np.arange(len(y_loss))

plt.plot(x_len, y_loss, marker='.', c='red', label = 'training_loss')

plt.plot(x_len, y_vloss, marker='*', c='blue', label = 'validation_loss')
plt.legend()
plt.xlabel('epochs')
plt.ylabel('loss')
plt.grid()
plt.show()
plt.plot(x_len, y_acc, marker='.', c='red', label = 'training_accuracy')
plt.plot(x_len, y_vacc, marker='*', c='blue', label = 'validation_accuracy')
plt.legend()
plt.xlabel('epochs')
plt.ylabel('loss')
plt.grid()
plt.show()
#######################################################################


#testing set 에 대해 판독
score = model.evaluate(X_test, Y_test)


print('training loss : ' ,score[0]*100)  
print('training accuracy : ', score[1]*100)

 

#모델을 저장할 경로와 파일명을 지정한다.
model.save('cnnModel_jase.h5')
