```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

- Linux 得到 `build/libfast-vcycle-gpu.so`。
- Wendous 得到 `build\fast-vcycle-gpu.dll`。

```bash
python cloth3d.py -N 1024
```

产生 `results` 文件夹。
