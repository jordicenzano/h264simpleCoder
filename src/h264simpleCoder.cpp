//============================================================================
// Name        : h264simpleCoder.cpp
// Author      : Jordi Cenzano (www.jordicenzano.name)
// Version     : 1.0
// Copyright   : Copyright Jordi Cenzano 2014
// Description : Simple h264 encoder
//============================================================================

#include <iostream>
#include "CJOCh264encoder.h"

using namespace std;

int main(int argc, char **argv)
{
	int nRc = 1;

	puts("Simple h264 coder by Jordi Cenzano (www.jordicenzano.name)");
	puts("This is NOT a video compressor, only uses I_PCM macroblocks (intra without compression)");
	puts("It is made only for learning purposes");
	puts("**********************************************************");

	if (argc < 3)
	{
		puts("------------------------------------------------------------------------");
		puts("Usage: h264mincoder input.yuv output.h264 [image width] [image height] [fps] [AR SARw] [AR SARh]");
		puts("Default parameters: Image width=128 Image height=96 Fps=25 SARw=1 SARh=1");
		puts("Assumptions: Input file is yuv420p");
		puts("------------------------------------------------------------------------");

		return nRc;
	}

	char szInputFile[512];
	char szOutputFile[512];
	int nImWidth = 128;
	int nImHeight = 96;
	int nFps = 25;
	int nSARw = 1;
	int nSARh = 1;

	//Get input file
	strncpy (szInputFile,argv[1],512);

	//Get output file
	strncpy (szOutputFile,argv[2],512);

	//Get image width
	if (argc > 3)
	{
		nImWidth = (int) atol (argv[3]);
		if (nImWidth == 0)
			puts ("Error reading image width input parameter");
	}

	//Get image height
	if (argc > 4)
	{
		nImHeight = (int) atol (argv[4]);
		if (nImHeight == 0)
			puts ("Error reading image height input parameter");
	}

	//Get fps
	if (argc > 5)
	{
		nFps = (int) atol (argv[5]);
		if (nFps == 0)
			puts ("Error reading fps input parameter");
	}

	//Get SARw
	if (argc > 6)
	{
		nSARw = (int) atol (argv[6]);
		if (nSARw == 0)
			puts ("Error reading AR SARw input parameter");
	}

	//Get SARh
	if (argc > 7)
	{
		nSARh = (int) atol (argv[7]);
		if (nSARh == 0)
			puts ("Error reading AR SARh input parameter");
	}

	FILE *pfsrc = NULL;
	FILE *pfdst = NULL;

	pfsrc = fopen (szInputFile,"rb");
	if (pfsrc == NULL)
	{
		puts ("Error opening source file");
		return nRc;
	}

	pfdst = fopen (szOutputFile,"wb");
	if (pfdst == NULL)
	{
		puts ("Error opening destination file");
		return nRc;
	}

	try
	{
		//Instantiate the h264 coder
		CJOCh264encoder *ph264encoder = new CJOCh264encoder(pfdst);

		//Initialize the h264 coder with frame parameters
		ph264encoder->IniCoder(nImWidth,nImHeight,nFps,CJOCh264encoder::SAMPLE_FORMAT_YUV420p, nSARw, nSARh);

		int nSavedFrames = 0;
		char szLog[256];

		//Iterate through all frames
		while (! feof(pfsrc))
		{
			//Get frame pointer to fill
			void *pFrame = ph264encoder->GetFramePtr ();

			//Get the size allocated in pFrame
			unsigned int nFrameSize = ph264encoder->GetFrameSize();

			//Load frame from disk and load it into pFrame
			size_t nreaded = fread (pFrame,1, nFrameSize, pfsrc);
			if (nreaded != nFrameSize)
			{
				if (! feof(pfsrc))
					throw "Error: Reading frame";
			}
			else
			{
				//Encode & save frame
				ph264encoder->CodeAndSaveFrame();

				//Get the number of saved frames
				nSavedFrames = ph264encoder->GetSavedFrames();

				//Show the number of saved / encoded frames iin console
				sprintf(szLog,"Saved frame num: %d", nSavedFrames - 1);
				puts (szLog);
			}
		}

		//Close encoder
		ph264encoder->CloseCoder();

		//Set return code to 0
		nRc = 0;
	}
	catch (const char *szErrorDesc)
	{
		//Show the error description on console
		puts (szErrorDesc);
	}

	//Close previously opened files
	if (pfsrc != NULL)
		fclose (pfsrc);

	if (pfdst != NULL)
		fclose (pfdst);

	return nRc;
}
