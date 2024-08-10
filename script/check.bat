# запускать из root
cls
cppcheck -j15 -DNDEBUG -DWINDOWS ^
--enable=warning,performance,portability,information,missingInclude ^
-I"src" -I"test" --language=c++ --inconclusive --library=posix ^
--platform=win64 --std=c++20 -i"info" -i"thirdparty" ^
--suppress=missingIncludeSystem ^
--quiet . 2> cppcheck.txt
code cppcheck.txt
