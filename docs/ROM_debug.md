Discord user ewen (05/31/2024 4:18 AM)

Partial list of debug menu command characters and the code sections that handle them (there's about 10 more command characters I know about that I haven't yet tracked down exactly where they're handled).  In a few cases I've guessed what they do (eg V prints a version string), but mostly it's kind of vague.   A bunch of them just set internal state, a couple of them look like they do IPC (to the other processor), some of them print things out (eg, X seems to print the CSRs).

```
Known debug menu characters:

!, ", %, A, B, C, E, I, L, M, O, S, T, V, W, X, Z, a-i, m, o, q, s, t, v, w

---------------------------------------------------------------------------

! -- 00802ff6 -- stores word at D7 to 00c00860, returns to top of debug loop
" -- 00803006 -- stores byte at D7 to 00c00860, returns to top of debug loop
% -- 00803016 -- complex set of checks....

A -- 00802a4e -- if D4 == 0xc call function else if D7 = 1 else... print string
B -- 00802d82 -- something something IPC ???
C -- 00802db8 -- store D4 to 00c00802 or D4 to 000c006de, depending on D7
E -- 00802e32 -- do IPC ???, then zero memory
I -- 00802816 -- ??? (shared with 0x64 -- 'd' -- too) ???
L -- 00802e4a -- something something IPC ???
M -- 00802e98 -- ???
O -- 00802eb0 -- ???, store to memory, ???
S -- 00802ed6 -- print "jumping to", then ???
T -- 00802f04 -- ???
V -- 00802f5a -- print version
W -- 00802f36 -- ???
X -- 00802f72 -- print the CSRs
Z -- 00802f7c -- are you sure?  Then ???

a -- 00802714 -- chekck if D7, D4 != 0, if so use D6
b -- 0080272a -- calculate with D6, D7+D4, D7; print if result is non-zero?
c -- 0080275a -- D3 = 1 if D3 is 0; calculations, print string
...
f -- 0080275a -- D3 = 1 if D3 is 0; calculations, print string
i -- 0080275a -- D3 = 1 if D3 is 0; calculations, print string
...
```
