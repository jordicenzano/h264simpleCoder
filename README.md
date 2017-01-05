h264simpleCoder
===============

With the aim to improve my understanding of h264 bitstream and its techniques I have created the following code in C++ that generates a compliant h264 file from a YUV420p file.

First of all I have to say that the following source code is NOT a h264 encoder but it generates a compliant (playable) h264 stream, this means that the size of output file will be slightly bigger than the size of the input file. This it is because I have used only I_PCM macroblock codification (non compressed format) to describe the images in h264 stream.

I think that to read and understand this code could be a good starting point to start flirting with the h264 standard, better than dive into the standard (more than 700 pages of dense text).

Usage example:

> h264simpleCoder AVsyncTest.yuv OutTest.h264 128 96 25 16 9

More information on: http://jordicenzano.name/2014/08/31/the-source-code-of-a-minimal-h264-encoder-c/


Generating a YUV file
A 10 second yuv file (SQCIF 128x96) with a test image can be made with this ffmpeg command
> ffmpeg -f lavfi -i testsrc -t 10  -s sqcif -pix_fmt yuv420p AVsyncTest.yuv

