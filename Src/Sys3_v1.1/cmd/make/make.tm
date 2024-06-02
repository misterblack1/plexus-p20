'\"	@(#)/usr/src/cmd/make/make.tm	3.2
.po 5
.nr Pt 0
.ND "July 1, 1979"
.TL 49579-210 40320-1
An Augmented Version of Make
.AU "E. G. Bradford" EGB CB 5255 2804 1C-249
.TM 79-5255-1 5255-790701.01MF
.AS
.P
This paper describes an augmented version
of the \fBmake\fR command supplied with UNIX/TS.
With one debatable exception, this version
is completely upward compatible with the
UNIX/TS version.
In this paper, I describe and give examples only
of additional features. The reader is
assumed to have read or have available the
original \fBmake\fR paper by S. I. Feldman.\*F
.FS
Feldman, S. I., MAKE, A Program for Maintaining Computer Programs,
Computing Science Technical Report Number 57
.FE
Further developments for \fBmake\fR are also discussed.
.AE
.OK "make" "program building"
.MT 1
.H 1 "INTRODUCTION"
This paper will describe in some detail
an augmented version of the \fBmake\fR program
now running on the Columbus operating systems group
UNIX machine.
I will give some justification for the chosen
implementation and describe with examples
the additional features.
.H 1 "MOTIVATION FOR THE CURRENT IMPLEMENTATION"
The \fBmake\fR program was originally written
for personal use by S. I. Feldman.
However, it became popular on the
research UNIX machine and a more formal version
was built and installed for general use.
For the purpose of maintaining executable programs
in the Center 127 environment, it has served this purpose
well.
Further developments of \fBmake\fR have not been necessary
and thus have not been done.
.P
In Columbus \fBmake\fR was perceived as an excellent
program administrative tool and has been used
extensively in at least one project (NOCS)
for over two years. 
However, \fBmake\fR had many shortcomings:
handling of libraries was tedious;
handling of the SCCS filename format was difficult or impossible;
environment variables are completely ignored by \fBmake\fR;
and the general lack of ability to maintain files in a remote directory.
These shortcomings hindered large scale use of \fBmake\fR
as a program support tool.
.P
There were at least two avenues  for solving the
above problems. The first was a complete redesign.
This would probably mean a new syntax and of necessity
force new makefiles to be incompatible with old ones.
The advantages however would be a more general implementation
that would be \fIgrowable\fR. This point of view was not
chosen because of the compatibility problem.
The second and more tame point of view was to modify
the current implementation to handle the problems above.
This point of view had the advantage that if done carefully
it could be completely upward compatible. It was this second
avenue which was chosen.
.P
The additional features are within the original syntactic framework
of \fBmake\fR and few if any new syntactical entities have been introduced.
A notable exception is the \fIinclude\fR file capability.
Further, most of the additions result in a "Don't know how to make ..."
message from the old version of \fBmake\fR.
.H 1 "THE ADDITIONAL FEATURES"
The following paragraphs describe with examples the additional features
of the \fBmake\fR program.
In general, the examples are taken from existing
\fImakefiles\fR.  Also, the appendices are working
\fImakefiles\fR.
.H 2 "The Environment Variables"
Environment variables are read and added to the
macro definitions each time \fBmake\fR executes. Precedence
is a prime consideration in doing this properly.  Thus,
if the environment variable \fBCC\fR is set to \fBocc\fR,
does it override the command line? Does it override the
definition in the makefile?
To answer these questions I need to describe the order
in which \fBmake\fR does the macro assignments.
.P
First, a new macro, \fBMAKEFLAGS\fR, must be described.
\fBMAKEFLAGS\fR is maintained by \fBmake\fR. It
It is defined as the collection of
all input flag arguments into a string (without the minus sign).
It is exported, and
thus accessible to further invocations of \fBmake\fR.
Command line flags and assignments in the "makefile"
update \fBMAKEFLAGS\fR.
Thus, to describe how the environment
interacts with \fBmake\fR, we also need to consider
the \fBMAKEFLAGS\fR macro (environment variable).
.P
When executed \fBmake\fR assigns macro definitions in the following
order:
.AL 1 12
.LI
read the \fBMAKEFLAGS\fR environment variable. If it
is not present or null, the internal \fBmake\fR variable
\fBMAKEFLAGS\fR is set to the null string.
Otherwise, each letter in \fBMAKEFLAGS\fR is assumed to
be an input flag argument and is processed as such.
(The only exceptions are the "-f", "-p", and "-r" flags.)
.LI
read and set the input flags from the command line.
The command line adds to the previous settings from
the \fBMAKEFLAGS\fR environment variable.
.LI
read macro definitions from the command line.
These are made
\fInot resettable\fR. Thus any further assignments to these
names are ignored.
.LI
read the internal list of macro definitions.
These are found
in the file \fIfiles.c\fR of the source for \fBmake\fR.
(See Appendix A for the complete makefile which represents
the internally defined macros and rules.)
They give default definitions for the C compiler
(CC=cc), the assembler (AS=as), etc.
.LI
read the environment. The environment variables are
treated as macro definitions and marked as \fIexported\fR
(in the shell sense). Note, \fBMAKEFLAGS\fR will get read
again and set again.
However, since it is not an internally
defined variable (in \fIfiles.c\fR), this has the effect
of doing the same assignment twice. The exception
to this is when \fBMAKEFLAGS\fR is assigned on the
command line.
(The reason it was read previously, was to be able to turn
the debug flag on before anything else was done.)
.LI
read the \fImakefile\fR(s). The assignments in the \fImakefile(s)\fR
will override
the environment. This order was chosen so when
one reads a makefile and executes \fBmake\fR one
knows what to expect.
That is, one gets what one sees unless the "-e" flag is used.
The "-e" is an additional command line flag which
tells \fBmake\fR  to
have the environment override the \fImakefile\fR assignments.
Thus if \fBmake -e ...\fR is typed, the variables
in the environment override the definitions in the \fImakefile\fR.
(Note, there is no way to override the command
line assignments.)
Also note that if \fBMAKEFLAGS\fR is assigned it will override
the environment.
(This would be useful for further invocations of \fBmake\fR
from the current "makefile".)
.LE
.P
This description may be hard to follow. No doubt it is. It might
be more useful to list the precedence of assignments.
Thus, in order from least binding to most binding, we have:
.AL 1 12 1
.LI
internal definitions (from \fIfiles.c\fR)
.LI
environment
.LI
"makefile(s)"
.LI
command line
.LE
.P
The "-e" flag has the effect of changing the order to:
.AL 1 12 1
.LI
internal definitions (from \fIfiles.c\fR)
.LI
"makefile(s)"
.LI
environment
.LI
command line
.LE

