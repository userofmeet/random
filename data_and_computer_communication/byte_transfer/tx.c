#include<unistd.h>
#include <stdio.h>
#include <sys/io.h>
int main()
{
  char data;
  printf("Enter character to transmit:");
  scanf("%c",&data);
  ioperm(0x3f8,7,1);
  outb(0x83,0x3fb);
  outb(0x0c,0x3f8);
  outb(0x00,0x3f9);
  outb(0x03,0x3fb);
  while(((inb(0x3fd))&0x60)!=0x60);
  printf("Transmitting...\n");
  outb(data,0x3f8);
  printf("data transmitted=%c\n",data);
}
