#include<stdio.h>
#include<sys/io.h>
#include<unistd.h>
int main(){
    FILE *f1;
    char ch;
    ioperm(0x3f8,7,1);
    outb(0x83,0x3fb);
    outb(0x0c,0x3f8);
    outb(0x00,0x3f9);
    outb(0x03,0x3fb);
    f1=fopen("Rx.txt","w");
    do{
        while(((inb(0x3fd)) & 0x01)!=0x01);
        ch=inb(0x3f8);
        putc(ch,f1);
        printf("%c",ch);
    }while(ch!=EOF);
    fclose(f1);
    printf("\nTHE FILE RECEIVED\n");
}