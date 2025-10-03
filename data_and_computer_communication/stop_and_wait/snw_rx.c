#include<stdio.h>
#include<sys/io.h>

int flag=0,R=0,i;
char ack,frame[10],data[8];
FILE *fp;
int frame_count = 0;

void baud() {
    ioperm(0x3f8,7,1);
    outb(0x83,0x3fb);
    outb(0x0c,0x3f8);
    outb(0x00,0x3f9);
    outb(0x03,0x3fb);
    printf("[RX] Baud rate configured\n");
}

void fr_phy_layer() {
    for(i=0; i<10; i++) {
        while((inb(0x3fd)&0x01)!=0x01);
        frame[i] = inb(0x3f8);;
    }

    frame_count++;
    printf("\n[RX] Frame %d received: seq=%c\n", frame_count, frame[0]);

    // check sequence
    if(frame[0] == (R+'0')) {
        for(i=0; i<8; i++){
         data[i]=frame[i+1];
         }
       	 printf("[RX] Received : "); 
         for(int j=0; j<10;j++) {
         printf("%c",frame[j]);
         }
         printf("\n");
        ack = frame[0];  // default correct ACK
        printf("[RX] Frame accepted (seq=%c).\n", frame[0]);
        R = 1 - R;       // toggle expected
    } else {
        // duplicate frame
        ack = (R==0)?'1':'0';
        printf("[RX] Duplicate frame\n");
    }

    // ---- INTERACTIVE ACK SELECTION ----
    char choice;
    printf("[RX] Enter ACK to send (0/1) or X for timeout: ");
    scanf(" %c", &choice);

    if(choice == 'X' || choice == 'x') {
        ack = 'X';  // send special timeout marker
    } else if(choice == '0' || choice == '1') {
        ack = choice;
        //printf("[RX] Using ACK=%c\n", ack);
    } else {
        printf("[RX] Invalid input, using default ACK=%c\n", ack);
    }

    // send ACK (0,1 or X)
    while((inb(0x3fd)&0x60)!=0x60);
    outb(ack,0x3f8);
    printf("[RX] Sent ACK='%c'\n", ack);
}

void to_nw_lyr() {
    for(i=0; i<8; i++) {
        if(data[i]!=EOF) {
            fputc(data[i],fp);
        } else {
            printf("[RX] EOF detected. Reception complete.\n");
            flag=1;
            break;
        }
    }
}

int main() {
    baud();
    fp=fopen("rx.txt","w");
    if(!fp) {
        printf("[RX] Error: cannot open rx.txt\n");
        return -1;
    }

    while(!flag) {
        fr_phy_layer();
        to_nw_lyr();
    }

    fclose(fp);
    return 0;
}
