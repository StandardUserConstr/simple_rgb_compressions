#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rgb_compressions0.h"

//compress if returns 0, decomprepss if returns 1;
//returns 2 if there was an error;
int check_if_compress_or_decompress(int argc_arg,char** argv_arg);

//returns 0 if there is no problem; else returns 1;
uint8_t compress_option(const char* bmp_file_to_compress,const char* output_file);

//returns 0 if there is no problem; else returns 1;
uint8_t decompress_option(const char* data_to_decompress,const char* output_bmp_file);

void write_bmp(const char* const filename,const uint8_t* const bitmap,const uint16_t  width,const uint16_t  height);



int main(int argc,char** argv)
{
    int return_value = check_if_compress_or_decompress(argc,argv);
    if(return_value==0)
    {
        return_value = compress_option(argv[2],argv[3]);
        if(return_value==1) return 1;
    }
    else if(return_value==1)
    {
        return_value = decompress_option(argv[2],argv[3]);
        if(return_value==1) return 1;
    }
    else return 1;
    return 0;
}



int check_if_compress_or_decompress(int argc_arg,char** argv_arg)
{
    if(argc_arg==0||argc_arg==1)
    {
        fprintf(stderr,"\nthere is no argument passed to the program;\n"
                       "pass argument -h to receive help;\n");
        return 2;
    }
    if(argv_arg[1][0]=='-'&&argv_arg[1][1]=='h')
    {
        printf("\n0 <input_bmp_to_compress> <output_compressed_bmp_data_to_save>  --  compress bmp file\n"
               "1 <input_compressed_bmp_data> <output_bmp_to_save>  --  decompress bmp file\n\n"
               "examples:\n"
               "0 image.bmp compressed_bmp.bin\n"
               "1 compressed_bmp.bin image_decompressed.bmp\n");
        return 2;
    }
    else if(argv_arg[1][0]=='0')
    {
        if(argc_arg<3)
        {
            fprintf(stderr,"\nfew arguments are missing;\n"
                           "pass argument -h to receive help;\n");
            return 2;
        }
        else return 0;
    }
    else if(argv_arg[1][0]=='1')
    {
        if(argc_arg<3)
        {
            fprintf(stderr,"\nfew arguments are missing;\n"
                           "pass argument -h to receive help;\n");
            return 2;
        }
        return 1;
    }
    else
    {
        fprintf(stderr,"\ninvalid argument;\n"
                       "pass argument -h to receive help;\n");
        return 2;
    }
    return 0;
}


uint8_t compress_option(const char* bmp_file_to_compress,const char* output_file)
{
    FILE* file = fopen(bmp_file_to_compress,"rb");
    if(file==NULL)
    {
        fprintf(stderr,"\nfile: \"%s\" doesn't exist\n",bmp_file_to_compress);
        return 1;
    }

    unsigned int size_of_file;
    unsigned int starting_pixel_data;
    unsigned int width;
    unsigned int height;
    unsigned int size_of_bitmap;
    fseek(file,2,SEEK_SET);
    fread(&size_of_file,4,1,file);
    printf("size_of_input_bmp_file=%zd bytes\n",size_of_file);

    fseek(file,10,SEEK_SET);
    fread(&starting_pixel_data,4,1,file);
    printf("start_position_of_bitmap_data=%zd\n",starting_pixel_data);

    fseek(file,18,SEEK_SET);
    fread(&width,4,1,file);
    printf("width=%d\n",width);

    fseek(file,22,SEEK_SET);
    fread(&height,4,1,file);
    printf("height=%d\n",height);

    size_of_bitmap = size_of_file-starting_pixel_data;
    printf("size_of_bitmap=%d bytes\n",size_of_bitmap);

    uint8_t* bitmap_data0 = (uint8_t*)malloc(size_of_bitmap);
    uint8_t* bitmap_data1 = (uint8_t*)malloc(size_of_bitmap);
    uint8_t* header_data = (uint8_t*)malloc(starting_pixel_data);
    fseek(file,0,SEEK_SET);
    fread(header_data,1,starting_pixel_data,file);
    fseek(file,starting_pixel_data,SEEK_SET);
    fread(bitmap_data0,1,size_of_bitmap,file);

    fclose(file);

    printf("\ncompressing in progress...");
    unsigned int data_size = compress_rgb_into_8bits_map(bitmap_data0,bitmap_data1,size_of_bitmap,1,16384);       //EXAMPLE OF USING COMPRESS ALGORITHMS
    data_size = compress_per1byte_data(bitmap_data1,bitmap_data0,data_size);                                      //EXAMPLE OF USING COMPRESS ALGORITHMS

    printf(" DONE");
    printf("\nsize_of_compressed_data=%zd bytes\n",data_size);
    printf("compressed_data_is_lower_than_oryginal_data_by=%zd bytes\n",size_of_bitmap-data_size);

    file = fopen(output_file,"wb");
    if(file==NULL)
    {
        fprintf(stderr,"\nunexpected error\n");
        return 1;
    }
    fwrite(header_data,1,starting_pixel_data,file);
    fwrite(bitmap_data0,1,data_size,file);
    printf("\nfile: \"%s\" has been saved\nDONE",output_file);

    free(bitmap_data0);
    free(bitmap_data1);
    free(header_data);
    return 0;
}

