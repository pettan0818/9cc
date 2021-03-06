#! /bin/bash
try() {
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	gcc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" != "$expected" ]; then
		echo "Case $input : $expected expected, but got $actual"
		exit 1
	fi
}

try 0 0
try 42 42
try 21 '5+20-4'
try 41 ' 12 +    34         - 5           '
try 10 '2*5'
try 10 '20/2'
try 1 '1/1'
try 3 '(10-4)/2'

echo "DONE"
