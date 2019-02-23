import os  
import numpy as np

#
def listdir(path, list_name):  
    for file in os.listdir(path):  
        file_path = os.path.join(path, file)  
        if os.path.isdir(file_path):  
            listdir(file_path, list_name)  
        else:  
            list_name.append(file_path)  
    return sorted(list_name)
# flies = sorted(os.listdir(sour_path))

path = '/home/chuchienshu/disk2/eccv_2018/train/input/videvo/'
#/home/chuchienshu/disk2/eccv_2018/train/input/videvo/
#/home/chuchienshu/disk2/eccv_2018/test/videvo-color-pm-test
files = listdir(path, [])
files = [f + '\n' for f in files if '00000.jpg' not in f]


# print files[0][:-5]

# files = [f for f in files if os.path.exists(f[:-5] + '.flo')]


with open('gray_train_full.txt', 'w+') as f:
    f.writelines(files)

print len(files)


#path = '/home/chuchienshu/caffecode/sintal/training/albedo/'
