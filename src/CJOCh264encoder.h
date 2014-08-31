/*
 * CJOCh264encoder.h
 *
 *  Created on: Aug 17, 2014
 *      Author: Jordi Cenzano (www.jordicenzano.name)
 */

#ifndef CJOCH264ENCODER_H_
#define CJOCH264ENCODER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CJOCh264bitstream.h"

//! h264 encoder class
/*!
 It is used to create the h264 compliant stream
 */
class CJOCh264encoder : CJOCh264bitstream
{
public:

	/**
	 * Allowed sample formats
	 */
	enum enSampleFormat
	{
		SAMPLE_FORMAT_YUV420p//!< SAMPLE_FORMAT_YUV420p
	};

private:
	/*!Set the used Y macroblock size for I PCM in YUV420p */
	#define MACROBLOCK_Y_WIDTH	16
	#define MACROBLOCK_Y_HEIGHT	16

	/*!Set time base in Hz */
	#define TIME_SCALE_IN_HZ 	27000000

	/*!Pointer to pixels */
	typedef struct
	{
		unsigned char *pYCbCr;
	}YUV420p_frame_t;

	/*! Frame  */
	typedef struct
	{
		enSampleFormat sampleformat; 	/*!< Sample format */
		unsigned int nYwidth; 			/*!< Y (luminance) block width in pixels */
		unsigned int nYheight;			/*!< Y (luminance) block height in pixels */
		unsigned int nCwidth;			/*!< C (Crominance) block width in pixels */
		unsigned int nCheight;			/*!< C (Crominance) block height in pixels */

		unsigned int nYmbwidth;			/*!< Y (luminance) macroblock width in pixels */
		unsigned int nYmbheight;		/*!< Y (luminance) macroblock height in pixels */
		unsigned int nCmbwidth;			/*!< Y (Crominance) macroblock width in pixels */
		unsigned int nCmbheight;		/*!< Y (Crominance) macroblock height in pixels */

		YUV420p_frame_t yuv420pframe;	/*!< Pointer to current frame data */
		unsigned int nyuv420pframesize;	/*!< Size in bytes of yuv420pframe */
	}frame_t;

	/*! The frame var*/
	frame_t m_frame;

	/*! The frames per second var*/
	int m_nFps;

	/*! Number of frames sent to the output */
	unsigned long m_lNumFramesAdded;

	//! Frees the frame yuv420pframe allocated memory
	void free_video_src_frame ();

	//! Allocs the frame yuv420pframe memory according to the frame properties
	void alloc_video_src_frame ();

	//! Creates SPS NAL and add it to the output
	/*!
		\param nImW Frame width in pixels
		\param nImH Frame height in pixels
		\param nMbW macroblock width in pixels
		\param nMbH macroblock height in pixels
		\param nFps frames x second (tipical values are: 25, 30, 50, etc)
		\param nSARw Indicates the horizontal size of the sample aspect ratio (tipical values are:1, 4, 16, etc)
		\param nSARh Indicates the vertical size of the sample aspect ratio (tipical values are:1, 3, 9, etc)
	 */
	void create_sps (int nImW, int nImH, int nMbW, int nMbH, int nFps, int nSARw, int nSARh);

	//! Creates PPS NAL and add it to the output
	void create_pps ();

	//! Creates Slice NAL and add it to the output
	/*!
		\param lFrameNum number of frame
	 */
	void create_slice_header(unsigned long lFrameNum);

	//! Creates macroblock header and add it to the output
	void create_macroblock_header ();

	//! Creates the slice footer and add it to the output
	void create_slice_footer();

	//! Creates SPS NAL and add it to the output
	/*!
		\param nYpos First vertical macroblock pixel inside the frame
		\param nYpos nXpos horizontal macroblock pixel inside the frame
	 */
	void create_macroblock(unsigned int nYpos, unsigned int nXpos);

public:
	//! Constructor
	/*!
		 \param pOutFile The output file pointer
	 */
	CJOCh264encoder(FILE *pOutFile);

	//! Destructor
	virtual ~CJOCh264encoder();

	//! Initializes the coder
	/*!
		\param nImW Frame width in pixels
		\param nImH Frame height in pixels
		\param nFps Desired frames per second of the output file (typical values are: 25, 30, 50, etc)
		\param SampleFormat Sample format if the input file. In this implementation only SAMPLE_FORMAT_YUV420p is allowed
		\param nSARw Indicates the horizontal size of the sample aspect ratio (typical values are:1, 4, 16, etc)
		\param nSARh Indicates the vertical size of the sample aspect ratio (typical values are:1, 3, 9, etc)
	*/
	void IniCoder (int nImW, int nImH, int nImFps, CJOCh264encoder::enSampleFormat SampleFormat, int nSARw = 1, int nSARh = 1);

	//! Returns the frame pointer
	/*!
		\return Frame pointer ready to fill with frame pixels data (the format to fill the data is indicated by SampleFormat parameter when the coder is initialized
	*/
	void* GetFramePtr();

	//! Returns the allocated frame memory in bytes
	/*!
		\return The allocated memory to store the frame data
	*/
	unsigned int GetFrameSize();

	//! It codes the frame that is in frame memory a it saves the coded data to disc
	void CodeAndSaveFrame();

	//! Returns number of coded frames
	/*!
		\return The number of coded frames
	*/
	unsigned long GetSavedFrames();

	//! Flush all data and save the trailing bits
	void CloseCoder ();
};

#endif /* CJOCH264ENCODER_H_ */
