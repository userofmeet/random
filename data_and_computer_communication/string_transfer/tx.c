#include<unistd.h>
#include <stdio.h>
#include <sys/io.h>
#include <string.h>

int main()
{
    char str_tx[] = "TODAY IS HOLIDAY";
    ioperm(0x3f8,7,1);
    outb(0x83,0x3fb);
    outb(0x0c,0x3f8);
    outb(0x00,0x3f9);
    outb(0x03,0x3fb);
    for(int i=0;i<strlen(str_tx);i++)
    {
        while(((inb(0x3fd))&0x60)!=0x60);
        print("Transmitting %c\n",char_tx[i]);
        outb(str_tx[i],0x3f8);
    }
    while(((inb(0x3fd))&0x60)!=0x60);
    outb('\0',0x3f8);
    printf("data transmitted=%s",str_tx);
}
