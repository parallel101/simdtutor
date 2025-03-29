program=./build/main

set -e

rm -rf xray-log.main.*
XRAY_OPTIONS="patch_premain=true xray_mode=xray-basic verbosity=1" $program
llvm-xray convert --symbolize --instr_map=$program --output-format=trace_event xray-log.* | gzip > /tmp/trace.txt.gz
rm -rf xray-log.main.*
