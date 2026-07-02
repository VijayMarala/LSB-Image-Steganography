#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"types.h"
typedef struct _DecodeeInfo
{
    //stego img 
    char *stego_imgname;
    FILE *fptr_stego_fname;
    int sec_file_size;


    char magic_str[3];
    char buffer[8];


    //output data
    char *temp;
    char *out_name;
    FILE *fptr_out_fname;
    int extn_s;
    char extn_d[8];

} DecodeInfo;



/*validate the arguments*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/*skip 54 bytes for header*/
Status skip_header_54(DecodeInfo *DecodeInfo, FILE *fptr_stego_fname);

/* Get File pointers for i/p and o/p files */
Status open_files_dec(DecodeInfo *decInfo);

/*decode magic string*/
Status decode_magic_string(DecodeInfo *decInfo);

/*decode lsb to byte*/
Status lsb_to_byte(char *buffer , char *);

/*decode size to lsb*/
Status size_to_lsb(char *buffer, int *extn_s);

/*decod size of file extension*/
Status decode_extn_size(int *extn_s, DecodeInfo *decInfo);

/*decode extndata*/
Status decode_extn_data(char *extn_d, DecodeInfo *decInfo);

/*create a outputfile*/
Status create_op_file(char* out_name, DecodeInfo *decInfo);

/*decode sec file size*/
Status decode_sec_size(int *size, DecodeInfo *decInfo);

/*decode secret file size*/
Status decode_sec_data(DecodeInfo *decInfo);
