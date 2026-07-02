#include <stdio.h>
#include "encode.h"
#include "types.h"
#include"common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("\nwidth = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}


/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }


    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }


    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}


OperationType check_operation_type(char *argv[])
{
    if(argv[1]==NULL)
    {
        printf("Pass arg\n");
        return e_unsupported;
    }
    if(strcmp(argv[1], "-e")==0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d")==0)
    {
        return e_decode;
    }
    else
    return e_success;
}


Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2]==NULL)
    {
        printf("plz provide img file name\n");
        return e_failure;
    }
    if(strstr(argv[2], ".bmp")==NULL)
    {
        printf("img file error\n");
       return e_failure;
    }
    else
        encInfo->src_image_fname= argv[2];
        //printf("%s\n",encInfo->src_image_fname);
    if(strchr(argv[3], '.')==NULL)
    {
        printf("plz enter valid file with extension\n");
        return e_failure;
    }
    else
        encInfo->secret_fname=argv[3];
    if(argv[4]==NULL)
    {
        printf("created a file by compiler\n");
        argv[4]="stego_img.bmp";
        encInfo->stego_image_fname=argv[4];
        printf("%s\n", encInfo->stego_image_fname);
    }
    else
         encInfo->stego_image_fname=argv[4];
    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{

    /*calling open files function it opens all necessary files*/
    if(open_files(encInfo)==e_failure)
    {
        return e_failure;
    }
    printf("files are opened successfully\n");
    
    // source bmp size
    uint  bmp=get_image_size_for_bmp(encInfo->fptr_src_image);

    //extension file size
    strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname, "."));      
    encInfo->extn_size=strlen(encInfo->extn_secret_file);

    //calculate secret file size
    fseek(encInfo->fptr_secret, 0, SEEK_END);
    encInfo->size_secret_file=ftell(encInfo->fptr_secret);

    /*calculating required to encode bytes*/
    uint noof_img_bytes_need=16 + 32 + (encInfo->size_secret_file*8) + (encInfo->extn_size*8) + 4*8;

    
    /*checking no.of bytes need to encode less than original bmp file size*/
    if(noof_img_bytes_need < bmp-54)
    {
        printf("\nencoding process started\n\n");
    }
    else
    return e_failure;

    /*copy all 54bytes for header*/
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)!=e_success)
    {
        printf("copy header failed\n");
        return e_failure;
    }
    printf("54 bytes of header copied successfully\n");

    /*encoding magic string "#*" */
    if(encode_magic_string(MAGIC_STRING, encInfo)==e_failure)
    {
        printf("Encode magic str failed\n");
        return e_failure;
    }
    printf("magic string successfully encoded\n");

    /*encoding sec file extension size */
    if(encode_secret_file_extn_size(encInfo->extn_size, encInfo)==e_failure)
    {
        printf("secrete file extension size failed\n");
        return 0;
    }
    printf("secrete file extension size successfully encoded\n");

    /*encoding secret file extension like .txt or .cmv */
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo)==e_failure)
    {
        printf("secret file extension failed\n");
        return e_failure;
    }
    printf("secret file extn successfully encoded\n");

    /*encoding secret file size for encoding secrete data*/
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo)==e_failure)
    {
        printf("extension secrete file size is failed\n");
        return 0;
    }
    printf("secrete file size successfully encoded\n");

    /*encoding secret file data*/
    if(encode_secret_file_data(encInfo)==e_failure)
    {
        printf("fai;ed\n");
        return 0;
    }
    printf("secrete data successfully encoded\n");

    /*encoding remaining data to stego file*/
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_failure)
    {
        printf("encoding of remaining data is failed\n");
        return 0;
    }
    printf("remaining data successfully encoded\n");
}


Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];
    // Move file pointer to beginning of the file
    fseek(fptr_src_image, 0, SEEK_SET);
    // Read 54 bytes of BMP header
    fread(header, sizeof(char), 54, fptr_src_image);
    // Write the header to destination image
    fwrite(header, sizeof(char), 54, fptr_dest_image);

    return e_success;
}


Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buffer[8];
    for(int i=0; i<2; i++)
    {
        fread(buffer, sizeof(char), 8, encInfo->fptr_src_image);

        encode_byte_to_lsb(magic_string[i], buffer);

        fwrite(buffer, sizeof(char), 8, encInfo->fptr_stego_image);
    }
    return e_success;
}


Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char buffer[8];
    for(int i=0; i<encInfo->extn_size; i++)
    {
        fread(buffer, sizeof(char), 8, encInfo->fptr_src_image);

        encode_byte_to_lsb(file_extn[i], buffer);

        fwrite(buffer, sizeof(char), 8, encInfo->fptr_stego_image);
    }
    return e_success;
}


Status encode_secret_file_extn_size(int data, EncodeInfo *encInfo)
{
    char buffer[32];

    fread(buffer, 1, 32, encInfo->fptr_src_image);

    encode_byte_to_size(data, buffer);

    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);

    return e_success;
}


Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];

    fread(buffer, 1, 32, encInfo->fptr_src_image);

    encode_byte_to_size(file_size, buffer);

    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);

    return e_success;
}


Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    char buffer[8];

    rewind(encInfo->fptr_secret);

    while((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
        fread(buffer, 1, 8, encInfo->fptr_src_image);

        encode_byte_to_lsb(ch, buffer);

        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }

    return e_success;
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int ch;
    while((ch=fgetc(fptr_src))!=EOF)
    {
        fputc(ch, fptr_dest);
    }
    return e_success;
}


Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int n=7;
    for(int i=0; i<=7; i++)
    {
        image_buffer[i]= image_buffer[i] & 0Xfe;  // clearing last bit of original img at byte by byte
        int mask=1<<n;     // creatin mask for 1000 0000 to get 1 byte 1 bit from magic string
        unsigned char bit=mask & data;   // // getting msb bit of 1st byte for magic str
        image_buffer[i]= (bit>>n) | image_buffer[i];  // bit right shift to lsb position and doing or
        n--;
    }
}


Status encode_byte_to_size(int data, char *image_buffer)
{
    int n=31;
    for(int i=0; i<=31; i++)
    {
        image_buffer[i]= image_buffer[i] & 0Xfffffffe;  // clearing last bit of original img at byte by byte
        int mask=1<<n;     // creatin mask for 1000 0000 to get 1 byte 1 bit from magic string
        unsigned int bit=mask & data;   // // getting msb bit of 1st byte for magic str
        image_buffer[i]= (bit>>n) | image_buffer[i];  // bit right shift to lsb position and doing or
        n--;
    }
}





