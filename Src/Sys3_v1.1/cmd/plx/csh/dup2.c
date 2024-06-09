#include <fcntl.h>
dup2(f1,f2)
int f1,f2;
{
    close(f2);
    fcntl(f1,F_DUPFD,f2);
}
