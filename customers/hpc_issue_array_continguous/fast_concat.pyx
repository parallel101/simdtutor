cimport numpy as np
import numpy as npy
from libc.stdint cimport uint8_t, int64_t, uintptr_t
from libcpp cimport bool

cdef extern from "fast_concat_lib.hpp":
    # 导入C++中的函数声明

    void concat[T](T *source, T *destination, const size_t DIM_SRC_N,
        const size_t DIM_SRC_X, const size_t DIM_SRC_Y, const size_t DIM_SRC_Z,
        const size_t DIM_DST_X, const size_t DIM_DST_Y, const size_t DIM_DST_Z,
        const size_t START_X,   const size_t START_Y,   const size_t START_Z
        )


def concat_py(np.ndarray img_NXYZ,  
                DIM_DST_X:size_t, DIM_DST_Y:size_t, DIM_DST_Z:size_t, 
                START_X: size_t,  START_Y: size_t,  START_Z: size_t, 
                mode:size_t = 0):
    # 提取数组的指针
    cdef size_t DIM_SRC_X = img_NXYZ.shape[0]
    cdef size_t DIM_SRC_Y = img_NXYZ.shape[1]
    cdef size_t DIM_SRC_Z = img_NXYZ.shape[2]
    cdef size_t DIM_SRC_N = img_NXYZ.shape[3]

    cdef np.ndarray output_NXYZ = npy.empty((DIM_DST_X, DIM_DST_Y, DIM_DST_Z, DIM_SRC_N), dtype=img_NXYZ.dtype)
    cdef int64_t * img_NXYZ_ptr = <int64_t *> img_NXYZ.data
    cdef int64_t * output_NXYZ_ptr = <int64_t *> output_NXYZ.data
    # 调用C++函数处理数组
    concat[int64_t](img_NXYZ_ptr, output_NXYZ_ptr, DIM_SRC_N,
            DIM_SRC_X, DIM_SRC_Y, DIM_SRC_Z,
            DIM_DST_X, DIM_DST_Y, DIM_DST_Z,
            START_X,   START_Y,   START_Z
            )
    return output_NXYZ


# def getuintptrofarr(np.ndarray arr):
#     cdef uintptr_t ptr = <uintptr_t> <void *> arr.data
#     return ptr
#
# def makesurealign(maker):
#     raii = []
#     for i in range(1000):
#         arr = maker()
#         alignment = getuintptrofarr(arr) % 64
#         if alignment == 0:
#             return arr
#         print('retrying', i, alignment)
#         raii.append(arr)
#         if len(raii) > 100:
#             raii.clear()
#     else:
#         raise RuntimeError('failed to allocate a cacheline-aligned array')
