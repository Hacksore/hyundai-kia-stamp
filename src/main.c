#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h> 

// A lot of inspiration from
//  https://github.com/ManuBatBat/aes_whitebox
// Base64 credit to
//  https://github.com/jwerle/b64.c
// AES whitebox credit to
//  https://github.com/balena/aes-whitebox/blob/master/aes_whitebox.cc

#define B64_BUFFER_SIZE		(1024 * 64) // 64K
#define Nr 14   // AES256
#define LIST_STEP_MS 120 * 1000
#define LIST_COUNT 1000

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

typedef struct{
    uint8_t     iv[16];                       // iv
    uint32_t    pad;
    uint32_t    Tyboxes[Nr-1][16][256];
    uint8_t     Xor[Nr-1][24*4][16][16];
    uint32_t    MBL[Nr-1][16][256];
    uint8_t     TboxesLast[16][256];
}AES256_WHITEBOX_DATA;

static const char b64_table[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};

int bufc = 0;

char* b64_buf_malloc()
{
	char* buf = malloc(B64_BUFFER_SIZE);
	bufc = 1;
	return buf;
}

char* b64_buf_realloc(unsigned char* ptr, size_t size)
{
	if (size > bufc * B64_BUFFER_SIZE)
	{
		while (size > bufc * B64_BUFFER_SIZE) bufc++;
		char* buf = realloc(ptr, B64_BUFFER_SIZE * bufc);
		if (!buf) return NULL;
		return buf;
	}

	return ptr;
}

char * b64_encode (const unsigned char *src, size_t len) {
  int i = 0;
  int j = 0;
  char *enc = NULL;
  size_t size = 0;
  unsigned char buf[4];
  unsigned char tmp[3];

  // alloc
  enc = (char *) b64_buf_malloc();
  if (NULL == enc) { return NULL; }

  // parse until end of source
  while (len--) {
    // read up to 3 bytes at a time into `tmp'
    tmp[i++] = *(src++);

    // if 3 bytes read then encode into `buf'
    if (3 == i) {
      buf[0] = (tmp[0] & 0xfc) >> 2;
      buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
      buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
      buf[3] = tmp[2] & 0x3f;

      // allocate 4 new byts for `enc` and
      // then translate each encoded buffer
      // part by index from the base 64 index table
      // into `enc' unsigned char array
      enc = (char *) b64_buf_realloc(enc, size + 4);
      for (i = 0; i < 4; ++i) {
        enc[size++] = b64_table[buf[i]];
      }

      // reset index
      i = 0;
    }
  }

  // remainder
  if (i > 0) {
    // fill `tmp' with `\0' at most 3 times
    for (j = i; j < 3; ++j) {
      tmp[j] = '\0';
    }

    // perform same codec as above
    buf[0] = (tmp[0] & 0xfc) >> 2;
    buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
    buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
    buf[3] = tmp[2] & 0x3f;

    // perform same write to `enc` with new allocation
    for (j = 0; (j < i + 1); ++j) {
      enc = (char *) b64_buf_realloc(enc, size + 1);
      enc[size++] = b64_table[buf[j]];
    }

    // while there is still a remainder
    // append `=' to `enc'
    while ((i++ < 3)) {
      enc = (char *) b64_buf_realloc(enc, size + 1);
      enc[size++] = '=';
    }
  }

  // Make sure we have enough space to add '\0' character at end.
  enc = (char *) b64_buf_realloc(enc, size + 1);
  enc[size] = '\0';

  return enc;
}

/**************************************************************************/
AES256_WHITEBOX_DATA *aes256_whitebox_get_data(char *buf,size_t len,char *iv){
/**************************************************************************/
AES256_WHITEBOX_DATA *aes;
char *p,*c;
    aes=malloc(sizeof(AES256_WHITEBOX_DATA)); // 749588
    if (!aes){
        printf("Unable to alloc whitebox struct\r\n");
        return NULL;
    }
    memset(aes,0,sizeof(AES256_WHITEBOX_DATA));
    p=buf;
    while (p<buf+len){
        c=memchr(p,iv[0],len);
        if (!c || len-(c-buf)<strlen(iv)){
            printf("IV not found\r\n");
            return NULL;
        }
        if (!memcmp(c,iv,strlen(iv))) break;
        p=c+1;
        c=NULL;
    }
    if (!c){
        printf("IV not found\r\n");
        return NULL;
    }
    memcpy(aes,c,sizeof(AES256_WHITEBOX_DATA));
    return aes;
}

/**************************************************************************/
void ShiftRows(uint8_t state[16]) {
/**************************************************************************/
  const int shifts[16] = {
     0,  5, 10, 15,
     4,  9, 14,  3,
     8, 13,  2,  7,
    12,  1,  6, 11,
  };

  const uint8_t in[16] = {
    state[ 0], state[ 1], state[ 2], state[ 3],
    state[ 4], state[ 5], state[ 6], state[ 7],
    state[ 8], state[ 9], state[10], state[11],
    state[12], state[13], state[14], state[15],
  };

  for (int i = 0; i < 16; i++)
    state[i] = in[shifts[i]];
}

