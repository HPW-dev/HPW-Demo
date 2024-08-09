:: запускать из root
cpplint --root=. --repository=. ^
--filter=-whitespace,-runtime/references,-whitespace/comments,-legal/copyright,-readability/todo,-readability/namespace,-runtime/explicit,-build/c++11,-build/include_subdir,-build/namespaces ^
--recursive src\ 2> cpplint.txt
code cpplint.txt
