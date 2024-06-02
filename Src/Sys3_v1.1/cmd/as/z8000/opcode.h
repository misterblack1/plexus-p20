
/* opcode.decs */

#define	OPCB1		1	/* opcode - upper byte */
#define	OPCN1		2	/* opcode - most sig. nibble of upper byte */
#define	OPCN3		3	/* opcode - most sig. nibble of lower byte */
#define	OPCN4		4	/* opcode - lease sig. nibble of lower byte*/
#define	OPCBT7		5	/* opcode - bit 7 */
#define	OPCBT3		6	/* opcode - bit 3 */
#define	OPCBTS67	7	/* opcode - bits 6 and 7 */
#define	OPC7BTS		8	/* opcode - upper 7 bits */
#define	OPCW		9	/* opcode - whole word */

#define	N0		10	/* nibble - 0 */
#define	N0E		11	/* nibble - E */
#define	N1		12	/* nibble - 1 */
#define	OP1N		13	/* operand 1 - nibble */
#define	OP2N		14	/* operand 2 - nibble */		 
#define	OP3N		15	/* operand 3 - nibble */	   
#define	OP4N		16	/* operand 4 - nibble */	    
#define	OP2NM1		17	/* operand 2 - nibble minus 1 */
#define	OP3NM1		18	/* operand 3 - nibble minus 1 */
#define	OP4NM1		19	/* operand 4 - nibble minus 1 */
#define	B0		20	/* byte - 0 */
#define	B1		21	/* byte - 1 */
#define	OP1B		22	/* operand 1 - byte */
#define	OP2B		23	/* operand 2 - byte */
#define	OP3B		24	/* operand 3 - byte */
#define	OP1W		25	/* operand 1 - word */
#define	OP2W		26	/* operand 2 - word */
#define	OP3W		27	/* operand 3 - word */
#define	OP1AW		28	/* operand 1 - word address */
#define	OP2AW		29	/* operand 2 - word address */
#define	OP1L		30	/* operand 1 - long word */
#define	OP2L		31	/* operand 2 - long word */
#define	OP3L		32	/* operand 3 - long word */
#define	OP1SC		33	/* operand 1 - 12 bits -- signed */
#define	OP12BTS		34	/* operand 1 - 2 bits */
#define	OP13BTS		35	/* operand 1 - 2b bits */
#define	OP19BTS		36	/* operand 1 - 9 bits */
#define	OP23BTS		37	/* operand 2 - 3 bits */
#define	OP24BTS		38	/* operand 2 - 4 bits */
#define	OP25BTS		39	/* operand 2 - 5 bits */
#define	OP26BTS		40	/* operand 2 - 6 bits */
#define	OP27BTS		41	/* operand 2 - 7 bits */
#define	OP215BTS	42	/* operand 2 - 15 bits */
#define	OP2BT1		43	/* operand 2 - bit 1 */
#define	OP23BTS2	44	/* operand 2 - 3 bits (negative) */
#define	OP24BTS2	45	/* operand 2 - 4 bits (negative) */
#define	OP25BTS2	46	/* operand 2 - 5 bits (negative) */
#define	BT0		47	/* 1 bit - 0 */
#define	BT1		48	/* 1 bit - 1 */
#define	OR2		49	/* logical or of next two operands */
#define	OR3		50	/* logical or of next three operands */
#define	OR4		51	/* logical or of next four operands	*/
#define AND22BTS	52	/* logical and of two operands (2 bits)	*/
#define WVALUE1		53	/* a word with the value 1 */
#define OP26BTS2	54	/* 16 bits, but only eight are significant */
#define OP2SB		55	/* opcode 2 byte -- signed */
#define OP1SB		56	/* opcode 1 byte -- signed */
#define OP1FD		57	/* operand 1 fp -- double register */
#define OP1FF		58	/* operand 1 fp -- float or long register */
#define OP1FI		59	/* operand 1 fp -- integer register */
#define OP2FD		60	/* operand 2 fp -- double register */
#define OP2FF		61	/* operand 2 fp -- float or long register */
#define OP2FI		62	/* operand 2 fp -- integer register */
#define OPCBTS45	63	/* opcode - bits 4 and 5 */
#define OP2W2		64	/* operand 2 - twos complement */

