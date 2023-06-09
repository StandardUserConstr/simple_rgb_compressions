#include <stdint.h>

//=======================================================================================================================================
//  --------------------
// | 55 12 52 55 12 52 |   ORYGINAL
// |                   |
// | 2  55 12 52       |   COMPRESSED
// |                   |
//  -------------------

//-- DANGEROUS --
//returns size of new data that has been compressed and returned in data_out;
//if returns 0 then it's error: invalid format;
//DATA_OUT SHOULD HAVE MINIMUM SIZE: (SIZEOF(data_in)+(SIZEOF(data_in)/3));
uint32_t compress_per3bytes_rgb_data(const uint8_t* const data_in,uint8_t* const data_out,const uint32_t size_of_data_in);
uint64_t compress_per3bytes_rgb_data64(const uint8_t* const data_in,uint8_t* const data_out,const uint64_t size_of_data_in);

//-- DANGEROUS --
//returns size of decompressed data pushed into "data_out" and decompressed data in "data_out";
//if "data_out" is too small, then program will crash; size of data_out should be decompressed version of this data;
//if returns 0 then it's error: invalid format;
uint32_t decompress_per3bytes_rgb_data(const uint8_t* const data_in,uint8_t* const data_out,const uint32_t size_of_data_in);
uint64_t decompress_per3bytes_rgb_data64(const uint8_t* const data_in,uint8_t* const data_out,const uint64_t size_of_data_in);
//=======================================================================================================================================


//=======================================================================================================================================
//  --------------------
// | 55 55 55 55 55 55 |   ORYGINAL
// |                   |
// | 6  55             |   COMPRESSED
// |                   |
//  -------------------

//-- DANGEROUS --
//returns size of new data that has been compressed and returned in data_out;
//data_out should have minimum SIZEOF(data_in)*2;
uint32_t compress_per1byte_data(const uint8_t* const data_in,uint8_t* const data_out,const uint32_t size_of_data_in);
uint64_t compress_per1byte_data64(const uint8_t* const data_in,uint8_t* const data_out,const uint64_t size_of_data_in);

//-- DANGEROUS --
//returns size of decompressed data pushed into "data_out" and decompressed data in "data_out";
//if "data_out" is too small, then program will crash; size of data_out should be decompressed version of this data;
//if returns 0 then it's error: invalid format;
uint32_t decompress_per1byte_data(const uint8_t* const data_in,uint8_t* const data_out,const uint32_t size_of_data_in);
uint64_t decompress_per1byte_data64(const uint8_t* const data_in,uint8_t* const data_out,const uint64_t size_of_data_in);
//=======================================================================================================================================


//THIS COMPRESSION 'CAUSING THE LOSS OF THE IMAGE; THIS COMPRESSION CHANGING COLOR PALETTE INTO 8BIT COLOR PALETTE;
//=======================================================================================================================================
//
//             -------------------                   (IT'S NOT 1:1 VISUALISATION OF SIZE)
// ORYGINAL   | 55 55 55 22 22 22 |           8it rgb map is in first 1024bytes of compressed data
//             -------------------              ---------------------------------------------                  format of compressed data:
//             -------------------             | 0 22 22 22 1 55 55 55 - - - - - - - - - - - |
// COMPRESSED | 1  0              |            | - - - - - - - - - - - - - - - - - - - - - - |              0-1024bytes:     actual rgb map
//             -------------------             | - - - - - - - - - - - - - - - - - - - - - - |              1024-1026bytes:  how many pixels is keeped in rgb map (256 is max)
//                                             | - - - - - - - - - - - - - - - - - - - - - - |              1026-1030bytes:  image size in bytes
//             format of rgb map:              | - - - - - - - - - - - - - - - - - - - - - - |              1030-*bytes:     actual image in rgb map format
//                                             | - - - - - - - - - - - - - - - - - - - - - - |
//   (0-1byte) identificator from 1 to 255     | - - - - - - - - - - - - - - - - - - - - - - |
//        (1-4bytes) 1 pixel (R G B)            ---------------------------------------------
//
//

