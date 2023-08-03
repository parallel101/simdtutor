cimport numpy as np
import numpy as npy
from libc.stdint cimport uint8_t
from libcpp cimport bool

cdef extern from "fast_mask_lib.hpp":
    # 导入C++中的函数声明
    void multiply_cpp(char* img_HWN_ptr, bool* mask_HW_ptr, char* output_ptr,
                   int N, int H, int W, int mode, int parallel)


def multiply_py(np.ndarray[uint8_t, ndim=3] img_HWN, np.ndarray[uint8_t, ndim=2] mask_HW, mode:int = 0, parallel:int = 0):
    # 提取2D数组的指针列表
    cdef bool* mask_HW_ptr = <bool*> mask_HW.data
    cdef char* img_HWN_ptr = <char*> img_HWN.data
    cdef int H = img_HWN.shape[0]
    cdef int W = img_HWN.shape[1]
    cdef int N = img_HWN.shape[2]
    assert mask_HW.shape[0]==H and mask_HW.shape[1]==W
    cdef np.ndarray[uint8_t, ndim=3] output = npy.ascontiguousarray(npy.empty((H, W, N), dtype=npy.uint8))
    cdef char* output_ptr = <char*> output.data

    # 调用C++函数处理数组
    multiply_cpp(img_HWN_ptr, mask_HW_ptr,  output_ptr,
                  N, H, W, mode, parallel)
    return output
    
