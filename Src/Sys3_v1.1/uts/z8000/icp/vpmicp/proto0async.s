#define SSTACK 50
.byte 46,50   /* GETXBUF(type) */
.byte 20,1    /* TSTEQLI 1     */
.byte 0240,0  /* JMPNEZ  0     */
.byte 39,53   /* GETBYTE(byte) */
.byte 20,1    /* TSTEQLI 1     */
.byte 0240,6  /* JMPNEZ  6     */
.byte 61,53   /* MOV byte to AC */
.byte 20,0x2a /* TSTEQLI(*)    */
.byte 0240,22 /* JMPNEZ 22     */
.byte 29,53   /* XMT(byte)     */
.byte 0200,6  /* JMP 6         */
.byte 55,0x2f /* XEOMI slash   */
.byte 44,50   /* RTNXBUF(type) */
.byte 0200,0  /* JMP 0         */
