# python speed_test.py
# %%
import numpy as np
# from PIL import Image
import tqdm
import os
import fast_mask

# %%
H, W, N = 800, 1280, 3
img_HWN = np.ascontiguousarray(np.random.randint(0, 256, size=(H, W, N)).astype(np.uint8))
mask_HW = np.ascontiguousarray((np.random.rand(H, W) > 0.5).astype(np.uint8))

out_np_HWN = img_HWN * mask_HW[..., None]
out_0_HWN = fast_mask.multiply_py(img_HWN, mask_HW, 0, 0)
out_1_HWN = fast_mask.multiply_py(img_HWN, mask_HW, 1, 0)
out_2_HWN = fast_mask.multiply_py(img_HWN, mask_HW, 2, 0)
out_0p_HWN = fast_mask.multiply_py(img_HWN, mask_HW, 0, 1)
out_1p_HWN = fast_mask.multiply_py(img_HWN, mask_HW, 1, 1)
out_2p_HWN = fast_mask.multiply_py(img_HWN, mask_HW, 2, 1)
# Image.fromarray(out_0p_HWN).show()
assert np.all(out_np_HWN == out_0_HWN), (out_np_HWN, out_0_HWN)
assert np.all(out_np_HWN == out_1_HWN), (out_np_HWN, out_1_HWN)
assert np.all(out_np_HWN == out_2_HWN), (out_np_HWN, out_2_HWN)
assert np.all(out_np_HWN == out_0p_HWN), (out_np_HWN, out_0p_HWN)
assert np.all(out_np_HWN == out_1p_HWN), (out_np_HWN, out_1p_HWN)
assert np.all(out_np_HWN == out_2p_HWN), (out_np_HWN, out_2p_HWN)

times = 5000

# %% bench mark 可以1. 查看速度  2.查看CPU的多核占用情况
print('top -p ', os.getpid())
for _ in tqdm.trange(times // 10, desc='Numpy'):
    out_np_HWN = img_HWN * mask_HW[..., None]

# for loop
for _ in tqdm.trange(times // 10, desc="C++ for loop"):
    out_0_HWN = fast_mask.multiply_py(img_HWN, mask_HW, 0, 0)

# avx2
for _ in tqdm.trange(times, desc="C++ AVX2"):
    out_1_HWN = fast_mask.multiply_py(img_HWN, mask_HW, 1, 0)

# avx512
for _ in tqdm.trange(times, desc="C++ AVX2 full"):
    out_2_HWN = fast_mask.multiply_py(img_HWN, mask_HW, 2, 0)

# for loop (parallel)
for _ in tqdm.trange(times // 10, desc="C++ for loop (OpenMP)"):
    out_0_HWN = fast_mask.multiply_py(img_HWN, mask_HW, 0, 1)

# avx2 (parallel)
for _ in tqdm.trange(times, desc="C++ AVX2 (OpenMP)"):
    out_1_HWN = fast_mask.multiply_py(img_HWN, mask_HW, 1, 1)

# avx512 (parallel)
for _ in tqdm.trange(times, desc="C++ AVX2 full (OpenMP)"):
    out_2_HWN = fast_mask.multiply_py(img_HWN, mask_HW, 2, 1)