//-- DANGEROUS --
//returns size of new data that has been compressed/mapped and returned in data_out;
//if returns 0 then it's error: invalid format;
//
//"size_of_palette_map_buffor" is for optymalisation purpose; this variable determines how much memory do you allocate for algorithm;
//if u allocate not enough memory (for example for very colorful image 'cause there will be needed a lot of memory) then ur processor will do extra job to allocate memory in run;
uint32_t compress_rgb_into_8bits_map(const uint8_t* const data_in,uint8_t* const data_out,const uint32_t size_of_data_in,uint32_t size_of_palette_map_buffor = 16384);
uint64_t compress_rgb_into_8bits_map64(const uint8_t* const data_in,uint8_t* const data_out,const uint64_t size_of_data_in,uint64_t size_of_palette_map_buffor = 16384);

//-- DANGEROUS --
//returns size of decompressed data pushed into "data_out" and decompressed data in "data_out";
//if "data_out" is too small, then program will crash; size of data_out should be decompressed version of this data;
uint32_t decompress_8bits_map_into_rgb(const uint8_t* const data_in,uint8_t* const data_out);
uint64_t decompress_8bits_map_into_rgb64(const uint8_t* const data_in,uint8_t* const data_out);

//NOTE: a good combination is to combine compressing first through "compress_rgb_into_8bit_map" and then through "compress_per1byte_data"

//=======================================================================================================================================


uint32_t compress_per3bytes_rgb_data(const uint8_t* const data_in,uint8_t* const data_out,const uint32_t size_of_data_in)
{
    int32_t buffor0[3];
    buffor0[0] = data_in[0]; buffor0[1] = data_in[1];
    buffor0[2] = data_in[2];

    uint32_t i = 0;
    uint32_t new_data_counter = 0;
    int32_t counter;
    if(size_of_data_in%3==0) //if data format can be threated as real rgb format;
    {
        for(; i!=size_of_data_in; i+=3,new_data_counter+=4)
        {
            counter = 0;
            for(; i!=size_of_data_in&&counter!=255; i+=3,counter++)
            {
                if(!(data_in[i]==buffor0[0]&&data_in[i+1]==buffor0[1]&&data_in[i+2]==buffor0[2])) break;
            }
            if(counter==0)
            {
                data_out[new_data_counter] = 1; data_out[new_data_counter+1] = data_in[i];
                data_out[new_data_counter+2] = data_in[i+1]; data_out[new_data_counter+3] = data_in[i+2];
                if(i==size_of_data_in) {new_data_counter+=4; return new_data_counter;}
                buffor0[0] = data_in[i+3]; buffor0[1] = data_in[i+4];
                buffor0[2] = data_in[i+5];


            }
            else
            {

                data_out[new_data_counter] = counter; data_out[new_data_counter+1] = data_in[i-3];
                data_out[new_data_counter+2] = data_in[i-2]; data_out[new_data_counter+3] = data_in[i-1];
                if(i==size_of_data_in) {new_data_counter+=4; return new_data_counter;}
                buffor0[0] = data_in[i]; buffor0[1] = data_in[i+1];
                buffor0[2] = data_in[i+2];
                i-=3;
            }
        }
    }
    else
    {
        return 0;
    }

    return new_data_counter;

}

uint64_t compress_per3bytes_rgb_data64(const uint8_t* const data_in,uint8_t* const data_out,const uint64_t size_of_data_in)
{
    int32_t buffor0[3];
    buffor0[0] = data_in[0]; buffor0[1] = data_in[1];
    buffor0[2] = data_in[2];

    uint64_t i = 0;
    uint64_t new_data_counter = 0;
    int32_t counter;
    if(size_of_data_in%3==0) //if data format can be threated as real rgb format;
    {
        for(; i!=size_of_data_in; i+=6,new_data_counter+=4)
        {
            counter = 0;
            for(; i!=size_of_data_in&&counter!=255; i+=3,counter++)
            {
                if(!(data_in[i]==buffor0[0]&&data_in[i+1]==buffor0[1]&&data_in[i+2]==buffor0[2])) break;
            }
            if(counter==0)
            {
                data_out[new_data_counter] = 1; data_out[new_data_counter+1] = data_in[i];
                data_out[new_data_counter+2] = data_in[i+1]; data_out[new_data_counter+3] = data_in[i+2];
                if(i==size_of_data_in) {new_data_counter+=4; return new_data_counter;}
                buffor0[0] = data_in[i+3]; buffor0[1] = data_in[i+4];
                buffor0[2] = data_in[i+5];


            }
            else
            {
                data_out[new_data_counter] = counter; data_out[new_data_counter+1] = data_in[i-3];
                data_out[new_data_counter+2] = data_in[i-2]; data_out[new_data_counter+3] = data_in[i-1];
                if(i==size_of_data_in) {new_data_counter+=4; return new_data_counter;}
                buffor0[0] = data_in[i]; buffor0[1] = data_in[i+1];
                buffor0[2] = data_in[i+2];
                i-=3;
            }
        }
    }
    else
    {
        return 0;
    }

    return new_data_counter;

}