This ordering is general enough to allow
a programmer to define a "makefile" or set of "makefiles"
whose parameters are dynamically definable.
.H 2 "Recursive Makefiles"
One other useful feature was added to \fBmake\fR concerning
the environment and recursive invocations.
If the sequence "$(MAKE)" appears anywhere in a shell
command line, the line will be executed even if
the "-n" flag is set. Since the "-n" flag is exported across
invocations of \fBmake\fR, (through the \fBMAKEFLAGS\fR variable)
the only thing which
will actually get executed is the \fBmake\fR command
itself. This feature is useful when a hierarchy
of \fImakefile(s)\fR describes a set of software
subsystems.  For testing purposes, \fBmake -n ...\fR
can be executed and everything that would have been done
will get printed out;
including output from lower level invocations of \fBmake\fR.
.H 2 "Format of Shell commands within \fBmake\fR"
\fBMake\fR remembers embedded newlines and tabs in shell
command sequences. Thus, if the programmer puts a
\fIfor\fR loop in the makefile with indentation, when
\fBmake\fR prints it out, it still has the indentation
and the backslashes in it. The output is still pipe-able to
the shell and is readable.
This is obviously a cosmetic change; no new
functionality is gained.
.H 2 "Archive Libraries"
\fBMake\fR has an intelligent interface to archive libraries.
Due to a lack of documentation,
most people are probably not aware of the current syntax
of addressing members of archive libraries.
The UNIX/TS version allows a user to name a member of a library
in the following manner:
.nf

	lib(object.o)

or

	lib((_localtime))

.fi
where the second method actually refers to an
entry point of an object file within the library.
(\fBMake\fR looks through the library,
locates the entry point and translates it to the correct
object file name.)
.P
To use the UNIX/TS \fBmake\fR to maintain
an archive library, the following type of \fBmakefile\fR is
required:
.nf

	lib::	lib(ctime.o)
		$(CC) -c -O ctime.c
		ar rv lib ctime.o
		rm ctime.o
	lib::	lib(fopen.o)
		$(CC) -c -O fopen.c
		ar rv lib fopen.o
		rm fopen.o
	...and so on for each object ...

.fi
This is tedious and error prone. Obviously,
the command sequences for adding a C file to a library
are the same for each invocation, the filename being the only
difference each time. (This is true in most cases.)
Similarly for assembler and YACC and LEX files.
.P
The current version gives the user access to a rule for
building libraries. The "handle" for the rule is the
".a" suffix. Thus a ".c.a" rule is the rule for
compiling a C source file and adding it to the library
and removing the ".o" cadaver.
Similarly, the ".y.a", the ".s.a" and the ".l.a" rules
rebuild YACC, assembler, and LEX files respectively.
The current archive rules defined internally are
".c.a", ".c~.a", and .s~.a". (The wiggle (~) syntax will
be described shortly.)
The user may define in his makefile any other rules
he may need.
.P
The above two-member library is then maintained with the
following shorter makefile:
.nf

	lib:	lib(ctime.o) lib(fopen.o)
		@echo lib up-to-date.

.fi
The internal rules are already defined to complete the
preceding library maintenance.
The actual ".c.a" rules is as follows:
.nf

	.c.a:
		$(CC) -c $(CFLAGS) $<
		ar rv $@ $*.o
		rm -f $*.o

.fi
Thus, the "$@" macro is the ".a" target (\fBlib\fR) and the "$<" and "$*"
macros are set to the out-of-date C file and the filename
sans suffix respectively (\fBctime.c\fR and \fBctime\fR).
The "$<" macro (in the preceding rule)
could have been changed to "$*.c".
.P
It might be useful to go into some detail about exactly
what \fBmake\fR thinks about when it sees  the construction
.nf

	lib:	lib(ctime.o)
		@echo lib up-to-date

