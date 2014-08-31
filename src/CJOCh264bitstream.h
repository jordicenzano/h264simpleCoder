/*
 * CJOCh264bitstream.h
 *
 *  Created on: Aug 23, 2014
 *      Author: Jordi Cenzano (www.jordicenzano.name)
 */

#ifndef CJOCH264BITSTREAM_H_
#define CJOCH264BITSTREAM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//! h264 bitstream class
/*!
 It is used to create the h264 bit oriented stream, it contains different functions that helps you to create the h264 compliant stream (bit oriented, exp golomb coder)
 */
class CJOCh264bitstream
{
#define BUFFER_SIZE_BITS 24			/*! Buffer size in bits used for emulation prevention */
#define BUFFER_SIZE_BYTES (24/8)	/*! Buffer size in bytes used for emulation prevention */

#define H264_EMULATION_PREVENTION_BYTE 0x03		/*! Emulation prevention byte */

private:

	/*! Buffer  */
	unsigned char m_buffer[BUFFER_SIZE_BITS];

	/*! Bit buffer index  */
	unsigned int m_nLastbitinbuffer;

	/*! Starting byte indicator  */
	unsigned int m_nStartingbyte;

	/*! Pointer to output file */
	FILE *m_pOutFile;

	//! Clears the buffer
	void clearbuffer();

	//! Returns the nNumbit value (1 or 0) of lval
	/*!
	 	 \param lval number to extract the nNumbit value
	 	 \param nNumbit Bit position that we want to know if its 1 or 0 (from 0 to 63)
	 	 \return bit value (1 or 0)
	 */
	static int getbitnum (unsigned long lval, int nNumbit);

	//! Adds 1 bit to the end of h264 bitstream
	/*!
		 \param nVal bit to add at the end of h264 bitstream
	 */
	void addbittostream (int nVal);

	//! Adds 8 bit to the end of h264 bitstream (it is optimized for byte aligned situations)
	/*!
		 \param nVal byte to add at the end of h264 bitstream (from 0 to 255)
	 */
	void addbytetostream (int nVal);

	//! Save all buffer to file
	/*!
		 \param bemulationprevention Indicates if it will insert the emulation prevention byte or not (when it is needed)
	 */
	void savebufferbyte(bool bemulationprevention = true);

public:
	//! Constructor
	/*!
		 \param pOutBinaryFile The output file pointer
	 */
	CJOCh264bitstream(FILE *pOutBinaryFile);

	//! Destructor
	virtual ~CJOCh264bitstream();

	//! Add 4 bytes to h264 bistream without taking into acount the emulation prevention. Used to add the NAL header to the h264 bistream
	/*!
		 \param nVal The 32b value to add
		 \param bDoAlign Indicates if the function will insert 0 in order to create a byte aligned stream before adding nVal 4 bytes to stream. If you try to call this function and the stream is not byte aligned an exception will be thrown
	 */
	void add4bytesnoemulationprevention (unsigned int nVal, bool bDoAlign = false);

	//! Adds nNumbits of lval to the end of h264 bitstream
	/*!
		 \param nVal value to add at the end of the h264 stream (only the LAST nNumbits will be added)
		 \param nNumbits number of bits of lval that will be added to h264 stream (counting from left)
	 */
	void addbits (unsigned long lval, int nNumbits);

	//! Adds lval to the end of h264 bitstream using exp golomb coding for unsigned values
	/*!
		 \param nVal value to add at the end of the h264 stream
	 */
	void addexpgolombunsigned (unsigned long lval);

	//! Adds lval to the end of h264 bitstream using exp golomb coding for signed values
	/*!
		 \param nVal value to add at the end of the h264 stream
	 */
	void addexpgolombsigned (long lval);

	//! Adds 0 to the end of h264 bistream in order to leave a byte aligned stream (It will insert seven 0 maximum)
	void dobytealign();

	//! Adds cByte (8 bits) to the end of h264 bitstream. This function it is optimized in byte aligned streams.
	/*!
		 \param cByte value to add at the end of the h264 stream (from 0 to 255)
	 */
	void addbyte (unsigned char cByte);

	//! Close the h264 stream saving to disk the last remaing bits in buffer
	void close();
};

#endif /* CJOCH264BITSTREAM_H_ */