uint32_t decompress_per3bytes_rgb_data(const uint8_t* const data_in,uint8_t* const data_out,const uint32_t size_of_data_in)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t new_position = 0;
    int32_t counter;
    if(size_of_data_in%4==0) //if data format can be threated as real rgb compressed per3bytes format;
    {
        for(; i!=size_of_data_in; i+=4)
        {
            counter = data_in[i];
            for(; ; j+=3)
            {
                if(j==new_position+(counter*3)){break;}
                data_out[j] = data_in[i+1];
                data_out[j+1] = data_in[i+2];
                data_out[j+2] = data_in[i+3];

            }
            new_position = j;
        }
    }
    else
    {
        return 0;
    }
    return new_position;

}

uint64_t decompress_per3bytes_rgb_data64(const uint8_t* const data_in,uint8_t* const data_out,const uint64_t size_of_data_in)
{
    uint64_t i = 0;
    uint64_t j = 0;
    uint64_t new_position = 0;
    int32_t counter;
    if(size_of_data_in%4==0) //if data format can be threated as real rgb compressed per3bytes format;
    {
        for(; i!=size_of_data_in; i+=4)
        {
            counter = data_in[i];
            for(; ; j+=3)
            {
                if(j==new_position+(counter*3)){break;}
                data_out[j] = data_in[i+1];
                data_out[j+1] = data_in[i+2];
                data_out[j+2] = data_in[i+3];
            }
            new_position = j;
        }
    }
    else
    {
        return 0;
    }
    return new_position;
}

uint32_t compress_per1byte_data(const uint8_t* const data_in,uint8_t* const data_out,const uint32_t size_of_data_in)
{
    int32_t buffor0;

    uint32_t i = 0;
    uint32_t new_data_counter = 0;
    int32_t counter;

    for(; i!=size_of_data_in; i++,new_data_counter+=2)
    {
        counter = 1;
        buffor0  = data_in[i];
        for(; i+1!=size_of_data_in&&counter!=255; i++,counter++)
        {
            if((data_in[i+1]!=buffor0)) break;
        }
        data_out[new_data_counter] = counter; data_out[new_data_counter+1] = data_in[i];
        if(i+1==size_of_data_in){new_data_counter+=2; return new_data_counter;}
    }

    return new_data_counter;
}

uint64_t compress_per1byte_data64(const uint8_t* const data_in,uint8_t* const data_out,const uint64_t size_of_data_in)
{
    int32_t buffor0;

    uint64_t i = 0;
    uint64_t new_data_counter = 0;
    int32_t counter;

    for(; i!=size_of_data_in; i++,new_data_counter+=2)
    {
        counter = 1;
        buffor0  = data_in[i];
        for(; i+1!=size_of_data_in&&counter!=255; i++,counter++)
        {
            if((data_in[i+1]!=buffor0)) break;
        }
        data_out[new_data_counter] = counter; data_out[new_data_counter+1] = data_in[i];
        if(i+1==size_of_data_in){new_data_counter+=2; return new_data_counter;}
    }

    return new_data_counter;
}

uint32_t decompress_per1byte_data(const uint8_t* const data_in,uint8_t* const data_out,const uint32_t size_of_data_in)
{
    uint32_t i = 0;
    uint32_t new_data_counter = 0;
    int32_t counter;

    if(size_of_data_in%2!=0) return 0; //if data wasnt propably compressed before;

    for(; i!=size_of_data_in; i+=2)
    {
        counter = 0;
        for(; counter!=data_in[i]; counter++)
        {
            data_out[new_data_counter+counter] = data_in[i+1];
        }
        new_data_counter+=data_in[i];
    }

    return new_data_counter;
}

