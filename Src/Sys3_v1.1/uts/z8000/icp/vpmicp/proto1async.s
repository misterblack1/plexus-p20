#define SSTACK 50
.byte 47,51   /* GETRBUF(name) */
.byte 31,50   /* RCV byte    */
.byte 40,50   /* PUT byte    */
.byte 61,50   /* MOV byte to AC */
.byte 20,0x2f /* TSTEQLI slash  */
.byte 0220,2  /* JMPEQZ 2    */
.byte 43,51   /* RTNRBUF(name) */
.byte 0200,0  /* JMP 0       */