/**************************************************************************/
void Cipher(uint8_t in[16],AES256_WHITEBOX_DATA *aes){
/**************************************************************************/
    for (int r = 0; r < Nr-1; r++) {
        ShiftRows(in);
        for (int j = 0; j < 4; ++j) {
            uint32_t aa, bb, cc, dd;
            uint8_t n0, n1, n2, n3;

            aa = aes->Tyboxes[r][j*4 + 0][in[j*4 + 0]];
            bb = aes->Tyboxes[r][j*4 + 1][in[j*4 + 1]];
            cc = aes->Tyboxes[r][j*4 + 2][in[j*4 + 2]];
            dd = aes->Tyboxes[r][j*4 + 3][in[j*4 + 3]];

            n0 = aes->Xor[r][j*24 +  0][(aa >> 28) & 0xf][(bb >> 28) & 0xf];
            n1 = aes->Xor[r][j*24 +  1][(cc >> 28) & 0xf][(dd >> 28) & 0xf];
            n2 = aes->Xor[r][j*24 +  2][(aa >> 24) & 0xf][(bb >> 24) & 0xf];
            n3 = aes->Xor[r][j*24 +  3][(cc >> 24) & 0xf][(dd >> 24) & 0xf];
            in[j*4 + 0] = (aes->Xor[r][j*24 + 4][n0][n1] << 4) | aes->Xor[r][j*24 + 5][n2][n3];

            n0 = aes->Xor[r][j*24 +  6][(aa >> 20) & 0xf][(bb >> 20) & 0xf];
            n1 = aes->Xor[r][j*24 +  7][(cc >> 20) & 0xf][(dd >> 20) & 0xf];
            n2 = aes->Xor[r][j*24 +  8][(aa >> 16) & 0xf][(bb >> 16) & 0xf];
            n3 = aes->Xor[r][j*24 +  9][(cc >> 16) & 0xf][(dd >> 16) & 0xf];
            in[j*4 + 1] = (aes->Xor[r][j*24 + 10][n0][n1] << 4) | aes->Xor[r][j*24 + 11][n2][n3];

            n0 = aes->Xor[r][j*24 + 12][(aa >> 12) & 0xf][(bb >> 12) & 0xf];
            n1 = aes->Xor[r][j*24 + 13][(cc >> 12) & 0xf][(dd >> 12) & 0xf];
            n2 = aes->Xor[r][j*24 + 14][(aa >>  8) & 0xf][(bb >>  8) & 0xf];
            n3 = aes->Xor[r][j*24 + 15][(cc >>  8) & 0xf][(dd >>  8) & 0xf];
            in[j*4 + 2] = (aes->Xor[r][j*24 + 16][n0][n1] << 4) | aes->Xor[r][j*24 + 17][n2][n3];

            n0 = aes->Xor[r][j*24 + 18][(aa >>  4) & 0xf][(bb >>  4) & 0xf];
            n1 = aes->Xor[r][j*24 + 19][(cc >>  4) & 0xf][(dd >>  4) & 0xf];
            n2 = aes->Xor[r][j*24 + 20][(aa >>  0) & 0xf][(bb >>  0) & 0xf];
            n3 = aes->Xor[r][j*24 + 21][(cc >>  0) & 0xf][(dd >>  0) & 0xf];
            in[j*4 + 3] = (aes->Xor[r][j*24 + 22][n0][n1] << 4) | aes->Xor[r][j*24 + 23][n2][n3];

            aa = aes->MBL[r][j*4 + 0][in[j*4 + 0]];
            bb = aes->MBL[r][j*4 + 1][in[j*4 + 1]];
            cc = aes->MBL[r][j*4 + 2][in[j*4 + 2]];
            dd = aes->MBL[r][j*4 + 3][in[j*4 + 3]];

            n0 = aes->Xor[r][j*24 +  0][(aa >> 28) & 0xf][(bb >> 28) & 0xf];
            n1 = aes->Xor[r][j*24 +  1][(cc >> 28) & 0xf][(dd >> 28) & 0xf];
            n2 = aes->Xor[r][j*24 +  2][(aa >> 24) & 0xf][(bb >> 24) & 0xf];
            n3 = aes->Xor[r][j*24 +  3][(cc >> 24) & 0xf][(dd >> 24) & 0xf];
            in[j*4 + 0] = (aes->Xor[r][j*24 + 4][n0][n1] << 4) | aes->Xor[r][j*24 + 5][n2][n3];

            n0 = aes->Xor[r][j*24 +  6][(aa >> 20) & 0xf][(bb >> 20) & 0xf];
            n1 = aes->Xor[r][j*24 +  7][(cc >> 20) & 0xf][(dd >> 20) & 0xf];
            n2 = aes->Xor[r][j*24 +  8][(aa >> 16) & 0xf][(bb >> 16) & 0xf];
            n3 = aes->Xor[r][j*24 +  9][(cc >> 16) & 0xf][(dd >> 16) & 0xf];
            in[j*4 + 1] = (aes->Xor[r][j*24 + 10][n0][n1] << 4) | aes->Xor[r][j*24 + 11][n2][n3];

            n0 = aes->Xor[r][j*24 + 12][(aa >> 12) & 0xf][(bb >> 12) & 0xf];
            n1 = aes->Xor[r][j*24 + 13][(cc >> 12) & 0xf][(dd >> 12) & 0xf];
            n2 = aes->Xor[r][j*24 + 14][(aa >>  8) & 0xf][(bb >>  8) & 0xf];
            n3 = aes->Xor[r][j*24 + 15][(cc >>  8) & 0xf][(dd >>  8) & 0xf];
            in[j*4 + 2] = (aes->Xor[r][j*24 + 16][n0][n1] << 4) | aes->Xor[r][j*24 + 17][n2][n3];

            n0 = aes->Xor[r][j*24 + 18][(aa >>  4) & 0xf][(bb >>  4) & 0xf];
            n1 = aes->Xor[r][j*24 + 19][(cc >>  4) & 0xf][(dd >>  4) & 0xf];
            n2 = aes->Xor[r][j*24 + 20][(aa >>  0) & 0xf][(bb >>  0) & 0xf];
            n3 = aes->Xor[r][j*24 + 21][(cc >>  0) & 0xf][(dd >>  0) & 0xf];
            in[j*4 + 3] = (aes->Xor[r][j*24 + 22][n0][n1] << 4) | aes->Xor[r][j*24 + 23][n2][n3];
        }
    }
    ShiftRows(in);
    // Using T-boxes:
    for (int i = 0; i < 16; i++)
        in[i] = aes->TboxesLast[i][in[i]];
}