uint64_t decompress_per1byte_data64(const uint8_t* const data_in,uint8_t* const data_out,const uint64_t size_of_data_in)
{
    uint64_t i = 0;
    uint64_t new_data_counter = 0;
    int32_t counter;

    if(size_of_data_in%2!=0) return 0; //if data wasnt propably compressed before;

    for(; i!=size_of_data_in; i+=2)
    {
        counter = 0;
        for(; counter!=data_in[i]; counter++)
        {
            data_out[new_data_counter+counter] = data_in[i+1];
        }
        new_data_counter+=data_in[i];
    }

    return new_data_counter;
}


uint32_t compress_rgb_into_8bits_map(const uint8_t* const data_in,uint8_t* const data_out,const uint32_t size_of_data_in,uint32_t size_of_palette_map_buffor)
{
    if(size_of_data_in%3!=0) return 0; //if data for sure isn't in rgb format;
    if(size_of_palette_map_buffor==0) return 0;

    uint8_t** all_color_palettes = (uint8_t**)malloc(size_of_palette_map_buffor*sizeof(uint8_t*));
    for(uint32_t i = 0; i!=size_of_palette_map_buffor; i++) all_color_palettes[i] = (uint8_t*)malloc(3);

    uint32_t* numbers_of_repeating_palletes = (uint32_t*)malloc(size_of_palette_map_buffor*sizeof(uint32_t));
    uint32_t numbers_of_all_palettes = 0;

    uint32_t check_pallete_loop_counter;
    for(uint32_t i = 0; i!=size_of_data_in; i+=3)
    {
        for(check_pallete_loop_counter = 0; check_pallete_loop_counter!=numbers_of_all_palettes; check_pallete_loop_counter++)
        {
            if(*(uint16_t*)((uint8_t*)&all_color_palettes[check_pallete_loop_counter][0])==*(uint16_t*)((uint8_t*)&data_in[i])&&
               all_color_palettes[check_pallete_loop_counter][2]==data_in[i+2])
               {
                    numbers_of_repeating_palletes[check_pallete_loop_counter]++;
                    break;
               }
        }

        if(check_pallete_loop_counter==numbers_of_all_palettes)
        {
            numbers_of_all_palettes++;
            if(numbers_of_all_palettes==size_of_palette_map_buffor) //if array is too small, then allocate more memory;
            {
                uint8_t** temporary_all_color_palletes = (uint8_t**)malloc(size_of_palette_map_buffor*sizeof(uint8_t*));
                for(uint32_t i = 0; i!=size_of_palette_map_buffor; i++) temporary_all_color_palletes[i] = (uint8_t*)malloc(3);
                uint8_t* temporary_numbers_of_repeating_palletes = (uint8_t*)malloc(size_of_palette_map_buffor*sizeof(uint32_t));

                for(uint32_t i = 0; i!=size_of_palette_map_buffor; i++)
                {
                    *(uint16_t*)((uint8_t*)&temporary_all_color_palletes[i][0]) = *(uint16_t*)((uint8_t*)&all_color_palettes[i][0]);
                    temporary_all_color_palletes[i][2] = all_color_palettes[i][2];
                    temporary_numbers_of_repeating_palletes[i] = numbers_of_repeating_palletes[i];
                }

                for(uint32_t i = 0; i!=size_of_palette_map_buffor; i++) free(all_color_palettes[i]);
                free(all_color_palettes);
                free(numbers_of_repeating_palletes);

                size_of_palette_map_buffor*=2;
                all_color_palettes = (uint8_t**)malloc(size_of_palette_map_buffor*sizeof(uint8_t*));
                for(uint32_t i = 0; i!=size_of_palette_map_buffor; i++) all_color_palettes[i] = (uint8_t*)malloc(3);
                numbers_of_repeating_palletes = (uint32_t*)malloc(size_of_palette_map_buffor*sizeof(uint32_t));

                for(uint32_t i = 0; i!=numbers_of_all_palettes; i++)
                {
                    *(uint16_t*)((uint8_t*)&all_color_palettes[i][0]) = *(uint16_t*)((uint8_t*)&temporary_all_color_palletes[i][0]);
                    all_color_palettes[i][2] = temporary_all_color_palletes[i][2];
                    numbers_of_repeating_palletes[i] = temporary_numbers_of_repeating_palletes[i];
                }

                for(uint32_t i = 0; i!=numbers_of_all_palettes; i++) free(temporary_all_color_palletes[i]);
                free(temporary_all_color_palletes);
                free(temporary_numbers_of_repeating_palletes);
            }
            *(uint16_t*)((uint8_t*)&all_color_palettes[check_pallete_loop_counter][0]) = *(uint16_t*)((uint8_t*)&data_in[i]);
            all_color_palettes[check_pallete_loop_counter][2] = data_in[i+2];
            numbers_of_repeating_palletes[check_pallete_loop_counter] = 1;
            continue;
        }
    }

    int32_t size_of_bitmap = (numbers_of_all_palettes>256) ? 256 : numbers_of_all_palettes;

    uint32_t* bitmap = (uint32_t*)malloc(size_of_bitmap*sizeof(uint32_t));
    for(int32_t i = 0; i!=size_of_bitmap; i++) *((uint8_t*)((uint32_t*)&bitmap[i])) = i;

    uint32_t higher_number = numbers_of_repeating_palletes[0];
    uint32_t higher_number_in_array = 0;

    for(uint32_t i = 0,j; i!=size_of_bitmap; i++)
    {
        for(j = 0; j!=numbers_of_all_palettes; j++)
        {
            if(numbers_of_repeating_palletes[j]>higher_number)
            {
                higher_number = numbers_of_repeating_palletes[j];
                higher_number_in_array = j;
            }
        }
        numbers_of_repeating_palletes[higher_number_in_array] = 0;
        *(uint16_t*)((uint8_t*)((uint32_t*)&bitmap[i])+1) = *(uint16_t*)((uint8_t*)&all_color_palettes[higher_number_in_array][0]);
        *((uint8_t*)((uint32_t*)&bitmap[i])+3) = all_color_palettes[higher_number_in_array][2];
        higher_number = 0;
    }

    for(int32_t i = 0; i!=size_of_bitmap; i++) *((uint32_t*)data_out+i) = bitmap[i];
    *(uint16_t*)((uint8_t*)data_out+1024) = size_of_bitmap;

    const uint32_t size_of_header = 1030;

    uint32_t new_size_of_data_out = size_of_header;
    uint32_t abs_points[size_of_bitmap];
    uint32_t lower_number;

    int32_t j;
    for(uint32_t i = 0; i!=size_of_data_in; i+=3,new_size_of_data_out++)
    {
        for(j = 0; j!=size_of_bitmap; j++)
        {
            abs_points[j]=abs(*((uint8_t*)((uint32_t*)&bitmap[j])+1)-data_in[i]);
            abs_points[j]+=abs(*((uint8_t*)((uint32_t*)&bitmap[j])+2)-data_in[i+1]);
            abs_points[j]+=abs(*((uint8_t*)((uint32_t*)&bitmap[j])+3)-data_in[i+2]);
        }
        lower_number = abs_points[0];
        data_out[new_size_of_data_out] = (*((uint8_t*)((uint32_t*)&bitmap[0])));
        for(j = 0; j!=size_of_bitmap; j++)
        {
            if(abs_points[j]<lower_number)
            {
                data_out[new_size_of_data_out] = (*((uint8_t*)((uint32_t*)&bitmap[j])));
                lower_number = abs_points[j];
            }
        }

    }
    *(uint32_t*)((uint8_t*)data_out+1026) = new_size_of_data_out-size_of_header;

    for(uint32_t i = 0; i!=size_of_palette_map_buffor; i++) free(all_color_palettes[i]);
    free(all_color_palettes);
    free(numbers_of_repeating_palletes);

    free(bitmap);

    return new_size_of_data_out;
}

