#ifdef _MSC_VER
    #define _CRT_SECURE_NO_WARNINGS
    #define DLL_API  __declspec(dllexport)
#else
    #define DLL_API
#endif

#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>


typedef struct _Tiff_header
{
    char endian_order[2];//"II" or "MM"
    unsigned short tiff_num; //must be 42
    unsigned int first_ifd_offset;
}Tiff_header;

typedef struct _IFD_entry
{
    unsigned short tag;
    unsigned short type;
    unsigned int count;
    unsigned int offset;
}IFD_entry;

typedef struct _IFD
{
    unsigned short IFD_entry_cnt;
    IFD_entry* entry_list;
    unsigned int next_IFD_offset;
}IFD;

static unsigned short convertBigEndian2LittenEndian_u16(unsigned short in)
{
    unsigned short high = (in & 0x00ff) << 8;
    unsigned short low = (in & 0xff00) >> 8;
    return high | low;
}

static unsigned int convertBigEndian2LittenEndian_u32(unsigned int in)
{
    unsigned int a0 = (in & 0x000000ff) << 24;
    unsigned int a1 = (in & 0x0000ff00) << 8;
    unsigned int a2 = (in & 0x00ff0000) >> 8;
    unsigned int a3 = (in & 0xff000000) >> 24;
    return a0 | a1 | a2 | a3;
}

typedef struct _decoded_rgb_t
{
    int orientation;
    int image_width;
    int image_height;
    JSAMPLE* buffer_ptr;
}decoded_rgb_t;

typedef struct _decoded_yuv_t
{
    int orientation;
    int image_width;
    int image_height;
    int buffer_y_width;
    int buffer_y_height;
    int buffer_u_width;
    int buffer_u_height;
    int buffer_v_width;
    int buffer_v_height;
    JSAMPLE* buffer_ptr;
}decoded_yuv_t;

struct my_error_mgr {
    struct jpeg_error_mgr pub;

    jmp_buf setjmp_buffer;  /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

static JSAMPLE* buffer = NULL;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr)cinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message) (cinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}

static unsigned int get_orientation(jpeg_saved_marker_ptr marker_ptr)
{
    if (marker_ptr == NULL)
        return 1;
    while (marker_ptr != NULL && marker_ptr->marker != JPEG_APP0 + 1)
    {
        marker_ptr = marker_ptr->next;
    }
    if (marker_ptr == NULL)
        return 1;

    printf("marker:%d, %d, %d\n", marker_ptr->marker, marker_ptr->original_length, marker_ptr->data_length);

    char exif_code[6] = { '\0' };
    memcpy(exif_code, marker_ptr->data, 6);
    if (strcmp(exif_code, "Exif\0") != 0)
    {
        printf("warnning: no exif found.\n");
        return 1;
    }
    else
    {
        printf("exif found !\n");
    }

    Tiff_header* header = (Tiff_header*)(marker_ptr->data + 6); // "Exif\0\0"
    printf("tiff byte order: %c%c || MM=big-endian, II=litter-endian\n", header->endian_order[0], header->endian_order[1]);
    int endian = 0;//0 = litter-endian, 1 = big-endian
    if (header->endian_order[0] == 'M' && header->endian_order[1] == 'M')
        endian = 1;
    if (endian == 0)
    {
        printf("tiff magic number:%d\n", header->tiff_num);
        printf("APP1 firset IFD offset = %d\n", header->first_ifd_offset);
    }
    else
    {
        header->tiff_num = convertBigEndian2LittenEndian_u16(header->tiff_num);
        printf("tiff magic number:%d\n", header->tiff_num);
        header->first_ifd_offset = convertBigEndian2LittenEndian_u32(header->first_ifd_offset);
        printf("APP1 firset IFD offset = %d\n", header->first_ifd_offset);
    }

    unsigned int ifd_offset = header->first_ifd_offset;
    if (ifd_offset < 8)
    {
        printf("no ifd in APP1\n");
        return 1;
    }
    int find_orientation_tag = FALSE;
    unsigned short orientation = 1;

    while (ifd_offset >= 8)
    {
        IFD ifd_0;
        ifd_0.IFD_entry_cnt = *(unsigned short*)(marker_ptr->data + 6 + ifd_offset);
        if (endian == 1)
        {
            ifd_0.IFD_entry_cnt = convertBigEndian2LittenEndian_u16(ifd_0.IFD_entry_cnt);
        }
        printf("ifd entry cnt = %d\n", ifd_0.IFD_entry_cnt);

        ifd_0.entry_list = (IFD_entry*)(marker_ptr->data + 6 + ifd_offset + 2);
        int i = 0;
        while (i < ifd_0.IFD_entry_cnt)
        {
            IFD_entry* entry = ifd_0.entry_list + i;
            if (endian == 1)
            {
                entry->tag = convertBigEndian2LittenEndian_u16(entry->tag);
            }
            printf("ifd  tag:%d\n", entry->tag);
            if (entry->tag == 274)
            {
                find_orientation_tag = TRUE;
                orientation = (unsigned short)entry->offset;
                if (endian == 1)
                {
                    orientation = convertBigEndian2LittenEndian_u16(orientation);
                    printf("orientation: %d\n", orientation);
                }
            }
            i++;
        }

        ifd_0.next_IFD_offset = *(unsigned int*)(marker_ptr->data + 6 + ifd_offset + ifd_0.IFD_entry_cnt * 12 + 2);
        if (endian == 1)
        {
            ifd_0.next_IFD_offset = convertBigEndian2LittenEndian_u32(ifd_0.next_IFD_offset);
        }
        ifd_offset = ifd_0.next_IFD_offset;
        printf("next ifd (offset relative to tiff header):%d\n", ifd_offset);
    }


    if (find_orientation_tag == FALSE)
    {
        printf("no orientation tag found !\n");
        return 1;
    }
    else
    {
        //IFD_entry* orientation_entry = exif_IFD.entry_list + j;
        //if (endian == 1)
        //    orientation_entry->offset = convertBigEndian2LittenEndian_u32(orientation_entry->offset);
        //return orientation_entry->offset;
        return orientation;
    }
}

