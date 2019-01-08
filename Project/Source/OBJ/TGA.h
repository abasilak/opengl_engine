//----------------------------------------------------//
//                                                    //
// File: TGA.h                                        //
// TGA class. Loads TGA files                         //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//
#ifndef TGA_H
#define TGA_H

#pragma once
//using namespace

// includes ////////////////////////////////////////


// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class TGA
{
public:
	struct TEXTURE_INFO
	{
		unsigned int						dimensions;
		unsigned int						size;
	};

	unsigned char						m_identsize;          // size of ID field that follows 18 byte header (0 usually)
	unsigned char  						m_colourmaptype;      // type of colour map 0=none, 1=has palette
	unsigned char  						m_imagetype;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

	unsigned short 						m_colourmapstart;     // first colour map entry in palette
	unsigned short 						m_colourmaplength;    // number of colours in palette
	unsigned char  						m_colourmapbits;      // number of bits per palette entry 15,16,24,32

	unsigned short 						m_xstart;             // image x origin
	unsigned short 						m_ystart;             // image y origin
	unsigned short 						m_width;              // image width in pixels
	unsigned short 						m_height;             // image height in pixels
	unsigned char  						m_bits;               // image bits per pixel 8,16,24,32
	unsigned char  						m_descriptor;         // image descriptor bits (vh flip bits)
	unsigned char*						m_palette;
	TEXTURE_INFO						m_texture_info;

public:
	// Constructor
	TGA(void);

	// Destructor
	~TGA(void);

	// public function declarations
	bool								Load(std::string& filename, unsigned char** texture_data, std::string& error_msg);
	bool								TGAError(std::string& error_msg, const char* msg, FILE* file);
};

#endif //TGA_H

// eof ///////////////////////////////// class TGA