uint64_t compress_rgb_into_8bits_map64(const uint8_t* const data_in,uint8_t* const data_out,const uint64_t size_of_data_in,uint64_t size_of_palette_map_buffor)
{
    if(size_of_data_in%3!=0) return 0; //if data for sure isn't in rgb format;
    if(size_of_palette_map_buffor==0) return 0;

    uint8_t** all_color_palettes = (uint8_t**)malloc(size_of_palette_map_buffor*sizeof(uint8_t*));
    for(uint64_t i = 0; i!=size_of_palette_map_buffor; i++) all_color_palettes[i] = (uint8_t*)malloc(3);

    uint64_t* numbers_of_repeating_palletes = (uint64_t*)malloc(size_of_palette_map_buffor*sizeof(uint64_t));
    uint64_t numbers_of_all_palettes = 0;

    uint64_t check_pallete_loop_counter;
    for(uint64_t i = 0; i!=size_of_data_in; i+=3)
    {
        for(check_pallete_loop_counter = 0; check_pallete_loop_counter!=numbers_of_all_palettes; check_pallete_loop_counter++)
        {
            if(*(uint16_t*)((uint8_t*)&all_color_palettes[check_pallete_loop_counter][0])==*(uint16_t*)((uint8_t*)&data_in[i])&&
               all_color_palettes[check_pallete_loop_counter][2]==data_in[i+2])
               {
                    numbers_of_repeating_palletes[check_pallete_loop_counter]++;
                    break;
               }
        }

        if(check_pallete_loop_counter==numbers_of_all_palettes)
        {
            numbers_of_all_palettes++;
            if(numbers_of_all_palettes==size_of_palette_map_buffor) //if array is too small, then allocate more memory;
            {
                uint8_t** temporary_all_color_palletes = (uint8_t**)malloc(size_of_palette_map_buffor*sizeof(uint8_t*));
                for(uint64_t i = 0; i!=size_of_palette_map_buffor; i++) temporary_all_color_palletes[i] = (uint8_t*)malloc(3);
                uint8_t* temporary_numbers_of_repeating_palletes = (uint8_t*)malloc(size_of_palette_map_buffor*sizeof(uint64_t));

                for(uint64_t i = 0; i!=size_of_palette_map_buffor; i++)
                {
                    *(uint16_t*)((uint8_t*)&temporary_all_color_palletes[i][0]) = *(uint16_t*)((uint8_t*)&all_color_palettes[i][0]);
                    temporary_all_color_palletes[i][2] = all_color_palettes[i][2];
                    temporary_numbers_of_repeating_palletes[i] = numbers_of_repeating_palletes[i];
                }

                for(uint64_t i = 0; i!=size_of_palette_map_buffor; i++) free(all_color_palettes[i]);
                free(all_color_palettes);
                free(numbers_of_repeating_palletes);

                size_of_palette_map_buffor*=2;
                all_color_palettes = (uint8_t**)malloc(size_of_palette_map_buffor*sizeof(uint8_t*));
                for(uint64_t i = 0; i!=size_of_palette_map_buffor; i++) all_color_palettes[i] = (uint8_t*)malloc(3);
                numbers_of_repeating_palletes = (uint64_t*)malloc(size_of_palette_map_buffor*sizeof(uint64_t));

                for(uint64_t i = 0; i!=numbers_of_all_palettes; i++)
                {
                    *(uint16_t*)((uint8_t*)&all_color_palettes[i][0]) = *(uint16_t*)((uint8_t*)&temporary_all_color_palletes[i][0]);
                    all_color_palettes[i][2] = temporary_all_color_palletes[i][2];
                    numbers_of_repeating_palletes[i] = temporary_numbers_of_repeating_palletes[i];
                }

                for(uint64_t i = 0; i!=numbers_of_all_palettes; i++) free(temporary_all_color_palletes[i]);
                free(temporary_all_color_palletes);
                free(temporary_numbers_of_repeating_palletes);
            }
            *(uint16_t*)((uint8_t*)&all_color_palettes[check_pallete_loop_counter][0]) = *(uint16_t*)((uint8_t*)&data_in[i]);
            all_color_palettes[check_pallete_loop_counter][2] = data_in[i+2];
            numbers_of_repeating_palletes[check_pallete_loop_counter] = 1;
            continue;
        }
    }

    int32_t size_of_bitmap = (numbers_of_all_palettes>256) ? 256 : numbers_of_all_palettes;

    uint32_t* bitmap = (uint32_t*)malloc(size_of_bitmap*sizeof(uint32_t));
    for(int32_t i = 0; i!=size_of_bitmap; i++) *((uint8_t*)((uint32_t*)&bitmap[i])) = i;

    uint64_t higher_number = numbers_of_repeating_palletes[0];
    uint64_t higher_number_in_array = 0;

    for(uint64_t i = 0,j; i!=size_of_bitmap; i++)
    {
        for(j = 0; j!=numbers_of_all_palettes; j++)
        {
            if(numbers_of_repeating_palletes[j]>higher_number)
            {
                higher_number = numbers_of_repeating_palletes[j];
                higher_number_in_array = j;
            }
        }
        numbers_of_repeating_palletes[higher_number_in_array] = 0;
        *(uint16_t*)((uint8_t*)((uint32_t*)&bitmap[i])+1) = *(uint16_t*)((uint8_t*)&all_color_palettes[higher_number_in_array][0]);
        *((uint8_t*)((uint32_t*)&bitmap[i])+3) = all_color_palettes[higher_number_in_array][2];
        higher_number = 0;
    }

    for(int32_t i = 0; i!=size_of_bitmap; i++) *((uint32_t*)data_out+i) = bitmap[i];
    *(uint16_t*)((uint8_t*)data_out+1024) = size_of_bitmap;

    const uint64_t size_of_header = 1030;

    uint64_t new_size_of_data_out = size_of_header;
    int32_t abs_points[size_of_bitmap];
    uint64_t lower_number;

    int32_t j;
    for(uint64_t i = 0; i!=size_of_data_in; i+=3,new_size_of_data_out++)
    {
        for(j = 0; j!=size_of_bitmap; j++)
        {
            abs_points[j]=abs(*((uint8_t*)((uint32_t*)&bitmap[j])+1)-data_in[i]);
            abs_points[j]+=abs(*((uint8_t*)((uint32_t*)&bitmap[j])+2)-data_in[i+1]);
            abs_points[j]+=abs(*((uint8_t*)((uint32_t*)&bitmap[j])+3)-data_in[i+2]);
        }
        lower_number = abs_points[0];
        data_out[new_size_of_data_out] = (*((uint8_t*)((uint32_t*)&bitmap[0])));
        for(j = 0; j!=size_of_bitmap; j++)
        {
            if(abs_points[j]<lower_number)
            {
                data_out[new_size_of_data_out] = (*((uint8_t*)((uint32_t*)&bitmap[j])));
                lower_number = abs_points[j];
            }
        }

    }
    *(uint32_t*)((uint8_t*)data_out+1026) = new_size_of_data_out-size_of_header;

    for(uint64_t i = 0; i!=size_of_palette_map_buffor; i++) free(all_color_palettes[i]);
    free(all_color_palettes);
    free(numbers_of_repeating_palletes);

    free(bitmap);

    return new_size_of_data_out;
}