.fi
Assume the object in the library is out of date with respect
to \fIctime.c\fR. Also, there is no \fIctime.o\fR file.
To itself, \fBmake\fR thinks
.AL 1 12
.LI
I must do \fIlib\fR.
.LI
To do \fIlib\fR, I must do each dependent of \fIlib\fR.
.LI
I must do \fIlib(ctime.o)\fR.
.LI
To do \fIlib(ctime.o)\fR I must do each dependent
of \fIlib(ctime.o)\fR. (There are none).
.LI
Use my internal rules to try to build \fIlib(ctime.o)\fR.
(There is no explicit rule.)
Note that \fIlib(ctime.o)\fR has a parenthesis, '(', in the
name so I identify the target suffix as ".a".
(This is the key. There is no explicit ".a" at the end of the
\fIlib\fR library name. The parenthesis forces the ".a" suffix.)
In this sense, the ".a" is hardwired into \fBmake\fR.
.LI
Since I am working on a ".a" suffix I must break
the name \fIlib(ctime.o)\fR up into \fIlib\fR and \fIctime.o\fR.
I now define the two macros "$@" (=\fIlib\fR) and "$*" (=\fIctime\fR).
.LI
Look for a rule ".X.a" and a file "$*.X".
The first "X" (in the .SUFFIXES list) which fulfills these conditions is "c" so
the rule is ".c.a" and the file is \fIctime.c\fR.
I set "$<" to be \fIctime.c\fR and execute the rule.
(In fact, \fBmake\fR must then do "ctime.c". However,
the search of the current directory yields no other
candidates, whence, the search ends.)
.LI
The library has been updated. I must now
do the rule associated with the "lib:" dependency; namely
.nf

	echo lib up-to-date

.fi
.LE
It should be noted that to let \fIctime.o\fR have dependencies
the following syntax is required:
.nf

	lib(ctime.o):	$(INCDIR)/stdio.h

.fi
Thus, explicit references to ".o" files are unnecessary. There is also
a new macro for referencing the archive member name when this form
is used. "$%" is evaluated each time "$@" is evaluated. If there
is no current archive member, "$%" is null. If an archive member
exists, then "$%" evaluates to the expression between the parenthesis.
.P
An example makefile for a larger library is given in Appendix B.
The reader will note also, that there are no \fIlingering\fR
"*.o" files left around.
The result is a library maintained directly from the
source files (or more generally from the SCCS files!).
.H 2 "SCCS File Names -- The Wiggle"
The syntax of \fBmake\fR does not directly
permit referencing of prefixes. For most types
of files on UNIX machines this is acceptable since
nearly everyone uses a suffix to distinguish different
types of files. SCCS files are the exception.
Here, "s." precedes the filename part of the complete pathname.
.P
To allow \fBmake\fR easy access
to the prefix "s." requires either
a redefinition of the rule naming syntax of \fBmake\fR or
a \fItrick\fR.
I used a trick.
The trick is to use the wiggle (~) as an identifier
of SCCS files. Hence, ".c~.o" refers to the rule
which transforms an SCCS C source file into an object.
Specifically, the internal rule is:
.nf

	.c~.o:
		$(GET) $(GFLAGS) -p $< > $*.c
		$(CC) $(CFLAGS) -c $*.c
		-rm -f $*.c

.fi
(The motivation for the "-p" flag associated
with the $(GET) command above is obscure and debatable.
For the purpose of this discussion
we can assume the following apparently equivalent rule:
.nf

	.c~.o:
		$(GET) $(GFLAGS) $<
		$(CC) $(CFLAGS) -c $*.c
		-rm -f $*.c

.fi
Suffice it to say, that when doing a generic
build, the "make" should not fail because a file happens
to be left out in the SCCS directory.)
.P
Thus the wiggle appended to any suffix transforms the
file search into an SCCS filename search with the actual suffix
named by the dot and all characters up to (but not including)
the wiggle.
.P
The following SCCS suffixes are internally defined:
.nf

		.c~
		.y~
		.s~
		.sh~
		.h~

.fi

The following rules involving SCCS transformations
are internally defined:
.nf

		.c~:
		.sh~:
		.c~.o:
		.s~.o:
		.y~.o:
		.l~.o:
		.y~.c:
		.c~.a:
		.s~.a:
		.h~.h:

.fi
.P
Obviously, the user can define other rules and suffixes which
may prove useful. The \fIwiggle\fR gives him a handle on
the SCCS filename format so that this is possible.
.H 2 "The Null Suffix"
In the UNIX/TS source code, there are many
commands which consist of a single source file.
\fIcat.c\fR, \fIdd.c\fR, \fIecho.c\fR, and \fIdate.c\fR are a few
well known ones. It seemed a pity to maintain an object
of such files for \fBmake\fR's pleasure.
The current implementation supports single suffix rules,
or if one prefers, a null suffix. Thus, to maintain the
above files one needs a \fImakefile\fR of the following form:
.nf

	.c:
		$(CC) -n -O $< -o $@

.fi
(In fact, this ".c:" rule is internally
defined so no \fImakefile\fR is necessary at all!)
One then need only type
.nf

	make cat dd echo date

