#include<unistd.h>
#include <stdio.h>
#include <sys/io.h>
int main( ){
  char data;
  ioperm(0x3f8,7,1);
  outb(0x83,0x3fb);
  outb(0x0c,0x3f8);
  outb(0x00,0x3f9);
  outb(0x03,0x3fb);
  while(((inb(0x3fd)) & 0x01)!=0x01);
  printf("Receiving...\n");
  data=inb(0x3f8);
  printf("data received=%c\n",data);
}
