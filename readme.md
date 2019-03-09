使用IJG JPEG LIBRARY
========
&ensp;&ensp;&ensp;&ensp;该文件描述了如何在应用程序中使用IJG JPEG库程序。 如果要编写使用该库的程序，请阅读它。

&ensp;&ensp;&ensp;&ensp;文件example.c提供了大量注释的框架代码，用于调用JPEG库。 另请参阅jpeglib.h（应用程序要使用的包含文件）程序）有关数据结构和函数参数列表的完整详细信息。当然，库源代码是最终的参考。

&ensp;&ensp;&ensp;&ensp;请注意，目前应用程序接口已发生**重大**更改，区别于IJG第4版和早期版本提供。旧版本存在一些固有的缺陷, 在我们不断添加新特性的时候我们尽量最小化接口的改动，因此积累了很多坏的设计；在重写第五版的时候我们牺牲了向后兼容性，但是我们认为这是值得的；

# 目录
- [使用IJG JPEG LIBRARY](#%E4%BD%BF%E7%94%A8ijg-jpeg-library)
- [目录](#%E7%9B%AE%E5%BD%95)
- [总览](#%E6%80%BB%E8%A7%88)
	- [库提供的功能](#%E5%BA%93%E6%8F%90%E4%BE%9B%E7%9A%84%E5%8A%9F%E8%83%BD)
	- [典型用法概述](#%E5%85%B8%E5%9E%8B%E7%94%A8%E6%B3%95%E6%A6%82%E8%BF%B0)
	- [# 基本的库使用方法](#%E5%9F%BA%E6%9C%AC%E7%9A%84%E5%BA%93%E4%BD%BF%E7%94%A8%E6%96%B9%E6%B3%95)
	- [数据形式](#%E6%95%B0%E6%8D%AE%E5%BD%A2%E5%BC%8F)
	- [压缩操作细节](#%E5%8E%8B%E7%BC%A9%E6%93%8D%E4%BD%9C%E7%BB%86%E8%8A%82)
		- [分配并初始化一个JPEG compression对象。](#%E5%88%86%E9%85%8D%E5%B9%B6%E5%88%9D%E5%A7%8B%E5%8C%96%E4%B8%80%E4%B8%AAjpeg-compression%E5%AF%B9%E8%B1%A1)
		- [为压缩后的数据指定输出目标(比如，一个文件)](#%E4%B8%BA%E5%8E%8B%E7%BC%A9%E5%90%8E%E7%9A%84%E6%95%B0%E6%8D%AE%E6%8C%87%E5%AE%9A%E8%BE%93%E5%87%BA%E7%9B%AE%E6%A0%87%E6%AF%94%E5%A6%82%E4%B8%80%E4%B8%AA%E6%96%87%E4%BB%B6)
		- [压缩设置参数，包括图像大小和色彩空间](#%E5%8E%8B%E7%BC%A9%E8%AE%BE%E7%BD%AE%E5%8F%82%E6%95%B0%E5%8C%85%E6%8B%AC%E5%9B%BE%E5%83%8F%E5%A4%A7%E5%B0%8F%E5%92%8C%E8%89%B2%E5%BD%A9%E7%A9%BA%E9%97%B4)
		- [调用jpeg_start_compress(...);](#%E8%B0%83%E7%94%A8jpegstartcompress)
		- [while (scan lines remain to be written) jpeg_write_scanlines(...);](#while-scan-lines-remain-to-be-written-jpegwritescanlines)
		- [jpeg_finish_compress(...);](#jpegfinishcompress)
		- [释放JPEG压缩对象](#%E9%87%8A%E6%94%BEjpeg%E5%8E%8B%E7%BC%A9%E5%AF%B9%E8%B1%A1)
		- [中断](#%E4%B8%AD%E6%96%AD)
	- [解压缩细节](#%E8%A7%A3%E5%8E%8B%E7%BC%A9%E7%BB%86%E8%8A%82)
		- [1 分配并初始化一个JPEG解压缩对象](#1-%E5%88%86%E9%85%8D%E5%B9%B6%E5%88%9D%E5%A7%8B%E5%8C%96%E4%B8%80%E4%B8%AAjpeg%E8%A7%A3%E5%8E%8B%E7%BC%A9%E5%AF%B9%E8%B1%A1)
		- [2 指定一个待解压的源对象(比如，一个文件)](#2-%E6%8C%87%E5%AE%9A%E4%B8%80%E4%B8%AA%E5%BE%85%E8%A7%A3%E5%8E%8B%E7%9A%84%E6%BA%90%E5%AF%B9%E8%B1%A1%E6%AF%94%E5%A6%82%E4%B8%80%E4%B8%AA%E6%96%87%E4%BB%B6)
		- [3 调用`jpeg_read_header()`获取文件头信息](#3-%E8%B0%83%E7%94%A8jpegreadheader%E8%8E%B7%E5%8F%96%E6%96%87%E4%BB%B6%E5%A4%B4%E4%BF%A1%E6%81%AF)
		- [4 设置解压参数](#4-%E8%AE%BE%E7%BD%AE%E8%A7%A3%E5%8E%8B%E5%8F%82%E6%95%B0)
		- [5 jpeg_start_decompress(...);](#5-jpegstartdecompress)
		- [6 while (scan lines remain to be read) jpeg_read_scanlines(...);](#6-while-scan-lines-remain-to-be-read-jpegreadscanlines)
		- [7 jpeg_finish_decompress(...);](#7-jpegfinishdecompress)
		- [8 释放JPEG解压对象](#8-%E9%87%8A%E6%94%BEjpeg%E8%A7%A3%E5%8E%8B%E5%AF%B9%E8%B1%A1)
		- [9 中断](#9-%E4%B8%AD%E6%96%AD)
- [高级特性](#%E9%AB%98%E7%BA%A7%E7%89%B9%E6%80%A7)
	- [Compression parameter selection](#compression-parameter-selection)
	- [Decompression parameter selection](#decompression-parameter-selection)
	- [Special color spaces](#special-color-spaces)
	- [Error handling](#error-handling)
	- [Compressed data handling (source and destination managers)](#compressed-data-handling-source-and-destination-managers)
	- [I/O suspension](#io-suspension)
	- [Progressive JPEG support](#progressive-jpeg-support)
	- [Buffered-image mode](#buffered-image-mode)
	- [Abbreviated datastreams and multiple images](#abbreviated-datastreams-and-multiple-images)
	- [Special markers](#special-markers)
	- [原始的下采样数据](#%E5%8E%9F%E5%A7%8B%E7%9A%84%E4%B8%8B%E9%87%87%E6%A0%B7%E6%95%B0%E6%8D%AE)
	- [Really raw data: DCT coefficients](#really-raw-data-dct-coefficients)
	- [Progress monitoring](#progress-monitoring)
	- [Memory management](#memory-management)
	- [Memory usage](#memory-usage)
	- [Library compile-time options](#library-compile-time-options)
	- [Portability considerations](#portability-considerations)
	- [Notes for MS-DOS implementors](#notes-for-ms-dos-implementors)


# 总览

## 库提供的功能


&ensp;&ensp;&ensp;&ensp;IJG JPEG库提供C代码来读取和写入JPEG压缩图像文件。 周围的应用程序接每次收或提供一个扫描行未经压缩的图像数据。所有颜色转换和其他预处理/后处理的细节都可以交给库处理。

&ensp;&ensp;&ensp;&ensp;该库包含大量未涵盖JPEG标准的代码，但对于JPEG的典型应用是必需的。 这些功能主要是在JPEG压缩之前预处理图像或在解压缩之后进行后处理。 它们包括色彩空间转换，下采样/上采样，和颜色量化。  
应用程序通过指定希望提供或接收图像数据的格式，间接选择使用这些代码。
例如，如果请求了colormapped输出，那么解压缩库自动调用颜色量化。

&ensp;&ensp;&ensp;&ensp;JPEG处理过程中有广泛的质量与速度权衡，在解压后处理中更是如此。 解压缩库提供涵盖大多数有用权衡的多种实现，从高品质到快速预览操作。在压缩方面我们一般都没有提供低质量的选择，因为压缩通常对时间要求较低。 应该注意的是低质量模式可能不符合JPEG标准的精度要求;尽管如此，它们对观众很有用。

&ensp;&ensp;&ensp;&ensp;一句话概括库未提供的功能。 我们提供的是ISO JPEG标准子集; 包括大多数基线，扩展序列和渐进式JPEG支持。（我们的子集包括现在共有的所有使用的功能。）不支持的ISO选项包括：
1. Hierarchical storage（分层存储）
2. Lossless JPEG（无损JPEG）
3. DNL marker（DNL标记）
4. Nonintegral subsampling ratios（非整数子采样率）
   
&ensp;&ensp;&ensp;&ensp;我们支持8位到12位数据精度，但这是编译时的选择而不是运行时选择; 因此很难在单个应用程序中使用不同的的精度。

&ensp;&ensp;&ensp;&ensp;本身而言，库只处理交换JPEG数据流---特别是广泛使用的JFIF文件格式。 该库可以被周围的代码调用来处理交换或缩略的嵌入在更复杂的文件格式中JPEG数据。比如, LIBTIFF库使用了这个库来支持在TIFF文件中使用JPEG压缩。


## 典型用法概述

JPEG压缩操作的大致流程是:

* 分配并初始化一个JPEG compression 对象
* 给compressed data指定输出目标，比如一个文件
* 设置参数, 包括image大小 & 色彩空间
* jpeg_start_compress(...);
* while (scan lines remain to be written)
      jpeg_write_scanlines(...);
* jpeg_finish_compress(...);
* 释放JPEG compression对象

&ensp;&ensp;&ensp;&ensp;对于JPEG library来说，一个JPEG compression 对象保存参数以及工作运行状态。  我们把对象的创建/销毁同图像压缩开始/结束分离；同一个对象可以被一系列的压缩操作所复用。这样就可以很容易地给一系列图像操作设置相同的参数。复用JPEG对象对处理缩略JPEG数据流也有重要意义，这一点稍后讨论。

&ensp;&ensp;&ensp;&ensp;要压缩的图像数据从内存缓冲区提供给jpeg_write_scanlines（）函数。 如果应用程序进行文件到文件压缩，从源文件中读取图像数据是应用程序的职责。

&ensp;&ensp;&ensp;&ensp;libjpeg库通过调用“data destination manager”发出压缩数据，通常会将数据写入文件; 但应用程序可以提供自己的destination manager来做其他事情。

类似的, JPEG解压缩的大致流程是:

* 分配并初始化一个 JPEG decompression 对象
* 指定源文件(被压缩的图像文件)
* 调用jpeg_read_header()获取图像信息
* 设置decompression参数
* jpeg_start_decompress(...);
    while (scan lines remain to be read)
* jpeg_read_scanlines(...);
* jpeg_finish_decompress(...);
* 释放JPEG decompression 对象

&ensp;&ensp;&ensp;&ensp;解压和压缩操作大部分是类似的，除了读取数据流头信息这一步。这个步骤非常有用，因为可以获取到图像的size信息,色彩空间，等等。这些值可以用来设置解压参数。比如，应用程序可以选择一个缩放的输出比例，来适应当前可用的屏幕大小。

&ensp;&ensp;&ensp;&ensp;JPEG解压操作通过调用一个data source manager获取源压缩数据, 通常这个管理器从一个文件读取源压缩数据; 但是用户也可以通过自定义source manager来提供其他操作。 解压后的数据被传递到内存缓冲区，之后传递给jpeg_read_scanlines()函数。

&ensp;&ensp;&ensp;&ensp;通过调用jpeg_abort()可以中断一个进行中的压缩/解压操作; 或者，如果你不再需要JPEG对象,调用jpeg_destroy()释放掉它。

&ensp;&ensp;&ensp;&ensp;JPEG 压缩/解压对象是两个单数的数据类型。尽管如此，它们还是会共享一些公共字段，一些例行方法也可以兼容处理这两种对象，比如jpeg_destroy()函数。

&ensp;&ensp;&ensp;&ensp;JPEG库没有静态变量: 所有的状态都放在compression或者decompression对象中。因此可以通过使用多个对象同时处理多个JPEG压缩和解压缩操作。

&ensp;&ensp;&ensp;&ensp;提供合适的source/destination管理器，解压和压缩操作都可以以增量内存--内存的方式完成。具体细节请参考"I/O suspension".


# 基本的库使用方法
----------

## 数据形式

&ensp;&ensp;&ensp;&ensp;在深入了解程序细节之前，了解一下JPEG库期望或返回的图像数据格式是非常帮助的。
 
&ensp;&ensp;&ensp;&ensp;标准的输入图像是一个矩形pixel数组，每个pixel拥有相同的色彩通道。
你必须指定具有多少个色彩通道，同时解释Pixel对应的色彩空间。
大多数程序使用rgb或者gray色彩空间。注意：rgb有3个通道，gray只有一个通道，很多人忘记了这个导致程序不兼容gray图像。

&ensp;&ensp;&ensp;&ensp;没有规定要输入要使用哪种色彩空间映射。JPEG通常要么是全彩要么是黑白图像，有时候也会有CMYK这种色彩空间。
您可以通过将其扩展为全彩色格式来输入色彩空间映射图像。 然而对于已经进行色彩映射的源数据，因为抖动的噪音，JPEG通常不能很好地工作。这在JPEG FAQ中有更详细的讨论，以及README文件中提到的其他引用。

&ensp;&ensp;&ensp;&ensp;pixel以扫描行的方式存储，每一行表示从左到右方向。每个pixel的分量在行里面的位置是相邻的；比如对于24-bit的rgb图像, 存储顺序是R,G,B,R,G,B,R,G,B,...每一个扫描行都是JSAMPLE结构的数组，JSAMPLE通常是"unsigned char",除非你在jmorecfg.h里面做了修改。（通过修改jmorecfg.h，你也可以修改RGB pixel的排列顺序比如B,G,R顺序, 修改之前请仔细阅读这个文件中的规则限制）

&ensp;&ensp;&ensp;&ensp;一个2-D的pixel数组，构成方式是一个List保存每一扫描行的头指针，因此扫描行没有必要一定要在内存中相邻。
即使你一次只处理一个扫描行，你也得必须构建一个这样的结构。指向JSAMPLE行的指针被定义为JSAMPROW，指向这这个指针数组的指针被定义为JSAMPARRAY。

&ensp;&ensp;&ensp;&ensp;JPEG库每次处理都接受或者提供一行或者多行scalline，不可能一次只处理一行的一部分。Scanlines 总是从上到下顺序。
你可以一次处理整个图像，如果你一次把所有的数据都读到内存中，但通常来说，每次处理一个扫描行是最简单的方式； 

&ensp;&ensp;&ensp;&ensp;为了获取最好的结果, 源数据需要拥有指定的位宽精度，这个位宽由BITS_IN_JSAMPLE定义(一般都是8 bits)。
比方说，如果你选择压缩数据为6 bits/channel，那你首先就要把数据左移到8bit（一个byte）对齐，然后再传递给压缩处理。  
如果你的数据超过了8 bits/channel, 则需要再编译前设置BITS_IN_JSAMPLE = 9 ~ 12这个范围内。
(细节请参考后续的"Library compile-time options")


&ensp;&ensp;&ensp;&ensp;解压缩器返回的数据格式在所有细节中都是相同的，除非支持色彩映射输出。（同样，JPEG文件永远不会做色域转换，但是你可以要求解压缩器执行即时颜色
量化以提供色彩映射输出）
如果你要求进行色彩空间转换，输出的数据中一个Pixel包含一个JSAMPLE，它的值是对应的色彩空间的索引。
注意因为色彩空间索引值是存储在JSAMPLE中，所以色彩种类数量被JSAMPLE位宽限制：比如 8 bit的JPEG最多表示256种色彩。


## 压缩操作细节


这里我们再次回顾一下JPEG压缩操作的概述。

### 分配并初始化一个JPEG compression对象。
&ensp;&ensp;&ensp;&ensp;一个JPEG compression对象一个"struct jpeg_compress_struct"实例。（它也有一堆通过malloc（）分配的子结构，但是应用程序不直接控制它们。）如果是在一个例程执行整个JPEG压缩序列，这个结构可以是一个调用过程中的局部变量。 否则它可以是静态的或来自malloc（）分配的堆变量。

&ensp;&ensp;&ensp;&ensp;你还需要一个结构体来表示JPEG错误处理句柄。在JPEG库里面，这个结构体是`struct jpeg_error_mgr`。如果你想自己实现一个错误处理器，通常情况下你会把`jpeg_error_mgr struct` 这个结构嵌入到一个大的结构体当中；这部分我们会在后续的"Errorhandling"介绍。现在我们假设你只使用默认的错误处理器，默认的错误处理器将会把error/warning信息打印出来。

&ensp;&ensp;&ensp;&ensp;用户必须首先初始化错误处理器结构体，然后让JPEG对象的`err`成员指针指向这个错误处理器，再然后调用`jpeg_create_compress()`去初始化剩余的JPEG对象成员。

典型的代码如下，如果你使用默认的错误处理器的话：
```C++
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	/*.....*/
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
```
`jpeg_create_compress`分配一个小部分堆内存，所以它可能会调用失败，如果内存耗尽的话。这种情况下，程序会通过错误处理器退出。这既是为什么一定要首先初始化错误管理器。


### 为压缩后的数据指定输出目标(比如，一个文件)

&ensp;&ensp;&ensp;&ensp;如我们之前所述，JPEG库会把压缩后的数据传递给`data destination`模块， JPEG库包含了一个`data destination`模块，这个模块负责如何把压缩后的数据写入标准输出流，你可以使用自定义的`data destination`模块，如果你想做一点其他事情的话，这部分我们放在后面介绍。如果你使用标准的`destination module`，你一定要首先打开目标输出流，典型的代码如下：
```C++
	FILE * outfile;
	...
	if ((outfile = fopen(filename, "wb")) == NULL) {
	    fprintf(stderr, "can't open %s\n", filename);
	    exit(1);
	}
	jpeg_stdio_dest(&cinfo, outfile);
```
最后一行调用了标准目标模块。

**WARNING**: 将二进制压缩数据保持不变地传递给输出文件是至关重要的。在非Unix系统上，stdio库可以执行添加换行或其他损坏二进制数据的行为。 为了防止这个行为，您可能需要使用“b”选项来使用fopen（如上所示），或使用setmode（）或另一个例程将stdio流置于二进制模式。你可以查看cjpeg.c和djpeg.c，它们提供了可在许多系统上运行的代码。

&ensp;&ensp;&ensp;&ensp;如果方便的话你还可以在设置了其他参数后指定`data destination`。但是一般不要在调用 `jpeg_start_compress()` and `jpeg_finish_compress()`期间去改变`data destination`.


### 压缩设置参数，包括图像大小和色彩空间

你必须提供如下关于源图像的信息，并在代码里设置JPEG对象(cinfo structure)对应的字段：

|       字段       |                     含义                     |
| :--------------: | :------------------------------------------: |
|   image_width    |          Width of image, in pixels           |
|   image_height   |          Height of image, in pixels          |
| input_components | Number of color channels (samples per pixel) |
|  in_color_space  |         Color space of source image          |

&ensp;&ensp;&ensp;&ensp;图像的维度，对于使用者来说是显而易见的。JPEG支持从1~64K的长度和宽度。输入的色彩空间典型情况一般是RGB或者灰度，分别对应3个通道和1个通道。字段`in_color_space` 必须要被设置为枚举类型`J_COLOR_SPACE`，典型值为`JCS_RGB`或`JCS_GRAYSCALE`。

&ensp;&ensp;&ensp;&ensp;JPEG拥有很多控制压缩的参数，决定了图像数据如何编码。 大部分程序没有必要或者根本不用去了解那么多参数。你可以通过调用`jpeg_set_defaults()` 把所有的参数设置为默认值；然后，如果有某个特殊的参数需要去改变，再去单独设置它。 在"Compression parameter 章节"讨论所有的参数。

&ensp;&ensp;&ensp;&ensp;调用`peg_set_defaults()`之前，必须手动设置正确的色彩空间`in_color_space`，因为默认参数依赖于源图像的色彩空间。注意，其他3个参数在你调用`jpeg_start_compress()`必须可用。多次调用`jpeg_set_defaults()` 是允许的，如果对你有帮助的话。

24-bit RGB源图像的典型代码如下：
```C
	cinfo.image_width = Width; 	/* image width and height, in pixels */
	cinfo.image_height = Height;
	cinfo.input_components = 3;	/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; /* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	/* Make optional parameter settings here */
```

### 调用jpeg_start_compress(...);

&ensp;&ensp;&ensp;&ensp;在你已经确定了data destination以后并且设置好了所有的必须参数（包括图像大小等等）后，调用`jpeg_start_compress()` 开始进入压缩循环。开始初始化内部工作状态，分配工作存储空间，触发生成JPEG数据流的头信息。

典型代码：
```C
	jpeg_start_compress(&cinfo, TRUE);
```
参数`TRUE`确保了一个完整的JPEG交换数据流会被写入。对于大多数情况是这个参数是适用的，如果你想使用缩略JPEG数据流，请参考下面的章节：abbreviated datastreams

一旦调用了`jpeg_start_compress()`，你就不能改变JPEG参数或者JPEG对象的其他字段了，直到压缩循环停止。


### while (scan lines remain to be written) jpeg_write_scanlines(...);

&ensp;&ensp;&ensp;&ensp;现在通过一次或者多册调用 `jpeg_write_scanlines()`把所需要的图像数据写入到stdio数据流中，你可以在每次调用这个函数的时候传入一行或者多行数据，最大为整个图像的高度。在大多数程序里，每次调用`jpeg_write_scanlines()`只传一行或者若干行是一个便捷做法。传入的数据格式，如上面的"Data formats"所要求的那样。

&ensp;&ensp;&ensp;&ensp;数据写入应当按照top-bottom行的顺序，JPEG规范包含一些关于顶部和底部是如何定义的复杂措辞术语（英语中的奇怪生僻的解释......）但如果你想要你的文件与其他人的程序兼容的话，你最好使用top-bottom的顺序。如果必须按从下到上的顺序读取源数据，则可以使用JPEG库的虚拟阵列机制有效地反转数据。你可以在示例应用程序cjpeg中找到范例代码。

&ensp;&ensp;&ensp;&ensp;JPEG库包含了一个计数器，指明当前操作的下一行行数，这个计数器是JPEG对象的`next_scanline`字段。通常情况下，你可以使用这个值作为循环计数器，所以循环语句可以这样写:
`while (cinfo.next_scanline < cinfo.image_height)`

这一步的代码严重依赖你怎么存储源图像数据，example.c演示了如下代码，用于处理2维图像，每个pixel包含3-byte RGB子像素

```C
	JSAMPROW row_pointer[1];	/* pointer to a single row */
	int row_stride;			/* physical row width in buffer */

	row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
	    row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
	    jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
```
&ensp;&ensp;&ensp;&ensp;`jpeg_write_scanlines()` 返回被写入的扫描行数量，正常情况下，这个返回值等于传入的扫描行数量，因此你可以不理会它。但是有2种情况下，这个返回值不等于传入的行数量：

  * 如果你试图写入比前面声明的图像高度还多的行，则你传入的多于部分行会被无视，不做写入处理；
  * 如果使用挂起数据目标管理器，则输出缓冲区溢出将导致压缩器在接受所有传递的线路之前返回。此功能在下面的“I/O挂起”中讨论。 正常stdio目标管理器不会导致这种情况发生。
除这两种情况，其他情况下返回值都等于`next_scanline`字段值。


### jpeg_finish_compress(...);

&ensp;&ensp;&ensp;&ensp;在所有的图像数据被写入到缓冲区后，调用`jpeg_finish_compress()`来完成压缩循环。这一步对于确保最后一个缓冲区数据写入数据目标至关重要。`jpeg_finish_compress()`还释放与JPEG对象相关的内存。

典型代码：
```c
	jpeg_finish_compress(&cinfo);
```
&ensp;&ensp;&ensp;&ensp;如果你使用stdio目标管理器，不要忘了在这一步完成以后关闭stdio输出流。

&ensp;&ensp;&ensp;&ensp;如果您已请求multi-pass操作模式，例如霍夫曼代码优化，`jpeg_finish_compress()`将执行额外的数据传递，传递第一次传入的缓冲的数据。 在这种情况下，`jpeg_finish_compress()`可能会占用很长一段时间才能完成。如果使用默认压缩参数，这将不会发生。

&ensp;&ensp;&ensp;&ensp;不要试图在尚未完全写入扫描行的情况下调用`jpeg_finish_compress()`，这样做将产生错误，如果你想中断压缩，调用`jpeg_abort()` ，下面我们将会介绍如何使用它。

&ensp;&ensp;&ensp;&ensp;在完成一次压缩以后，你可能想保留JPEG对象常驻在内存中，或者你想使用这个对象压缩另一个图像。在这种情况下，你将要正确返回回到第2，3，4步。如果你不改变目标管理器，则新的数据流会被写入到同样的目标文件中。如果你不改变压缩参数，则新的压缩数据流将使用之前的压缩参数。注意一下，你可以自由改变图像的几何尺寸在压缩循环间歇期。但是一旦你重新设置了色彩空间的话，则应该再次调用`jpeg_set_defaults()`来使压缩参数适应新的色彩空间。然后你再开始前面的第三步：调用`jpeg_start_compress(...);`


### 释放JPEG压缩对象

&ensp;&ensp;&ensp;&ensp;完成JPEG压缩后，通过调用jpeg_destroy_compress()将其销毁。 这将释放所有辅助内存（无论对象的先前状态如何）。 或者你可以调用jpeg_destroy（），它适用于压缩或解压缩对象---如果你在压缩和解压缩的情况下共享代码，这可能会更方便。 （实际上，除了传递已声明的指针类型之外，这些例程是等效的。为了避免来自ANSI C编译器的抱怨，应该给jpeg_destroy()传递j_common_ptr类型。）

如果你是通过`malloc()`分配的`jpeg_compress_struct structure` ，则释放这个对象是你的责任，`jpeg_destroy()`不会释放这个结构对象，错误处理对象也是这样。

典型代码
```c
	jpeg_destroy_compress(&cinfo);
```

### 中断

&ensp;&ensp;&ensp;&ensp;如果你决定中断一个压缩循环，你可以用如下2种方式做收尾清理工作：

* 如果您不再需要JPEG对象，只需调用`jpeg_destroy_compress()`或`jpeg_destroy()`释放内存即可。在调用`jpeg_create_compress()`之后的任何时候调用这两个函数都是合法的，事实上，即使`jpeg_create_compress()`失败也很安全。

* 如果要复用JPEG对象，请调用`jpeg_abort_compress()`或调用适用于压缩和解压缩对象的`jpeg_abort()`。这将使对象返回空闲状态，释放任何工作内存。成功创建JPEG对象后，可以随时使用`jpeg_abort()`。

&ensp;&ensp;&ensp;&ensp;请注意：清理数据目标输出，是你的责任；这些例程都不会调用`term destination()`。（有关详细信息，请参阅下面的“Compressed data handling”。）

&ensp;&ensp;&ensp;&ensp;`jpeg_destroy()`和`jpeg_abort()` 是仅有的2种安全方式处理去处理报告了错误的JPEG对象，JPEG对象调用error_exit退出。(请参考更多关于"Error handling"的信息)。JPEG对象的内部状态这个时候是不正常状态，`jpeg_destroy()`和`jpeg_abort()` 会把JPEG对象恢复到一种已知状态。


## 解压缩细节

&ensp;&ensp;&ensp;&ensp;再回顾一下概述中提到的JPEG解压操作。

### 1 分配并初始化一个JPEG解压缩对象

&ensp;&ensp;&ensp;&ensp;这一步类似与前面的创建并初始化JPEG压缩对象，除了类型变成了`struct jpeg_decompress_struct`，你要使用`jpeg_create_decompress()`来做初始化，错误处理对象和之前完全一样。 

典型代码：
```c
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	...
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
```
(这里和IJG的代码中，我们总是使用`cinfo`这个变量名来表示JPEG压缩和解压缩对象。)


### 2 指定一个待解压的源对象(比如，一个文件)

&ensp;&ensp;&ensp;&ensp;JPEG库从"data
source" module读取已被压缩的数据，JPEG库包含了一个data source module，这个module已知如何从stdio stream读取数据。 如果你想做一点其他事情，可以自定义一个source module，这个在后面的章节讨论。

&ensp;&ensp;&ensp;&ensp;如果你使用标准的source module，你必须首先打开目标源标准输入流stdio stream，这一步的典型代码如下：
```c
	FILE * infile;
	...
	if ((infile = fopen(filename, "rb")) == NULL) {
	    fprintf(stderr, "can't open %s\n", filename);
	    exit(1);
	}
	jpeg_stdio_src(&cinfo, infile);
```
这里的最后一行代码调用了标准源输入module。

**WARNING**: 保持被压缩的二进制数据不被篡改是至关重要的，在非unix系统中，stdio可能会执行添加换行符之类的破坏操作，为此，在打开文件的时候，确保开启"b二进制模式。或者通过setmode()、另一个路径把stdio设置成二进制模式。cjpeg.c和djpeg.c提供了一些可以在多个系统上正常工作的代码。

&ensp;&ensp;&ensp;&ensp;在调用`jpeg_read_header()`和`jpeg_finish_decompress()`期间不要去修改源数据，如果你想从一个单一的源文件读取一系列的JPEG图像，你应该重复调用`jpeg_read_header()`到`jpeg_finish_decompress()` 之间的序列，但是不需要重复初始化JPEG对象和data source module；这样做可以防止读取已经丢弃的图像文件。


### 3 调用`jpeg_read_header()`获取文件头信息

典型代码：
```c
	jpeg_read_header(&cinfo, TRUE);
```
&ensp;&ensp;&ensp;&ensp;这个步骤会读取JPEG数据流的头标记信息，直到被压缩的data开始位置。图像的尺寸信息和其他信息会被读取并存储到JPEG对象中去。程序应该去查询这些信息来选择合适的解压参数。

如果碰到如下2种情况，则需要更复杂的代码去处理：
  * JPEG文件使用了suspending data源 --- 这种情况下，`jpeg_read_header()`将会提前返回，将不会读取完整的头信息。详情请参阅下面的高级主题"I/O suspension"， 正常情况下的stdio source manager不会碰到这种情况。
  * 需要处理缩略JPEG文件---详情请参考**abbreviated datastreams**。标准的程序只处理直接交换数据的JPEG，所以我们也不必很关注这种情况。

&ensp;&ensp;&ensp;&ensp;如果你仅仅只想读取JPEG文件的一些头信息，那么你可以在这里打住，这种情况下，通过调用`jpeg_destroy()`去销毁JPEG对象，或者在选择一个新的源数据并读取它的头信息之前调用`jpeg_abort()`将JPEG对象重置为空闲态。

### 4 设置解压参数

&ensp;&ensp;&ensp;&ensp;`jpeg_read_header()`基于图像的属性（主要是色彩空间）来设置合适的默认解压参数，然而，你可能会想在解压之前修改一些参数值，比如，默认的参数是从一个彩色图像解压出全彩输出。如果你想让输出做色彩映射的话，你就必须在参数中做设定。其他的参数比如让输出做缩放，选择压缩速度/质量平衡，等等请参考高级特性中的"Decompression parameter selection"章节。

如果默认参数是合适的，那这一步你什么都不用做。

&ensp;&ensp;&ensp;&ensp;请注意默认参数是通过调用`jpeg_read_header()`而生成的，如果你复用一个解压JPEG对象，千万不要期望在你的整个解压过程中解压参数都是合适的，正如在压缩过程中我们已经提过的那样，你必须在每次解压的时候设定好匹配的参数。

### 5 jpeg_start_decompress(...);

一旦解压参数设置完成，就可以调用`jpeg_start_decompress()`来开始解压。这个函数初始化内部状态，分配内存，准备返回数据。

典型代码：
```c
	jpeg_start_decompress(&cinfo);
```
如果你申请了multi-pass operating mode，比如2-pass色彩量化，`jpeg_start_decompress()`将做完所有的准备工作直到可以开始输出数据。这种情况下`jpeg_start_decompress()`将需要一小会时间来完成操作。但是对于single-scan (non progressive)的JPEG文件，并且使用默认解压参数的情况来说，这种情况不会发生，`jpeg_start_decompress()`将会很快返回。

在调用这个函数之后，最终的输出图像维度，包括任何要求的缩放，都可以再JPEG对象中使用了。色彩空间也是如此，如果要求输出结果经过色彩映射，则如下的字段是有用的：

|          字段           |                  含义                  |
| :---------------------: | :------------------------------------: |
|      output_width       |         image width, as scaled         |
|      output_height      |        image height, as scaled         |
|  out_color_components   | of color components in out_color_space |
|    output_components    | of color components returned per pixel |
|        colormap         |     the selected colormap, if any      |
| actual_number_of_colors |     number of entries in colormap      |

 当输出是单一量化色彩时output_components = 1，就是说色彩空间只用一个索引就足够；否则的话就等于`out_color_components`，这个值代表了JSAMPLE在输出序列里包含了几个子像素。

通常情况下，你需要分配内存来保存传入的图像数据，你将需要分配`output_width*output_components JSAMPLE`的行宽度，高度为`output_height`。

&ensp;&ensp;&ensp;&ensp;注意：如果你使用JPEG内部的内存管理器（如djpeg所做的那样），这个内存管理器的规定要求你在调用`jpeg_start_decompress()`之前申请大内存缓冲区，这个地方有点棘手，在于当时output_xxx字段还不可用。你可以在设定好相关的缩放，色彩空间参数以后通过调用`jpeg_calc_output_dimensions()`来是的output_xxx字段生效。从而来计算内存缓冲区的大小。

### 6 while (scan lines remain to be read) jpeg_read_scanlines(...);

&ensp;&ensp;&ensp;&ensp;现在你可以通过调用`jpeg_read_scanlines()`一次或者多次来读取解压后的图像数据。在每次调用的时候，你都要传递一个最大扫描行数值（比如，工作缓存的高度），`jpeg_read_scanlines()`将会返回读取的行数，最大到你传递的行数，返回的数值代表真实读取的行数。返回的数据格式如上面的"Data
formats"章节所讨论的那样，不要忘了灰度图和彩色图像返回的数据格式是不同的。

&ensp;&ensp;&ensp;&ensp;返回的图像数据是按照top-to-bottom扫描顺序排列的。如果你一定需要反向bottom-to-top排列，你可以使用JPEG库的虚拟矩阵机制高效地反转。示例代码可以在示范程序djpeg当中找到。

&ensp;&ensp;&ensp;&ensp;JPEG库包含一个计数器，表明当前返回了多少个扫描行数据，这个计数器就是JPEG对象的`output_scanline`属性。通常我们都使用这个值作为循环计数器，所以循环大致都是这个样子的：`while (cinfo.output_scanline < cinfo.output_height)`。（注意测试条件不应当是等于image_height，除非你永不使用缩放，image_height指的是未缩放的源图像高度）`jpeg_read_scanlines(...)`的返回值等于`output_scanline`的变动值。

&ensp;&ensp;&ensp;&ensp;如果你不使用suspending data source，那么调用`jpeg_read_scanlines()`会安全的保证每次调用都会读取至少一行数据，知道达到图像的底部。

&ensp;&ensp;&ensp;&ensp;如果你使用的缓冲区大小超过一行，那么无法保证`jpeg_read_scanlines()`会正确的填满缓冲区。(目前的实现仅仅在调用的时候返回一些扫描行，而不管你传递了多大的缓冲区)，所以你必须提供一个循环去调用`jpeg_read_scanlines()`知道整个图像被读取完。 

### 7 jpeg_finish_decompress(...);

&ensp;&ensp;&ensp;&ensp;读取完所有的图像数据以后，调用`jpeg_finish_decompress()`来完成解压，这一步会引发释放和JPEG对象相关联的工作内存。

典型代码:
```c
	jpeg_finish_decompress(&cinfo);
```
&ensp;&ensp;&ensp;&ensp;如果使用stdio source manager，不要忘了关闭stdio stream。

&ensp;&ensp;&ensp;&ensp;在全部读取完扫描行数据之前调用`jpeg_finish_decompress()`是错误行为，如果你想提前中断解压，通过调用`jpeg_abort()`来实现。 

&ensp;&ensp;&ensp;&ensp;在完成解压循环以后，你可能想销毁JPEG对象，也可能想留着用于下一次解压；如果想复用这个JPEG对象，这个时候需要回到步骤2或者3，如果你不改变source manager，则下一次读取的图像和这一次的图像是一样的。

### 8 释放JPEG解压对象

解压完成以后需要销毁JPEG解压对象，通过调用`jpeg_destroy_decompress()`或者`jpeg_destroy()`实现这一目的。前面关于销毁压缩对象的描述这里也是适用的。 

典型代码:
```c
	jpeg_destroy_decompress(&cinfo);
```

### 9 中断

通过调用`jpeg_destroy_decompress()`或者`jpeg_destroy()`中断解压过程，这两个函数将销毁JPEG对象`jpeg_abort_decompress()`或者`jpeg_abort()`则不销毁JPEG对象，和之前的关于压缩中断所说的一样。

# 高级特性

## Compression parameter selection

This section describes all the optional parameters you can set for JPEG
compression, as well as the "helper" routines provided to assist in this
task.  Proper setting of some parameters requires detailed understanding
of the JPEG standard; if you don't know what a parameter is for, it's best
not to mess with it!  See REFERENCES in the README file for pointers to
more info about JPEG.

It's a good idea to call jpeg_set_defaults() first, even if you plan to set
all the parameters; that way your code is more likely to work with future JPEG
libraries that have additional parameters.  For the same reason, we recommend
you use a helper routine where one is provided, in preference to twiddling
cinfo fields directly.

The helper routines are:

jpeg_set_defaults (j_compress_ptr cinfo)
	This routine sets all JPEG parameters to reasonable defaults, using
	only the input image's color space (field in_color_space, which must
	already be set in cinfo).  Many applications will only need to use
	this routine and perhaps jpeg_set_quality().

jpeg_set_colorspace (j_compress_ptr cinfo, J_COLOR_SPACE colorspace)
	Sets the JPEG file's colorspace (field jpeg_color_space) as specified,
	and sets other color-space-dependent parameters appropriately.  See
	"Special color spaces", below, before using this.  A large number of
	parameters, including all per-component parameters, are set by this
	routine; if you want to twiddle individual parameters you should call
	jpeg_set_colorspace() before rather than after.

jpeg_default_colorspace (j_compress_ptr cinfo)
	Selects an appropriate JPEG colorspace based on cinfo->in_color_space,
	and calls jpeg_set_colorspace().  This is actually a subroutine of
	jpeg_set_defaults().  It's broken out in case you want to change
	just the colorspace-dependent JPEG parameters.

jpeg_set_quality (j_compress_ptr cinfo, int quality, boolean force_baseline)
	Constructs JPEG quantization tables appropriate for the indicated
	quality setting.  The quality value is expressed on the 0..100 scale
	recommended by IJG (cjpeg's "-quality" switch uses this routine).
	Note that the exact mapping from quality values to tables may change
	in future IJG releases as more is learned about DCT quantization.
	If the force_baseline parameter is TRUE, then the quantization table
	entries are constrained to the range 1..255 for full JPEG baseline
	compatibility.  In the current implementation, this only makes a
	difference for quality settings below 25, and it effectively prevents
	very small/low quality files from being generated.  The IJG decoder
	is capable of reading the non-baseline files generated at low quality
	settings when force_baseline is FALSE, but other decoders may not be.

jpeg_set_linear_quality (j_compress_ptr cinfo, int scale_factor,
			 boolean force_baseline)
	Same as jpeg_set_quality() except that the generated tables are the
	sample tables given in the JPEC spec section K.1, multiplied by the
	specified scale factor (which is expressed as a percentage; thus
	scale_factor = 100 reproduces the spec's tables).  Note that larger
	scale factors give lower quality.  This entry point is useful for
	conforming to the Adobe PostScript DCT conventions, but we do not
	recommend linear scaling as a user-visible quality scale otherwise.
	force_baseline again constrains the computed table entries to 1..255.

int jpeg_quality_scaling (int quality)
	Converts a value on the IJG-recommended quality scale to a linear
	scaling percentage.  Note that this routine may change or go away
	in future releases --- IJG may choose to adopt a scaling method that
	can't be expressed as a simple scalar multiplier, in which case the
	premise of this routine collapses.  Caveat user.

jpeg_default_qtables (j_compress_ptr cinfo, boolean force_baseline)
	Set default quantization tables with linear q_scale_factor[] values
	(see below).

jpeg_add_quant_table (j_compress_ptr cinfo, int which_tbl,
		      const unsigned int *basic_table,
		      int scale_factor, boolean force_baseline)
	Allows an arbitrary quantization table to be created.  which_tbl
	indicates which table slot to fill.  basic_table points to an array
	of 64 unsigned ints given in normal array order.  These values are
	multiplied by scale_factor/100 and then clamped to the range 1..65535
	(or to 1..255 if force_baseline is TRUE).
	CAUTION: prior to library version 6a, jpeg_add_quant_table expected
	the basic table to be given in JPEG zigzag order.  If you need to
	write code that works with either older or newer versions of this
	routine, you must check the library version number.  Something like
	"#if JPEG_LIB_VERSION >= 61" is the right test.

jpeg_simple_progression (j_compress_ptr cinfo)
	Generates a default scan script for writing a progressive-JPEG file.
	This is the recommended method of creating a progressive file,
	unless you want to make a custom scan sequence.  You must ensure that
	the JPEG color space is set correctly before calling this routine.


Compression parameters (cinfo fields) include:

boolean arith_code
	If TRUE, use arithmetic coding.
	If FALSE, use Huffman coding.

int block_size
	Set DCT block size.  All N from 1 to 16 are possible.
	Default is 8 (baseline format).
	Larger values produce higher compression,
	smaller values produce higher quality.
	An exact DCT stage is possible with 1 or 2.
	With the default quality of 75 and default Luminance qtable
	the DCT+Quantization stage is lossless for value 1.
	Note that values other than 8 require a SmartScale capable decoder,
	introduced with IJG JPEG 8.  Setting the block_size parameter for
	compression works with version 8c and later.

J_DCT_METHOD dct_method
	Selects the algorithm used for the DCT step.  Choices are:
		JDCT_ISLOW: slow but accurate integer algorithm
		JDCT_IFAST: faster, less accurate integer method
		JDCT_FLOAT: floating-point method
		JDCT_DEFAULT: default method (normally JDCT_ISLOW)
		JDCT_FASTEST: fastest method (normally JDCT_IFAST)
	The FLOAT method is very slightly more accurate than the ISLOW method,
	but may give different results on different machines due to varying
	roundoff behavior.  The integer methods should give the same results
	on all machines.  On machines with sufficiently fast FP hardware, the
	floating-point method may also be the fastest.  The IFAST method is
	considerably less accurate than the other two; its use is not
	recommended if high quality is a concern.  JDCT_DEFAULT and
	JDCT_FASTEST are macros configurable by each installation.

unsigned int scale_num, scale_denom
	Scale the image by the fraction scale_num/scale_denom.  Default is
	1/1, or no scaling.  Currently, the supported scaling ratios are
	M/N with all N from 1 to 16, where M is the destination DCT size,
	which is 8 by default (see block_size parameter above).
	(The library design allows for arbitrary scaling ratios but this
	is not likely to be implemented any time soon.)

J_COLOR_SPACE jpeg_color_space
int num_components
	The JPEG color space and corresponding number of components; see
	"Special color spaces", below, for more info.  We recommend using
	jpeg_set_colorspace() if you want to change these.

J_COLOR_TRANSFORM color_transform
	Internal color transform identifier, writes LSE marker if nonzero
	(requires decoder with inverse color transform support, introduced
	with IJG JPEG 9).
	Two values are currently possible: JCT_NONE and JCT_SUBTRACT_GREEN.
	Set this value for lossless RGB application *before* calling
	jpeg_set_colorspace(), because entropy table assignment in
	jpeg_set_colorspace() depends on color_transform.

boolean optimize_coding
	TRUE causes the compressor to compute optimal Huffman coding tables
	for the image.  This requires an extra pass over the data and
	therefore costs a good deal of space and time.  The default is
	FALSE, which tells the compressor to use the supplied or default
	Huffman tables.  In most cases optimal tables save only a few percent
	of file size compared to the default tables.  Note that when this is
	TRUE, you need not supply Huffman tables at all, and any you do
	supply will be overwritten.

unsigned int restart_interval
int restart_in_rows
	To emit restart markers in the JPEG file, set one of these nonzero.
	Set restart_interval to specify the exact interval in MCU blocks.
	Set restart_in_rows to specify the interval in MCU rows.  (If
	restart_in_rows is not 0, then restart_interval is set after the
	image width in MCUs is computed.)  Defaults are zero (no restarts).
	One restart marker per MCU row is often a good choice.
	NOTE: the overhead of restart markers is higher in grayscale JPEG
	files than in color files, and MUCH higher in progressive JPEGs.
	If you use restarts, you may want to use larger intervals in those
	cases.

const jpeg_scan_info * scan_info
int num_scans
	By default, scan_info is NULL; this causes the compressor to write a
	single-scan sequential JPEG file.  If not NULL, scan_info points to
	an array of scan definition records of length num_scans.  The
	compressor will then write a JPEG file having one scan for each scan
	definition record.  This is used to generate noninterleaved or
	progressive JPEG files.  The library checks that the scan array
	defines a valid JPEG scan sequence.  (jpeg_simple_progression creates
	a suitable scan definition array for progressive JPEG.)  This is
	discussed further under "Progressive JPEG support".

boolean do_fancy_downsampling
	If TRUE, use direct DCT scaling with DCT size > 8 for downsampling
	of chroma components.
	If FALSE, use only DCT size <= 8 and simple separate downsampling.
	Default is TRUE.
	For better image stability in multiple generation compression cycles
	it is preferable that this value matches the corresponding
	do_fancy_upsampling value in decompression.

int smoothing_factor
	If non-zero, the input image is smoothed; the value should be 1 for
	minimal smoothing to 100 for maximum smoothing.  Consult jcsample.c
	for details of the smoothing algorithm.  The default is zero.

boolean write_JFIF_header
	If TRUE, a JFIF APP0 marker is emitted.  jpeg_set_defaults() and
	jpeg_set_colorspace() set this TRUE if a JFIF-legal JPEG color space
	(ie, YCbCr or grayscale) is selected, otherwise FALSE.

UINT8 JFIF_major_version
UINT8 JFIF_minor_version
	The version number to be written into the JFIF marker.
	jpeg_set_defaults() initializes the version to 1.01 (major=minor=1).
	You should set it to 1.02 (major=1, minor=2) if you plan to write
	any JFIF 1.02 extension markers.

UINT8 density_unit
UINT16 X_density
UINT16 Y_density
	The resolution information to be written into the JFIF marker;
	not used otherwise.  density_unit may be 0 for unknown,
	1 for dots/inch, or 2 for dots/cm.  The default values are 0,1,1
	indicating square pixels of unknown size.

boolean write_Adobe_marker
	If TRUE, an Adobe APP14 marker is emitted.  jpeg_set_defaults() and
	jpeg_set_colorspace() set this TRUE if JPEG color space RGB, CMYK,
	or YCCK is selected, otherwise FALSE.  It is generally a bad idea
	to set both write_JFIF_header and write_Adobe_marker.  In fact,
	you probably shouldn't change the default settings at all --- the
	default behavior ensures that the JPEG file's color space can be
	recognized by the decoder.

JQUANT_TBL * quant_tbl_ptrs[NUM_QUANT_TBLS]
	Pointers to coefficient quantization tables, one per table slot,
	or NULL if no table is defined for a slot.  Usually these should
	be set via one of the above helper routines; jpeg_add_quant_table()
	is general enough to define any quantization table.  The other
	routines will set up table slot 0 for luminance quality and table
	slot 1 for chrominance.

int q_scale_factor[NUM_QUANT_TBLS]
	Linear quantization scaling factors (percentage, initialized 100)
	for use with jpeg_default_qtables().
	See rdswitch.c and cjpeg.c for an example of usage.
	Note that the q_scale_factor[] fields are the "linear" scales, so you
	have to convert from user-defined ratings via jpeg_quality_scaling().
	Here is an example code which corresponds to cjpeg -quality 90,70:

		jpeg_set_defaults(cinfo);

		/* Set luminance quality 90. */
		cinfo->q_scale_factor[0] = jpeg_quality_scaling(90);
		/* Set chrominance quality 70. */
		cinfo->q_scale_factor[1] = jpeg_quality_scaling(70);

		jpeg_default_qtables(cinfo, force_baseline);

	CAUTION: You must also set 1x1 subsampling for efficient separate
	color quality selection, since the default value used by library
	is 2x2:

		cinfo->comp_info[0].v_samp_factor = 1;
		cinfo->comp_info[0].h_samp_factor = 1;

JHUFF_TBL * dc_huff_tbl_ptrs[NUM_HUFF_TBLS]
JHUFF_TBL * ac_huff_tbl_ptrs[NUM_HUFF_TBLS]
	Pointers to Huffman coding tables, one per table slot, or NULL if
	no table is defined for a slot.  Slots 0 and 1 are filled with the
	JPEG sample tables by jpeg_set_defaults().  If you need to allocate
	more table structures, jpeg_alloc_huff_table() may be used.
	Note that optimal Huffman tables can be computed for an image
	by setting optimize_coding, as discussed above; there's seldom
	any need to mess with providing your own Huffman tables.


The actual dimensions of the JPEG image that will be written to the file are
given by the following fields.  These are computed from the input image
dimensions and the compression parameters by jpeg_start_compress().  You can
also call jpeg_calc_jpeg_dimensions() to obtain the values that will result
from the current parameter settings.  This can be useful if you are trying
to pick a scaling ratio that will get close to a desired target size.

JDIMENSION jpeg_width		Actual dimensions of output image.
JDIMENSION jpeg_height


Per-component parameters are stored in the struct cinfo.comp_info[i] for
component number i.  Note that components here refer to components of the
JPEG color space, *not* the source image color space.  A suitably large
comp_info[] array is allocated by jpeg_set_defaults(); if you choose not
to use that routine, it's up to you to allocate the array.

int component_id
	The one-byte identifier code to be recorded in the JPEG file for
	this component.  For the standard color spaces, we recommend you
	leave the default values alone.

int h_samp_factor
int v_samp_factor
	Horizontal and vertical sampling factors for the component; must
	be 1..4 according to the JPEG standard.  Note that larger sampling
	factors indicate a higher-resolution component; many people find
	this behavior quite unintuitive.  The default values are 2,2 for
	luminance components and 1,1 for chrominance components, except
	for grayscale where 1,1 is used.

int quant_tbl_no
	Quantization table number for component.  The default value is
	0 for luminance components and 1 for chrominance components.

int dc_tbl_no
int ac_tbl_no
	DC and AC entropy coding table numbers.  The default values are
	0 for luminance components and 1 for chrominance components.

int component_index
	Must equal the component's index in comp_info[].  (Beginning in
	release v6, the compressor library will fill this in automatically;
	you don't have to.)


## Decompression parameter selection

Decompression parameter selection is somewhat simpler than compression
parameter selection, since all of the JPEG internal parameters are
recorded in the source file and need not be supplied by the application.
(Unless you are working with abbreviated files, in which case see
"Abbreviated datastreams", below.)  Decompression parameters control
the postprocessing done on the image to deliver it in a format suitable
for the application's use.  Many of the parameters control speed/quality
tradeoffs, in which faster decompression may be obtained at the price of
a poorer-quality image.  The defaults select the highest quality (slowest)
processing.

The following fields in the JPEG object are set by jpeg_read_header() and
may be useful to the application in choosing decompression parameters:

JDIMENSION image_width			Width and height of image
JDIMENSION image_height
int num_components			Number of color components
J_COLOR_SPACE jpeg_color_space		Colorspace of image
boolean saw_JFIF_marker			TRUE if a JFIF APP0 marker was seen
  UINT8 JFIF_major_version		Version information from JFIF marker
  UINT8 JFIF_minor_version
  UINT8 density_unit			Resolution data from JFIF marker
  UINT16 X_density
  UINT16 Y_density
boolean saw_Adobe_marker		TRUE if an Adobe APP14 marker was seen
  UINT8 Adobe_transform			Color transform code from Adobe marker

The JPEG color space, unfortunately, is something of a guess since the JPEG
standard proper does not provide a way to record it.  In practice most files
adhere to the JFIF or Adobe conventions, and the decoder will recognize these
correctly.  See "Special color spaces", below, for more info.


The decompression parameters that determine the basic properties of the
returned image are:

J_COLOR_SPACE out_color_space
	Output color space.  jpeg_read_header() sets an appropriate default
	based on jpeg_color_space; typically it will be RGB or grayscale.
	The application can change this field to request output in a different
	colorspace.  For example, set it to JCS_GRAYSCALE to get grayscale
	output from a color file.  (This is useful for previewing: grayscale
	output is faster than full color since the color components need not
	be processed.)  Note that not all possible color space transforms are
	currently implemented; you may need to extend jdcolor.c if you want an
	unusual conversion.

unsigned int scale_num, scale_denom
	Scale the image by the fraction scale_num/scale_denom.  Currently,
	the supported scaling ratios are M/N with all M from 1 to 16, where
	N is the source DCT size, which is 8 for baseline JPEG.  (The library
	design allows for arbitrary scaling ratios but this is not likely
	to be implemented any time soon.)  The values are initialized by
	jpeg_read_header() with the source DCT size.  For baseline JPEG
	this is 8/8.  If you change only the scale_num value while leaving
	the other unchanged, then this specifies the DCT scaled size to be
	applied on the given input.  For baseline JPEG this is equivalent
	to M/8 scaling, since the source DCT size for baseline JPEG is 8.
	Smaller scaling ratios permit significantly faster decoding since
	fewer pixels need be processed and a simpler IDCT method can be used.

boolean quantize_colors
	If set TRUE, colormapped output will be delivered.  Default is FALSE,
	meaning that full-color output will be delivered.

The next three parameters are relevant only if quantize_colors is TRUE.

int desired_number_of_colors
	Maximum number of colors to use in generating a library-supplied color
	map (the actual number of colors is returned in a different field).
	Default 256.  Ignored when the application supplies its own color map.

boolean two_pass_quantize
	If TRUE, an extra pass over the image is made to select a custom color
	map for the image.  This usually looks a lot better than the one-size-
	fits-all colormap that is used otherwise.  Default is TRUE.  Ignored
	when the application supplies its own color map.

J_DITHER_MODE dither_mode
	Selects color dithering method.  Supported values are:
		JDITHER_NONE	no dithering: fast, very low quality
		JDITHER_ORDERED	ordered dither: moderate speed and quality
		JDITHER_FS	Floyd-Steinberg dither: slow, high quality
	Default is JDITHER_FS.  (At present, ordered dither is implemented
	only in the single-pass, standard-colormap case.  If you ask for
	ordered dither when two_pass_quantize is TRUE or when you supply
	an external color map, you'll get F-S dithering.)

When quantize_colors is TRUE, the target color map is described by the next
two fields.  colormap is set to NULL by jpeg_read_header().  The application
can supply a color map by setting colormap non-NULL and setting
actual_number_of_colors to the map size.  Otherwise, jpeg_start_decompress()
selects a suitable color map and sets these two fields itself.
[Implementation restriction: at present, an externally supplied colormap is
only accepted for 3-component output color spaces.]

JSAMPARRAY colormap
	The color map, represented as a 2-D pixel array of out_color_components
	rows and actual_number_of_colors columns.  Ignored if not quantizing.
	CAUTION: if the JPEG library creates its own colormap, the storage
	pointed to by this field is released by jpeg_finish_decompress().
	Copy the colormap somewhere else first, if you want to save it.

int actual_number_of_colors
	The number of colors in the color map.

Additional decompression parameters that the application may set include:

J_DCT_METHOD dct_method
	Selects the algorithm used for the DCT step.  Choices are the same
	as described above for compression.

boolean do_fancy_upsampling
	If TRUE, use direct DCT scaling with DCT size > 8 for upsampling
	of chroma components.
	If FALSE, use only DCT size <= 8 and simple separate upsampling.
	Default is TRUE.
	For better image stability in multiple generation compression cycles
	it is preferable that this value matches the corresponding
	do_fancy_downsampling value in compression.

boolean do_block_smoothing
	If TRUE, interblock smoothing is applied in early stages of decoding
	progressive JPEG files; if FALSE, not.  Default is TRUE.  Early
	progression stages look "fuzzy" with smoothing, "blocky" without.
	In any case, block smoothing ceases to be applied after the first few
	AC coefficients are known to full accuracy, so it is relevant only
	when using buffered-image mode for progressive images.

boolean enable_1pass_quant
boolean enable_external_quant
boolean enable_2pass_quant
	These are significant only in buffered-image mode, which is
	described in its own section below.


The output image dimensions are given by the following fields.  These are
computed from the source image dimensions and the decompression parameters
by jpeg_start_decompress().  You can also call jpeg_calc_output_dimensions()
to obtain the values that will result from the current parameter settings.
This can be useful if you are trying to pick a scaling ratio that will get
close to a desired target size.  It's also important if you are using the
JPEG library's memory manager to allocate output buffer space, because you
are supposed to request such buffers *before* jpeg_start_decompress().

JDIMENSION output_width		Actual dimensions of output image.
JDIMENSION output_height
int out_color_components	Number of color components in out_color_space.
int output_components		Number of color components returned.
int rec_outbuf_height		Recommended height of scanline buffer.

When quantizing colors, output_components is 1, indicating a single color map
index per pixel.  Otherwise it equals out_color_components.  The output arrays
are required to be output_width * output_components JSAMPLEs wide.

rec_outbuf_height is the recommended minimum height (in scanlines) of the
buffer passed to jpeg_read_scanlines().  If the buffer is smaller, the
library will still work, but time will be wasted due to unnecessary data
copying.  In high-quality modes, rec_outbuf_height is always 1, but some
faster, lower-quality modes set it to larger values (typically 2 to 4).
If you are going to ask for a high-speed processing mode, you may as well
go to the trouble of honoring rec_outbuf_height so as to avoid data copying.
(An output buffer larger than rec_outbuf_height lines is OK, but won't
provide any material speed improvement over that height.)


## Special color spaces

The JPEG standard itself is "color blind" and doesn't specify any particular
color space.  It is customary to convert color data to a luminance/chrominance
color space before compressing, since this permits greater compression.  The
existing JPEG file interchange format standards specify YCbCr or GRAYSCALE
data (JFIF version 1), GRAYSCALE, RGB, YCbCr, CMYK, or YCCK (Adobe), or BG_RGB
or BG_YCC (big gamut color spaces, JFIF version 2).  For special applications
such as multispectral images, other color spaces can be used,
but it must be understood that such files will be unportable.

The JPEG library can handle the most common colorspace conversions (namely
RGB <=> YCbCr and CMYK <=> YCCK).  It can also deal with data of an unknown
color space, passing it through without conversion.  If you deal extensively
with an unusual color space, you can easily extend the library to understand
additional color spaces and perform appropriate conversions.

For compression, the source data's color space is specified by field
in_color_space.  This is transformed to the JPEG file's color space given
by jpeg_color_space.  jpeg_set_defaults() chooses a reasonable JPEG color
space depending on in_color_space, but you can override this by calling
jpeg_set_colorspace().  Of course you must select a supported transformation.
jccolor.c currently supports the following transformations:
	RGB => YCbCr
	RGB => GRAYSCALE
	RGB => BG_YCC
	YCbCr => GRAYSCALE
	YCbCr => BG_YCC
	CMYK => YCCK
plus the null transforms: GRAYSCALE => GRAYSCALE, RGB => RGB,
BG_RGB => BG_RGB, YCbCr => YCbCr, BG_YCC => BG_YCC, CMYK => CMYK,
YCCK => YCCK, and UNKNOWN => UNKNOWN.

The file interchange format standards (JFIF and Adobe) specify APPn markers
that indicate the color space of the JPEG file.  It is important to ensure
that these are written correctly, or omitted if the JPEG file's color space
is not one of the ones supported by the interchange standards.
jpeg_set_colorspace() will set the compression parameters to include or omit
the APPn markers properly, so long as it is told the truth about the JPEG
color space.  For example, if you are writing some random 3-component color
space without conversion, don't try to fake out the library by setting
in_color_space and jpeg_color_space to JCS_YCbCr; use JCS_UNKNOWN.
You may want to write an APPn marker of your own devising to identify
the colorspace --- see "Special markers", below.

When told that the color space is UNKNOWN, the library will default to using
luminance-quality compression parameters for all color components.  You may
well want to change these parameters.  See the source code for
jpeg_set_colorspace(), in jcparam.c, for details.

For decompression, the JPEG file's color space is given in jpeg_color_space,
and this is transformed to the output color space out_color_space.
jpeg_read_header's setting of jpeg_color_space can be relied on if the file
conforms to JFIF or Adobe conventions, but otherwise it is no better than a
guess.  If you know the JPEG file's color space for certain, you can override
jpeg_read_header's guess by setting jpeg_color_space.  jpeg_read_header also
selects a default output color space based on (its guess of) jpeg_color_space;
set out_color_space to override this.  Again, you must select a supported
transformation.  jdcolor.c currently supports
	YCbCr => RGB
	YCbCr => GRAYSCALE
	BG_YCC => RGB
	BG_YCC => GRAYSCALE
	RGB => GRAYSCALE
	GRAYSCALE => RGB
	YCCK => CMYK
as well as the null transforms.  (Since GRAYSCALE=>RGB is provided, an
application can force grayscale JPEGs to look like color JPEGs if it only
wants to handle one case.)

The two-pass color quantizer, jquant2.c, is specialized to handle RGB data
(it weights distances appropriately for RGB colors).  You'll need to modify
the code if you want to use it for non-RGB output color spaces.  Note that
jquant2.c is used to map to an application-supplied colormap as well as for
the normal two-pass colormap selection process.

CAUTION: it appears that Adobe Photoshop writes inverted data in CMYK JPEG
files: 0 represents 100% ink coverage, rather than 0% ink as you'd expect.
This is arguably a bug in Photoshop, but if you need to work with Photoshop
CMYK files, you will have to deal with it in your application.  We cannot
"fix" this in the library by inverting the data during the CMYK<=>YCCK
transform, because that would break other applications, notably Ghostscript.
Photoshop versions prior to 3.0 write EPS files containing JPEG-encoded CMYK
data in the same inverted-YCCK representation used in bare JPEG files, but
the surrounding PostScript code performs an inversion using the PS image
operator.  I am told that Photoshop 3.0 will write uninverted YCCK in
EPS/JPEG files, and will omit the PS-level inversion.  (But the data
polarity used in bare JPEG files will not change in 3.0.)  In either case,
the JPEG library must not invert the data itself, or else Ghostscript would
read these EPS files incorrectly.


## Error handling

When the default error handler is used, any error detected inside the JPEG
routines will cause a message to be printed on stderr, followed by exit().
You can supply your own error handling routines to override this behavior
and to control the treatment of nonfatal warnings and trace/debug messages.
The file example.c illustrates the most common case, which is to have the
application regain control after an error rather than exiting.

The JPEG library never writes any message directly; it always goes through
the error handling routines.  Three classes of messages are recognized:
  * Fatal errors: the library cannot continue.
  * Warnings: the library can continue, but the data is corrupt, and a
    damaged output image is likely to result.
  * Trace/informational messages.  These come with a trace level indicating
    the importance of the message; you can control the verbosity of the
    program by adjusting the maximum trace level that will be displayed.

You may, if you wish, simply replace the entire JPEG error handling module
(jerror.c) with your own code.  However, you can avoid code duplication by
only replacing some of the routines depending on the behavior you need.
This is accomplished by calling jpeg_std_error() as usual, but then overriding
some of the method pointers in the jpeg_error_mgr struct, as illustrated by
example.c.

All of the error handling routines will receive a pointer to the JPEG object
(a j_common_ptr which points to either a jpeg_compress_struct or a
jpeg_decompress_struct; if you need to tell which, test the is_decompressor
field).  This struct includes a pointer to the error manager struct in its
"err" field.  Frequently, custom error handler routines will need to access
additional data which is not known to the JPEG library or the standard error
handler.  The most convenient way to do this is to embed either the JPEG
object or the jpeg_error_mgr struct in a larger structure that contains
additional fields; then casting the passed pointer provides access to the
additional fields.  Again, see example.c for one way to do it.  (Beginning
with IJG version 6b, there is also a void pointer "client_data" in each
JPEG object, which the application can also use to find related data.
The library does not touch client_data at all.)

The individual methods that you might wish to override are:

error_exit (j_common_ptr cinfo)
	Receives control for a fatal error.  Information sufficient to
	generate the error message has been stored in cinfo->err; call
	output_message to display it.  Control must NOT return to the caller;
	generally this routine will exit() or longjmp() somewhere.
	Typically you would override this routine to get rid of the exit()
	default behavior.  Note that if you continue processing, you should
	clean up the JPEG object with jpeg_abort() or jpeg_destroy().

output_message (j_common_ptr cinfo)
	Actual output of any JPEG message.  Override this to send messages
	somewhere other than stderr.  Note that this method does not know
	how to generate a message, only where to send it.

format_message (j_common_ptr cinfo, char * buffer)
	Constructs a readable error message string based on the error info
	stored in cinfo->err.  This method is called by output_message.  Few
	applications should need to override this method.  One possible
	reason for doing so is to implement dynamic switching of error message
	language.

emit_message (j_common_ptr cinfo, int msg_level)
	Decide whether or not to emit a warning or trace message; if so,
	calls output_message.  The main reason for overriding this method
	would be to abort on warnings.  msg_level is -1 for warnings,
	0 and up for trace messages.

Only error_exit() and emit_message() are called from the rest of the JPEG
library; the other two are internal to the error handler.

The actual message texts are stored in an array of strings which is pointed to
by the field err->jpeg_message_table.  The messages are numbered from 0 to
err->last_jpeg_message, and it is these code numbers that are used in the
JPEG library code.  You could replace the message texts (for instance, with
messages in French or German) by changing the message table pointer.  See
jerror.h for the default texts.  CAUTION: this table will almost certainly
change or grow from one library version to the next.

It may be useful for an application to add its own message texts that are
handled by the same mechanism.  The error handler supports a second "add-on"
message table for this purpose.  To define an addon table, set the pointer
err->addon_message_table and the message numbers err->first_addon_message and
err->last_addon_message.  If you number the addon messages beginning at 1000
or so, you won't have to worry about conflicts with the library's built-in
messages.  See the sample applications cjpeg/djpeg for an example of using
addon messages (the addon messages are defined in cderror.h).

Actual invocation of the error handler is done via macros defined in jerror.h:
	ERREXITn(...)	for fatal errors
	WARNMSn(...)	for corrupt-data warnings
	TRACEMSn(...)	for trace and informational messages.
These macros store the message code and any additional parameters into the
error handler struct, then invoke the error_exit() or emit_message() method.
The variants of each macro are for varying numbers of additional parameters.
The additional parameters are inserted into the generated message using
standard printf() format codes.

See jerror.h and jerror.c for further details.


## Compressed data handling (source and destination managers)

The JPEG compression library sends its compressed data to a "destination
manager" module.  The default destination manager just writes the data to a
memory buffer or to a stdio stream, but you can provide your own manager to
do something else.  Similarly, the decompression library calls a "source
manager" to obtain the compressed data; you can provide your own source
manager if you want the data to come from somewhere other than a memory
buffer or a stdio stream.

In both cases, compressed data is processed a bufferload at a time: the
destination or source manager provides a work buffer, and the library invokes
the manager only when the buffer is filled or emptied.  (You could define a
one-character buffer to force the manager to be invoked for each byte, but
that would be rather inefficient.)  The buffer's size and location are
controlled by the manager, not by the library.  For example, the memory
source manager just makes the buffer pointer and length point to the original
data in memory.  In this case the buffer-reload procedure will be invoked
only if the decompressor ran off the end of the datastream, which would
indicate an erroneous datastream.

The work buffer is defined as an array of datatype JOCTET, which is generally
"char" or "unsigned char".  On a machine where char is not exactly 8 bits
wide, you must define JOCTET as a wider data type and then modify the data
source and destination modules to transcribe the work arrays into 8-bit units
on external storage.

A data destination manager struct contains a pointer and count defining the
next byte to write in the work buffer and the remaining free space:

	JOCTET * next_output_byte;  /* => next byte to write in buffer */
	size_t free_in_buffer;      /* # of byte spaces remaining in buffer */

The library increments the pointer and decrements the count until the buffer
is filled.  The manager's empty_output_buffer method must reset the pointer
and count.  The manager is expected to remember the buffer's starting address
and total size in private fields not visible to the library.

A data destination manager provides three methods:

init_destination (j_compress_ptr cinfo)
	Initialize destination.  This is called by jpeg_start_compress()
	before any data is actually written.  It must initialize
	next_output_byte and free_in_buffer.  free_in_buffer must be
	initialized to a positive value.

empty_output_buffer (j_compress_ptr cinfo)
	This is called whenever the buffer has filled (free_in_buffer
	reaches zero).  In typical applications, it should write out the
	*entire* buffer (use the saved start address and buffer length;
	ignore the current state of next_output_byte and free_in_buffer).
	Then reset the pointer & count to the start of the buffer, and
	return TRUE indicating that the buffer has been dumped.
	free_in_buffer must be set to a positive value when TRUE is
	returned.  A FALSE return should only be used when I/O suspension is
	desired (this operating mode is discussed in the next section).

term_destination (j_compress_ptr cinfo)
	Terminate destination --- called by jpeg_finish_compress() after all
	data has been written.  In most applications, this must flush any
	data remaining in the buffer.  Use either next_output_byte or
	free_in_buffer to determine how much data is in the buffer.

term_destination() is NOT called by jpeg_abort() or jpeg_destroy().  If you
want the destination manager to be cleaned up during an abort, you must do it
yourself.

You will also need code to create a jpeg_destination_mgr struct, fill in its
method pointers, and insert a pointer to the struct into the "dest" field of
the JPEG compression object.  This can be done in-line in your setup code if
you like, but it's probably cleaner to provide a separate routine similar to
the jpeg_stdio_dest() or jpeg_mem_dest() routines of the supplied destination
managers.

Decompression source managers follow a parallel design, but with some
additional frammishes.  The source manager struct contains a pointer and count
defining the next byte to read from the work buffer and the number of bytes
remaining:

	const JOCTET * next_input_byte; /* => next byte to read from buffer */
	size_t bytes_in_buffer;         /* # of bytes remaining in buffer */

The library increments the pointer and decrements the count until the buffer
is emptied.  The manager's fill_input_buffer method must reset the pointer and
count.  In most applications, the manager must remember the buffer's starting
address and total size in private fields not visible to the library.

A data source manager provides five methods:

init_source (j_decompress_ptr cinfo)
	Initialize source.  This is called by jpeg_read_header() before any
	data is actually read.  Unlike init_destination(), it may leave
	bytes_in_buffer set to 0 (in which case a fill_input_buffer() call
	will occur immediately).

fill_input_buffer (j_decompress_ptr cinfo)
	This is called whenever bytes_in_buffer has reached zero and more
	data is wanted.  In typical applications, it should read fresh data
	into the buffer (ignoring the current state of next_input_byte and
	bytes_in_buffer), reset the pointer & count to the start of the
	buffer, and return TRUE indicating that the buffer has been reloaded.
	It is not necessary to fill the buffer entirely, only to obtain at
	least one more byte.  bytes_in_buffer MUST be set to a positive value
	if TRUE is returned.  A FALSE return should only be used when I/O
	suspension is desired (this mode is discussed in the next section).

skip_input_data (j_decompress_ptr cinfo, long num_bytes)
	Skip num_bytes worth of data.  The buffer pointer and count should
	be advanced over num_bytes input bytes, refilling the buffer as
	needed.  This is used to skip over a potentially large amount of
	uninteresting data (such as an APPn marker).  In some applications
	it may be possible to optimize away the reading of the skipped data,
	but it's not clear that being smart is worth much trouble; large
	skips are uncommon.  bytes_in_buffer may be zero on return.
	A zero or negative skip count should be treated as a no-op.

resync_to_restart (j_decompress_ptr cinfo, int desired)
	This routine is called only when the decompressor has failed to find
	a restart (RSTn) marker where one is expected.  Its mission is to
	find a suitable point for resuming decompression.  For most
	applications, we recommend that you just use the default resync
	procedure, jpeg_resync_to_restart().  However, if you are able to back
	up in the input data stream, or if you have a-priori knowledge about
	the likely location of restart markers, you may be able to do better.
	Read the read_restart_marker() and jpeg_resync_to_restart() routines
	in jdmarker.c if you think you'd like to implement your own resync
	procedure.

term_source (j_decompress_ptr cinfo)
	Terminate source --- called by jpeg_finish_decompress() after all
	data has been read.  Often a no-op.

For both fill_input_buffer() and skip_input_data(), there is no such thing
as an EOF return.  If the end of the file has been reached, the routine has
a choice of exiting via ERREXIT() or inserting fake data into the buffer.
In most cases, generating a warning message and inserting a fake EOI marker
is the best course of action --- this will allow the decompressor to output
however much of the image is there.  In pathological cases, the decompressor
may swallow the EOI and again demand data ... just keep feeding it fake EOIs.
jdatasrc.c illustrates the recommended error recovery behavior.

term_source() is NOT called by jpeg_abort() or jpeg_destroy().  If you want
the source manager to be cleaned up during an abort, you must do it yourself.

You will also need code to create a jpeg_source_mgr struct, fill in its method
pointers, and insert a pointer to the struct into the "src" field of the JPEG
decompression object.  This can be done in-line in your setup code if you
like, but it's probably cleaner to provide a separate routine similar to the
jpeg_stdio_src() or jpeg_mem_src() routines of the supplied source managers.

For more information, consult the memory and stdio source and destination
managers in jdatasrc.c and jdatadst.c.


## I/O suspension

Some applications need to use the JPEG library as an incremental memory-to-
memory filter: when the compressed data buffer is filled or emptied, they want
control to return to the outer loop, rather than expecting that the buffer can
be emptied or reloaded within the data source/destination manager subroutine.
The library supports this need by providing an "I/O suspension" mode, which we
describe in this section.

The I/O suspension mode is not a panacea: nothing is guaranteed about the
maximum amount of time spent in any one call to the library, so it will not
eliminate response-time problems in single-threaded applications.  If you
need guaranteed response time, we suggest you "bite the bullet" and implement
a real multi-tasking capability.

To use I/O suspension, cooperation is needed between the calling application
and the data source or destination manager; you will always need a custom
source/destination manager.  (Please read the previous section if you haven't
already.)  The basic idea is that the empty_output_buffer() or
fill_input_buffer() routine is a no-op, merely returning FALSE to indicate
that it has done nothing.  Upon seeing this, the JPEG library suspends
operation and returns to its caller.  The surrounding application is
responsible for emptying or refilling the work buffer before calling the
JPEG library again.

Compression suspension:

For compression suspension, use an empty_output_buffer() routine that returns
FALSE; typically it will not do anything else.  This will cause the
compressor to return to the caller of jpeg_write_scanlines(), with the return
value indicating that not all the supplied scanlines have been accepted.
The application must make more room in the output buffer, adjust the output
buffer pointer/count appropriately, and then call jpeg_write_scanlines()
again, pointing to the first unconsumed scanline.

When forced to suspend, the compressor will backtrack to a convenient stopping
point (usually the start of the current MCU); it will regenerate some output
data when restarted.  Therefore, although empty_output_buffer() is only
called when the buffer is filled, you should NOT write out the entire buffer
after a suspension.  Write only the data up to the current position of
next_output_byte/free_in_buffer.  The data beyond that point will be
regenerated after resumption.

Because of the backtracking behavior, a good-size output buffer is essential
for efficiency; you don't want the compressor to suspend often.  (In fact, an
overly small buffer could lead to infinite looping, if a single MCU required
more data than would fit in the buffer.)  We recommend a buffer of at least
several Kbytes.  You may want to insert explicit code to ensure that you don't
call jpeg_write_scanlines() unless there is a reasonable amount of space in
the output buffer; in other words, flush the buffer before trying to compress
more data.

The compressor does not allow suspension while it is trying to write JPEG
markers at the beginning and end of the file.  This means that:
  * At the beginning of a compression operation, there must be enough free
    space in the output buffer to hold the header markers (typically 600 or
    so bytes).  The recommended buffer size is bigger than this anyway, so
    this is not a problem as long as you start with an empty buffer.  However,
    this restriction might catch you if you insert large special markers, such
    as a JFIF thumbnail image, without flushing the buffer afterwards.
  * When you call jpeg_finish_compress(), there must be enough space in the
    output buffer to emit any buffered data and the final EOI marker.  In the
    current implementation, half a dozen bytes should suffice for this, but
    for safety's sake we recommend ensuring that at least 100 bytes are free
    before calling jpeg_finish_compress().

A more significant restriction is that jpeg_finish_compress() cannot suspend.
This means you cannot use suspension with multi-pass operating modes, namely
Huffman code optimization and multiple-scan output.  Those modes write the
whole file during jpeg_finish_compress(), which will certainly result in
buffer overrun.  (Note that this restriction applies only to compression,
not decompression.  The decompressor supports input suspension in all of its
operating modes.)

Decompression suspension:

For decompression suspension, use a fill_input_buffer() routine that simply
returns FALSE (except perhaps during error recovery, as discussed below).
This will cause the decompressor to return to its caller with an indication
that suspension has occurred.  This can happen at four places:
  * jpeg_read_header(): will return JPEG_SUSPENDED.
  * jpeg_start_decompress(): will return FALSE, rather than its usual TRUE.
  * jpeg_read_scanlines(): will return the number of scanlines already
	completed (possibly 0).
  * jpeg_finish_decompress(): will return FALSE, rather than its usual TRUE.
The surrounding application must recognize these cases, load more data into
the input buffer, and repeat the call.  In the case of jpeg_read_scanlines(),
increment the passed pointers past any scanlines successfully read.

Just as with compression, the decompressor will typically backtrack to a
convenient restart point before suspending.  When fill_input_buffer() is
called, next_input_byte/bytes_in_buffer point to the current restart point,
which is where the decompressor will backtrack to if FALSE is returned.
The data beyond that position must NOT be discarded if you suspend; it needs
to be re-read upon resumption.  In most implementations, you'll need to shift
this data down to the start of your work buffer and then load more data after
it.  Again, this behavior means that a several-Kbyte work buffer is essential
for decent performance; furthermore, you should load a reasonable amount of
new data before resuming decompression.  (If you loaded, say, only one new
byte each time around, you could waste a LOT of cycles.)

The skip_input_data() source manager routine requires special care in a
suspension scenario.  This routine is NOT granted the ability to suspend the
decompressor; it can decrement bytes_in_buffer to zero, but no more.  If the
requested skip distance exceeds the amount of data currently in the input
buffer, then skip_input_data() must set bytes_in_buffer to zero and record the
additional skip distance somewhere else.  The decompressor will immediately
call fill_input_buffer(), which should return FALSE, which will cause a
suspension return.  The surrounding application must then arrange to discard
the recorded number of bytes before it resumes loading the input buffer.
(Yes, this design is rather baroque, but it avoids complexity in the far more
common case where a non-suspending source manager is used.)

If the input data has been exhausted, we recommend that you emit a warning
and insert dummy EOI markers just as a non-suspending data source manager
would do.  This can be handled either in the surrounding application logic or
within fill_input_buffer(); the latter is probably more efficient.  If
fill_input_buffer() knows that no more data is available, it can set the
pointer/count to point to a dummy EOI marker and then return TRUE just as
though it had read more data in a non-suspending situation.

The decompressor does not attempt to suspend within standard JPEG markers;
instead it will backtrack to the start of the marker and reprocess the whole
marker next time.  Hence the input buffer must be large enough to hold the
longest standard marker in the file.  Standard JPEG markers should normally
not exceed a few hundred bytes each (DHT tables are typically the longest).
We recommend at least a 2K buffer for performance reasons, which is much
larger than any correct marker is likely to be.  For robustness against
damaged marker length counts, you may wish to insert a test in your
application for the case that the input buffer is completely full and yet
the decoder has suspended without consuming any data --- otherwise, if this
situation did occur, it would lead to an endless loop.  (The library can't
provide this test since it has no idea whether "the buffer is full", or
even whether there is a fixed-size input buffer.)

The input buffer would need to be 64K to allow for arbitrary COM or APPn
markers, but these are handled specially: they are either saved into allocated
memory, or skipped over by calling skip_input_data().  In the former case,
suspension is handled correctly, and in the latter case, the problem of
buffer overrun is placed on skip_input_data's shoulders, as explained above.
Note that if you provide your own marker handling routine for large markers,
you should consider how to deal with buffer overflow.

Multiple-buffer management:

In some applications it is desirable to store the compressed data in a linked
list of buffer areas, so as to avoid data copying.  This can be handled by
having empty_output_buffer() or fill_input_buffer() set the pointer and count
to reference the next available buffer; FALSE is returned only if no more
buffers are available.  Although seemingly straightforward, there is a
pitfall in this approach: the backtrack that occurs when FALSE is returned
could back up into an earlier buffer.  For example, when fill_input_buffer()
is called, the current pointer & count indicate the backtrack restart point.
Since fill_input_buffer() will set the pointer and count to refer to a new
buffer, the restart position must be saved somewhere else.  Suppose a second
call to fill_input_buffer() occurs in the same library call, and no
additional input data is available, so fill_input_buffer must return FALSE.
If the JPEG library has not moved the pointer/count forward in the current
buffer, then *the correct restart point is the saved position in the prior
buffer*.  Prior buffers may be discarded only after the library establishes
a restart point within a later buffer.  Similar remarks apply for output into
a chain of buffers.

The library will never attempt to backtrack over a skip_input_data() call,
so any skipped data can be permanently discarded.  You still have to deal
with the case of skipping not-yet-received data, however.

It's much simpler to use only a single buffer; when fill_input_buffer() is
called, move any unconsumed data (beyond the current pointer/count) down to
the beginning of this buffer and then load new data into the remaining buffer
space.  This approach requires a little more data copying but is far easier
to get right.


## Progressive JPEG support

Progressive JPEG rearranges the stored data into a series of scans of
increasing quality.  In situations where a JPEG file is transmitted across a
slow communications link, a decoder can generate a low-quality image very
quickly from the first scan, then gradually improve the displayed quality as
more scans are received.  The final image after all scans are complete is
identical to that of a regular (sequential) JPEG file of the same quality
setting.  Progressive JPEG files are often slightly smaller than equivalent
sequential JPEG files, but the possibility of incremental display is the main
reason for using progressive JPEG.

The IJG encoder library generates progressive JPEG files when given a
suitable "scan script" defining how to divide the data into scans.
Creation of progressive JPEG files is otherwise transparent to the encoder.
Progressive JPEG files can also be read transparently by the decoder library.
If the decoding application simply uses the library as defined above, it
will receive a final decoded image without any indication that the file was
progressive.  Of course, this approach does not allow incremental display.
To perform incremental display, an application needs to use the decoder
library's "buffered-image" mode, in which it receives a decoded image
multiple times.

Each displayed scan requires about as much work to decode as a full JPEG
image of the same size, so the decoder must be fairly fast in relation to the
data transmission rate in order to make incremental display useful.  However,
it is possible to skip displaying the image and simply add the incoming bits
to the decoder's coefficient buffer.  This is fast because only Huffman
decoding need be done, not IDCT, upsampling, colorspace conversion, etc.
The IJG decoder library allows the application to switch dynamically between
displaying the image and simply absorbing the incoming bits.  A properly
coded application can automatically adapt the number of display passes to
suit the time available as the image is received.  Also, a final
higher-quality display cycle can be performed from the buffered data after
the end of the file is reached.

Progressive compression:

To create a progressive JPEG file (or a multiple-scan sequential JPEG file),
set the scan_info cinfo field to point to an array of scan descriptors, and
perform compression as usual.  Instead of constructing your own scan list,
you can call the jpeg_simple_progression() helper routine to create a
recommended progression sequence; this method should be used by all
applications that don't want to get involved in the nitty-gritty of
progressive scan sequence design.  (If you want to provide user control of
scan sequences, you may wish to borrow the scan script reading code found
in rdswitch.c, so that you can read scan script files just like cjpeg's.)
When scan_info is not NULL, the compression library will store DCT'd data
into a buffer array as jpeg_write_scanlines() is called, and will emit all
the requested scans during jpeg_finish_compress().  This implies that
multiple-scan output cannot be created with a suspending data destination
manager, since jpeg_finish_compress() does not support suspension.  We
should also note that the compressor currently forces Huffman optimization
mode when creating a progressive JPEG file, because the default Huffman
tables are unsuitable for progressive files.

Progressive decompression:

When buffered-image mode is not used, the decoder library will read all of
a multi-scan file during jpeg_start_decompress(), so that it can provide a
final decoded image.  (Here "multi-scan" means either progressive or
multi-scan sequential.)  This makes multi-scan files transparent to the
decoding application.  However, existing applications that used suspending
input with version 5 of the IJG library will need to be modified to check
for a suspension return from jpeg_start_decompress().

To perform incremental display, an application must use the library's
buffered-image mode.  This is described in the next section.


## Buffered-image mode

In buffered-image mode, the library stores the partially decoded image in a
coefficient buffer, from which it can be read out as many times as desired.
This mode is typically used for incremental display of progressive JPEG files,
but it can be used with any JPEG file.  Each scan of a progressive JPEG file
adds more data (more detail) to the buffered image.  The application can
display in lockstep with the source file (one display pass per input scan),
or it can allow input processing to outrun display processing.  By making
input and display processing run independently, it is possible for the
application to adapt progressive display to a wide range of data transmission
rates.

The basic control flow for buffered-image decoding is
```c
	jpeg_create_decompress()
	set data source
	jpeg_read_header()
	set overall decompression parameters
	cinfo.buffered_image = TRUE;	/* select buffered-image mode */
	jpeg_start_decompress()
	for (each output pass) {
	    adjust output decompression parameters if required
	    jpeg_start_output()		/* start a new output pass */
	    for (all scanlines in image) {
	        jpeg_read_scanlines()
	        display scanlines
	    }
	    jpeg_finish_output()	/* terminate output pass */
	}
	jpeg_finish_decompress()
	jpeg_destroy_decompress()
```
This differs from ordinary unbuffered decoding in that there is an additional
level of looping.  The application can choose how many output passes to make
and how to display each pass.

The simplest approach to displaying progressive images is to do one display
pass for each scan appearing in the input file.  In this case the outer loop
condition is typically
	while (! jpeg_input_complete(&cinfo))
and the start-output call should read
	jpeg_start_output(&cinfo, cinfo.input_scan_number);
The second parameter to jpeg_start_output() indicates which scan of the input
file is to be displayed; the scans are numbered starting at 1 for this
purpose.  (You can use a loop counter starting at 1 if you like, but using
the library's input scan counter is easier.)  The library automatically reads
data as necessary to complete each requested scan, and jpeg_finish_output()
advances to the next scan or end-of-image marker (hence input_scan_number
will be incremented by the time control arrives back at jpeg_start_output()).
With this technique, data is read from the input file only as needed, and
input and output processing run in lockstep.

After reading the final scan and reaching the end of the input file, the
buffered image remains available; it can be read additional times by
repeating the jpeg_start_output()/jpeg_read_scanlines()/jpeg_finish_output()
sequence.  For example, a useful technique is to use fast one-pass color
quantization for display passes made while the image is arriving, followed by
a final display pass using two-pass quantization for highest quality.  This
is done by changing the library parameters before the final output pass.
Changing parameters between passes is discussed in detail below.

In general the last scan of a progressive file cannot be recognized as such
until after it is read, so a post-input display pass is the best approach if
you want special processing in the final pass.

When done with the image, be sure to call jpeg_finish_decompress() to release
the buffered image (or just use jpeg_destroy_decompress()).

If input data arrives faster than it can be displayed, the application can
cause the library to decode input data in advance of what's needed to produce
output.  This is done by calling the routine jpeg_consume_input().
The return value is one of the following:
	JPEG_REACHED_SOS:    reached an SOS marker (the start of a new scan)
	JPEG_REACHED_EOI:    reached the EOI marker (end of image)
	JPEG_ROW_COMPLETED:  completed reading one MCU row of compressed data
	JPEG_SCAN_COMPLETED: completed reading last MCU row of current scan
	JPEG_SUSPENDED:      suspended before completing any of the above
(JPEG_SUSPENDED can occur only if a suspending data source is used.)  This
routine can be called at any time after initializing the JPEG object.  It
reads some additional data and returns when one of the indicated significant
events occurs.  (If called after the EOI marker is reached, it will
immediately return JPEG_REACHED_EOI without attempting to read more data.)

The library's output processing will automatically call jpeg_consume_input()
whenever the output processing overtakes the input; thus, simple lockstep
display requires no direct calls to jpeg_consume_input().  But by adding
calls to jpeg_consume_input(), you can absorb data in advance of what is
being displayed.  This has two benefits:
  * You can limit buildup of unprocessed data in your input buffer.
  * You can eliminate extra display passes by paying attention to the
    state of the library's input processing.

The first of these benefits only requires interspersing calls to
jpeg_consume_input() with your display operations and any other processing
you may be doing.  To avoid wasting cycles due to backtracking, it's best to
call jpeg_consume_input() only after a hundred or so new bytes have arrived.
This is discussed further under "I/O suspension", above.  (Note: the JPEG
library currently is not thread-safe.  You must not call jpeg_consume_input()
from one thread of control if a different library routine is working on the
same JPEG object in another thread.)

When input arrives fast enough that more than one new scan is available
before you start a new output pass, you may as well skip the output pass
corresponding to the completed scan.  This occurs for free if you pass
cinfo.input_scan_number as the target scan number to jpeg_start_output().
The input_scan_number field is simply the index of the scan currently being
consumed by the input processor.  You can ensure that this is up-to-date by
emptying the input buffer just before calling jpeg_start_output(): call
jpeg_consume_input() repeatedly until it returns JPEG_SUSPENDED or
JPEG_REACHED_EOI.

The target scan number passed to jpeg_start_output() is saved in the
cinfo.output_scan_number field.  The library's output processing calls
jpeg_consume_input() whenever the current input scan number and row within
that scan is less than or equal to the current output scan number and row.
Thus, input processing can "get ahead" of the output processing but is not
allowed to "fall behind".  You can achieve several different effects by
manipulating this interlock rule.  For example, if you pass a target scan
number greater than the current input scan number, the output processor will
wait until that scan starts to arrive before producing any output.  (To avoid
an infinite loop, the target scan number is automatically reset to the last
scan number when the end of image is reached.  Thus, if you specify a large
target scan number, the library will just absorb the entire input file and
then perform an output pass.  This is effectively the same as what
jpeg_start_decompress() does when you don't select buffered-image mode.)
When you pass a target scan number equal to the current input scan number,
the image is displayed no faster than the current input scan arrives.  The
final possibility is to pass a target scan number less than the current input
scan number; this disables the input/output interlock and causes the output
processor to simply display whatever it finds in the image buffer, without
waiting for input.  (However, the library will not accept a target scan
number less than one, so you can't avoid waiting for the first scan.)

When data is arriving faster than the output display processing can advance
through the image, jpeg_consume_input() will store data into the buffered
image beyond the point at which the output processing is reading data out
again.  If the input arrives fast enough, it may "wrap around" the buffer to
the point where the input is more than one whole scan ahead of the output.
If the output processing simply proceeds through its display pass without
paying attention to the input, the effect seen on-screen is that the lower
part of the image is one or more scans better in quality than the upper part.
Then, when the next output scan is started, you have a choice of what target
scan number to use.  The recommended choice is to use the current input scan
number at that time, which implies that you've skipped the output scans
corresponding to the input scans that were completed while you processed the
previous output scan.  In this way, the decoder automatically adapts its
speed to the arriving data, by skipping output scans as necessary to keep up
with the arriving data.

When using this strategy, you'll want to be sure that you perform a final
output pass after receiving all the data; otherwise your last display may not
be full quality across the whole screen.  So the right outer loop logic is
something like this:
```c
	do {
	    absorb any waiting input by calling jpeg_consume_input()
	    final_pass = jpeg_input_complete(&cinfo);
	    adjust output decompression parameters if required
	    jpeg_start_output(&cinfo, cinfo.input_scan_number);
	    ...
	    jpeg_finish_output()
	} while (! final_pass);
rather than quitting as soon as jpeg_input_complete() returns TRUE.  This
arrangement makes it simple to use higher-quality decoding parameters
for the final pass.  But if you don't want to use special parameters for
the final pass, the right loop logic is like this:
	for (;;) {
	    absorb any waiting input by calling jpeg_consume_input()
	    jpeg_start_output(&cinfo, cinfo.input_scan_number);
	    ...
	    jpeg_finish_output()
	    if (jpeg_input_complete(&cinfo) &&
	        cinfo.input_scan_number == cinfo.output_scan_number)
	      break;
	}
```
In this case you don't need to know in advance whether an output pass is to
be the last one, so it's not necessary to have reached EOF before starting
the final output pass; rather, what you want to test is whether the output
pass was performed in sync with the final input scan.  This form of the loop
will avoid an extra output pass whenever the decoder is able (or nearly able)
to keep up with the incoming data.

When the data transmission speed is high, you might begin a display pass,
then find that much or all of the file has arrived before you can complete
the pass.  (You can detect this by noting the JPEG_REACHED_EOI return code
from jpeg_consume_input(), or equivalently by testing jpeg_input_complete().)
In this situation you may wish to abort the current display pass and start a
new one using the newly arrived information.  To do so, just call
jpeg_finish_output() and then start a new pass with jpeg_start_output().

A variant strategy is to abort and restart display if more than one complete
scan arrives during an output pass; this can be detected by noting
JPEG_REACHED_SOS returns and/or examining cinfo.input_scan_number.  This
idea should be employed with caution, however, since the display process
might never get to the bottom of the image before being aborted, resulting
in the lower part of the screen being several passes worse than the upper.
In most cases it's probably best to abort an output pass only if the whole
file has arrived and you want to begin the final output pass immediately.

When receiving data across a communication link, we recommend always using
the current input scan number for the output target scan number; if a
higher-quality final pass is to be done, it should be started (aborting any
incomplete output pass) as soon as the end of file is received.  However,
many other strategies are possible.  For example, the application can examine
the parameters of the current input scan and decide whether to display it or
not.  If the scan contains only chroma data, one might choose not to use it
as the target scan, expecting that the scan will be small and will arrive
quickly.  To skip to the next scan, call jpeg_consume_input() until it
returns JPEG_REACHED_SOS or JPEG_REACHED_EOI.  Or just use the next higher
number as the target scan for jpeg_start_output(); but that method doesn't
let you inspect the next scan's parameters before deciding to display it.


In buffered-image mode, jpeg_start_decompress() never performs input and
thus never suspends.  An application that uses input suspension with
buffered-image mode must be prepared for suspension returns from these
routines:
* jpeg_start_output() performs input only if you request 2-pass quantization
  and the target scan isn't fully read yet.  (This is discussed below.)
* jpeg_read_scanlines(), as always, returns the number of scanlines that it
  was able to produce before suspending.
* jpeg_finish_output() will read any markers following the target scan,
  up to the end of the file or the SOS marker that begins another scan.
  (But it reads no input if jpeg_consume_input() has already reached the
  end of the file or a SOS marker beyond the target output scan.)
* jpeg_finish_decompress() will read until the end of file, and thus can
  suspend if the end hasn't already been reached (as can be tested by
  calling jpeg_input_complete()).
jpeg_start_output(), jpeg_finish_output(), and jpeg_finish_decompress()
all return TRUE if they completed their tasks, FALSE if they had to suspend.
In the event of a FALSE return, the application must load more input data
and repeat the call.  Applications that use non-suspending data sources need
not check the return values of these three routines.


It is possible to change decoding parameters between output passes in the
buffered-image mode.  The decoder library currently supports only very
limited changes of parameters.  ONLY THE FOLLOWING parameter changes are
allowed after jpeg_start_decompress() is called:
* dct_method can be changed before each call to jpeg_start_output().
  For example, one could use a fast DCT method for early scans, changing
  to a higher quality method for the final scan.
* dither_mode can be changed before each call to jpeg_start_output();
  of course this has no impact if not using color quantization.  Typically
  one would use ordered dither for initial passes, then switch to
  Floyd-Steinberg dither for the final pass.  Caution: changing dither mode
  can cause more memory to be allocated by the library.  Although the amount
  of memory involved is not large (a scanline or so), it may cause the
  initial max_memory_to_use specification to be exceeded, which in the worst
  case would result in an out-of-memory failure.
* do_block_smoothing can be changed before each call to jpeg_start_output().
  This setting is relevant only when decoding a progressive JPEG image.
  During the first DC-only scan, block smoothing provides a very "fuzzy" look
  instead of the very "blocky" look seen without it; which is better seems a
  matter of personal taste.  But block smoothing is nearly always a win
  during later stages, especially when decoding a successive-approximation
  image: smoothing helps to hide the slight blockiness that otherwise shows
  up on smooth gradients until the lowest coefficient bits are sent.
* Color quantization mode can be changed under the rules described below.
  You *cannot* change between full-color and quantized output (because that
  would alter the required I/O buffer sizes), but you can change which
  quantization method is used.

When generating color-quantized output, changing quantization method is a
very useful way of switching between high-speed and high-quality display.
The library allows you to change among its three quantization methods:
1. Single-pass quantization to a fixed color cube.
   Selected by cinfo.two_pass_quantize = FALSE and cinfo.colormap = NULL.
2. Single-pass quantization to an application-supplied colormap.
   Selected by setting cinfo.colormap to point to the colormap (the value of
   two_pass_quantize is ignored); also set cinfo.actual_number_of_colors.
3. Two-pass quantization to a colormap chosen specifically for the image.
   Selected by cinfo.two_pass_quantize = TRUE and cinfo.colormap = NULL.
   (This is the default setting selected by jpeg_read_header, but it is
   probably NOT what you want for the first pass of progressive display!)
These methods offer successively better quality and lesser speed.  However,
only the first method is available for quantizing in non-RGB color spaces.

IMPORTANT: because the different quantizer methods have very different
working-storage requirements, the library requires you to indicate which
one(s) you intend to use before you call jpeg_start_decompress().  (If we did
not require this, the max_memory_to_use setting would be a complete fiction.)
You do this by setting one or more of these three cinfo fields to TRUE:
	enable_1pass_quant		Fixed color cube colormap
	enable_external_quant		Externally-supplied colormap
	enable_2pass_quant		Two-pass custom colormap
All three are initialized FALSE by jpeg_read_header().  But
jpeg_start_decompress() automatically sets TRUE the one selected by the
current two_pass_quantize and colormap settings, so you only need to set the
enable flags for any other quantization methods you plan to change to later.

After setting the enable flags correctly at jpeg_start_decompress() time, you
can change to any enabled quantization method by setting two_pass_quantize
and colormap properly just before calling jpeg_start_output().  The following
special rules apply:
1. You must explicitly set cinfo.colormap to NULL when switching to 1-pass
   or 2-pass mode from a different mode, or when you want the 2-pass
   quantizer to be re-run to generate a new colormap.
2. To switch to an external colormap, or to change to a different external
   colormap than was used on the prior pass, you must call
   jpeg_new_colormap() after setting cinfo.colormap.
NOTE: if you want to use the same colormap as was used in the prior pass,
you should not do either of these things.  This will save some nontrivial
switchover costs.
(These requirements exist because cinfo.colormap will always be non-NULL
after completing a prior output pass, since both the 1-pass and 2-pass
quantizers set it to point to their output colormaps.  Thus you have to
do one of these two things to notify the library that something has changed.
Yup, it's a bit klugy, but it's necessary to do it this way for backwards
compatibility.)

Note that in buffered-image mode, the library generates any requested colormap
during jpeg_start_output(), not during jpeg_start_decompress().

When using two-pass quantization, jpeg_start_output() makes a pass over the
buffered image to determine the optimum color map; it therefore may take a
significant amount of time, whereas ordinarily it does little work.  The
progress monitor hook is called during this pass, if defined.  It is also
important to realize that if the specified target scan number is greater than
or equal to the current input scan number, jpeg_start_output() will attempt
to consume input as it makes this pass.  If you use a suspending data source,
you need to check for a FALSE return from jpeg_start_output() under these
conditions.  The combination of 2-pass quantization and a not-yet-fully-read
target scan is the only case in which jpeg_start_output() will consume input.


Application authors who support buffered-image mode may be tempted to use it
for all JPEG images, even single-scan ones.  This will work, but it is
inefficient: there is no need to create an image-sized coefficient buffer for
single-scan images.  Requesting buffered-image mode for such an image wastes
memory.  Worse, it can cost time on large images, since the buffered data has
to be swapped out or written to a temporary file.  If you are concerned about
maximum performance on baseline JPEG files, you should use buffered-image
mode only when the incoming file actually has multiple scans.  This can be
tested by calling jpeg_has_multiple_scans(), which will return a correct
result at any time after jpeg_read_header() completes.

It is also worth noting that when you use jpeg_consume_input() to let input
processing get ahead of output processing, the resulting pattern of access to
the coefficient buffer is quite nonsequential.  It's best to use the memory
manager jmemnobs.c if you can (ie, if you have enough real or virtual main
memory).  If not, at least make sure that max_memory_to_use is set as high as
possible.  If the JPEG memory manager has to use a temporary file, you will
probably see a lot of disk traffic and poor performance.  (This could be
improved with additional work on the memory manager, but we haven't gotten
around to it yet.)

In some applications it may be convenient to use jpeg_consume_input() for all
input processing, including reading the initial markers; that is, you may
wish to call jpeg_consume_input() instead of jpeg_read_header() during
startup.  This works, but note that you must check for JPEG_REACHED_SOS and
JPEG_REACHED_EOI return codes as the equivalent of jpeg_read_header's codes.
Once the first SOS marker has been reached, you must call
jpeg_start_decompress() before jpeg_consume_input() will consume more input;
it'll just keep returning JPEG_REACHED_SOS until you do.  If you read a
tables-only file this way, jpeg_consume_input() will return JPEG_REACHED_EOI
without ever returning JPEG_REACHED_SOS; be sure to check for this case.
If this happens, the decompressor will not read any more input until you call
jpeg_abort() to reset it.  It is OK to call jpeg_consume_input() even when not
using buffered-image mode, but in that case it's basically a no-op after the
initial markers have been read: it will just return JPEG_SUSPENDED.


## Abbreviated datastreams and multiple images

A JPEG compression or decompression object can be reused to process multiple
images.  This saves a small amount of time per image by eliminating the
"create" and "destroy" operations, but that isn't the real purpose of the
feature.  Rather, reuse of an object provides support for abbreviated JPEG
datastreams.  Object reuse can also simplify processing a series of images in
a single input or output file.  This section explains these features.

A JPEG file normally contains several hundred bytes worth of quantization
and Huffman tables.  In a situation where many images will be stored or
transmitted with identical tables, this may represent an annoying overhead.
The JPEG standard therefore permits tables to be omitted.  The standard
defines three classes of JPEG datastreams:
  * "Interchange" datastreams contain an image and all tables needed to decode
     the image.  These are the usual kind of JPEG file.
  * "Abbreviated image" datastreams contain an image, but are missing some or
    all of the tables needed to decode that image.
  * "Abbreviated table specification" (henceforth "tables-only") datastreams
    contain only table specifications.
To decode an abbreviated image, it is necessary to load the missing table(s)
into the decoder beforehand.  This can be accomplished by reading a separate
tables-only file.  A variant scheme uses a series of images in which the first
image is an interchange (complete) datastream, while subsequent ones are
abbreviated and rely on the tables loaded by the first image.  It is assumed
that once the decoder has read a table, it will remember that table until a
new definition for the same table number is encountered.

It is the application designer's responsibility to figure out how to associate
the correct tables with an abbreviated image.  While abbreviated datastreams
can be useful in a closed environment, their use is strongly discouraged in
any situation where data exchange with other applications might be needed.
Caveat designer.

The JPEG library provides support for reading and writing any combination of
tables-only datastreams and abbreviated images.  In both compression and
decompression objects, a quantization or Huffman table will be retained for
the lifetime of the object, unless it is overwritten by a new table definition.


To create abbreviated image datastreams, it is only necessary to tell the
compressor not to emit some or all of the tables it is using.  Each
quantization and Huffman table struct contains a boolean field "sent_table",
which normally is initialized to FALSE.  For each table used by the image, the
header-writing process emits the table and sets sent_table = TRUE unless it is
already TRUE.  (In normal usage, this prevents outputting the same table
definition multiple times, as would otherwise occur because the chroma
components typically share tables.)  Thus, setting this field to TRUE before
calling jpeg_start_compress() will prevent the table from being written at
all.

If you want to create a "pure" abbreviated image file containing no tables,
just call "jpeg_suppress_tables(&cinfo, TRUE)" after constructing all the
tables.  If you want to emit some but not all tables, you'll need to set the
individual sent_table fields directly.

To create an abbreviated image, you must also call jpeg_start_compress()
with a second parameter of FALSE, not TRUE.  Otherwise jpeg_start_compress()
will force all the sent_table fields to FALSE.  (This is a safety feature to
prevent abbreviated images from being created accidentally.)

To create a tables-only file, perform the same parameter setup that you
normally would, but instead of calling jpeg_start_compress() and so on, call
jpeg_write_tables(&cinfo).  This will write an abbreviated datastream
containing only SOI, DQT and/or DHT markers, and EOI.  All the quantization
and Huffman tables that are currently defined in the compression object will
be emitted unless their sent_tables flag is already TRUE, and then all the
sent_tables flags will be set TRUE.

A sure-fire way to create matching tables-only and abbreviated image files
is to proceed as follows:

	create JPEG compression object
	set JPEG parameters
	set destination to tables-only file
	jpeg_write_tables(&cinfo);
	set destination to image file
	jpeg_start_compress(&cinfo, FALSE);
	write data...
	jpeg_finish_compress(&cinfo);

Since the JPEG parameters are not altered between writing the table file and
the abbreviated image file, the same tables are sure to be used.  Of course,
you can repeat the jpeg_start_compress() ... jpeg_finish_compress() sequence
many times to produce many abbreviated image files matching the table file.

You cannot suppress output of the computed Huffman tables when Huffman
optimization is selected.  (If you could, there'd be no way to decode the
image...)  Generally, you don't want to set optimize_coding = TRUE when
you are trying to produce abbreviated files.

In some cases you might want to compress an image using tables which are
not stored in the application, but are defined in an interchange or
tables-only file readable by the application.  This can be done by setting up
a JPEG decompression object to read the specification file, then copying the
tables into your compression object.  See jpeg_copy_critical_parameters()
for an example of copying quantization tables.


To read abbreviated image files, you simply need to load the proper tables
into the decompression object before trying to read the abbreviated image.
If the proper tables are stored in the application program, you can just
allocate the table structs and fill in their contents directly.  For example,
to load a fixed quantization table into table slot "n":
```c
    if (cinfo.quant_tbl_ptrs[n] == NULL)
      cinfo.quant_tbl_ptrs[n] = jpeg_alloc_quant_table((j_common_ptr) &cinfo);
    quant_ptr = cinfo.quant_tbl_ptrs[n];	/* quant_ptr is JQUANT_TBL* */
    for (i = 0; i < 64; i++) {
      /* Qtable[] is desired quantization table, in natural array order */
      quant_ptr->quantval[i] = Qtable[i];
    }

Code to load a fixed Huffman table is typically (for AC table "n"):

    if (cinfo.ac_huff_tbl_ptrs[n] == NULL)
      cinfo.ac_huff_tbl_ptrs[n] = jpeg_alloc_huff_table((j_common_ptr) &cinfo);
    huff_ptr = cinfo.ac_huff_tbl_ptrs[n];	/* huff_ptr is JHUFF_TBL* */
    for (i = 1; i <= 16; i++) {
      /* counts[i] is number of Huffman codes of length i bits, i=1..16 */
      huff_ptr->bits[i] = counts[i];
    }
    for (i = 0; i < 256; i++) {
      /* symbols[] is the list of Huffman symbols, in code-length order */
      huff_ptr->huffval[i] = symbols[i];
    }
```
(Note that trying to set cinfo.quant_tbl_ptrs[n] to point directly at a
constant JQUANT_TBL object is not safe.  If the incoming file happened to
contain a quantization table definition, your master table would get
overwritten!  Instead allocate a working table copy and copy the master table
into it, as illustrated above.  Ditto for Huffman tables, of course.)

You might want to read the tables from a tables-only file, rather than
hard-wiring them into your application.  The jpeg_read_header() call is
sufficient to read a tables-only file.  You must pass a second parameter of
FALSE to indicate that you do not require an image to be present.  Thus, the
typical scenario is

	create JPEG decompression object
	set source to tables-only file
	jpeg_read_header(&cinfo, FALSE);
	set source to abbreviated image file
	jpeg_read_header(&cinfo, TRUE);
	set decompression parameters
	jpeg_start_decompress(&cinfo);
	read data...
	jpeg_finish_decompress(&cinfo);

In some cases, you may want to read a file without knowing whether it contains
an image or just tables.  In that case, pass FALSE and check the return value
from jpeg_read_header(): it will be JPEG_HEADER_OK if an image was found,
JPEG_HEADER_TABLES_ONLY if only tables were found.  (A third return value,
JPEG_SUSPENDED, is possible when using a suspending data source manager.)
Note that jpeg_read_header() will not complain if you read an abbreviated
image for which you haven't loaded the missing tables; the missing-table check
occurs later, in jpeg_start_decompress().


It is possible to read a series of images from a single source file by
repeating the jpeg_read_header() ... jpeg_finish_decompress() sequence,
without releasing/recreating the JPEG object or the data source module.
(If you did reinitialize, any partial bufferload left in the data source
buffer at the end of one image would be discarded, causing you to lose the
start of the next image.)  When you use this method, stored tables are
automatically carried forward, so some of the images can be abbreviated images
that depend on tables from earlier images.

If you intend to write a series of images into a single destination file,
you might want to make a specialized data destination module that doesn't
flush the output buffer at term_destination() time.  This would speed things
up by some trifling amount.  Of course, you'd need to remember to flush the
buffer after the last image.  You can make the later images be abbreviated
ones by passing FALSE to jpeg_start_compress().


## Special markers

Some applications may need to insert or extract special data in the JPEG
datastream.  The JPEG standard provides marker types "COM" (comment) and
"APP0" through "APP15" (application) to hold application-specific data.
Unfortunately, the use of these markers is not specified by the standard.
COM markers are fairly widely used to hold user-supplied text.  The JFIF file
format spec uses APP0 markers with specified initial strings to hold certain
data.  Adobe applications use APP14 markers beginning with the string "Adobe"
for miscellaneous data.  Other APPn markers are rarely seen, but might
contain almost anything.

If you wish to store user-supplied text, we recommend you use COM markers
and place readable 7-bit ASCII text in them.  Newline conventions are not
standardized --- expect to find LF (Unix style), CR/LF (DOS style), or CR
(Mac style).  A robust COM reader should be able to cope with random binary
garbage, including nulls, since some applications generate COM markers
containing non-ASCII junk.  (But yours should not be one of them.)

For program-supplied data, use an APPn marker, and be sure to begin it with an
identifying string so that you can tell whether the marker is actually yours.
It's probably best to avoid using APP0 or APP14 for any private markers.
(NOTE: the upcoming SPIFF standard will use APP8 markers; we recommend you
not use APP8 markers for any private purposes, either.)

Keep in mind that at most 65533 bytes can be put into one marker, but you
can have as many markers as you like.

By default, the IJG compression library will write a JFIF APP0 marker if the
selected JPEG colorspace is grayscale or YCbCr, or an Adobe APP14 marker if
the selected colorspace is RGB, CMYK, or YCCK.  You can disable this, but
we don't recommend it.  The decompression library will recognize JFIF and
Adobe markers and will set the JPEG colorspace properly when one is found.


You can write special markers immediately following the datastream header by
calling jpeg_write_marker() after jpeg_start_compress() and before the first
call to jpeg_write_scanlines().  When you do this, the markers appear after
the SOI and the JFIF APP0 and Adobe APP14 markers (if written), but before
all else.  Specify the marker type parameter as "JPEG_COM" for COM or
"JPEG_APP0 + n" for APPn.  (Actually, jpeg_write_marker will let you write
any marker type, but we don't recommend writing any other kinds of marker.)
For example, to write a user comment string pointed to by comment_text:
	jpeg_write_marker(cinfo, JPEG_COM, comment_text, strlen(comment_text));

If it's not convenient to store all the marker data in memory at once,
you can instead call jpeg_write_m_header() followed by multiple calls to
jpeg_write_m_byte().  If you do it this way, it's your responsibility to
call jpeg_write_m_byte() exactly the number of times given in the length
parameter to jpeg_write_m_header().  (This method lets you empty the
output buffer partway through a marker, which might be important when
using a suspending data destination module.  In any case, if you are using
a suspending destination, you should flush its buffer after inserting
any special markers.  See "I/O suspension".)

Or, if you prefer to synthesize the marker byte sequence yourself,
you can just cram it straight into the data destination module.

If you are writing JFIF 1.02 extension markers (thumbnail images), don't
forget to set cinfo.JFIF_minor_version = 2 so that the encoder will write the
correct JFIF version number in the JFIF header marker.  The library's default
is to write version 1.01, but that's wrong if you insert any 1.02 extension
markers.  (We could probably get away with just defaulting to 1.02, but there
used to be broken decoders that would complain about unknown minor version
numbers.  To reduce compatibility risks it's safest not to write 1.02 unless
you are actually using 1.02 extensions.)


When reading, two methods of handling special markers are available:
1. You can ask the library to save the contents of COM and/or APPn markers
into memory, and then examine them at your leisure afterwards.
2. You can supply your own routine to process COM and/or APPn markers
on-the-fly as they are read.
The first method is simpler to use, especially if you are using a suspending
data source; writing a marker processor that copes with input suspension is
not easy (consider what happens if the marker is longer than your available
input buffer).  However, the second method conserves memory since the marker
data need not be kept around after it's been processed.

For either method, you'd normally set up marker handling after creating a
decompression object and before calling jpeg_read_header(), because the
markers of interest will typically be near the head of the file and so will
be scanned by jpeg_read_header.  Once you've established a marker handling
method, it will be used for the life of that decompression object
(potentially many datastreams), unless you change it.  Marker handling is
determined separately for COM markers and for each APPn marker code.


To save the contents of special markers in memory, call
	jpeg_save_markers(cinfo, marker_code, length_limit)
where marker_code is the marker type to save, JPEG_COM or JPEG_APP0+n.
(To arrange to save all the special marker types, you need to call this
routine 17 times, for COM and APP0-APP15.)  If the incoming marker is longer
than length_limit data bytes, only length_limit bytes will be saved; this
parameter allows you to avoid chewing up memory when you only need to see the
first few bytes of a potentially large marker.  If you want to save all the
data, set length_limit to 0xFFFF; that is enough since marker lengths are only
16 bits.  As a special case, setting length_limit to 0 prevents that marker
type from being saved at all.  (That is the default behavior, in fact.)

After jpeg_read_header() completes, you can examine the special markers by
following the cinfo->marker_list pointer chain.  All the special markers in
the file appear in this list, in order of their occurrence in the file (but
omitting any markers of types you didn't ask for).  Both the original data
length and the saved data length are recorded for each list entry; the latter
will not exceed length_limit for the particular marker type.  Note that these
lengths exclude the marker length word, whereas the stored representation
within the JPEG file includes it.  (Hence the maximum data length is really
only 65533.)

It is possible that additional special markers appear in the file beyond the
SOS marker at which jpeg_read_header stops; if so, the marker list will be
extended during reading of the rest of the file.  This is not expected to be
common, however.  If you are short on memory you may want to reset the length
limit to zero for all marker types after finishing jpeg_read_header, to
ensure that the max_memory_to_use setting cannot be exceeded due to addition
of later markers.

The marker list remains stored until you call jpeg_finish_decompress or
jpeg_abort, at which point the memory is freed and the list is set to empty.
(jpeg_destroy also releases the storage, of course.)

Note that the library is internally interested in APP0 and APP14 markers;
if you try to set a small nonzero length limit on these types, the library
will silently force the length up to the minimum it wants.  (But you can set
a zero length limit to prevent them from being saved at all.)  Also, in a
16-bit environment, the maximum length limit may be constrained to less than
65533 by malloc() limitations.  It is therefore best not to assume that the
effective length limit is exactly what you set it to be.


If you want to supply your own marker-reading routine, you do it by calling
jpeg_set_marker_processor().  A marker processor routine must have the
signature
	boolean jpeg_marker_parser_method (j_decompress_ptr cinfo)
Although the marker code is not explicitly passed, the routine can find it
in cinfo->unread_marker.  At the time of call, the marker proper has been
read from the data source module.  The processor routine is responsible for
reading the marker length word and the remaining parameter bytes, if any.
Return TRUE to indicate success.  (FALSE should be returned only if you are
using a suspending data source and it tells you to suspend.  See the standard
marker processors in jdmarker.c for appropriate coding methods if you need to
use a suspending data source.)

If you override the default APP0 or APP14 processors, it is up to you to
recognize JFIF and Adobe markers if you want colorspace recognition to occur
properly.  We recommend copying and extending the default processors if you
want to do that.  (A better idea is to save these marker types for later
examination by calling jpeg_save_markers(); that method doesn't interfere
with the library's own processing of these markers.)

jpeg_set_marker_processor() and jpeg_save_markers() are mutually exclusive
--- if you call one it overrides any previous call to the other, for the
particular marker type specified.

A simple example of an external COM processor can be found in djpeg.c.
Also, see jpegtran.c for an example of using jpeg_save_markers.


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


## Really raw data: DCT coefficients

It is possible to read or write the contents of a JPEG file as raw DCT
coefficients.  This facility is mainly intended for use in lossless
transcoding between different JPEG file formats.  Other possible applications
include lossless cropping of a JPEG image, lossless reassembly of a
multi-strip or multi-tile TIFF/JPEG file into a single JPEG datastream, etc.

To read the contents of a JPEG file as DCT coefficients, open the file and do
jpeg_read_header() as usual.  But instead of calling jpeg_start_decompress()
and jpeg_read_scanlines(), call jpeg_read_coefficients().  This will read the
entire image into a set of virtual coefficient-block arrays, one array per
component.  The return value is a pointer to an array of virtual-array
descriptors.  Each virtual array can be accessed directly using the JPEG
memory manager's access_virt_barray method (see Memory management, below,
and also read structure.txt's discussion of virtual array handling).  Or,
for simple transcoding to a different JPEG file format, the array list can
just be handed directly to jpeg_write_coefficients().

Each block in the block arrays contains quantized coefficient values in
normal array order (not JPEG zigzag order).  The block arrays contain only
DCT blocks containing real data; any entirely-dummy blocks added to fill out
interleaved MCUs at the right or bottom edges of the image are discarded
during reading and are not stored in the block arrays.  (The size of each
block array can be determined from the width_in_blocks and height_in_blocks
fields of the component's comp_info entry.)  This is also the data format
expected by jpeg_write_coefficients().

When you are done using the virtual arrays, call jpeg_finish_decompress()
to release the array storage and return the decompression object to an idle
state; or just call jpeg_destroy() if you don't need to reuse the object.

If you use a suspending data source, jpeg_read_coefficients() will return
NULL if it is forced to suspend; a non-NULL return value indicates successful
completion.  You need not test for a NULL return value when using a
non-suspending data source.

It is also possible to call jpeg_read_coefficients() to obtain access to the
decoder's coefficient arrays during a normal decode cycle in buffered-image
mode.  This frammish might be useful for progressively displaying an incoming
image and then re-encoding it without loss.  To do this, decode in buffered-
image mode as discussed previously, then call jpeg_read_coefficients() after
the last jpeg_finish_output() call.  The arrays will be available for your use
until you call jpeg_finish_decompress().


To write the contents of a JPEG file as DCT coefficients, you must provide
the DCT coefficients stored in virtual block arrays.  You can either pass
block arrays read from an input JPEG file by jpeg_read_coefficients(), or
allocate virtual arrays from the JPEG compression object and fill them
yourself.  In either case, jpeg_write_coefficients() is substituted for
jpeg_start_compress() and jpeg_write_scanlines().  Thus the sequence is
  * Create compression object
  * Set all compression parameters as necessary
  * Request virtual arrays if needed
  * jpeg_write_coefficients()
  * jpeg_finish_compress()
  * Destroy or re-use compression object
jpeg_write_coefficients() is passed a pointer to an array of virtual block
array descriptors; the number of arrays is equal to cinfo.num_components.

The virtual arrays need only have been requested, not realized, before
jpeg_write_coefficients() is called.  A side-effect of
jpeg_write_coefficients() is to realize any virtual arrays that have been
requested from the compression object's memory manager.  Thus, when obtaining
the virtual arrays from the compression object, you should fill the arrays
after calling jpeg_write_coefficients().  The data is actually written out
when you call jpeg_finish_compress(); jpeg_write_coefficients() only writes
the file header.

When writing raw DCT coefficients, it is crucial that the JPEG quantization
tables and sampling factors match the way the data was encoded, or the
resulting file will be invalid.  For transcoding from an existing JPEG file,
we recommend using jpeg_copy_critical_parameters().  This routine initializes
all the compression parameters to default values (like jpeg_set_defaults()),
then copies the critical information from a source decompression object.
The decompression object should have just been used to read the entire
JPEG input file --- that is, it should be awaiting jpeg_finish_decompress().

jpeg_write_coefficients() marks all tables stored in the compression object
as needing to be written to the output file (thus, it acts like
jpeg_start_compress(cinfo, TRUE)).  This is for safety's sake, to avoid
emitting abbreviated JPEG files by accident.  If you really want to emit an
abbreviated JPEG file, call jpeg_suppress_tables(), or set the tables'
individual sent_table flags, between calling jpeg_write_coefficients() and
jpeg_finish_compress().


## Progress monitoring

Some applications may need to regain control from the JPEG library every so
often.  The typical use of this feature is to produce a percent-done bar or
other progress display.  (For a simple example, see cjpeg.c or djpeg.c.)
Although you do get control back frequently during the data-transferring pass
(the jpeg_read_scanlines or jpeg_write_scanlines loop), any additional passes
will occur inside jpeg_finish_compress or jpeg_start_decompress; those
routines may take a long time to execute, and you don't get control back
until they are done.

You can define a progress-monitor routine which will be called periodically
by the library.  No guarantees are made about how often this call will occur,
so we don't recommend you use it for mouse tracking or anything like that.
At present, a call will occur once per MCU row, scanline, or sample row
group, whichever unit is convenient for the current processing mode; so the
wider the image, the longer the time between calls.  During the data
transferring pass, only one call occurs per call of jpeg_read_scanlines or
jpeg_write_scanlines, so don't pass a large number of scanlines at once if
you want fine resolution in the progress count.  (If you really need to use
the callback mechanism for time-critical tasks like mouse tracking, you could
insert additional calls inside some of the library's inner loops.)

To establish a progress-monitor callback, create a struct jpeg_progress_mgr,
fill in its progress_monitor field with a pointer to your callback routine,
and set cinfo->progress to point to the struct.  The callback will be called
whenever cinfo->progress is non-NULL.  (This pointer is set to NULL by
jpeg_create_compress or jpeg_create_decompress; the library will not change
it thereafter.  So if you allocate dynamic storage for the progress struct,
make sure it will live as long as the JPEG object does.  Allocating from the
JPEG memory manager with lifetime JPOOL_PERMANENT will work nicely.)  You
can use the same callback routine for both compression and decompression.

The jpeg_progress_mgr struct contains four fields which are set by the library:
	long pass_counter;	/* work units completed in this pass */
	long pass_limit;	/* total number of work units in this pass */
	int completed_passes;	/* passes completed so far */
	int total_passes;	/* total number of passes expected */
During any one pass, pass_counter increases from 0 up to (not including)
pass_limit; the step size is usually but not necessarily 1.  The pass_limit
value may change from one pass to another.  The expected total number of
passes is in total_passes, and the number of passes already completed is in
completed_passes.  Thus the fraction of work completed may be estimated as
		completed_passes + (pass_counter/pass_limit)
		--------------------------------------------
				total_passes
ignoring the fact that the passes may not be equal amounts of work.

When decompressing, pass_limit can even change within a pass, because it
depends on the number of scans in the JPEG file, which isn't always known in
advance.  The computed fraction-of-work-done may jump suddenly (if the library
discovers it has overestimated the number of scans) or even decrease (in the
opposite case).  It is not wise to put great faith in the work estimate.

When using the decompressor's buffered-image mode, the progress monitor work
estimate is likely to be completely unhelpful, because the library has no way
to know how many output passes will be demanded of it.  Currently, the library
sets total_passes based on the assumption that there will be one more output
pass if the input file end hasn't yet been read (jpeg_input_complete() isn't
TRUE), but no more output passes if the file end has been reached when the
output pass is started.  This means that total_passes will rise as additional
output passes are requested.  If you have a way of determining the input file
size, estimating progress based on the fraction of the file that's been read
will probably be more useful than using the library's value.


## Memory management

This section covers some key facts about the JPEG library's built-in memory
manager.  For more info, please read structure.txt's section about the memory
manager, and consult the source code if necessary.

All memory and temporary file allocation within the library is done via the
memory manager.  If necessary, you can replace the "back end" of the memory
manager to control allocation yourself (for example, if you don't want the
library to use malloc() and free() for some reason).

Some data is allocated "permanently" and will not be freed until the JPEG
object is destroyed.  Most data is allocated "per image" and is freed by
jpeg_finish_compress, jpeg_finish_decompress, or jpeg_abort.  You can call the
memory manager yourself to allocate structures that will automatically be
freed at these times.  Typical code for this is
  ptr = (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE, size);
Use JPOOL_PERMANENT to get storage that lasts as long as the JPEG object.
Use alloc_large instead of alloc_small for anything bigger than a few Kbytes.
There are also alloc_sarray and alloc_barray routines that automatically
build 2-D sample or block arrays.

The library's minimum space requirements to process an image depend on the
image's width, but not on its height, because the library ordinarily works
with "strip" buffers that are as wide as the image but just a few rows high.
Some operating modes (eg, two-pass color quantization) require full-image
buffers.  Such buffers are treated as "virtual arrays": only the current strip
need be in memory, and the rest can be swapped out to a temporary file.

If you use the simplest memory manager back end (jmemnobs.c), then no
temporary files are used; virtual arrays are simply malloc()'d.  Images bigger
than memory can be processed only if your system supports virtual memory.
The other memory manager back ends support temporary files of various flavors
and thus work in machines without virtual memory.  They may also be useful on
Unix machines if you need to process images that exceed available swap space.

When using temporary files, the library will make the in-memory buffers for
its virtual arrays just big enough to stay within a "maximum memory" setting.
Your application can set this limit by setting cinfo->mem->max_memory_to_use
after creating the JPEG object.  (Of course, there is still a minimum size for
the buffers, so the max-memory setting is effective only if it is bigger than
the minimum space needed.)  If you allocate any large structures yourself, you
must allocate them before jpeg_start_compress() or jpeg_start_decompress() in
order to have them counted against the max memory limit.  Also keep in mind
that space allocated with alloc_small() is ignored, on the assumption that
it's too small to be worth worrying about; so a reasonable safety margin
should be left when setting max_memory_to_use.

If you use the jmemname.c or jmemdos.c memory manager back end, it is
important to clean up the JPEG object properly to ensure that the temporary
files get deleted.  (This is especially crucial with jmemdos.c, where the
"temporary files" may be extended-memory segments; if they are not freed,
DOS will require a reboot to recover the memory.)  Thus, with these memory
managers, it's a good idea to provide a signal handler that will trap any
early exit from your program.  The handler should call either jpeg_abort()
or jpeg_destroy() for any active JPEG objects.  A handler is not needed with
jmemnobs.c, and shouldn't be necessary with jmemansi.c or jmemmac.c either,
since the C library is supposed to take care of deleting files made with
tmpfile().


## Memory usage

Working memory requirements while performing compression or decompression
depend on image dimensions, image characteristics (such as colorspace and
JPEG process), and operating mode (application-selected options).

As of v6b, the decompressor requires:
 1. About 24K in more-or-less-fixed-size data.  This varies a bit depending
    on operating mode and image characteristics (particularly color vs.
    grayscale), but it doesn't depend on image dimensions.
 2. Strip buffers (of size proportional to the image width) for IDCT and
    upsampling results.  The worst case for commonly used sampling factors
    is about 34 bytes * width in pixels for a color image.  A grayscale image
    only needs about 8 bytes per pixel column.
 3. A full-image DCT coefficient buffer is needed to decode a multi-scan JPEG
    file (including progressive JPEGs), or whenever you select buffered-image
    mode.  This takes 2 bytes/coefficient.  At typical 2x2 sampling, that's
    3 bytes per pixel for a color image.  Worst case (1x1 sampling) requires
    6 bytes/pixel.  For grayscale, figure 2 bytes/pixel.
 4. To perform 2-pass color quantization, the decompressor also needs a
    128K color lookup table and a full-image pixel buffer (3 bytes/pixel).
This does not count any memory allocated by the application, such as a
buffer to hold the final output image.

The above figures are valid for 8-bit JPEG data precision and a machine with
32-bit ints.  For 9-bit to 12-bit JPEG data, double the size of the strip
buffers and quantization pixel buffer.  The "fixed-size" data will be
somewhat smaller with 16-bit ints, larger with 64-bit ints.  Also, CMYK
or other unusual color spaces will require different amounts of space.

The full-image coefficient and pixel buffers, if needed at all, do not
have to be fully RAM resident; you can have the library use temporary
files instead when the total memory usage would exceed a limit you set.
(But if your OS supports virtual memory, it's probably better to just use
jmemnobs and let the OS do the swapping.)

The compressor's memory requirements are similar, except that it has no need
for color quantization.  Also, it needs a full-image DCT coefficient buffer
if Huffman-table optimization is asked for, even if progressive mode is not
requested.

If you need more detailed information about memory usage in a particular
situation, you can enable the MEM_STATS code in jmemmgr.c.


## Library compile-time options

A number of compile-time options are available by modifying jmorecfg.h.

The IJG code currently supports 8-bit to 12-bit sample data precision by
defining BITS_IN_JSAMPLE as 8, 9, 10, 11, or 12.
Note that a value larger than 8 causes JSAMPLE to be larger than a char,
so it affects the surrounding application's image data.
The sample applications cjpeg and djpeg can support deeper than 8-bit data
only for PPM and GIF file formats; you must disable the other file formats
to compile a 9-bit to 12-bit cjpeg or djpeg.  (install.txt has more
information about that.)
Run-time selection and conversion of data precision are currently not
supported and may be added later.
Exception:  The transcoding part (jpegtran) supports all settings in a
single instance, since it operates on the level of DCT coefficients and
not sample values.
(If you need to include an 8-bit library and a 9-bit to 12-bit library for
compression or decompression in a single application, you could probably do
it by defining NEED_SHORT_EXTERNAL_NAMES for just one of the copies.  You'd
have to access the 8-bit and the 9-bit to 12-bit copies from separate
application source files.  This is untested ... if you try it, we'd like to
hear whether it works!)

Note that the standard Huffman tables are only valid for 8-bit data precision.
If you selected more than 8-bit data precision, cjpeg uses arithmetic coding
by default.  The Huffman encoder normally uses entropy optimization to
compute usable tables for higher precision.  Otherwise, you'll have to
supply different default Huffman tables.  You may also want to supply your
own DCT quantization tables; the existing quality-scaling code has been
developed for 8-bit use, and probably doesn't generate especially good tables
for 9-bit to 12-bit.

The maximum number of components (color channels) in the image is determined
by MAX_COMPONENTS.  The JPEG standard allows up to 255 components, but we
expect that few applications will need more than four or so.

On machines with unusual data type sizes, you may be able to improve
performance or reduce memory space by tweaking the various typedefs in
jmorecfg.h.  In particular, on some RISC CPUs, access to arrays of "short"s
is quite slow; consider trading memory for speed by making JCOEF, INT16, and
UINT16 be "int" or "unsigned int".  UINT8 is also a candidate to become int.
You probably don't want to make JSAMPLE be int unless you have lots of memory
to burn.

You can reduce the size of the library by compiling out various optional
functions.  To do this, undefine xxx_SUPPORTED symbols as necessary.

You can also save a few K by not having text error messages in the library;
the standard error message table occupies about 5Kb.  This is particularly
reasonable for embedded applications where there's no good way to display 
a message anyway.  To do this, remove the creation of the message table
(jpeg_std_message_table[]) from jerror.c, and alter format_message to do
something reasonable without it.  You could output the numeric value of the
message code number, for example.  If you do this, you can also save a couple
more K by modifying the TRACEMSn() macros in jerror.h to expand to nothing;
you don't need trace capability anyway, right?


## Portability considerations

The JPEG library has been written to be extremely portable; the sample
applications cjpeg and djpeg are slightly less so.  This section summarizes
the design goals in this area.  (If you encounter any bugs that cause the
library to be less portable than is claimed here, we'd appreciate hearing
about them.)

The code works fine on ANSI C, C++, and pre-ANSI C compilers, using any of
the popular system include file setups, and some not-so-popular ones too.
See install.txt for configuration procedures.

The code is not dependent on the exact sizes of the C data types.  As
distributed, we make the assumptions that
	char	is at least 8 bits wide
	short	is at least 16 bits wide
	int	is at least 16 bits wide
	long	is at least 32 bits wide
(These are the minimum requirements of the ANSI C standard.)  Wider types will
work fine, although memory may be used inefficiently if char is much larger
than 8 bits or short is much bigger than 16 bits.  The code should work
equally well with 16- or 32-bit ints.

In a system where these assumptions are not met, you may be able to make the
code work by modifying the typedefs in jmorecfg.h.  However, you will probably
have difficulty if int is less than 16 bits wide, since references to plain
int abound in the code.

char can be either signed or unsigned, although the code runs faster if an
unsigned char type is available.  If char is wider than 8 bits, you will need
to redefine JOCTET and/or provide custom data source/destination managers so
that JOCTET represents exactly 8 bits of data on external storage.

The JPEG library proper does not assume ASCII representation of characters.
But some of the image file I/O modules in cjpeg/djpeg do have ASCII
dependencies in file-header manipulation; so does cjpeg's select_file_type()
routine.

The JPEG library does not rely heavily on the C library.  In particular, C
stdio is used only by the data source/destination modules and the error
handler, all of which are application-replaceable.  (cjpeg/djpeg are more
heavily dependent on stdio.)  malloc and free are called only from the memory
manager "back end" module, so you can use a different memory allocator by
replacing that one file.

The code generally assumes that C names must be unique in the first 15
characters.  However, global function names can be made unique in the
first 6 characters by defining NEED_SHORT_EXTERNAL_NAMES.

More info about porting the code may be gleaned by reading jconfig.txt,
jmorecfg.h, and jinclude.h.


## Notes for MS-DOS implementors

The IJG code is designed to work efficiently in 80x86 "small" or "medium"
memory models (i.e., data pointers are 16 bits unless explicitly declared
"far"; code pointers can be either size).  You may be able to use small
model to compile cjpeg or djpeg by itself, but you will probably have to use
medium model for any larger application.  This won't make much difference in
performance.  You *will* take a noticeable performance hit if you use a
large-data memory model (perhaps 10%-25%), and you should avoid "huge" model
if at all possible.

The JPEG library typically needs 2Kb-3Kb of stack space.  It will also
malloc about 20K-30K of near heap space while executing (and lots of far
heap, but that doesn't count in this calculation).  This figure will vary
depending on selected operating mode, and to a lesser extent on image size.
There is also about 5Kb-6Kb of constant data which will be allocated in the
near data segment (about 4Kb of this is the error message table).
Thus you have perhaps 20K available for other modules' static data and near
heap space before you need to go to a larger memory model.  The C library's
static data will account for several K of this, but that still leaves a good
deal for your needs.  (If you are tight on space, you could reduce the sizes
of the I/O buffers allocated by jdatasrc.c and jdatadst.c, say from 4K to
1K.  Another possibility is to move the error message table to far memory;
this should be doable with only localized hacking on jerror.c.)

About 2K of the near heap space is "permanent" memory that will not be
released until you destroy the JPEG object.  This is only an issue if you
save a JPEG object between compression or decompression operations.

Far data space may also be a tight resource when you are dealing with large
images.  The most memory-intensive case is decompression with two-pass color
quantization, or single-pass quantization to an externally supplied color
map.  This requires a 128Kb color lookup table plus strip buffers amounting
to about 40 bytes per column for typical sampling ratios (eg, about 25600
bytes for a 640-pixel-wide image).  You may not be able to process wide
images if you have large data structures of your own.

Of course, all of these concerns vanish if you use a 32-bit flat-memory-model
compiler, such as DJGPP or Watcom C.  We highly recommend flat model if you
can use it; the JPEG library is significantly faster in flat model.
