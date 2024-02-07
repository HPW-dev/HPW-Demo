@echo off
:: запускать из root
:: собирай с --coverage
del gcovr.html 2> NUL
gcovr -r . --html --html-details -o "gcovr.html"
gcovr.html 2> NUL
