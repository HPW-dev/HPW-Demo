:: запускать из root
cpplint --root=. --repository=. ^
--exclude=src\graphic\image\table.* ^
--filter=-whitespace,-runtime/references,-whitespace/comments,-legal/copyright,-readability/todo,-readability/namespace,-runtime/explicit ^
--recursive src\ test\ tool\ 2> cpplint.txt
code cpplint.txt
