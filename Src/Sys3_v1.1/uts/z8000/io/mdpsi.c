#ifndef	OVKRNL
/* note: anything changed between these #if statments must be changed in
	 iotimo.c also
*/
siabort()
{
	register int i;

	for(i=0;i<NUMICP;i++) {
#ifdef VPMSYS
		if ( srchtp( &sctl[i], i ) )
#else
		if ( srchtp( &sctl[i] ) ) 
#endif
			return;
	}
}
#endif
