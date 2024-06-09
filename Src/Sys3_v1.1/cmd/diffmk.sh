if test -z "$3" -o "$3" = "$1" -o "$3" = "$2"; then
	echo "usage: name1 name2 name3 -- name3 must be different"
	exit
fi
diff -e $1 $2 | (sed -n -e '
/[ac]$/{
	p
	a\
.mc |
: loop
	n
	/^\.$/b done1
	p
	b loop
: done1
	a\
.mc\
.
	b
}

/d$/{
	s/d/c/p
	a\
.mc *\
.mc\
.
	b
}'; echo "w $3") | ed - $1
