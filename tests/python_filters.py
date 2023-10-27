import numpy as np
from scipy import io as spio


def sum_test():
    
    size_array = np.random.randint(500,999,size=1)[0]
    
    x = np.random.normal(20,30, size=(size_array,1))

    spio.mmwrite("x_test",x)

    print("%.16e" % sum(x))

def accumulator_test():

    size_array = np.random.randint(500,999,size=1)[0]
    
    x = np.random.normal(20,30, size=(size_array,1))
    y = np.cumsum(x)[:,None]
    
    spio.mmwrite("x_test",x)
    spio.mmwrite("y_test",y)

def mean_filter_test(**kwargs):

    if "filt_length" not in kwargs.keys():
        filt_length = np.random.randint(4,20,size=1)[0]
    else:
        filt_length = kwargs["filt_length"] 
        
    size_array = np.random.randint(500,999,size=1)[0]

    h = np.ones(filt_length)
    
    x = np.random.normal(20,30, size=(size_array))

    y = np.convolve(x,h)[:len(x),None]/filt_length
    x = x[:,None]
    
    spio.mmwrite("x_test",x)
    spio.mmwrite("y_test",y)

    print("%i" % filt_length);
