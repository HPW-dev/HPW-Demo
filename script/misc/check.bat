# запускать из root
cls
cppcheck -j15 -DNDEBUG -DWINDOWS ^
--enable=warning,performance,portability,information,missingInclude ^
-I"src" -I"test" -I"tool" --language=c++ --inconclusive --library=windows ^
--platform=win64 --std=c++20 -i"info" -i"thirdparty" -I"thirdparty" ^
--suppress=missingIncludeSystem --check-level=exhaustive ^
--quiet . 2> cppcheck.txt
code cppcheck.txt
