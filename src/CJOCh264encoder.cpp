/*
 * CJOCh264encoder.cpp
 *
 *  Created on: Aug 17, 2014
 *      Author: Jordi Cenzano (www.jordicenzano.name)
 */

#include "CJOCh264encoder.h"

//Private functions

//Contructor
CJOCh264encoder::CJOCh264encoder(FILE *pOutFile):CJOCh264bitstream(pOutFile)
{
	m_lNumFramesAdded = 0;

	memset (&m_frame, 0, sizeof (frame_t));
	m_nFps = 25;
}

//Destructor
CJOCh264encoder::~CJOCh264encoder()
{
	free_video_src_frame ();
}

//Free the allocated video frame mem
void CJOCh264encoder::free_video_src_frame ()
{
	if (m_frame.yuv420pframe.pYCbCr != NULL)
		free (m_frame.yuv420pframe.pYCbCr);

	memset (&m_frame, 0, sizeof (frame_t));
}

//Alloc mem to store a video frame
void CJOCh264encoder::alloc_video_src_frame ()
{
	if (m_frame.yuv420pframe.pYCbCr != NULL)
		throw "Error: null values in frame";

	int nYsize = m_frame.nYwidth * m_frame.nYheight;
	int nCsize = m_frame.nCwidth * m_frame.nCheight;
	m_frame.nyuv420pframesize = nYsize + nCsize + nCsize;

	m_frame.yuv420pframe.pYCbCr = (unsigned char*) malloc (sizeof (unsigned char) * m_frame.nyuv420pframesize);

	if (m_frame.yuv420pframe.pYCbCr == NULL)
		throw "Error: memory alloc";
}

//Creates and saves the NAL SPS (including VUI) (one per file)
void CJOCh264encoder::create_sps (int nImW, int nImH, int nMbW, int nMbH, int nFps, int nSARw, int nSARh)
{
	add4bytesnoemulationprevention (0x000001); // NAL header
	addbits (0x0,1); // forbidden_bit
	addbits (0x3,2); // nal_ref_idc
	addbits (0x7,5); // nal_unit_type : 7 ( SPS )
	addbits (0x42,8); // profile_idc = baseline ( 0x42 )
	addbits (0x0,1); // constraint_set0_flag
	addbits (0x0,1); // constraint_set1_flag
	addbits (0x0,1); // constraint_set2_flag
	addbits (0x0,1); // constraint_set3_flag
	addbits (0x0,1); // constraint_set4_flag
	addbits (0x0,1); // constraint_set5_flag
	addbits (0x0,2); // reserved_zero_2bits /* equal to 0 */
	addbits (0x0a,8); // level_idc: 3.1 (0x0a)
	addexpgolombunsigned(0); // seq_parameter_set_id
	addexpgolombunsigned(0); // log2_max_frame_num_minus4
	addexpgolombunsigned(0); // pic_order_cnt_type
	addexpgolombunsigned(0); // log2_max_pic_order_cnt_lsb_minus4
	addexpgolombunsigned(0); // max_num_refs_frames
	addbits(0x0,1); // gaps_in_frame_num_value_allowed_flag

	int nWinMbs = nImW / nMbW;
	addexpgolombunsigned(nWinMbs-1); // pic_width_in_mbs_minus_1
	int nHinMbs = nImH / nMbH;
	addexpgolombunsigned(nHinMbs-1); // pic_height_in_map_units_minus_1

	addbits(0x1,1); // frame_mbs_only_flag
	addbits(0x0,1); // direct_8x8_interfernce
	addbits(0x0,1); // frame_cropping_flag
	addbits(0x1,1); // vui_parameter_present

	//VUI parameters (AR, timming)
	addbits(0x1,1); //aspect_ratio_info_present_flag
	addbits(0xFF,8); //aspect_ratio_idc = Extended_SAR

	//AR
	addbits(nSARw, 16); //sar_width
	addbits(nSARh, 16); //sar_height

	addbits(0x0,1); //overscan_info_present_flag
	addbits(0x0,1); //video_signal_type_present_flag
	addbits(0x0,1); //chroma_loc_info_present_flag
	addbits(0x1,1); //timing_info_present_flag

	unsigned int nnum_units_in_tick = TIME_SCALE_IN_HZ / (2*nFps);
	addbits(nnum_units_in_tick,32); //num_units_in_tick
	addbits(TIME_SCALE_IN_HZ,32); //time_scale
	addbits(0x1,1);  //fixed_frame_rate_flag

	addbits(0x0,1);  //nal_hrd_parameters_present_flag
	addbits(0x0,1);  //vcl_hrd_parameters_present_flag
	addbits(0x0,1);  //pic_struct_present_flag
	addbits(0x0,1);  //bitstream_restriction_flag
	//END VUI

	addbits(0x1,1); // rbsp stop bit

	dobytealign();
}

