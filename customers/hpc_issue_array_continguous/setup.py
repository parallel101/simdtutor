#python setup.py build_ext --inplace
from setuptools import setup, Extension
import numpy as np
from Cython.Build import cythonize

ext_modules = [
    Extension("fast_concat", ["fast_concat.pyx"],
              include_dirs=[np.get_include()], # 设置头文件路径
              extra_compile_args=['-fopenmp', '-march=native', '-O3'],
              extra_link_args=['-fopenmp'],
              language="c++"),
]

setup(
    ext_modules = cythonize(ext_modules)
)
