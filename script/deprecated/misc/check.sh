# запускать из root
clear
cppcheck -j4 -DNDEBUG -DLINUX \
--enable=warning,performance,portability,information,missingInclude \
-I"src" -I"test" --language=c++ --inconclusive --library=posix \
--platform=unix64 --std=c++20 -i"info" -i"thirdparty" -i"ignore me" \
--quiet .