//Creates and saves the NAL PPS (one per file)
void CJOCh264encoder::create_pps ()
{
	add4bytesnoemulationprevention (0x000001); // NAL header
	addbits (0x0,1); // forbidden_bit
	addbits (0x3,2); // nal_ref_idc
	addbits (0x8,5); // nal_unit_type : 8 ( PPS )
	addexpgolombunsigned(0); // pic_parameter_set_id
	addexpgolombunsigned(0); // seq_parameter_set_id
	addbits (0x0,1); // entropy_coding_mode_flag
	addbits (0x0,1); // bottom_field_pic_order_in frame_present_flag
	addexpgolombunsigned(0); // nun_slices_groups_minus1
	addexpgolombunsigned(0); // num_ref_idx10_default_active_minus
	addexpgolombunsigned(0); // num_ref_idx11_default_active_minus
	addbits (0x0,1); // weighted_pred_flag
	addbits (0x0,2); // weighted_bipred_idc
	addexpgolombsigned(0); // pic_init_qp_minus26
	addexpgolombsigned(0); // pic_init_qs_minus26
	addexpgolombsigned(0); // chroma_qp_index_offset
	addbits (0x0,1); //deblocking_filter_present_flag
	addbits (0x0,1); // constrained_intra_pred_flag
	addbits (0x0,1); //redundant_pic_ent_present_flag
	addbits(0x1,1); // rbsp stop bit

	dobytealign();
}

//Creates and saves the NAL SLICE (one per frame)
void CJOCh264encoder::create_slice_header(unsigned long lFrameNum)
{
	add4bytesnoemulationprevention (0x000001); // NAL header
	addbits (0x0,1); // forbidden_bit
	addbits (0x3,2); // nal_ref_idc
	addbits (0x5,5); // nal_unit_type : 5 ( Coded slice of an IDR picture  )
	addexpgolombunsigned(0); // first_mb_in_slice
	addexpgolombunsigned(7); // slice_type
	addexpgolombunsigned(0); // pic_param_set_id

	unsigned char cFrameNum = 0; // FrameNum=0 for IDR frames [otherwise lFrameNum % 16; //(2⁴)]
	addbits (cFrameNum,4); // frame_num ( numbits = v = log2_max_frame_num_minus4 + 4)

	unsigned long lidr_pic_id = lFrameNum % 512; // could be % 65536 as valid range 0..65536
	//idr_pic_flag = 1
	addexpgolombunsigned(lidr_pic_id); // idr_pic_id
	addbits(0x0,4); // pic_order_cnt_lsb (numbits = v = log2_max_fpic_order_cnt_lsb_minus4 + 4)
	addbits(0x0,1); //no_output_of_prior_pics_flag
	addbits(0x0,1); //long_term_reference_flag
	addexpgolombsigned(0); //slice_qp_delta

	//Probably NOT byte aligned!!!
}

//Creates and saves the SLICE footer (one per SLICE)
void CJOCh264encoder::create_slice_footer()
{
	addbits(0x1,1); // rbsp stop bit
}

//Creates and saves the macroblock header(one per macroblock)
void CJOCh264encoder::create_macroblock_header ()
{
	addexpgolombunsigned(25); // mb_type (I_PCM)
}

