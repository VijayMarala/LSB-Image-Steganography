/********************************************************
 * Name :- marala vijaya kumar reddy
 * Date :- 05-06-2026
 * Description :- LSB image stegnography
 ********************************************************/
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;
/*
    // Fill with sample filenames
    encInfo.src_image_fname = "beautiful.bmp";
    encInfo.secret_fname = "secret.txt";
    encInfo.stego_image_fname = "stego_img.bmp";
    */

    //perform specific operation
    int ret=check_operation_type(argv);
    if(ret==e_encode)
    {
        printf("encode is selected\n");
        
         //reding input and validation
        ret=read_and_validate_encode_args(argv, &encInfo);
        if(ret==e_failure)
        {
             printf("validation is not successfull\n");
             return 0;
        }
        else
            printf("validation is succefull\n");
        ret=do_encoding(&encInfo);
        if (ret == e_failure)
            {
    	        printf("encoding is not done\n");
    	        return 1;
            }
            else
            {
    	        printf("encoding process successfully completed...\n" );
            }


    }
    else if(ret==e_decode)
    {
        printf("decode selected\n");

        //reading inputs
        ret=read_and_validate_decode_args(argv, &decInfo);
        if(ret==e_failure)
        {
            printf("read and validte is failed\n");
            return 0;
        }
        printf("validation succeffull\n");

       
        if( do_decoding(&decInfo)==e_failure)
        {
            printf("decoding failed\n");
            return 0;
        }
        printf("decoding successfully completed...\n");
    }
    
    
    else
    {
        printf("unsupporeted selection\n");
        return 0;
    }
    

    // Test get_image_size_for_bmp
   // img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
   // printf("INFO: Image size = %u\n", img_size);

    return 0;
}
