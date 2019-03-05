# 使用IJG JPEG LIBRARY
----------------------------
&ensp;&ensp;&ensp;&ensp;该文件描述了如何在应用程序中使用IJG JPEG库程序。 如果要编写使用该库的程序，请阅读它。

&ensp;&ensp;&ensp;&ensp;文件example.c提供了大量注释的框架代码，用于调用JPEG库。 另请参阅jpeglib.h（应用程序要使用的包含文件）程序）有关数据结构和函数参数列表的完整详细信息。当然，库源代码是最终的参考。

&ensp;&ensp;&ensp;&ensp;请注意，目前应用程序接口已发生**重大**更改，区别于IJG第4版和早期版本提供。旧版本存在一些固有的缺陷, 在我们不断添加新特性的时候我们尽量最小化接口的改动，因此积累了很多坏的设计；在重写第五版的时候我们牺牲了向后兼容性，但是我们认为这是值得的；

## **目录表**
-----------------

### 总览：
* 库提供的功能
* 典型用法
### 基本用法:
* 数据形式
* 压缩细节
* 解压细节
* 编译方法: 需包含的头文件, 链接, 等等
### 高级特性:
* 压缩参数选项
* 解压缩参数选项
* 特殊的色彩空间
* 错误处理
* 压缩数据处理（源和目标管理器）
* I/O 挂起停止
* 渐进式JPEG支持
* 缓冲图像模式
* 多图像和缩小的数据流
* 特殊的标记
* Raw (downsampled) image data
* 真正的raw数据: DCT 系数
* 进度监测
* 内存管理
* 内存使用
* lib编译选项
* 可移植性考虑
* MS-DOS实现的注意事项

在尝试用库编程之前，您应该至少阅读概述和基本用法部分。高级功能的部分可以你需要它们的时候阅读。


## **总览**
------------

### **库提供的功能**


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


### **典型用法概述**

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


## **基本的库使用方法**
----------

### **数据形式**

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


### **压缩操作细节**
------

这里我们再次回顾一下JPEG压缩操作的概述。

#### 分配并初始化一个JPEG compression对象。
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


#### 为压缩后的数据指定输出目标(比如，一个文件)

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


#### 压缩设置参数，包括图像大小和色彩空间

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

#### **调用jpeg_start_compress(...);**

&ensp;&ensp;&ensp;&ensp;在你已经确定了data destination以后并且设置好了所有的必须参数（包括图像大小等等）后，调用`jpeg_start_compress()` 开始进入压缩循环。开始初始化内部工作状态，分配工作存储空间，触发生成JPEG数据流的头信息。

典型代码：
```C
	jpeg_start_compress(&cinfo, TRUE);
```
参数`TRUE`确保了一个完整的JPEG交换数据流会被写入。对于大多数情况是这个参数是适用的，如果你想使用缩略JPEG数据流，请参考下面的章节：abbreviated datastreams

一旦调用了`jpeg_start_compress()`，你就不能改变JPEG参数或者JPEG对象的其他字段了，直到压缩循环停止。


#### **while (scan lines remain to be written) jpeg_write_scanlines(...);**

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


#### **jpeg_finish_compress(...);**

&ensp;&ensp;&ensp;&ensp;在所有的图像数据被写入到缓冲区后，调用`jpeg_finish_compress()`来完成压缩循环。这一步对于确保最后一个缓冲区数据写入数据目标至关重要。`jpeg_finish_compress()`还释放与JPEG对象相关的内存。

典型代码：
```c
	jpeg_finish_compress(&cinfo);
```
&ensp;&ensp;&ensp;&ensp;如果你使用stdio目标管理器，不要忘了在这一步完成以后关闭stdio输出流。

&ensp;&ensp;&ensp;&ensp;如果您已请求multi-pass操作模式，例如霍夫曼代码优化，`jpeg_finish_compress()`将执行额外的数据传递，传递第一次传入的缓冲的数据。 在这种情况下，`jpeg_finish_compress()`可能会占用很长一段时间才能完成。如果使用默认压缩参数，这将不会发生。

不要试图在尚未完全写入扫描行的情况下调用`jpeg_finish_compress()`，这样做将产生错误，如果你想中断压缩，调用`jpeg_abort()` ，下面我们将会介绍如何使用它。

在完成一次压缩以后，你可能想保留JPEG对象常驻在内存中，或者你想使用这个对象压缩另一个图像。在这种情况下，你将要正确返回回到第2，3，4步。如果你不改变目标管理器，则新的数据流会被写入到同样的目标文件中。如果你不改变压缩参数，则新的压缩数据流将使用之前的压缩参数。注意一下，你可以自由改变图像的几何尺寸在压缩循环间歇期。但是一旦你重新设置了色彩空间的话，则应该再次调用`jpeg_set_defaults()`来使压缩参数适应新的色彩空间。然后你再开始前面的第三步：调用`jpeg_start_compress(...);`


#### **释放JPEG压缩对象**

&ensp;&ensp;&ensp;&ensp;完成JPEG压缩后，通过调用jpeg_destroy_compress()将其销毁。 这将释放所有辅助内存（无论对象的先前状态如何）。 或者你可以调用jpeg_destroy（），它适用于压缩或解压缩对象---如果你在压缩和解压缩的情况下共享代码，这可能会更方便。 （实际上，除了传递已声明的指针类型之外，这些例程是等效的。为了避免来自ANSI C编译器的抱怨，应该给jpeg_destroy()传递j_common_ptr类型。）

如果你是通过`malloc()`分配的`jpeg_compress_struct structure` ，则释放这个对象是你的责任，`jpeg_destroy()`不会释放这个结构对象，错误处理对象也是这样。

典型代码
```c
	jpeg_destroy_compress(&cinfo);
```

#### **中断**

&ensp;&ensp;&ensp;&ensp;如果你决定中断一个压缩循环，你可以用如下2种方式做收尾清理工作：

* 如果您不再需要JPEG对象，只需调用`jpeg_destroy_compress()`或`jpeg_destroy()`释放内存即可。在调用`jpeg_create_compress()`之后的任何时候调用这两个函数都是合法的，事实上，即使`jpeg_create_compress()`失败也很安全。

* 如果要复用JPEG对象，请调用`jpeg_abort_compress()`或调用适用于压缩和解压缩对象的`jpeg_abort()`。这将使对象返回空闲状态，释放任何工作内存。成功创建JPEG对象后，可以随时使用`jpeg_abort()`。

&ensp;&ensp;&ensp;&ensp;请注意：清理数据目标输出，是你的责任；这些例程都不会调用`term destination()`。（有关详细信息，请参阅下面的“Compressed data handling”。）

&ensp;&ensp;&ensp;&ensp;`jpeg_destroy()`和`jpeg_abort()` 是仅有的2种安全方式处理去处理报告了错误的JPEG对象，JPEG对象调用error_exit退出。(请参考更多关于"Error handling"的信息)。JPEG对象的内部状态这个时候是不正常状态，`jpeg_destroy()`和`jpeg_abort()` 会把JPEG对象恢复到一种已知状态。