/**************************************************************************/
void aes_whitebox_encrypt_cfb(const uint8_t iv[16], const uint8_t* m, size_t len, uint8_t* c, AES256_WHITEBOX_DATA *aes){
/**************************************************************************/
uint8_t cfb_blk[16];

    for (int i = 0; i < 16; i++)
        cfb_blk[i] = iv[i];

    for (size_t i = 0; i < len; i++) {
        if ((i & 0xf) == 0)
            Cipher(cfb_blk,aes);
        cfb_blk[i & 0xf] ^= m[i];
        c[i] = cfb_blk[i & 0xf];
    }
}

void aes_whitebox_encrypt_cfb_and_print(const uint8_t iv[16], const uint8_t* m, size_t len, AES256_WHITEBOX_DATA *aes) {
    size_t i;
    uint8_t *c;
    c=malloc(len);
    if (!c){
        printf("Unable to allocate output buffer\r\n");
        free(aes);
        return;
    }

    aes_whitebox_encrypt_cfb(aes->iv,m,len,c,aes);

    printf("%s", b64_encode(c, len));
    free(c);
}

/**************************************************************************/
int main(int argc, char *argv[]){
/**************************************************************************/
FILE *f;
size_t size;
char *buf;
AES256_WHITEBOX_DATA *aes;
#define M argv[4]

    bool isExport = strcmp(argv[3], "export") == 0;

    if ((isExport && argc != 4) || (!isExport && argc!=5)){
        printf("AES256 CFB whitebox\r\n");
        printf("\r\n");
        printf("  usage:\r\n");
        printf("    aes_whitebox binary_file iv mode input\r\n");
        printf("\r\n");
        printf("       binary_file : file for recovering AES whitebox compiled data\r\n");
        printf("       iv          : initialisation vector to be searched on binary file and used as IV\r\n");
        printf("       mode        : The generation mode you want (single | list | export)\r\n");
        printf("       input       : data to encrypt, only used for single and list\r\n");
        return 1;
    }

    f=fopen(argv[1],"rb");
    if (f==NULL){
        printf("Unable to open lib file\r\n");
        return 1;
    }
    fseek(f,0,SEEK_END);
    size=ftell(f);
    fseek(f,0,SEEK_SET);
    buf=malloc(size);
    if (!buf){
        printf("Unable to alloc memory\r\n");
        fclose(f);
        return 1;
    }
    if (fread(buf,1,size,f)!=size){
        printf("Unable to read file\r\n");
        free(buf);
        fclose(f);
        return 1;
    }
    fclose(f);
    aes=aes256_whitebox_get_data(buf,size,argv[2]);
    if (!aes){
        printf("Unable to get aes256 whitebox data\r\n");
        free(buf);
        return 1;
    }
    free(buf);


    if (strcmp(argv[3], "single") == 0) {
        aes_whitebox_encrypt_cfb_and_print(aes->iv,M,strlen(M),aes);
        printf("\r\n");
    }
    else if (strcmp(argv[3], "list") == 0) {
        size_t loop;
        long long initialTime = current_timestamp();
        for (loop = 0; loop<LIST_COUNT; loop++) {
            char rawStamp[1000];
            long long currentStamp = initialTime + (LIST_STEP_MS * loop);
            sprintf(rawStamp, "%s:%lld", M, currentStamp);
            aes_whitebox_encrypt_cfb_and_print(aes->iv,rawStamp,strlen(rawStamp),aes);
            printf("\r\n");
        }
    }
    else if (isExport) {
        printf("{\r\n");
        printf("\t\"iv\": [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d],\r\n", aes->iv[0], aes->iv[1], aes->iv[2], aes->iv[3], aes->iv[4], aes->iv[5], aes->iv[6], aes->iv[7], aes->iv[8], aes->iv[9], aes->iv[10], aes->iv[11], aes->iv[12], aes->iv[13], aes->iv[14], aes->iv[15]);
        printf("\t\"pad\": %d,\r\n", aes->pad);

        printf("\t\"boxesLast\": [\r\n");
        for (int tboxesLastLoopX = 0; tboxesLastLoopX<16; tboxesLastLoopX++) {
            printf("\t\t[");
            for (int tboxesLastLoopY = 0; tboxesLastLoopY<256; tboxesLastLoopY++) {
                printf("%d", aes->TboxesLast[tboxesLastLoopX][tboxesLastLoopY]);
                if (tboxesLastLoopY < 255) {
                    printf(", ");
                }
            }
            printf("]%s\r\n", tboxesLastLoopX < 15 ? ", " : "");
        }
        printf("\t],\r\n");

        printf("\t\"tyboxes\": [\r\n");
        for (int tboxesLoopX = 0; tboxesLoopX<(Nr-1); tboxesLoopX++) {
            printf("\t\t[");
            for (int tboxesLoopY = 0; tboxesLoopY<(24*4); tboxesLoopY++) {
                printf("[");
                for (int tboxesLoopZ = 0; tboxesLoopZ<256; tboxesLoopZ++) {
                    printf("%d", aes->Tyboxes[tboxesLoopX][tboxesLoopY][tboxesLoopZ]);
                    if (tboxesLoopZ < 255) {
                        printf(", ");
                    }
                }
                printf("]");
                if (tboxesLoopY < (24*4)-1) {
                    printf(", ");
                }
            }
            printf("]%s\r\n", tboxesLoopX < (Nr-2) ? ", " : "");
        }
        printf("\t],\r\n");

        printf("\t\"mbl\": [\r\n");
        for (int MBLLoopX = 0; MBLLoopX<(Nr-1); MBLLoopX++) {
            printf("\t\t[");
            for (int MBLLoopY = 0; MBLLoopY<16; MBLLoopY++) {
                printf("[");
                for (int MBLLoopZ = 0; MBLLoopZ<256; MBLLoopZ++) {
                    printf("%d", aes->MBL[MBLLoopX][MBLLoopY][MBLLoopZ]);
                    if (MBLLoopZ < 255) {
                        printf(", ");
                    }
                }
                printf("]");
                if (MBLLoopY < 15) {
                    printf(", ");
                }
            }
            printf("]%s\r\n", MBLLoopX < (Nr-2) ? ", " : "");
        }
        printf("\t],\r\n");


        printf("\t\"xor\": [\r\n");
        for (int xorLoopX = 0; xorLoopX<(Nr-1); xorLoopX++) {
            printf("\t\t[");
            for (int xorLoopY = 0; xorLoopY<(24*4); xorLoopY++) {
                printf("[");
                for (int xorLoopZ = 0; xorLoopZ<16; xorLoopZ++) {
                    printf("[");
                    for (int xorLoopZprime = 0; xorLoopZprime<16; xorLoopZprime++) {
                        printf("%d", aes->Xor[xorLoopX][xorLoopY][xorLoopZ][xorLoopZprime]);
                        if (xorLoopZprime < 15) {
                            printf(", ");
                        }
                    }
                    printf("]");
                    if (xorLoopZ < 15) {
                        printf(", ");
                    }
                }
                printf("]");
                if (xorLoopY < (24*4)-1) {
                    printf(", ");
                }
            }
            printf("]%s\r\n", xorLoopX < (Nr-2) ? ", " : "");
        }
        printf("\t]\r\n");

        printf("}");
        printf("\r\n");
    }
    else {
        printf("Unable to get the mode %s\r\n", argv[3]);
        free(aes);
        return 1;
    }


    free(aes);
    return 0;
}
