#include <stdio.h>
#include"decode.h"
#include"types.h"


/*reading cmd line i/p and validate*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if((strcmp(argv[2], "stego_img.bmp")!=0))
    {
        printf("argv of stego file is not valid\n");
        return e_failure;
    }
    if(argv[3]==NULL || (strcmp(argv[3], ".")==0))
    {
        printf("plz provide op file without extension\n");
        return e_failure;
    }
    if(argv[3]!=NULL)
    {
        decInfo->temp=argv[3];
    }

    /*copy filenames*/
    decInfo->stego_imgname = argv[2];
}


/* process of decoding image*/
Status do_decoding(DecodeInfo *decInfo)
{
    
    /*open files*/
    int ret=open_files_dec(decInfo);
    if(ret==e_failure)
    {
        printf("open file for decoding is failed\n");
        return e_failure;
    }
    printf("file are opened\n");
    
    /*for decoding we need to start after 54 bytes bmp header so skipping 54 bytes*/
    if(skip_header_54(decInfo, decInfo->fptr_stego_fname)==e_failure)
    {
        printf("NOT skipped 54 bytes\n");
        return e_failure;
    }
    printf("skipped 54 bytes for header file successfull\n");

    printf("\nDecoding process started\n\n");

    /*decoding magic string for authentication purpose*/
    if(decode_magic_string(decInfo)==e_failure)
    {
        printf("magic str decode failed\n");
        return e_failure;
    }
    printf("magic string successfully decoded\n");

    /*authenticating password to proceed decoding*/
    if(strcmp(decInfo->magic_str, "#*")!=0)
    {
        printf("unauthorized login\n");
        return e_failure;
    }
    printf("authentication success\n");
    //printf("%s\n", decInfo->magic_str);

    /*decoding extension size*/
    if(decode_extn_size(&(decInfo->extn_s), decInfo)==e_failure)
    {
        printf("decode extn_size failed\n");
        return e_failure;
    }
    printf("extn_size successfully decoded\n");
    //printf("%d\n", decInfo->extn_s);

    /*decoding extn data for our project extn is .txt */
    if(decode_extn_data(decInfo->extn_d, decInfo)==e_failure)
    {
        printf("decoding extn type is failed\n");
        return e_failure;
    }
    printf("extn type successfully decoded\n");
    /*printf("%s\n", decInfo->extn_d);*/

    /*creating a o/p file same as encoded extn type*/
    if(create_op_file(decInfo->out_name, decInfo)==e_failure)
    {
	    printf("op file not created\n");
	    return e_failure;
    }
    printf("output file created and opened\n");

    /*decoding secret size*/
    if(decode_sec_size(&(decInfo->sec_file_size), decInfo)==e_failure)
    {
        printf("sec file size failed\n");
        return e_failure;
    }
    printf("secret file size successfully decoded\n");
    //printf("%d\n",decInfo->sec_file_size);

    /*decodin secret data*/
    if(decode_sec_data(decInfo)==e_failure)
    {
        printf("decode file data failed\n");
        return e_failure;
    }
    printf("secret file successfully decoded and written to o/p file\n");
    //free(decInfo->out_name);
}


/*open file of which we want to decode*/
Status open_files_dec(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_fname=fopen(decInfo->stego_imgname, "r");
    if(decInfo->fptr_stego_fname==NULL)
    {
        printf("file not opened\n");
        return e_failure;
    }
    printf("opened file successfully\n");
}


/*skipping 54 bytes header info*/
Status skip_header_54(DecodeInfo *DecodeInfo, FILE *fptr_stego_fname)
{
   int ret=fseek(fptr_stego_fname, 54, SEEK_SET);
    if(ret!=0)
    {
        return e_failure;
    }
}


/*decoding magic string*/
Status decode_magic_string(DecodeInfo *decInfo)
{
    char buffer[8];
    char data;
    for(int i=0; i<2; i++)
    {
        fread(buffer, sizeof(char), 8, decInfo->fptr_stego_fname);
        lsb_to_byte(buffer, &data);
        decInfo->magic_str[i]=data;
    }
    decInfo->magic_str[2]='\0';
    return e_success;
}


/*decoding extn size*/
Status decode_extn_size(int *extn_s, DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, sizeof(char), 32, decInfo->fptr_stego_fname);
    size_to_lsb(buffer, extn_s);
}


/*decoding extn data*/
Status decode_extn_data(char *extn_d, DecodeInfo *decInfo)
{
    char buffer[8];
    char data;
    for(int i=0; i<decInfo->extn_s; i++)
    {
        fread(buffer, sizeof(char), 8, decInfo->fptr_stego_fname);
        lsb_to_byte(buffer, &data);
        extn_d[i]=data;
    }
    extn_d[decInfo->extn_s]='\0';
}


/*creating o/p file to store secret data*/
Status create_op_file(char* out_name, DecodeInfo *decInfo)
{
	out_name=malloc((strlen(decInfo->temp)+decInfo->extn_s) * sizeof(char));
    //memmove used to copy the memory src to destination
    memmove(out_name+0, decInfo->temp, strlen(decInfo->temp));
    //here extension sec file .txt copy at legth of o/p file
    memmove(out_name+strlen(decInfo->temp), decInfo->extn_d, decInfo->extn_s);
    out_name[strlen(decInfo->temp)+decInfo->extn_s]='\0';
    printf("%s\n", out_name);
    decInfo->fptr_out_fname=fopen(out_name, "w");
    if(decInfo->fptr_out_fname==NULL)
    {
        return e_failure;
    }
}


/*decoding sec file size*/
Status decode_sec_size(int *size, DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego_fname);
    size_to_lsb(buffer, size);
}


/*decoding secrete data*/
Status decode_sec_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;
    for(int i=0; i<decInfo->sec_file_size; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_fname);
        lsb_to_byte(buffer, &ch);
        fprintf(decInfo->fptr_out_fname, "%c", ch);
    }
}


/*decode one byte for one lsb bit then merge to get o/p*/
Status lsb_to_byte(char *buffer, char *dataa)
{
    int mask=0;
    for(int i=0; i<8; i++)
    {
        char bit=buffer[i] & 1;
        mask =mask | (bit<<(7-i));
    }
    *dataa=mask;
    return e_success;
}


//decoding 32bits of int
Status size_to_lsb(char *buffer, int *size)
{
    int mask=0;
    for(int i=0; i<32; i++)
    {
        char bit= buffer[i] & 1;
        mask=mask | (bit<<(31-i));
    }
    *size=mask;
}
