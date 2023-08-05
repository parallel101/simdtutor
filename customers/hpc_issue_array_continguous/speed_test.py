# %%
import numpy as np
import tqdm
import fast_concat

index_all = np.empty(shape=(256,256,256,9),dtype=np.int64)
index_all[:] = np.random.randint(0,800*1280-1, size=(256,256,256,9),dtype=np.int64)
random_com3d = np.random.randint(0,100-1,size=(3),dtype=np.int64)
com3d_x, com3d_y, com3d_z = random_com3d.tolist()

index_conti2 = fast_concat.concat_py(index_all, 64,64,64, com3d_x, com3d_y, com3d_z)

index_conti = np.ascontiguousarray(index_all[com3d_x:com3d_x+64, com3d_y:com3d_y+64, com3d_z:com3d_z+64])

assert np.all(index_conti2==index_conti)

# %% CROP有序片段内存读取: 279fps, 4.9 GB/s
for _ in tqdm.trange(1000):
    index_conti = np.ascontiguousarray(index_all[com3d_x:com3d_x+64, com3d_y:com3d_y+64, com3d_z:com3d_z+64])


for _ in tqdm.trange(1000):
    index_conti2 = fast_concat.concat_py(index_all, 64,64,64, com3d_x, com3d_y, com3d_z)

# fps = 279
# IO_rate = fps * index_all.itemsize * index_conti2.size / 1024**3
# print(f'IO 的速度为 {IO_rate:.1f} GB/s')
#
# # %% 全随机内存读取 16fps， 288.0 MB/s
# rand_ind = np.random.randint(0, index_all.size-1, size=index_conti2.shape)
# index_ravel = index_all.ravel()
# for _ in tqdm.trange(200):
#     index_conti3 = index_ravel[rand_ind]
#
# fps = 16
# IO_rate = fps * index_all.itemsize * index_conti2.size / 1024**2
# print(f'全随机 IO 的速度为 {IO_rate:.1f} MB/s')
