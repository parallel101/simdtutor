import json
import sys
import os

jsonfile = sys.argv[1]
recfile = sys.argv[2]

content = sys.stdin.read()

with open(jsonfile, 'r') as f:
    bench = json.load(f)
    freq = bench.get('mhz_per_cpu', 4000) * 1000000
    result = [data for data in bench['benchmarks'] if data.get('aggregate_name', '') == 'median'][0]

if os.path.exists(recfile):
    with open(recfile, 'r') as f:
        records = json.load(f)
else:
    records = {}

records[content] = result

with open(recfile, 'w') as f:
    json.dump(records, f)

print('#if _')
print()
for content, result in records.items():
    if 'items_per_second' in result:
        print('//', round(result['real_time']), 'ns', round(freq / result['items_per_second'], 2), 'cpi')
    else:
        print('//', round(result['real_time']), 'ns')
    print(content.strip())
    print()
print('#endif')
