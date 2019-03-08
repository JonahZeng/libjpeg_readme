## 原始的下采样数据

&ensp;&ensp;&ensp;&ensp;一些程序要求提供已经经过downsample的图像数据给JPEG压缩器，或者要求从解压器接收经过下采样的数据。JPEG库通过允许读写下采样的原始数据支持这样的要求，而不经过预处理或者后处理步骤。这样的操作接口相比标准操作有点复杂。如果你仅仅只是想省略色域转换，你可以仅仅在压缩的时候设置`jpeg_color_space = in_color_space`，在解压的时候设置`jpeg_color_space = out_color_space`来达到这个目的。本节讨论的必要内容是提供或接受经过下采样的图像数据，它们的各个分量的几何尺寸是不同的。

&ensp;&ensp;&ensp;&ensp;在压缩阶段，你必须提供在JPEG文件里使用的色彩空间的图像数据，并根据JPEG参数里设置的下采样比例进行下采样操作。你必须按照JPEG库的内部格式要求提供数据，格式为`JSAMPIMAGE`，它指向JSAMPARRAY的指针数组，每个JSAMPARRAY指向一个成员分量。这样的结构是必需的，因为各个分量占据的空间大小是不一样的。如果图像的维度不是MCU的整倍数，你必须手动添加额外的数据进行对齐，通常是重复最后一行/一列。最后的空间长度，宽度必需是DCT block的整倍数：这是因为，每个色彩分量经过下采样的行都必需包含整数倍的DCT block size，对于很多应用来说，标准的图像尺寸经常是DCT block size的整数倍，所以可以省略额外的对齐操作。

&ensp;&ensp;&ensp;&ensp;压缩raw data的步骤和正常的步骤差不多，除了用`jpeg_write_raw_data()`代替`jpeg_write_scanlines()`。在调用`jpeg_start_compress()`之前，你必须完成以下设置：

  * 把`cinfo->raw_data_in`设置为`TRUE`。(默认情况下`jpeg_set_defaults()`会把这个设置为`FALSE`)，这个参数通知JPEG库，你提供的是raw data。然后把`cinfo->do_fancy_downsampling`设置为`FALSE`如果你提供经过下采样的数据的话。(这个值在调用`jpeg_set_defaults()`的时候会被设置为`TRUE`)
  * 确保色彩空间设置正确。 --- 通过`jpeg_set_colorspace()`显式的设置色彩空间是个好办法。请注意，因为色彩转换在这里被略过，除非使用了`jpeg_set_defaults()`选择默认的jpeg_color_space。
  * 确保采样因子系数正确`cinfo->comp_info[i].h_samp_factor`和`cinfo->comp_info[i].v_samp_factor`是正确的，因为这两个值表明了你提供的数据的尺寸维度，最好显式的设置这两个值，不要假设库提供的默认值符合你的需求。

&ensp;&ensp;&ensp;&ensp;要把raw data传递给JPEG库，你需要使用`jpeg_write_raw_data()`替换`jpeg_write_scanlines()`。这两个工作方式是类似的，除了一处不同：`jpeg_write_raw_data` 第二个参数是`JSAMPIMAGE`而不是原来的`JSAMPARRAY`。`jpeg_write_raw_data()`返回的扫描行数量是拥有最大`v_samp_factor`分量的行数。

&ensp;&ensp;&ensp;&ensp;每调用一次`jpeg_write_raw_data()`，都将会处理一个MCU行数，也就是说，每次调用处理的是每个分量的`v_samp_factor*MCU block_size`行数。传递的第三个参数`num_lines`必须是满足最小条件：`max_v_samp_factor*block_size`，返回值一定是这个值（或者是这个值的整数倍，将来的版本中可能是这样）。即使是到了图像的底部，这个情况也是成立的；千万不要忘记申请的buff要进行block_size补齐。

进行压缩要求提供的各分量数据大小可以用如下公式计算：

