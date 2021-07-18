import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.cm as cmx
# Some additional libraries which we'll use just
# to produce some visualizations of our training

import IPython.display as ipyd
plt.style.use('ggplot')
from keras.datasets import mnist

# load training and testing data
(train_X, train_y), (test_X, test_y) = mnist.load_data()
print('X_train: ' + str(train_X.shape))
print('Y_train: ' + str(train_y.shape))
print('X_test:  '  + str(test_X.shape))
print('Y_test:  '  + str(test_y.shape))


offset = 0

# generate test cases
test_X = test_X.reshape(-1, 28, 28)
test_X = np.pad(test_X, [(0,0), (2,2), (2,2)], 'constant', constant_values=0)
test_X = test_X/255
print(test_X.shape)
test_input = []
for i in range(1):
    line = []
    for row in test_X[offset+i]:
        line += row.tolist()
    test_input.append(line)

with open('C:/Users/xqsxl/Desktop/comp4601/CNN-using-HLS-master/test.in', 'w') as f:
    for item in test_input:
        for num in item:
            f.write("%s " % num)

test_y = test_y.tolist()
test_output = test_y[offset:offset+9]
with open('C:/Users/xqsxl/Desktop/comp4601/CNN-using-HLS-master/test.out', 'w') as f:
    for item in test_output:
        f.write("%s " % item)



"""# plot the test dataset offset to (offest + 9)
from matplotlib import pyplot
for i in range(9):  
    pyplot.subplot(330 + 1 + i)
    pyplot.imshow(test_X[offset+i], cmap=pyplot.get_cmap('gray'))
    print(test_y[offset+i])
pyplot.show()"""