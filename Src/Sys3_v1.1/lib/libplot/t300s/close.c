#include <stdio.h>
closevt(){
	closepl();
}
closepl(){
	fflush(stdout);
	reset();
}