//Creates & saves a macroblock (coded INTRA 16x16)
void CJOCh264encoder::create_macroblock(unsigned int nYpos, unsigned int nXpos)
{
	unsigned int x,y;

	create_macroblock_header();

	dobytealign();

	//Y
	unsigned int nYsize = m_frame.nYwidth * m_frame.nYheight;
	for(y = nYpos * m_frame.nYmbheight; y < (nYpos+1) * m_frame.nYmbheight; y++)
	{
		for (x = nXpos * m_frame.nYmbwidth; x < (nXpos+1) * m_frame.nYmbwidth; x++)
		{
			addbyte (m_frame.yuv420pframe.pYCbCr[(y * m_frame.nYwidth +  x)]);
		}
	}

	//Cb
	unsigned int nCsize = m_frame.nCwidth * m_frame.nCheight;
	for(y = nYpos * m_frame.nCmbheight; y < (nYpos+1) * m_frame.nCmbheight; y++)
	{
		for (x = nXpos * m_frame.nCmbwidth; x < (nXpos+1) * m_frame.nCmbwidth; x++)
		{
			addbyte(m_frame.yuv420pframe.pYCbCr[nYsize + (y * m_frame.nCwidth +  x)]);
		}
	}

	//Cr
	for(y = nYpos * m_frame.nCmbheight; y < (nYpos+1) * m_frame.nCmbheight; y++)
	{
		for (x = nXpos * m_frame.nCmbwidth; x < (nXpos+1) * m_frame.nCmbwidth; x++)
		{
			addbyte(m_frame.yuv420pframe.pYCbCr[nYsize + nCsize + (y * m_frame.nCwidth +  x)]);
		}
	}
}


//public functions

//Initilizes the h264 coder (mini-coder)
void CJOCh264encoder::IniCoder (int nImW, int nImH, int nImFps, CJOCh264encoder::enSampleFormat SampleFormat, int nSARw, int nSARh)
{
	m_lNumFramesAdded = 0;

	if (SampleFormat != SAMPLE_FORMAT_YUV420p)
		throw "Error: SAMPLE FORMAT not allowed. Only yuv420p is allowed in this version";

	free_video_src_frame ();

	//Ini vars
	m_frame.sampleformat = SampleFormat;
	m_frame.nYwidth = nImW;
	m_frame.nYheight = nImH;
	if (SampleFormat == SAMPLE_FORMAT_YUV420p)
	{
		//Set macroblock Y size
		m_frame.nYmbwidth = MACROBLOCK_Y_WIDTH;
		m_frame.nYmbheight = MACROBLOCK_Y_HEIGHT;

		//Set macroblock C size (in YUV420 is 1/2 of Y)
		m_frame.nCmbwidth = MACROBLOCK_Y_WIDTH/2;
		m_frame.nCmbheight = MACROBLOCK_Y_HEIGHT/2;

		//Set C size
		m_frame.nCwidth = m_frame.nYwidth / 2;
		m_frame.nCheight = m_frame.nYheight / 2;

		//In this implementation only picture sizes multiples of macroblock size (16x16) are allowed
		if (((nImW % MACROBLOCK_Y_WIDTH) != 0)||((nImH % MACROBLOCK_Y_HEIGHT) != 0))
			throw "Error: size not allowed. Only multiples of macroblock are allowed (macroblock size is: 16x16)";
	}
	m_nFps = nImFps;

	//Alloc mem for 1 frame
	alloc_video_src_frame ();

	//Create h264 SPS & PPS
	create_sps (m_frame.nYwidth , m_frame.nYheight, m_frame.nYmbwidth, m_frame.nYmbheight, nImFps , nSARw, nSARh);
	create_pps ();
}

//Returns the frame pointer to load the video frame
void* CJOCh264encoder::GetFramePtr()
{
	if (m_frame.yuv420pframe.pYCbCr == NULL)
		throw "Error: video frame is null (not initialized)";

	return (void*) m_frame.yuv420pframe.pYCbCr;
}

//Returns the the allocated size for video frame
unsigned int CJOCh264encoder::GetFrameSize()
{
	return m_frame.nyuv420pframesize;
}

//Codifies & save the video frame (it only uses 16x16 intra PCM -> NO COMPRESSION!)
void CJOCh264encoder::CodeAndSaveFrame()
{
	//The slice header is not byte aligned, so the first macroblock header is not byte aligned
	create_slice_header (m_lNumFramesAdded);

	//Loop over macroblock size
	unsigned int y,x;
	for (y = 0; y < m_frame.nYheight / m_frame.nYmbheight; y++)
	{
		for (x = 0; x < m_frame.nYwidth / m_frame.nYmbwidth; x++)
		{
			create_macroblock(y, x);
		}
	}

	create_slice_footer();
	dobytealign();

	m_lNumFramesAdded++;
}

//Returns the number of codified frames
unsigned long CJOCh264encoder::GetSavedFrames()
{
	return m_lNumFramesAdded;
}

//Closes the h264 coder saving the last bits in the buffer
void CJOCh264encoder::CloseCoder ()
{
	close();
}
