#include<unistd.h>
#include<stdio.h>
#include<sys/io.h>
#define DLE '*'
#define STX '@'
#define ETX '$'
int main()
{
    FILE *f1;
    char a[20];
    int i=2,flag=0,j;
    ioperm(0x3f8,7,1);
    outb(0x83,0x3fb);
    outb(0x0c,0x3f8);
    outb(0x00,0x3f9);
    outb(0x03,0x3fb);
    f1=fopen("tx.txt","r");
    do
    {
        a[0]=DLE;
        a[1]=STX;
        i=2;
        j=0;
        while((j<8) && (flag==0))
        {
            a[i]=getc(f1);
            if (a[i]==EOF)
                flag=1;
            if (a[i]==DLE)
            {
                i++;
                a[i]=DLE;
            }
            i++;
            j++;
        }
        a[i]=DLE;
        i++;
        a[i]=ETX;
        j=0;
        do
        {
            while((inb(0x3fd) & 0x60)!=0x60);
            outb(a[j],0x3f8);
            printf("%c",a[j]);
            j++;
        }
        while(j<(i+1));
    }
    while(flag!=1);
    printf("\n\n File transmission complete \n\n");
}
