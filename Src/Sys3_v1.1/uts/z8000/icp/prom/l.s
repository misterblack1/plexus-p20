/*
 * This source contains the trap and interrupt vectors. The object
 * produced by compiling the source must be the first object loaded
 * to form the system image since the psap must be on a 256 byte
 * boundary.
 */

#include "/p3/usr/include/icp/sioc.h"	/* icp specific */

/* externals */
.globl	start 
.globl	psa


psa:
jp	start

.word	SYS
.word	t_ei

.word	SYS	/* priveleged inst trap */
.word	t_pi

.word	SYS	/* syscall inst trap */
.word	t_sc

.word	0	/* segmentation trap (not used) */
.word	0

.word	SYS	/* non maskable interrupt */
.word	t_nmi

.word	SYS	/* non vectored interrupt */
.word	t_nvi

.word	SYS	/* vectored interrupts */

.word	i_xmit	/* id  0 */	/* sio 1 */
.word	i_bad	/* id  1 */
.word	i_ext	/* id  2 */
.word	i_bad	/* id  3 */
.word	i_rec	/* id  4 */
.word	i_bad	/* id  5 */
.word	i_spec	/* id  6 */
.word	i_bad	/* id  7 */

.word	i_xmit	/* id  8 */	/* sio 0 */
.word	i_bad	/* id  9 */
.word	i_ext	/* id 10 */
.word	i_bad	/* id 11 */
.word	i_rec	/* id 12 */
.word	i_bad	/* id 13 */
.word	i_spec	/* id 14 */
.word	i_bad	/* id 15 */

.word	i_xmit	/* id 16 */	/* sio 3 */
.word	i_bad	/* id 17 */
.word	i_ext	/* id 18 */
.word	i_bad	/* id 19 */
.word	i_rec	/* id 20 */
.word	i_bad	/* id 21 */
.word	i_spec	/* id 22 */
.word	i_bad	/* id 23 */

.word	i_xmit	/* id 24 */	/* sio 2 */
.word	i_bad	/* id 25 */
.word	i_ext	/* id 26 */
.word	i_bad	/* id 27 */
.word	i_rec	/* id 28 */
.word	i_bad	/* id 29 */
.word	i_spec	/* id 30 */
.word	i_bad	/* id 31 */

.word	i_xmit	/* id 32 */	/* sio 5 */
.word	i_bad	/* id 33 */
.word	i_ext	/* id 34 */
.word	i_bad	/* id 35 */
.word	i_rec	/* id 36 */
.word	i_bad	/* id 37 */
.word	i_spec	/* id 38 */
.word	i_bad	/* id 39 */

.word	i_xmit	/* id 40 */	/* sio 4 */
.word	i_bad	/* id 41 */
.word	i_ext	/* id 42 */
.word	i_bad	/* id 43 */
.word	i_rec	/* id 44 */
.word	i_bad	/* id 45 */
.word	i_spec	/* id 46 */
.word	i_bad	/* id 47 */

.word	i_xmit	/* id 48 */	/* sio 7 */
.word	i_bad	/* id 49 */
.word	i_ext	/* id 50 */
.word	i_bad	/* id 51 */
.word	i_rec	/* id 52 */
.word	i_bad	/* id 53 */
.word	i_spec	/* id 54 */
.word	i_bad	/* id 55 */

.word	i_xmit	/* id 56 */	/* sio 6 */
.word	i_bad	/* id 57 */
.word	i_ext	/* id 58 */
.word	i_bad	/* id 59 */
.word	i_rec	/* id 60 */
.word	i_bad	/* id 61 */
.word	i_spec	/* id 62 */
.word	i_bad	/* id 63 */