.fi
and all four C source files are passed through
the above shell command line associated with the ".c:" rule.
The internally defined single suffix
rules are:
.nf

		.c:
		.c~:
		.sh:
		.sh~:

.fi

Others may be added in the \fImakefile\fR by the user.
.H 2 "Include Files"
\fBMake\fR has an include file capability. If the string "include"
appears as the first seven letters of a line in a \fImakefile\fR
and is followed
by a blank or a tab
the following string
is assumed to be a file name which
the current invocation of \fBmake\fR
will read.
The file descriptors are stacked for reading
\fIinclude\fR files so no more that about sixteen levels of
nested includes is supported.
Does that bother anyone?
.H 2 "Invisible SCCS \fIMakefiles\fR"
SCCS \fImakefiles\fR are invisible to \fBmake\fR.
That is, if \fBmake\fR is typed and only a file named
\fIs.makefile\fR exists, \fBmake\fR will \fIget(I)\fR it,
read it and remove it.
Likewise for "-f"
arguments and \fIinclude\fR files.
.H 2 "Dynamic Dependency Parameters"
A new dependency parameter has been defined.
It has meaning only on the dependency line in a makefile.
It is "$$@".
"$$@" refers to the current "thing" at the left of
the colon (which is "$@"). Also the form "$$(@F)" exists
which allows access to the file part of "$@".
Thus, in the following:
.nf

	cat:	$$@.c

.fi
the dependency is translated at execution time to
the string "cat.c". This is useful for building a whole
raft of executable files, each of which has only one source file.
For instance the UNIX/TS command directory would have a \fImakefile\fR
like:
.nf

	CMDS = cat dd echo date cc cmp comm ar ld chown

	$(CMDS):	$$@.c
		$(CC) -O $? -o $@

.fi
Obviously, this is a subset of all the single file programs.
For multiple file programs, usually a directory is
allocated and a separate \fImakefile\fR is made.
For any particular file which has a peculiar
compilation procedure, a specific entry must be made in
the \fImakefile\fR.
.P
The second useful form of the dependency parameter is "$$(@F)".
It represents the filename part of "$$@". Again,
it is evaluated at execution time. Its usefulness
shows up when trying to maintain the "/usr/include"
directory from a makefile in the "/usr/src/head" directory.
Thus the "/usr/src/head/makefile" would look like:
.nf

	INCDIR = /usr/include

	INCLUDES = \\
		$(INCDIR)/stdio.h \\
		$(INCDIR)/pwd.h \\
		$(INCIDR)/dir.h \\
		$(INCDIR)/a.out.h

	$(INCLUDES):	$$(@F)
		cp $? $@
		chmod 0444 $@

