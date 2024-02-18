:: запускать из root
cpplint --root=. --repository=. ^
--filter=-whitespace,-runtime/references,-whitespace/comments,-legal/copyright,-readability/todo,-readability/namespace,-runtime/explicit ^
--recursive src\ 2> cpplint.txt
code cpplint.txt
