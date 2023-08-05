def calc(fps):
    IO_rate = fps * (8*64*64*64*9) / 1024**3
    print(f'IO 的速度为 {IO_rate:.1f} GB/s')
calc(387)
calc(508)
calc(977)
