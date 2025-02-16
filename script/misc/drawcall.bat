@echo off
:: запускать из root
del gmon.svg 2> NUL
gprof build\HPW.exe | gprof2dot --edge-thres=0.5 --node-thres=0.5 -s -w | dot -Tsvg -o gmon.svg
gmon.svg 2> NUL
