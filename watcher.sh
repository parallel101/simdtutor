#!/bin/bash
# Requirements: pacman -S inotify-tools gtest benchmark gcc python sed

file="main.cpp"
out="/tmp/watched.s"
bench="/tmp/benched.cpp"
result="/tmp/result.json"
record="/tmp/record.json"
cflags="-masm=intel -mavx2 -mfma -O3 -fopenmp"
cc="g++"

lastmd5=
set -o pipefail
while inotifywait "$file" -o /tmp/.W$$.ionotify.log -e close --timefmt '%y/%m/%d %H:%M:%S' --format '%T %w %f %e' || true
do
    newmd5=$(md5sum "$file" | cut -d' ' -f1)
    if [ "x$newmd5" == "x$lastmd5" ]
    then
        echo "file not changed ($newmd5), ignoring..."
    else
        lastmd5="$newmd5"
        rm -f /tmp/.W$$.gcc-error.log
        echo '-- Compiling...'
        cat "$file" | sed '/\#include <benchmark\/benchmark.h>/d; /^static void \w\+(benchmark::State/,/^BENCHMARK(\w\+)/d' | sed '/\#include <gtest\/gtest.h>/d; /^TEST(\w\+, \w\+) {$/,/^}$/d' | "$cc" -S -x c++ /dev/stdin $cflags -o /dev/stdout 2> /tmp/.W$$.gcc-error.log | sed 's/^\t\.\(align\|byte\|short\|long\|float\|quad\|rept\|string\|ascii\|asciz\)\t/  \.\1  /g' | sed '/^\t\..*[^:]$/d' | sed 's/\t/  /g' | sed '$a ; '"$(date +'Compiled at %Y\/%m\/%d %H:%M:%S')" | tee "$out"
        if [ -s /tmp/.W$$.gcc-error.log ]
        then
            cat /tmp/.W$$.gcc-error.log >> "$out"
        else
            echo '-- Testing...'
            rm -f /tmp/.W$$.executable.out
            cat "$file" | sed '/\#include <benchmark\/benchmark.h>/d; /^static void \w\+(benchmark::State/,/^BENCHMARK(\w\+)/d' | "$cc" -x c++ /dev/stdin $cflags -o /tmp/.W$$.executable.out -lgtest -lgtest_main 2> /tmp/.W$$.gcc-error.log
            if [ -f /tmp/.W$$.executable.out ]
            then
                /tmp/.W$$.executable.out | tee "$bench"
                if [ x"$?" == x0 ]
                then
                    sed -n '/^\/\/ BEGIN CODE$/,/^\/\/ END CODE$/p' "$file" | sed '1d; $d' | python .watcher-helper.py "$result" "$record" "$bench"
                    echo '-- Benchmarking...'
                    rm -f /tmp/.W$$.executable.out
                    cat "$file" | sed '/\#include <gtest\/gtest.h>/d; /^TEST(\w\+, \w\+) {$/,/^}$/d' | "$cc" -x c++ /dev/stdin $cflags -o /tmp/.W$$.executable.out -lbenchmark -lbenchmark_main 2> /tmp/.W$$.gcc-error.log
                    if [ -f /tmp/.W$$.executable.out ]
                    then
                        /tmp/.W$$.executable.out --benchmark_min_time=0.2s --benchmark_repetitions=5 --benchmark_out="$result" 2>&1 | tee "$bench"
                        if [ x"$?" == x0 ]
                        then
                            # sed '/^\#include <benchmark\/benchmark.h>$/d; /^\#include <gtest\/gtest.h>$/d' "$file" | sed -n '/^\(\#include\|namespace \w\+ =\|using namespace \)/p' | sed '$a' > "$bench"
                            sed -n '/^\/\/ BEGIN CODE$/,/^\/\/ END CODE$/p;' "$file" | sed '1d; $d' | python .watcher-helper.py "$result" "$record" > "$bench"
                        fi
                    else
                        cat /tmp/.W$$.gcc-error.log | tee "$bench"
                    fi
                fi
            else
                cat /tmp/.W$$.gcc-error.log | tee "$bench"
            fi
        fi
    fi
done
