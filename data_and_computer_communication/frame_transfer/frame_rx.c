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
    int i,j,flag=0;
    ioperm(0x3f8,7,1);
    outb(0x83,0x3fb);
    outb(0x0c,0x3f8);
    outb(0x00,0x3f9);
    outb(0x03,0x3fb);
    f1=fopen("rx.txt","w");
    do
    {
        i=0;
        do
        {
            while(((inb(0x3fd)) & 0x01)!=0x01);
            a[i]=inb(0x3f8);
            i++;
        }
        while(a[i-1]!=ETX);
        for(j=0; j<i; j++)
        {
            printf("%c",a[j]);
        }
        j=0;
        while(j<i)
        {
            if(a[j]==EOF)
                flag=1;
            if((a[j]!=DLE) && (a[j]!=STX) && (a[j]!=ETX))
            {
                putc(a[j],f1);
                j++;
            }
            else if((a[j]==DLE) && (a[j+1]==DLE))
            {
                putc(a[j],f1);
                j+=2;
            }
            else
                j++;
        }
    }
    while(flag==0);
    fclose(f1);
    printf("\n\nFile Reception Complete \n\n");
}

