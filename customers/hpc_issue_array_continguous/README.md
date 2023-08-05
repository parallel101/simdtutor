# 安装指南
---
```bash
git clone URL
cd GIT_PROJECT

python setup.py build_ext --inplace
```

## 速度测试
```bash
python speed_test.py
```


## 选项：是否使用多线程
打开`fast_concat_lib.hpp`. 修改配置后执行 `python setup.py build_ext --inplace` 。
```bash
void concat(...)
{
    // 开启 openmp 多线程加速, 好像2个线程就饱和了 // 小彭老师：我靠，memcpy是典型的内存瓶颈（membound）操作，当然没法用并行加速了
    // #pragma omp parallel for num_threads(2)
    for(int in=0; in < DIM_SRC_N; in++){
        T *source_batch = &source[in*DIM_SRC_X*DIM_SRC_Y*DIM_SRC_Z];
        T *destination_batch = &destination[in*DIM_DST_X*DIM_DST_Y*DIM_DST_Z];
        thread_concat(source_batch, destination_batch, DIM_SRC_X, DIM_SRC_Y, DIM_SRC_Z,
                    DIM_DST_X, DIM_DST_Y, DIM_DST_Z, START_X, START_Y, START_Z);
    }
}

```


## 问题？
---
我需要对一个高维度矩阵进行连续片段提取。从一个 (9,164,164,164) 数组抽提出 (9, 64, 64, 64) 的数据。numpy 版本的速度很慢。仅仅280FPS，折算内存的拷贝速度为 4.9 GByte/s。即使用 C++ 重写也很慢。
```python
# 280 FPS
for _ in tqdm.trange(5000):
    index_conti = np.ascontiguousarray(index_all[:, com3d_x:com3d_x+64, com3d_y:com3d_y+64, com3d_z:com3d_z+64])

# 280 FPS， C++ 重写，使用 memcpy 也没有任何提升
for _ in tqdm.trange(5000):
    index_conti2 = fast_concat.concat_py(index_all, 64, 64, 64, com3d_x, com3d_y, com3d_z)
```

C++ 版本的代码
```cpp
void thread_concat(...)
{

...
    for (int ix = 0; ix < DIM_DST_X; ix++){
        for (int iy = 0; iy < DIM_DST_Y; iy++){
            T * ptrSRC = &source[(ix + START_X) * DIM_SRC_Y * DIM_SRC_Z + (iy + START_Y) * DIM_SRC_Z + START_Z];
            T * ptrDST = &destination[ix * DIM_DST_Y * DIM_DST_Z + iy * DIM_DST_Z + 0];
            std::memcpy(ptrDST, ptrSRC, size_cp);
        }
    }
}
```