.fi
would completely maintain the "/usr/include" directory
whenever one of the above files in "/usr/src/head" was updated.
.H 2 "Extensions of $*, $@, and $<"
The internally generated macros "$*", "$@", and "$<"
are useful generic terms for current targets
and out-of-date relatives.
To this list has been added the following related
macros: "$(@D)", "$(@F)", "$(*D)", "$(*F)", "$(<D)",
and "$(<F)".
The "D" refers to the directory part of the
single letter macro.
The "F" refers to the filename part of the
single letter macro.
These additions are
useful when building hierarchical makefiles.
They allow access to directory names for purposes
of using the `cd' command of the shell.
Thus, a shell command can be:
.nf

	cd $(<D); $(MAKE) $(<F)

.fi
An interesting example of the use of these features can
be found in the set of \fImakefiles\fR which maintain
the Columbus UNIX operating system.
They may be seen in Appendix C.
.H 2 "Output Translations"
Macros in shell commands can now be translated when evaluated.
The form is as follows:
.nf
	$(macro:string1=string2)
.fi
The meaning is as follows: $(macro) is evaluated.
For each appearance of string1 in the evaluated macro
string2 is substituted. The meaning of finding string1 in $(macro)
is that the evaluated $(macro) is considered  as a bunch of strings
each delimited by whitespace (blanks or tabs).
Thus the occurrence of string1 in $(macro) means that a regular expression
of the following form has been found:
.nf
	.*<string1>[TAB|BLANK]
.fi
This particular form was chosen because \fBmake\fR usually concerns itself
with suffixes. A more general regular expression match could be implemented
if the need arises.
The usefulness of this type of translation occurs when maintaining
archive libraries. Now, all that is necessary is to accumulate the
out-of-date members and write a shell script which can handle all the
C programs (i.e. those file ending in ".c"). Thus the following
fragment will optimize the executions of make for maintaining and
archive library:
.nf
	$(LIB):	$(LIB)(a.o) $(LIB)(b.o) $(LIB)c.o)
		$(CC) -c $(CFLAGS) $(?:.o=.c)
		ar rv $(LIB) $?
		rm $?
.fi
Here, finally, is a legitimate use for the double colon. A dependency
of the preceding form would be necessary for each of the different
types of source files (suffices) which define the archive library.
These translations are added in an effort to make more general
use of the wealth of information which \fBmake\fR generates.
.H 2 "The Test \fBmakefile\fR"
A test \fImakefile\fR was written to explicitly
test each of the new features. When shipped to a
new machine and compiled, \fBmake\fR is assumed to
work if the test \fImakefile\fR executes without error.
.H 1 "INCOMPATIBILITIES WITH OLD VERSION"
The only known incompatibility with UNIX/TS \fBmake\fR
is seen in the following example makefile:
.nf

	all:	cat dd

	dd:	dd.o
		$(CC) -o $@ $?

	cat:	cat.o
		$(CC) -o $@ $?

.fi
UNIX/TS \fBmake\fR
will not complain that \fIall\fR does not have a rule associated
with it. The current version will.  The current version is
a strict interpretation of the original paper by
S. I. Feldman
and as such is described by his paper. The UNIX/TS \fBmake\fR
is wrong (according to Feldman's paper) but people have
learned (through trial and error) to use it in this fashion
and would resist the change.
The differences amount to one line of code in the
file \fIdoname.c\fR which is noted in my source code.
Furthermore, the "-b" option tells \fBmake\fR to revert
to the old method, whereby old makefiles can be run with
this new version of \fBmake\fR.
Any other differences are unintentional.
.H 1 "FUTURE DEVELOPMENTS"
Further developments of the \fBmake\fR program that have
been considered include redefinition of the colon, some
other type of comparison that "newer versus older",
searching out include files, and looking for source files
in "other" directories.
I will try to address each of these features.
.P
Redefinition of the colon would effectively give
the user the ability to provide \fBmake\fR with
information from a file, other than the time.
Within the current syntax of \fBmake\fR this does not
seem too difficult:
.nf

	make :=program ...

.fi
However, I can not see (nor has anyone pointed out to me)
a use for such a feature.
This would apparently be related to the second item above;
namely, a different comparison
other than the difference between
two long integers, (the comparison by time).
The basic problem, as I see it, is that
when it comes time to do the ":" thing,
\fBmake\fR does not know what information to
pass to the program. \fBMake\fR looks at
each dependency individually and not in subsets.
This prevents passing information like:
.nf

	program cat cat.c

.fi
to a user defined \fIcolon\fR routine because \fBmake\fR
does not know both \fIcat\fR and \fIcat.c\fR at the same time.
(There is an interesting "hidden" variable in
this version of \fBmake\fR: "$!". It represents the
current predecessor tree. In the following \fImakefile\fR:
.nf

	all:	cat
		@echo cat up-to-date

	cat:	cat.c
		echo $!

.fi
when the "echo $!" is executed, "$!" evaluates to
.nf

	cat.c cat all

.fi
which is not \fIall\fR that useful! Further, it occasionally
prints a message
.nf

	$! nulled, predecessor circle

.fi
This message means that the predecessors of a file
are circular. The actual evaluation
of the "$!" macro was aborted, and its value set to null. Otherwise
there is no effect.)
.P
The searching out of include files has been mentioned
many times as an improvement. This would require \fBmake\fR
to look through every line of every source file
mentioned in the makefile every time it is executed.
This would slow \fBmake\fR down to a slow crawl and
slowly defeat its usefulness.
.P
Having \fBmake\fR look in other directories for
file entries sounds useful.
However, interesting problems arise when this is considered.
What if the file in a remote directory is
out of date with respect to a file in the current directory?
Does \fBmake\fR rebuild the remote file?
(The user may not have write permission in that directory.)
Also, how are the shell commands parameterized
to be able to "see" the remote files.
Further, how does \fBmake\fR (or the programmer) guarantee
the resulting target file is in the remote
directory? (CC leaves the object in the current directory, which
would mean that part of a command line might have to refer to
a remote file while the rest of it would refer to
a relative in the current directory.)
I do not deny the usefulness of using remote directories,
but cannot see a consistent solution to locating results.
(I would be glad to here from anyone who
can clear up the mess.)
.H 1 CONCLUSION
The development described herein, produced a version of \fBmake\fR
which now serves the Columbus operating system group for
maintenance of all of the UNIX source files.
The \fImakefiles\fR supplied from the UNIX/TS
support group are edited slightly (for the annoying incompatibility
described above) and used intact. When a large improvement
can be made, they are rewritten. When no makefile exists,
(C library and
most of the "cmd" directory)
a \fImakefile\fR is written.
There are no "*.rc" files left in the source.
This gives a single interface to rebuilding parts
or all of the UNIX software.
.P
I feel that although the size of \fBmake\fR has grown from
.nf

	16320+3772+6352 = 26444b = 063514b

.fi
on the Center 127 machine to
.nf

	21632+4850+8748 = 35230b = 0104636b

.fi
on the Columbus machine, the trade of size for
functionality is worthwhile.
The unfortunate by product of such a development
is that there are two versions of the
\fBmake\fR program.
I leave it to the reader to decide on the merits or lack of same
on this issue.
.SG egb
.NS 4
Appendix A-Internal Definitions
Appendix B-Example Library Makefile
Appendix C-Example Recursive Use of Makefiles
.NE
.bp
.DS 2
APPENDIX A
.DE
.P
The following \fImakefile\fR
will exactly reproduce the internal
rules of the current version of
\fBmake\fR. Thus if \fBmake -r ...\fR
is typed and a \fImakefile\fR includes
this \fImakefile\fR the results would be identical
to excluding the "-r" option and the \fIinclude\fR
line in the makefile.
.nf

	#	LIST OF SUFFIXES
	.SUFFIXES: .o .c .c~ .y .y~ .l .l~ .s .s~ .sh .sh~ .h .h~
	
	#	PRESET VARIABLES
	MAKE=make
	YACC=yacc
	YFLAGS=
	LEX=lex
	LFLAGS=
	LD=ld
	LDFLAGS=
	CC=cc
	CFLAGS=-O
	AS=as
	ASFLAGS=
	GET=get
	GFLAGS=
	
	#	SINGLE SUFFIX RULES
	.c:
		$(CC) -n -O $< -o $@
	.c~:
		$(GET) $(GFLAGS) -p $< > $*.c
		$(CC) -n -O $*.c -o $*
		-rm -f $*.c
	.sh:
		cp $< $@
	.sh~:
		$(GET) $(GFLAGS) -p $< > $*.sh
		cp $*.sh $*
		-rm -f $*.sh
	
	#	DOUBLE SUFFIX RULES
	.c.o:
		$(CC) $(CFLAGS) -c $<
	.c~.o:
		$(GET) $(GFLAGS) -p $< > $*.c
		$(CC) $(CFLAGS) -c $*.c
		-rm -f $*.c
	.c~.c:
		$(GET) $(GFLAGS) -p $< > $*.c
	.s.o:
		$(AS) $(ASFLAGS) -o $@ $<
	.s~.o:
		$(GET) $(GFLAGS) -p $< > $*.s
		$(AS) $(ASFLAGS) -o $*.o $*.s
		-rm -f $*.s
	.y.o:
		$(YACC) $(YFLAGS) $<
		$(CC) $(CFLAGS) -c y.tab.c
		rm y.tab.c
		mv y.tab.o $@
	.y~.o:
		$(GET) $(GFLAGS) -p $< > $*.y
		$(YACC) $(YFLAGS) $*.y
		$(CC) $(CFLAGS) -c y.tab.c
		rm -f y.tab.c $*.y
		mv y.tab.o $*.o
	.l.o:
		$(LEX) $(LFLAGS) $<
		$(CC) $(CFLAGS) -c lex.yy.c
		rm lex.yy.c
		mv lex.yy.o $@
	.l~.o:
		$(GET) $(GFLAGS) -p $< > $*.l
		$(LEX) $(LFLAGS) $*.l
		$(CC) $(CFLAGS) -c lex.yy.c
		rm -f lex.yy.c $*.l
		mv lex.yy.o $*.o
	.y.c :
		$(YACC) $(YFLAGS) $<
		mv y.tab.c $@
	.y~.c :
		$(GET) $(GFLAGS) -p $< > $*.y
		$(YACC) $(YFLAGS) $*.y
		mv y.tab.c $*.c
		-rm -f $*.y
	.l.c :
		$(LEX) $<
		mv lex.yy.c $@
	.c.a:
		$(CC) -c $(CFLAGS) $<
		ar rv $@ $*.o
		rm -f $*.o
	.c~.a:
		$(GET) $(GFLAGS) -p $< > $*.c
		$(CC) -c $(CFLAGS) $*.c
		ar rv $@ $*.o
		rm -f $*.[co]
	.s~.a:
		$(GET) $(GFLAGS) -p $< > $*.s
		$(AS) $(ASFLAGS) -o $*.o $*.s
		ar rv $@ $*.o
		-rm -f $*.[so]
	.h~.h:
		$(GET) $(GFLAGS) -p $< > $*.h

.fi
.bp
.DS 2
APPENDIX B
.DE
.P
The following library maintaining makefile
is from my current work on LSX. It
completely maintains the LSX operating system
library.
.nf

	#	@(#)/usr/src/cmd/make/make.tm	3.2
	LIB = lsxlib
	
	#	I have a banner printing pr.
	PR = vpr -b LSX
	
	INSDIR = /r1/flop0/
	INS = eval
	
	lsx::	$(LIB) low.o mch.o
		ld -x low.o mch.o $(LIB)
		mv a.out lsx
		@size lsx
	
	#	Here, I have used $(INS) as either `:' or `eval'.
	lsx::
		$(INS) 'cp lsx $(INSDIR)lsx && \\
			strip $(INSDIR)lsx  && \\
			ls -l $(INSDIR)lsx'
	
	print:
		$(PR) header.s low.s mch.s *.h *.c Makefile
	
	$(LIB): \\
		$(LIB)(clock.o) \\
		$(LIB)(main.o) \\
		$(LIB)(tty.o) \\
		$(LIB)(trap.o) \\
		$(LIB)(sysent.o) \\
		$(LIB)(sys2.o) \\
		$(LIB)(sys3.o) \\
		$(LIB)(sys4.o) \\
		$(LIB)(sys1.o) \\
		$(LIB)(sig.o) \\
		$(LIB)(fio.o) \\
		$(LIB)(kl.o) \\
		$(LIB)(alloc.o) \\
		$(LIB)(nami.o) \\
		$(LIB)(iget.o) \\
		$(LIB)(rdwri.o) \\
		$(LIB)(subr.o) \\
		$(LIB)(bio.o) \\
		$(LIB)(decfd.o) \\
		$(LIB)(slp.o) \\
		$(LIB)(space.o) \\
		$(LIB)(puts.o)
		@echo $(LIB) now up-to-date.
	
	
	.s.o:
		as -o $*.o header.s $*.s
	.o.a:
		ar rv $@ $<
		rm -f $<
	
	.s.a:
		as -o $*.o header.s $*.s
		ar rv $@ $*.o
		rm -f $*.o
	
	.PRECIOUS:	$(LIB)

