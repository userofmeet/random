#include <stdio.h>
#include <time.h>
#include <sys/io.h>

char data[8], frame[10];
char seq = '0', ack;
FILE *fp;
int flag = 0;   // signals last frame

void delay(unsigned int ms){
    clock_t s = clock();
    while (clock() < s + ms*CLOCKS_PER_SEC / 1000);
}

void baud() {
    ioperm(0x3f8,7,1);
    outb(0x83,0x3fb);
    outb(0x0c,0x3f8);
    outb(0x00,0x3f9);
    outb(0x03,0x3fb);
    printf("[TX] Baud rate configured\n");
}

int frm_nw_layer() {
    int i;
    for (i=0; i<8; i++) {
        int c = getc(fp);
        if (c == EOF) {
            data[i] = EOF;   // store EOF in the frame
            flag = 1;        // mark last frame
            for(int j=i+1;j<8;j++) data[j]=EOF;
            break;
        }
        data[i] = (char)c;
    }
    return 1; // always generate a frame
}

void frame_generate() {
    frame[0] = seq;   // sequence number
    for(int i=0; i<8; i++)
        frame[i+1] = data[i];
    frame[9] = '0';   // dummy trailer
}

// Wait for one ACK byte from RX
char wait_for_ack_blocking(void) {
    while(1) {
        if ((inb(0x3fd) & 0x01) == 0x01) { // Data Ready
            char r = (char)inb(0x3f8);
            return r;
        }
        delay(5); // prevent busy spin
    }
}

int to_phy_layer() {
    int i;

    while(1) {
        // send frame
        printf("[TX] Sending frame (seq=%c): ", seq);
        for(i=0; i<10; i++) {
            while((inb(0x3fd)&(0x60))!=0x60);
            outb(frame[i],0x3f8);
            printf("%c", frame[i]);
        }
        printf("\n");

        // wait for ACK
        ack = wait_for_ack_blocking();
        
        if (ack == seq) {
            printf("[TX] Received ACK byte: '%c'\n", ack);

            printf("[TX] Correct ACK (%c). Proceeding to next frame.\n", ack);
            seq = (seq=='0') ? '1' : '0'; // toggle seq
            return 1;
        } else if (ack == 'X') {
            printf("[TX] TIMEOUT . Retransmitting frame.\n");
            continue;
        } else {
        printf("[TX] Received ACK byte: '%c'\n", ack);

            printf("[TX] Wrong ACK (%c). Retransmitting frame.\n", ack);
            continue;
        }
    }
}

int main() {
    baud();
    fp = fopen("tx.txt","r");
    if(!fp) {
        printf("[TX] Error: cannot open tx.txt\n");
        return -1;
    }

    while(1) {
        frm_nw_layer();
        frame_generate();
        to_phy_layer();
        if(flag) break;  // stop after EOF frame sent
    }

    fclose(fp);
    printf("[TX] Transmission complete\n");
    return 0;
}
