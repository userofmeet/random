#include <stdio.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/io.h>
int main()
{
    FILE *f1;
    char c;
    ioperm(0x3f8,7,1);
    outb(0x83,0x3fb);
    outb(0x0c,0x3f8);
    outb(0x00,0x3f9);
    outb(0x03,0x3fb);
    f1=fopen("tx.txt","r");
    do
    {
        while((inb(0x3fd) & 0x60)!=0x60);
        c=getc(f1);
        outb(c,0x3f8);
        printf("%c",c);
    }while(c!=EOF);
    fclose(f1);
    printf("\n the file transmitted\n");
}