.fi
.bp
.DS 2
APPENDIX C
.DE
.P
The following set of \fImakefiles\fR
maintain the UNIX operating system
for Columbus UNIX. They reside in the following
relative directories on the Columbus operating
systems group machine: "ucb", "ucb/os", "ucb/io", "ucb/head/sys".
Each one is named "70.mk".
The following command forces a complete rebuild of the operating
system:
.nf

	FRC=FRC make -f 70.mk

.fi
where the current directory is "ucb".
Here, I have used some of the conventions described
in A. Chellis' paper, \fIProposed Structure for UNIX/TS
and UNIX/RT Makefiles\fR (MF78-8234-73).
FRC is a convention for \fIF\fRo\fIRC\fRing
\fBmake\fR to completely rebuild a target starting
from scratch.
.bp
.DS 2
./ucb makefile
.DE
.nf

	#	@(#)/usr/src/cmd/make/make.tm	3.2
	#	ucb/70.mk makefile
	
	VERSION = 70
	
	DEPS = \\
		os/low.$(VERSION).o \\
		os/mch.$(VERSION).o \\
		os/conf.$(VERSION).o \\
		os/lib1.$(VERSION).a \\
		io/lib2.$(VERSION).a
	
	#	This makefile will re-load unix.$(VERSION) if any
	#	of the $(DEPS) is out-of-date wrt unix.$(VERSION).
	#	Note, it will not go out and check each member
	#	of the libraries. To do this, the FRC macro must
	#	be defined.

	unix.$(VERSION):	$(DEPS) $(FRC)
		load -s $(VERSION)
	
	$(DEPS):	$(FRC)
		cd $(@D); $(MAKE) -f $(VERSION).mk $(@F)
	
	all:	unix.$(VERSION)
		@echo unix.$(VERSION) up-to-date.
	
	includes:
		cd head/sys; $(MAKE) -f $(VERSION).mk
	
	FRC:	includes;