.word	i_bad	/* id 64 */
.word	i_bad	/* id 65 */
.word	i_bad	/* id 66 */
.word	i_bad	/* id 67 */
.word	i_bad	/* id 68 */
.word	i_bad	/* id 69 */
.word	i_atn	/* id 70 */	/* ctc 12, channel attention */
.word	i_bad	/* id 71 */
.word	i_bad	/* id 72 */
.word	i_bad	/* id 73 */
.word	i_bad	/* id 74 */
.word	i_bad	/* id 75 */
.word	i_bad	/* id 76 */
.word	i_bad	/* id 77 */
.word	i_mb	/* id 78 */	/* ctc 13, 796 bus grant */
.word	i_bad	/* id 79 */
.word	i_bad	/* id 80 */
.word	i_bad	/* id 81 */
.word	i_bad	/* id 82 */
.word	i_bad	/* id 83 */
.word	i_dma	/* id 84 */	/* ctc 10, dma 0 end */
.word	i_bad	/* id 85 */
.word	i_dma	/* id 86 */	/* ctc 11, dma 1 end */
.word	i_bad	/* id 87 */
.word	i_bad	/* id 88 */
.word	i_bad	/* id 89 */
.word	i_bad	/* id 90 */
.word	i_bad	/* id 91 */
.word	i_clk	/* id 92 */	/* ctc 14, real time clk */
.word	i_bad	/* id 93 */
.word	i_dma	/* id 94 */	/* ctc 15, dma 2 end */
.word	i_bad	/* id 95 */
.word	i_pio	/* id 96 */	/* pio 0 */
.word	i_bad	/* id 97 */
.word	i_pio	/* id 98 */	/* pio 1 */
.word	i_bad	/* id 99 */
.word	i_bad	/* id 100 */
.word	i_bad	/* id 101 */
.word	i_bad	/* id 102 */
.word	i_bad	/* id 103 */
.word	i_bad	/* id 104 */
.word	i_bad	/* id 105 */
.word	i_bad	/* id 106 */
.word	i_bad	/* id 107 */
.word	i_bad	/* id 108 */
.word	i_bad	/* id 109 */
.word	i_bad	/* id 110 */
.word	i_bad	/* id 111 */
.word	i_bad	/* id 112 */
.word	i_bad	/* id 113 */
.word	i_bad	/* id 114 */
.word	i_bad	/* id 115 */
.word	i_bad	/* id 116 */
.word	i_bad	/* id 117 */
.word	i_bad	/* id 118 */
.word	i_bad	/* id 119 */
.word	i_bad	/* id 120 */
.word	i_bad	/* id 121 */
.word	i_bad	/* id 122 */
.word	i_bad	/* id 123 */
.word	i_bad	/* id 124 */
.word	i_bad	/* id 125 */
.word	i_bad	/* id 126 */
.word	i_bad	/* id 127 */
.word	i_bad	/* id 128 */
.word	i_bad	/* id 129 */
.word	i_bad	/* id 130 */
.word	i_bad	/* id 131 */
.word	i_bad	/* id 132 */
.word	i_bad	/* id 133 */
.word	i_bad	/* id 134 */
.word	i_bad	/* id 135 */
.word	i_bad	/* id 136 */
.word	i_bad	/* id 137 */
.word	i_bad	/* id 138 */
.word	i_bad	/* id 139 */
.word	i_bad	/* id 140 */
.word	i_bad	/* id 141 */
.word	i_bad	/* id 142 */
.word	i_bad	/* id 143 */
.word	i_bad	/* id 144 */
.word	i_bad	/* id 145 */
.word	i_bad	/* id 146 */
.word	i_bad	/* id 147 */
.word	i_bad	/* id 148 */
.word	i_bad	/* id 149 */
.word	i_bad	/* id 150 */
.word	i_bad	/* id 151 */
.word	i_bad	/* id 152 */
.word	i_bad	/* id 153 */
.word	i_bad	/* id 154 */
.word	i_bad	/* id 155 */
.word	i_bad	/* id 156 */
.word	i_bad	/* id 157 */
.word	i_bad	/* id 158 */
.word	i_bad	/* id 159 */
.word	i_bad	/* id 160 */
.word	i_bad	/* id 161 */
.word	i_bad	/* id 162 */
.word	i_bad	/* id 163 */
.word	i_bad	/* id 164 */
.word	i_bad	/* id 165 */
.word	i_bad	/* id 166 */
.word	i_bad	/* id 167 */
.word	i_bad	/* id 168 */
.word	i_bad	/* id 169 */
.word	i_bad	/* id 170 */
.word	i_bad	/* id 171 */
.word	i_bad	/* id 172 */
.word	i_bad	/* id 173 */
.word	i_bad	/* id 174 */
.word	i_bad	/* id 175 */
.word	i_bad	/* id 176 */
.word	i_bad	/* id 177 */
.word	i_bad	/* id 178 */
.word	i_bad	/* id 179 */
.word	i_bad	/* id 180 */
.word	i_bad	/* id 181 */
.word	i_bad	/* id 182 */
.word	i_bad	/* id 183 */
.word	i_bad	/* id 184 */
.word	i_bad	/* id 185 */
.word	i_bad	/* id 186 */
.word	i_bad	/* id 187 */
.word	i_bad	/* id 188 */
.word	i_bad	/* id 189 */
.word	i_bad	/* id 190 */
.word	i_bad	/* id 191 */
.word	i_bad	/* id 192 */
.word	i_bad	/* id 193 */
.word	i_bad	/* id 194 */
.word	i_bad	/* id 195 */
.word	i_bad	/* id 196 */
.word	i_bad	/* id 197 */
.word	i_bad	/* id 198 */
.word	i_bad	/* id 199 */
.word	i_bad	/* id 200 */
.word	i_bad	/* id 201 */
.word	i_bad	/* id 202 */
.word	i_bad	/* id 203 */
.word	i_bad	/* id 204 */
.word	i_bad	/* id 205 */
.word	i_bad	/* id 206 */
.word	i_bad	/* id 207 */
.word	i_bad	/* id 208 */
.word	i_bad	/* id 209 */
.word	i_bad	/* id 210 */
.word	i_bad	/* id 211 */
.word	i_bad	/* id 212 */
.word	i_bad	/* id 213 */
.word	i_bad	/* id 214 */
.word	i_bad	/* id 215 */
.word	i_bad	/* id 216 */
.word	i_bad	/* id 217 */
.word	i_bad	/* id 218 */
.word	i_bad	/* id 219 */
.word	i_bad	/* id 220 */
.word	i_bad	/* id 221 */
.word	i_bad	/* id 222 */
.word	i_bad	/* id 223 */
.word	i_bad	/* id 224 */
.word	i_bad	/* id 225 */
.word	i_bad	/* id 226 */
.word	i_bad	/* id 227 */
.word	i_bad	/* id 228 */
.word	i_bad	/* id 229 */
.word	i_bad	/* id 230 */
.word	i_bad	/* id 231 */
.word	i_bad	/* id 232 */
.word	i_bad	/* id 233 */
.word	i_bad	/* id 234 */
.word	i_bad	/* id 235 */
.word	i_bad	/* id 236 */
.word	i_bad	/* id 237 */
.word	i_bad	/* id 238 */
.word	i_bad	/* id 239 */
.word	i_bad	/* id 240 */
.word	i_bad	/* id 241 */
.word	i_bad	/* id 242 */
.word	i_bad	/* id 243 */
.word	i_bad	/* id 244 */
.word	i_bad	/* id 245 */
.word	i_bad	/* id 246 */
.word	i_bad	/* id 247 */
.word	i_bad	/* id 248 */
.word	i_bad	/* id 249 */
.word	i_bad	/* id 250 */
.word	i_bad	/* id 251 */
.word	i_bad	/* id 252 */
.word	i_bad	/* id 253 */
.word	i_bad	/* id 254 */
.word	i_bad	/* id 255 */

/* 
 * low level part of interrupt and trap handlers
 */

/* externals */
.globl	_illinst
.globl	_illvi
.globl	_illnvi
.globl	_pwrfail

t_ei:
t_pi:
t_sc:
call	pushregs
call	_illinst
jp	intret

t_nmi:
call	pushregs
call	_pwrfail
jp	intret

t_nvi:
call	pushregs
call	_illnvi
jp	intret

i_xmit:
i_ext:
i_rec:
i_spec:
i_pio:
i_clk:
i_dma:
i_atn:
i_mb:
i_bad:
call	pushregs
call	_illvi
jp	intret
