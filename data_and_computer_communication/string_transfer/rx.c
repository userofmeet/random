#include<unistd.h>
#include <stdio.h>
#include <sys/io.h>
int main( ){
  char str_rx[30];
  int i=0;
  char ch;
  ioperm(0x3f8,7,1);
  outb(0x83,0x3fb);
  outb(0x0c,0x3f8);
  outb(0x00,0x3f9);
  outb(0x03,0x3fb);
  while(1)
  {
    while(((inb(0x3fd)) & 0x01)!=0x01);
    ch=inb(0x3f8);
    str_rx[i++]=ch;
    if (ch == '\0') break;
  }
  printf("data received =%s",str_rx);
}