.fi
.bp
.DS 2
./ucb/os makefile
.DE
.nf

	
	#	@(#)/usr/src/cmd/make/make.tm	3.2
	#	ucb/os/70.mk makefile
	
	VERSION = 70
	
	LIB = lib1.$(VERSION).a
	COMPOOL=
	
	LIBOBJS = \\
		$(LIB)(main.o) \\
		$(LIB)(alloc.o) \\
		$(LIB)(iget.o) \\
		$(LIB)(prf.o) \\
		$(LIB)(rdwri.o) \\
		$(LIB)(slp.o) \\
		$(LIB)(subr.o) \\
		$(LIB)(text.o) \\
		$(LIB)(trap.o) \\
		$(LIB)(sig.o) \\
		$(LIB)(sysent.o) \\
		$(LIB)(sys1.o) \\
		$(LIB)(sys2.o) \\
		$(LIB)(sys3.o) \\
		$(LIB)(sys4.o) \\
		$(LIB)(sys5.o) \\
		$(LIB)(syscb.o) \\
		$(LIB)(maus.o) \\
		$(LIB)(messag.o) \\
		$(LIB)(nami.o) \\
		$(LIB)(fio.o) \\
		$(LIB)(clock.o) \\
		$(LIB)(acct.o) \\
		$(LIB)(errlog.o)
	
	
	ALL = \\
		conf.$(VERSION).o \\
		low.$(VERSION).o \\
		mch.$(VERSION).o \\
		$(LIB)
	
	all:	$(ALL)
		@echo \`$(ALL)\' now up-to-date.
	
	$(LIB)::	$(LIBOBJS)
	
	$(LIBOBJS):	$(FRC)
	
	FRC:
		rm -f $(LIB)
	
	clobber:	cleanup
		-rm -f $(LIB)
	
	clean cleanup:
	
	install:	all
	
	.PRECIOUS:	$(LIB)

.fi
.bp
.DS 2
./ucb/io makefile
.DE
.nf

	
	#	@(#)/usr/src/cmd/make/make.tm	3.2
	#	ucb/io/70.mk makefile
	
	VERSION = 70
	
	LIB = lib2.$(VERSION).a
	COMPOOL=
	
	LIB2OBJS = \\
		$(LIB)(mx1.o) \\
		$(LIB)(mx2.o) \\
		$(LIB)(bio.o) \\
		$(LIB)(tty.o) \\
		$(LIB)(malloc.o) \\
		$(LIB)(pipe.o) \\
		$(LIB)(dhdm.o) \\
		$(LIB)(dh.o) \\
		$(LIB)(dhfdm.o) \\
		$(LIB)(dj.o) \\
		$(LIB)(dn.o) \\
		$(LIB)(ds40.o) \\
		$(LIB)(dz.o) \\
		$(LIB)(alarm.o) \\
		$(LIB)(hf.o) \\
		$(LIB)(hps.o) \\
		$(LIB)(hpmap.o) \\
		$(LIB)(hp45.o) \\
		$(LIB)(hs.o) \\
		$(LIB)(ht.o) \\
		$(LIB)(jy.o) \\
		$(LIB)(kl.o) \\
		$(LIB)(lfh.o) \\
		$(LIB)(lp.o) \\
		$(LIB)(mem.o) \\
		$(LIB)(nmpipe.o) \\
		$(LIB)(rf.o) \\
		$(LIB)(rk.o) \\
		$(LIB)(rp.o) \\
		$(LIB)(rx.o) \\
		$(LIB)(sys.o) \\
		$(LIB)(trans.o) \\
		$(LIB)(ttdma.o) \\
		$(LIB)(tec.o) \\
		$(LIB)(tex.o) \\
		$(LIB)(tm.o) \\
		$(LIB)(vp.o) \\
		$(LIB)(vs.o) \\
		$(LIB)(vtlp.o) \\
		$(LIB)(vt11.o) \\
		$(LIB)(fakevtlp.o) \\
		$(LIB)(vt61.o) \\
		$(LIB)(vt100.o) \\
		$(LIB)(vtmon.o) \\
		$(LIB)(vtdbg.o) \\
		$(LIB)(vtutil.o) \\
		$(LIB)(vtast.o) \\
		$(LIB)(partab.o) \\
		$(LIB)(rh.o) \\
		$(LIB)(devstart.o) \\
		$(LIB)(dmc11.o) \\
		$(LIB)(rop.o) \\
		$(LIB)(ioctl.o) \\
		$(LIB)(fakemx.o)
	
	all:	$(LIB)
		@echo $(LIB) is now up-to-date.
	
	$(LIB)::	$(LIB2OBJS)
	
	$(LIB2OBJS):	$(FRC)
	
	FRC:
		rm -f $(LIB)
	
	clobber: cleanup
		-rm -f $(LIB) *.o
	
	clean cleanup:
	
	install:	all
	
	.PRECIOUS:	$(LIB)
	
	.s.a:
		$(AS) $(ASFLAGS) -o $*.o $<
		ar rcv $@ $*.o
		rm $*.o

.fi
.bp
.DS 2
./ucb/head/sys makefile
.DE
.nf

	
	#	@(#)/usr/src/cmd/make/make.tm	3.2
	#	ucb/head/sys/70.mk makefile
	
	COMPOOL = /usr/include/sys
	
	HEADERS = \\
		$(COMPOOL)/buf.h \\
		$(COMPOOL)/bufx.h \\
		$(COMPOOL)/conf.h \\
		$(COMPOOL)/confx.h \\
		$(COMPOOL)/crtctl.h \\
		$(COMPOOL)/dir.h \\
		$(COMPOOL)/dm11.h \\
		$(COMPOOL)/elog.h \\
		$(COMPOOL)/file.h \\
		$(COMPOOL)/filex.h \\
		$(COMPOOL)/filsys.h \\
		$(COMPOOL)/ino.h \\
		$(COMPOOL)/inode.h \\
		$(COMPOOL)/inodex.h \\
		$(COMPOOL)/ioctl.h \\
		$(COMPOOL)/ipcomm.h \\
		$(COMPOOL)/ipcommx.h \\
		$(COMPOOL)/lfsh.h \\
		$(COMPOOL)/lock.h \\
		$(COMPOOL)/maus.h \\
		$(COMPOOL)/mx.h \\
		$(COMPOOL)/param.h \\
		$(COMPOOL)/proc.h \\
		$(COMPOOL)/procx.h \\
		$(COMPOOL)/reg.h \\
		$(COMPOOL)/seg.h \\
		$(COMPOOL)/sgtty.h \\
		$(COMPOOL)/sigdef.h \\
		$(COMPOOL)/sprof.h \\
		$(COMPOOL)/sprofx.h \\
		$(COMPOOL)/stat.h \\
		$(COMPOOL)/syserr.h \\
		$(COMPOOL)/sysmes.h \\
		$(COMPOOL)/sysmesx.h \\
		$(COMPOOL)/systm.h \\
		$(COMPOOL)/text.h \\
		$(COMPOOL)/textx.h \\
		$(COMPOOL)/timeb.h \\
		$(COMPOOL)/trans.h \\
		$(COMPOOL)/tty.h \\
		$(COMPOOL)/ttyx.h \\
		$(COMPOOL)/types.h \\
		$(COMPOOL)/user.h \\
		$(COMPOOL)/userx.h \\
		$(COMPOOL)/version.h \\
		$(COMPOOL)/votrax.h \\
		$(COMPOOL)/vt11.h \\
		$(COMPOOL)/vtmn.h
	
	all:	$(FRC) $(HEADERS)
		@echo Headers are now up to date.
	
	$(HEADERS):	s.$$/
		$(GET) -s -p $(GFLAGS) $? > xtemp
		move xtemp 444 src sys $@
	
	FRC:
		rm -f $(HEADERS)
	
	.PRECIOUS:	$(HEADERS)
	
	
	.h~.h:
		get -s $<
	
	.DEFAULT:
		cpmv $? 444 src sys $@

.fi
.if t .CS 22
.if n .CS 24