```c
cinfo->comp_info[i].width_in_blocks*block_size;  //samples per row
cinfo->comp_info[i].height_in_blocks*block_size; //rows in image
```
&ensp;&ensp;&ensp;&ensp;在调用`jpeg_start_compress()`后这些值会被初始化。如果可用的数据小于这个尺寸，则一定要进行补齐。对于某些采样系数和图像尺寸，需要在末尾插入额外的DCT block来保证是MCU维度的整倍数。JPEG库会自己完成额外的DCT block添加，但它不是从你提供的数据读取。所以你永远不需要添加超过一个block size的数据来补齐。这里有一个例子，假设YUV420格式（长宽都缩减2倍）：
```c
cinfo->comp_info[0].h_samp_factor = 2;		//for Y
cinfo->comp_info[0].v_samp_factor = 2;
cinfo->comp_info[1].h_samp_factor = 1;		//for Cb
cinfo->comp_info[1].v_samp_factor = 1;
cinfo->comp_info[2].h_samp_factor = 1;		//for Cr
cinfo->comp_info[2].v_samp_factor = 1;
```
&ensp;&ensp;&ensp;&ensp;我们假设名义上这个图片的尺寸`cinfo->image_width`和`cinfo->image_height`是101x101 pixels。然后调用`jpeg_start_compress()`将计算出Y通道的下采样宽度`downsampled_width = 101`，`width_in_blocks = 13`，Cb和Cr的
`downsampled_width = 51` and `width_in_blocks = 7`，这里你必须对Y数据进行补齐，补齐到至少`13*8=104`行和列，Cb、Cr必须至少`7*8=56`行和列。MCU高度是`max_v_samp_factor = 2`x DCT rows，所以你每次调用`jpeg_write_raw_data()`需要传递至少16个扫描行，也就是说Y 16行，Cb、Cr 8行。总共需要7个MCU行，所以你需要分配至少112个扫描行，最后一个Y数据的DCT row是虚假无意义的，所以你传递什么都可以，但是一定要保证112行空间来使得Cb、Cr保持完整。

&ensp;&ensp;&ensp;&ensp;在raw-data压缩过程中，输出中断是可以做到的：如果data destination module挂起，`jpeg_write_raw_data()`将返回0，这个时候一定要在下一次调用`jpeg_write_raw_data()`的时候传递相同的data rows。

&ensp;&ensp;&ensp;&ensp;解压raw data也就意味着省略所有的后处理操作（比如色彩空间转换），你必须自己处理色彩空间和采样系数，如果你的程序只处理比如YCbCr 422格式，那么你就必须检查是否有其他色彩空间和采样系数。JPEG库将不会为你做色彩空间转化。

&ensp;&ensp;&ensp;&ensp;为了获取raw data输出，要在`jpeg_start_decompress()`之前设置`cinfo->raw_data_out = TRUE` (这个值会被`jpeg_read_header()`设置为 FALSE )。 同时，你要确认色彩空间和采样系数是你需要的类型。然后再设置`cinfo->do_fancy_upsampling = FALSE`，如果你想获取真正的raw-data的话(这个值也被`jpeg_read_header()`设置为TRUE)。
然后用`jpeg_read_raw_data()`取代`jpeg_read_scanlines()`。解压过程和之前的标准解压过程基本类似。

&ensp;&ensp;&ensp;&ensp;调用`jpeg_read_raw_data()`返回一个MCU row，所以你必须传递至少`max_v_samp_factor*block_size`个扫描行的buffer。你所传递的buffer必须足够大，最少能够容纳实际的数据加上DCT-block补齐的边界数据。和压缩相比，解压不处理额外的虚假DCT block，所以你不必为此分配空间，但是total scanline数量是包含额外的DCT block的。上面的计算buffer维度的例子也适用于解压过程。

解压支持suspension输入: 如果data source
module挂起，`jpeg_read_raw_data()`将返回0。你可以在multiple passes情况下使用buffered-image mode来读取输入。
