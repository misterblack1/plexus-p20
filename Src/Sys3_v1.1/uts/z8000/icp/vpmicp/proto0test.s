#define SSTACK 50
.byte 46,50   /* GETXBUF(type) */
.byte 21,0    /* TSTNEQI 0     */
.byte 0220,8  /* JMPEQZ  8     */
.byte 0200,0  /* JMP 0         */
.byte 56,0x32 /* XSOMI sync    */
.byte 39,53   /* GETBYTE(byte) */
.byte 20,0    /* TSTEQLI 0     */
.byte 0220,20 /* JMPEQZ  20    */
.byte 29,53   /* XMT(byte)     */
.byte 0200,10 /* JMP 10        */
.byte 30,0xff /* XMTI ff       */
.byte 30,0xff /* XMTI ff       */
.byte 55,0xff /* XEOMI ff      */
.byte 53,20   /* TIMEOUTI 20   */
.byte 0220,28 /* JMPEQZ *      */
.byte 0200,0  /* JMP 0         */
