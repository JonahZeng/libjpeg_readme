# libjpeg解压原始YUV数据
## jpeg解码获得rgb数据
先看一下libjpeg提供的example.c文件中关于如何解压的：
```c
GLOBAL(int) read_JPEG_file (char * filename)
{
  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;

  FILE * infile;		/* source file */
  JSAMPARRAY buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }
  jpeg_create_decompress(&cinfo);


  jpeg_stdio_src(&cinfo, infile);

  (void) jpeg_read_header(&cinfo, TRUE);

  (void) jpeg_start_decompress(&cinfo);
  
  row_stride = cinfo.output_width * cinfo.output_components;

  buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);


  while (cinfo.output_scanline < cinfo.output_height) {
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    put_scanline_someplace(buffer[0], row_stride);
  }

  (void) jpeg_finish_decompress(&cinfo);

  jpeg_destroy_decompress(&cinfo);

  fclose(infile);

  return 1;
}
```
这个例子解压出来的是rgb排列的数据，而不是YUV444/422/420格式，libjpeg根据指定的colorspace做了色彩空间转换。

## jpeg文件解码获取yuv数据
如下的例子展示如何获取真实的YUV数据：
```c
GLOBAL(int) read_JPEG_file(char * filename, char* out_filename)
{
    struct jpeg_decompress_struct cinfo;

    struct my_error_mgr jerr;

    FILE * infile;		/* source file */
    FILE* outfile;

    JSAMPLE* buffer;
    JSAMPARRAY yuvbuffer[3];
    int y_row_stride, y_col_stride;		/* physical row width in output buffer */
    int y_row_blocks, y_col_blocks;
    int u_row_stride, u_col_stride, v_row_stride, v_col_stride;


    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        return 0;
    }

    outfile = fopen(out_filename, "wb");
    if (outfile == NULL) {
        fprintf(stderr, "can't open %s\n", out_filename);
        return 0;
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {

        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return 0;
    }

    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);

    (void)jpeg_read_header(&cinfo, TRUE);
    //注意，在读取了头信息以后，这个时候获取了默认解压参数，我们需要做3个修改
    //1, 修改输出数据的色域，默认是rgb，这里指定为YCbCr
    cinfo.out_color_space = JCS_YCbCr;
    //设置输出raw data，也就是yuv数据
    cinfo.raw_data_out = TRUE;
    //如果想得到yuv444，这个值设置为TRUE。如果想得到yuv422/420，则设置为FALSE
    cinfo.do_fancy_upsampling = FALSE;
    printf("jpeg file color space: %d, out file color space:%d\n", cinfo.jpeg_color_space, cinfo.out_color_space);

    (void)jpeg_start_decompress(&cinfo);
    //调用jpeg_start_decompress后, cinfo会获取到头yuv比例，比如yuv420的话，cinfo.comp_info[0].h_samp_factor/v_samp_factor = 2， uv的 h_samp_factor/v_samp_factor = 1
    if (cinfo.comp_info != NULL) {
        printf("y width_in_block=%d, y horizontal factor=%d\n", cinfo.comp_info[0].width_in_blocks, cinfo.comp_info[0].h_samp_factor);
        printf("y height_in_blocks=%d, y vertical factor=%d\n", cinfo.comp_info[0].height_in_blocks, cinfo.comp_info[0].v_samp_factor);
        printf("u width_in_block=%d, u horizontal factor=%d\n", cinfo.comp_info[1].width_in_blocks, cinfo.comp_info[1].h_samp_factor);
        printf("u height_in_blocks=%d, u vertical factor=%d\n", cinfo.comp_info[1].height_in_blocks, cinfo.comp_info[1].v_samp_factor);
        printf("v width_in_block=%d, v horizontal factor=%d\n", cinfo.comp_info[2].width_in_blocks, cinfo.comp_info[2].h_samp_factor);
        printf("v height_in_blocks=%d, v vertical factor=%d\n", cinfo.comp_info[2].height_in_blocks, cinfo.comp_info[2].v_samp_factor);
    }
    printf("block size=%d\n", cinfo.block_size);
    //这里打印的是DCT block size，通常都是8

    printf("width:%d, height:%d, color components:%d, out colorspace:%d\n", cinfo.output_width, cinfo.output_height, cinfo.out_color_components, cinfo.out_color_space);
    

    y_row_blocks = cinfo.comp_info[0].width_in_blocks;//获取 y 分量的行宽
    y_col_blocks = cinfo.comp_info[1].height_in_blocks;//获取 y 分量的高度

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

    printf("y_row_stride=%d, y_col_stride=%d\n", y_row_stride, y_col_stride);
    printf("u_row_stride=%d, u_col_stride=%d\n", u_row_stride, u_col_stride);
    printf("v_row_stride=%d, v_col_stride=%d\n", v_row_stride, v_col_stride);
    buffer = (JSAMPLE*)malloc(y_row_stride*y_col_stride + u_row_stride*u_col_stride + v_row_stride*v_col_stride);
    //分配所有的yuv缓存空间
    if (buffer == NULL)
    {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        fclose(outfile);
        return -1;
    }
    //把y的指针指向头部
    JSAMPARRAY y_ptr = (JSAMPROW*)malloc(sizeof(JSAMPROW*)*y_col_stride);
    for (int j = 0; j < y_col_stride; j++)
    {
        y_ptr[j] = buffer + y_row_stride*j;
    }
    //u的指针指向y空间的后面
    JSAMPARRAY u_ptr = (JSAMPROW*)malloc(sizeof(JSAMPROW*)*u_col_stride);
    for (int j = 0; j < u_col_stride; j++)
    {
        u_ptr[j] = buffer + y_row_stride*y_col_stride + u_row_stride*j;
    }

    JSAMPARRAY v_ptr = (JSAMPROW*)malloc(sizeof(JSAMPROW*)*v_col_stride);
    for (int j = 0; j < v_col_stride; j++)
    {
        v_ptr[j] = buffer + y_row_stride*y_col_stride + u_row_stride*u_col_stride + v_row_stride*j;
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

    fwrite(buffer, sizeof(JSAMPLE), y_row_stride*y_col_stride + u_row_stride*u_col_stride + v_row_stride*v_col_stride, outfile);

    fclose(outfile);
    free(y_ptr); free(u_ptr); free(v_ptr);
    free(buffer);

    (void)jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(infile);

    return 1;
}
```