DLL_API int read_rgb_from_JPEG_file (char * filename, decoded_rgb_t* out_rgb)
{
  struct jpeg_decompress_struct cinfo;
  jpeg_saved_marker_ptr marker_ptr;
  struct my_error_mgr jerr;

  FILE * infile;
  JSAMPARRAY buffer_line;
  JSAMPROW line;
  int row_stride;

  printf("run %s\n", __FUNCTION__);
  if ((infile = fopen(filename, "rb")) == NULL) 
  {
    fprintf(stderr, "can't open %s\n", filename);
    return -1;
  }

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  if (setjmp(jerr.setjmp_buffer)) 
  {
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return -1;
  }
  jpeg_create_decompress(&cinfo);


  jpeg_stdio_src(&cinfo, infile);

  jpeg_save_markers(&cinfo, JPEG_APP0 + 1, 0xffff);

  (void) jpeg_read_header(&cinfo, TRUE);

  marker_ptr = cinfo.marker_list;
  
  int orientation = get_orientation(marker_ptr);

  (void) jpeg_start_decompress(&cinfo);

  row_stride = cinfo.output_width * cinfo.output_components;

  line = (JSAMPROW)malloc(sizeof(JSAMPLE)*row_stride);
  if (line == NULL)
  {
      printf("malloc line buffer fail!\n");
      jpeg_destroy_decompress(&cinfo);
      fclose(infile);
      return -1;
  }
  buffer_line = &line;

  buffer = malloc(sizeof(JSAMPLE)*row_stride*cinfo.output_height);
  if (buffer == NULL)
  {
      printf("malloc buffer fail!\n");
      jpeg_destroy_decompress(&cinfo);
      fclose(infile);
      free(line);
      return -1;
  }
  printf("buffer address=%p\n", buffer);

  while (cinfo.output_scanline < cinfo.output_height)
  {
    (void) jpeg_read_scanlines(&cinfo, buffer_line, 1);
    //printf("%d\n",cinfo.output_scanline);
    memcpy(&buffer[row_stride*(cinfo.output_scanline-1)], buffer_line[0], row_stride);
  }
  out_rgb->orientation = orientation;
  out_rgb->buffer_ptr = buffer;
  out_rgb->image_width = cinfo.output_width;
  out_rgb->image_height = cinfo.output_height;
  printf("image width = %d, image height = %d\n", out_rgb->image_width, out_rgb->image_height);

  (void) jpeg_finish_decompress(&cinfo);

  jpeg_destroy_decompress(&cinfo);

  fclose(infile);
  free(line);

  return 0;
}




//这个例子解压出来的是rgb排列的数据，而不是YUV444/422/420格式，libjpeg根据指定的colorspace做了色彩空间转换。

//jpeg文件解码获取yuv数据
//如下的例子展示如何获取真实的YUV数据：

