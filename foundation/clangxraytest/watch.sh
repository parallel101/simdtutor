cd build

while true; do
    result="$(inotifywait -e create . 2>&- | grep xray-log | awk '{print $3}')"
    if [ "x$result" == x ]; then
        continue
    fi
    inotifywait -e close_write "$result" >&- 2>&-
    if [ ! -f "$result" ]; then
        continue
    fi
    echo "Processing: $result"
    program="$(echo "$result" | awk -F. '{print $2}')"

    llvm-xray graph "$result" --instr_map="$program" --color-edges=count --edge-label=count --color-vertices=sum --vertex-label=sum --keep-going | dot -Tpng -x > trace.png
    display trace.png &
    llvm-xray convert "$result" --symbolize --instr_map="$program" --output-format=trace_event | gzip > trace.txt.gz
    rm "$result"
done
