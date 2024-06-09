#define SSTACK 50
.byte 46,50   /* GETXBUF(type) */
.byte 20,1    /* TSTEQLI 1     */
.byte 0240,0  /* JMPNEZ  0     */
.byte 56,016  /* XSOMI sync    */
.byte 39,53   /* GETBYTE(byte) */
.byte 20,1    /* TSTEQLI 1     */
.byte 0240,8  /* JMPNEZ  8     */
.byte 61,53   /* MOV byte to AC */
.byte 20,0x2a /* TSTEQLI(*)    */
.byte 0240,24 /* JMPNEZ 24     */
.byte 29,53   /* XMT(byte)     */
.byte 0200,8  /* JMP 8         */
.byte 30,0x2f /* XMTI slash    */
.byte 30,0xff /* XMTI ff       */
.byte 30,0xff /* XMTI ff       */
.byte 55,0xff /* XEOMI ff      */
.byte 44,50   /* RTNXBUF(type) */
.byte 0200,0  /* JMP 0         */
.byte 55,0    /* XEOMI '0'     */
.byte 44,50   /* RTNXBUF(type) */
.byte 0200,0  /* JMP 0         */