DLL_API int read_raw_data_from_JPEG_file(char * filename, decoded_yuv_t* out_yuv)
{
    struct jpeg_decompress_struct cinfo;

    struct my_error_mgr jerr;

    FILE * infile;      /* source file */

    JSAMPARRAY yuvbuffer[3];
    JSAMPARRAY y_ptr;
    JSAMPARRAY u_ptr;
    JSAMPARRAY v_ptr;
    int y_row_stride, y_col_stride;     /* physical row width in output buffer */
    unsigned int y_row_blocks = 0, y_col_blocks = 0;
    int u_row_stride, u_col_stride, v_row_stride, v_col_stride;

    printf("run %s\n", __FUNCTION__);
    infile = fopen(filename, "rb");
    if (infile == NULL)
    {
        fprintf(stderr, "can't open %s\n", filename);
        return -1;
    }


    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {

        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return -1;
    }

    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);

    jpeg_save_markers(&cinfo, JPEG_APP0 + 1, 0xffff);

    (void)jpeg_read_header(&cinfo, TRUE);
    //注意，在读取了头信息以后，这个时候获取了默认解压参数，我们需要做3个修改
    //1, 修改输出数据的色域，默认是rgb，这里指定为YCbCr
    cinfo.out_color_space = cinfo.jpeg_color_space;
    //设置输出raw data，也就是yuv数据
    cinfo.raw_data_out = TRUE;
    //如果想得到yuv444，这个值设置为TRUE。如果想得到yuv422/420，则设置为FALSE
    cinfo.do_fancy_upsampling = FALSE;
    printf("    JCS_UNKNOWN=0,		 error/unspecified\n \
    JCS_GRAYSCALE=1,		monochrome\n \
    JCS_RGB=2,		red/green/blue, standard RGB (sRGB)\n \
    JCS_YCbCr=3,		 Y/Cb/Cr (also known as YUV), standard YCC\n \
    JCS_CMYK=4,		C/M/Y/K\n \
    JCS_YCCK=5,		Y/Cb/Cr/K\n \
    JCS_BG_RGB=6,		big gamut red/green/blue, bg-sRGB\n \
    JCS_BG_YCC=7		big gamut Y/Cb/Cr, bg-sYCC \n");
    printf("jpeg file color space: %d, out file color space:%d\n", cinfo.jpeg_color_space, cinfo.out_color_space);

    int orientation = get_orientation(cinfo.marker_list);
    (void)jpeg_start_decompress(&cinfo);
    //调用jpeg_start_decompress后, cinfo会获取到头yuv比例，比如yuv420的话，cinfo.comp_info[0].h_samp_factor/v_samp_factor = 2， uv的 h_samp_factor/v_samp_factor = 1
    if (cinfo.comp_info != NULL) {
        printf("compenent[0] width_in_block=%d, y horizontal factor=%d\n", cinfo.comp_info[0].width_in_blocks, cinfo.comp_info[0].h_samp_factor);
        printf("compenent[0] height_in_blocks=%d, y vertical factor=%d\n", cinfo.comp_info[0].height_in_blocks, cinfo.comp_info[0].v_samp_factor);
        printf("compenent[1] width_in_block=%d, u horizontal factor=%d\n", cinfo.comp_info[1].width_in_blocks, cinfo.comp_info[1].h_samp_factor);
        printf("compenent[1] height_in_blocks=%d, u vertical factor=%d\n", cinfo.comp_info[1].height_in_blocks, cinfo.comp_info[1].v_samp_factor);
        printf("compenent[2] width_in_block=%d, v horizontal factor=%d\n", cinfo.comp_info[2].width_in_blocks, cinfo.comp_info[2].h_samp_factor);
        printf("compenent[2] height_in_blocks=%d, v vertical factor=%d\n", cinfo.comp_info[2].height_in_blocks, cinfo.comp_info[2].v_samp_factor);
    }
    printf("block size=%d\n", cinfo.block_size);
    //这里打印的是DCT block size，通常都是8

    printf("image output width:%d, height:%d, color components:%d, out colorspace:%d\n", cinfo.output_width, cinfo.output_height, cinfo.out_color_components, cinfo.out_color_space);


    y_row_blocks = cinfo.comp_info[0].width_in_blocks;//获取 y 分量的行宽
    y_col_blocks = cinfo.comp_info[0].height_in_blocks;//获取 y 分量的高度

    while (y_row_blocks < cinfo.comp_info[1].width_in_blocks*cinfo.comp_info[0].h_samp_factor / cinfo.comp_info[1].h_samp_factor \
        || y_row_blocks < cinfo.comp_info[2].width_in_blocks*cinfo.comp_info[0].h_samp_factor / cinfo.comp_info[2].h_samp_factor)
    {
        y_row_blocks++;
    }

    while (y_col_blocks < cinfo.comp_info[1].height_in_blocks*cinfo.comp_info[0].v_samp_factor / cinfo.comp_info[1].v_samp_factor \
        || y_col_blocks < cinfo.comp_info[2].height_in_blocks*cinfo.comp_info[0].v_samp_factor / cinfo.comp_info[2].v_samp_factor)
    {
        y_col_blocks++;
    }
    //计算真正的y row/col,根据uv的DCT block计算
    y_row_stride = cinfo.block_size * y_row_blocks;
    y_col_stride = cinfo.block_size * y_col_blocks;
    u_row_stride = cinfo.comp_info[1].width_in_blocks*cinfo.block_size;
    u_col_stride = cinfo.comp_info[1].height_in_blocks*cinfo.block_size;
    v_row_stride = cinfo.comp_info[2].width_in_blocks*cinfo.block_size;
    v_col_stride = cinfo.comp_info[2].height_in_blocks*cinfo.block_size;

    printf("(output buffer)padded compnent[0]_width=%d, compnent[0]_height=%d\n", y_row_stride, y_col_stride);
    printf("(output buffer)padded compnent[1]_width=%d, compnent[1]_height=%d\n", u_row_stride, u_col_stride);
    printf("(output buffer)padded compnent[2]_width=%d, compnent[2]_height=%d\n", v_row_stride, v_col_stride);
    buffer = (JSAMPLE*)malloc(y_row_stride*y_col_stride + u_row_stride * u_col_stride + v_row_stride * v_col_stride);
    printf("buffer address = %p\n", buffer);
    //分配所有的yuv缓存空间
    if (buffer == NULL)
    {
        printf("buffer==null\n");
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return -1;
    }
    //把y的指针指向头部
    y_ptr = (JSAMPROW*)malloc(sizeof(JSAMPROW*)*y_col_stride);
    if(y_ptr==NULL)
    {
        printf("y_ptr==null\n");
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        free(buffer);
        buffer = NULL;
        return -1;
    }
    for (int j = 0; j < y_col_stride; j++)
    {
        y_ptr[j] = buffer + y_row_stride * j;
    }
    //u的指针指向y空间的后面
    u_ptr = (JSAMPROW*)malloc(sizeof(JSAMPROW*)*u_col_stride);
    if (u_ptr == NULL)
    {
        printf("u_ptr==null\n");
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        free(buffer);
        buffer = NULL;
        free(y_ptr);
        return -1;
    }
    for (int j = 0; j < u_col_stride; j++)
    {
        u_ptr[j] = buffer + y_row_stride * y_col_stride + u_row_stride * j;
    }

    v_ptr = (JSAMPROW*)malloc(sizeof(JSAMPROW*)*v_col_stride);
    if (v_ptr == NULL)
    {
        printf("v_ptr==null\n");
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        free(buffer);
        buffer = NULL;
        free(y_ptr);
        free(u_ptr);
        return -1;
    }
    for (int j = 0; j < v_col_stride; j++)
    {
        v_ptr[j] = buffer + y_row_stride * y_col_stride + u_row_stride * u_col_stride + v_row_stride * j;
    }

    yuvbuffer[0] = y_ptr;
    yuvbuffer[1] = u_ptr;
    yuvbuffer[2] = v_ptr;
    //按照参数要求，把yuv三个指针组成一个数组传给函数
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_raw_data(&cinfo, yuvbuffer, cinfo.comp_info[0].v_samp_factor*cinfo.block_size);
        yuvbuffer[0] += cinfo.comp_info[0].v_samp_factor*cinfo.block_size;
        yuvbuffer[1] += cinfo.comp_info[1].v_samp_factor*cinfo.block_size;
        yuvbuffer[2] += cinfo.comp_info[2].v_samp_factor*cinfo.block_size;
        //每次读取DCT*max_v_samp_factor行以后，手动把指针指向的缓存区往后移动
    }

    //fwrite(buffer, sizeof(JSAMPLE), y_row_stride*y_col_stride + u_row_stride * u_col_stride + v_row_stride * v_col_stride, outfile);

    //fclose(outfile);
    free(y_ptr); free(u_ptr); free(v_ptr);
    //free(buffer);
    out_yuv->orientation = orientation;
    out_yuv->buffer_ptr = buffer;
    out_yuv->image_width = cinfo.image_width;
    out_yuv->image_height = cinfo.image_height;
    out_yuv->buffer_y_width = cinfo.block_size * y_row_blocks;
    out_yuv->buffer_y_height = cinfo.block_size * y_col_blocks;
    out_yuv->buffer_u_width = cinfo.comp_info[1].width_in_blocks* cinfo.block_size;
    out_yuv->buffer_u_height = cinfo.comp_info[1].height_in_blocks* cinfo.block_size;
    out_yuv->buffer_v_width = cinfo.comp_info[2].width_in_blocks* cinfo.block_size;
    out_yuv->buffer_v_height = cinfo.comp_info[2].height_in_blocks* cinfo.block_size;
    jpeg_destroy_decompress(&cinfo);

    fclose(infile);

    return 0;
}


DLL_API int release_buffer()
{
    printf("run %s\n", __FUNCTION__);
    if (buffer != NULL)
    {
        printf("free buffer: %p\n", buffer);
        free(buffer);
    }
    return 0;
}

/*int main(int argc, char* argv[])
{
    decoded_yuv_t yuv;
    read_raw_data_from_JPEG_file(argv[1], &yuv);
    release_buffer();
    return 0;
}*/