uint32_t decompress_8bits_map_into_rgb(const uint8_t* const data_in,uint8_t* const data_out)
{
    uint32_t new_size_of_data_out = 0;
    //uint32_t size_of_rgb_map = *(uint16_t*)((uint8_t*)data_in+1024);
    uint32_t size_of_data_in = *(uint32_t*)((uint8_t*)data_in+1026);

    uint32_t size_of_header = 1030;
    uint32_t loop_iterator = size_of_header;
    uint32_t end_of_loop = size_of_header+size_of_data_in;

    for(; loop_iterator!=end_of_loop; loop_iterator++,new_size_of_data_out+=3)
    {
        *(uint16_t*)((uint8_t*)&data_out[new_size_of_data_out]) = *(uint16_t*)((uint8_t*)&data_in[(data_in[loop_iterator]*4)+1]);
        data_out[new_size_of_data_out+2] = data_in[(data_in[loop_iterator]*4)+3];
    }

    return new_size_of_data_out;

}

uint64_t decompress_8bits_map_into_rgb64(const uint8_t* const data_in,uint8_t* const data_out)
{
    uint32_t new_size_of_data_out = 0;
    //uint32_t size_of_rgb_map = *(uint16_t*)((uint8_t*)data_in+1024);
    uint32_t size_of_data_in = *(uint32_t*)((uint8_t*)data_in+1026);

    uint32_t size_of_header = 1030;
    uint32_t loop_iterator = size_of_header;
    uint32_t end_of_loop = size_of_header+size_of_data_in;

    for(; loop_iterator!=end_of_loop; loop_iterator++,new_size_of_data_out+=3)
    {
        *(uint16_t*)((uint8_t*)&data_out[new_size_of_data_out]) = *(uint16_t*)((uint8_t*)&data_in[(data_in[loop_iterator]*4)+1]);
        data_out[new_size_of_data_out+2] = data_in[(data_in[loop_iterator]*4)+3];
    }

    return new_size_of_data_out;
}