uint8_t decompress_option(const char* data_to_decompress,const char* output_bmp_file)
{
    FILE* file = fopen(data_to_decompress,"rb");
    if(file==NULL)
    {
        fprintf(stderr,"\nfile: \"%s\" doesn't exist\n",data_to_decompress);
        return 1;
    }

    unsigned int size_of_file;
    unsigned int starting_pixel_data;
    unsigned int width;
    unsigned int height;
    unsigned int size_of_bitmap;
    fseek(file,2,SEEK_SET);
    fread(&size_of_file,4,1,file);
    printf("size_of_decompressed_bmp_file=%zd bytes\n",size_of_file);

    fseek(file,10,SEEK_SET);
    fread(&starting_pixel_data,4,1,file);
    printf("start_position_of_bitmap_data=%zd\n",starting_pixel_data);

    fseek(file,18,SEEK_SET);
    fread(&width,4,1,file);
    printf("width=%d\n",width);

    fseek(file,22,SEEK_SET);
    fread(&height,4,1,file);
    printf("height=%d\n",height);

    size_of_bitmap = size_of_file-starting_pixel_data;
    printf("size_of_bitmap=%d bytes\n",size_of_bitmap);

    fseek(file,0,SEEK_END);
    unsigned int size_of_compressed_file = ftell(file);
    printf("size_of_compressed_data=%zd bytes\n",size_of_compressed_file);

    printf("\ndecompressing_in_progress...");
    uint8_t* bitmap_data0 = (uint8_t*)malloc(size_of_bitmap);
    uint8_t* bitmap_data1 = (uint8_t*)malloc(size_of_bitmap);
    fseek(file,starting_pixel_data,SEEK_SET);
    fread(bitmap_data0,1,size_of_compressed_file-starting_pixel_data,file);
    fclose(file);
    unsigned int data_size = decompress_per1byte_data(bitmap_data0,bitmap_data1,size_of_compressed_file-starting_pixel_data);    //EXAMPLE OF USING DECOMPRESS ALGORITHMS
    data_size = decompress_8bits_map_into_rgb(bitmap_data1,bitmap_data0);                                                        //EXAMPLE OF USING DECOMPRESS ALGORITHMS
    printf(" DONE\n");
    printf("size_of_decompressed_data=%zd bytes",data_size);
    write_bmp(output_bmp_file,bitmap_data0,width,height);
    printf("\nfile: \"%s\" has been saved\nDONE",output_bmp_file);

    free(bitmap_data0);
    free(bitmap_data1);
    return 0;
}

void write_bmp(const char* const filename,const uint8_t* const bitmap,const uint16_t  width,const uint16_t  height)
{

    const uint32_t bitmap_size =  3 * height * (width+1); //+1 for padding each row

    //54 bytes is weight of template to coding at beginning of file;
//=====================================================================================================
    const uint8_t template0[] = {'B','M'}; //+2bytes (1x1byte and 1x1byte);
    const uint32_t template1[] =
    {
       bitmap_size+54,   //+4bytes (1x4byte) size of the BMP file in bytes;
        0,               //+4bytes (1x4byte) reserved bytes (to skip);
       54,               //+4bytes (1x4byte) starting address of pixel array;
       40,               //+4bytes (1x4byte) size in bytes of this header from this moment to the pixel array;
       width,height      //+8bytes (1x4byte and 1x4byte) width and height of the bitmap;
    };
    const uint16_t template2[] =
    {
        1, //+2bytes (1x2byte) numbers of colors planes (must be 1);
       24  //+2bytes (1x2byte) number of bits per pixel;
    };
    const uint32_t template3[] =
    {
        0,                  //+4bytes (1x4byte) compression method (0 means no compression);
        bitmap_size,        //+4bytes (1x4byte) image size in bytes;
        9006,9006,          //+8bytes (1x4byte and 1x4byte) Dots Per Inch/horizontal and vertical resolution (only matter for printing so i use default parameters);
        0,                  //+8bytes number of colors in the color palette (0 stands for deafult);
        24                  //+8bytes number of important colors used or 0 when every color is important;
    };
//=====================================================================================================
    FILE* fp = fopen(filename,"wb");
    fwrite(template0, sizeof(template0),1,fp);
    fwrite(template1, sizeof(template1),1,fp);
    fwrite(template2, sizeof(template2),1,fp);
    fwrite(template3, sizeof(template3),1,fp);

    fwrite(bitmap,height*width*3,1,fp);
    fclose(fp);
    return;
}
