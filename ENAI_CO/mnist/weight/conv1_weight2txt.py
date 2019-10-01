import numpy as np
import sys
import os
path = "./"
file_list = os.listdir(path)
file_list_npy = [file for file in file_list if file.endswith(".npy")]

print("file_list_npy:{}".format(file_list_npy))

file_list_npy.sort()
for filename in file_list_npy:
    
    kernel = np.load(filename)
    print(kernel.shape)
    print("float/"+filename.split('.npy')[0]+".txt");
    f=open("float/"+filename.split('.npy')[0]+".txt","w")
    if kernel.ndim is 4:
       for i in range(kernel.shape[0]):
           for j in range(kernel.shape[1]):
               for k in range (kernel.shape[2]):
                   for l in range (kernel.shape[3]):
                       f.write('%s\n' %kernel[i,j,k,l])
    if kernel.ndim is 2:
       for i in range(kernel.shape[0]):
           for j in range(kernel.shape[1]):
               f.write('%s\n' %kernel[i,j])
    if kernel.ndim is 1:
       for i in range(kernel.shape[0]):
           f.write('%s\n' %kernel[i])
    f.close()