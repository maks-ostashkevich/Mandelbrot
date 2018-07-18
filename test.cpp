#include <fstream>

// IntToBMP.cpp : Defines the entry point for the console application.
//

// #include "stdafx.h"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <memory>

#include <cmath>
#include <algorithm>

#pragma pack( push, 1 ) 
struct BMP
{
	BMP();
	struct
	{
		uint16_t ID;
		uint32_t fileSizeInBytes;
		uint16_t reserved1;
		uint16_t reserved2;
		uint32_t pixelArrayOffsetInBytes;
	} FileHeader;

	enum class CompressionMethod : uint32_t {
		BI_RGB = 0x00,
		BI_RLE8 = 0x01,
		BI_RLE4 = 0x02,
		BI_BITFIELDS = 0x03,
		BI_JPEG = 0x04,
		BI_PNG = 0x05,
		BI_ALPHABITFIELDS = 0x06
	};

	struct
	{
		uint32_t headerSizeInBytes;
		uint32_t bitmapWidthInPixels;
		uint32_t bitmapHeightInPixels;
		uint16_t colorPlaneCount;
		uint16_t bitsPerPixel;
		CompressionMethod compressionMethod;
		uint32_t bitmapSizeInBytes;
		int32_t horizontalResolutionInPixelsPerMeter;
		int32_t verticalResolutionInPixelsPerMeter;
		uint32_t paletteColorCount;
		uint32_t importantColorCount;
	} DIBHeader;
};
#pragma pack( pop )

BMP::BMP()
{
	//Initialized fields
	FileHeader.ID = 0x4d42; // == 'BM' (little-endian)
	FileHeader.reserved1 = 0;
	FileHeader.reserved2 = 0;
	FileHeader.pixelArrayOffsetInBytes = sizeof(FileHeader) + sizeof(DIBHeader);
	
	// DIBHeader.bitmapHeightInPixels = 1024; // don't work
	// DIBHeader.bitmapWidthInPixels = 1024; // don't work

	DIBHeader.headerSizeInBytes = 40;
	DIBHeader.colorPlaneCount = 1; //  3; // 1;
	DIBHeader.bitsPerPixel = 24; // 32; // issue is not here
	DIBHeader.compressionMethod = CompressionMethod::BI_RGB;
	DIBHeader.horizontalResolutionInPixelsPerMeter = 2835; // 1024; // 2835; // == 72 ppi // issue is not here
	DIBHeader.verticalResolutionInPixelsPerMeter = 2835; // 1024; // 2835; // == 72 ppi // issue is not here
	DIBHeader.paletteColorCount = 0;
	DIBHeader.importantColorCount = 0;
}

void Exit(void)
{
	std::cout << "Press a key to exit...";
	std::getchar();

	exit(0);
}

struct Complex
{
	double x, y;
	Complex(double x, double y) : x(x), y(y)
	{

	}
};

double absComplex(Complex number)
{
	return sqrt(number.x * number.x + number.y * number.y);
}

Complex square(Complex number)
{
	return Complex(number.x * number.x - number.y * number.y, 2 * number.x * number.y); // sqrt(number.x * number.x + number.y * number.y);
}

Complex sumComplex(Complex left, Complex right)
{
	return Complex(left.x + right.x, left.y + right.y);
}

int main(int argc, char* argv[])  //Replace with int main( int argc, char* argv[] ) if you're not under Visual Studio
{
	//Assumption: 32-bit signed integers
	//Assumption: Distribution of values range from INT32_MIN through INT32_MAX, inclusive
	//Assumption: number of integers contained in file are unknown
	//Assumption: source file of integers is a series of space-delimitied strings representing integers
	//Assumption: source file contents are valid
	//Assumption: non-rectangular numbers of integers yield non-rectangular bitmaps (final scanline may be short)
	//            This may cause some .bmp parsers to fail; others may pad with 0's.  For simplicity, this implementation
	//            attempts to render square bitmaps.

	const std::string integerFilename = "integers.txt";
	const std::string bitmapFilename = "bitmap.bmp";

	// If quantity of integers being read is known, reserve or resize vector or use array

	// CHANGES COLOR DEPENDING ON THE NUMBER!
	std::vector< uint8_t >integers(3 * 1024 * 1024); // correspondence needed // DOES NOT work this way for some reason
	
	for (int i = 0; i < 1024; i++)
	{
		for (int j = 0; j < 1024; j++) // 3 * 
		{
			Complex z = Complex(0, 0);
			Complex c = Complex(-2 + 4./1024 * j, 1 - 2./1024 * i);
			for (int k = 0; k < 1000; k++)
			{
				z = sumComplex(square(z), c);
				if (absComplex(z) > 2)
				{
					integers[i * 3 * 1024 + 3 * j] = 255; // 1;
					integers[i * 3 * 1024 + 3 * j + 1] = 255; // 1;
					integers[i * 3 * 1024 + 3 * j + 2] = 255; // 1;
					break;
				}
			}
			
			// std::cout << i << " " << j << std::endl;
			// std::cout << absComplex(z) << std::endl;

			if (absComplex(z) > 2)
				continue;
			integers[i * 3 * 1024 + 3 * j] = 0;
			integers[i * 3 * 1024 + 3 * j + 1] = 0;
			integers[i * 3 * 1024 + 3 * j + 2] = 0;
		}
	}

	// Construct .bmp
	std::cout << "Constructing .BMP...\n";
	BMP bmp;
	bmp.DIBHeader.bitmapSizeInBytes = 3 * 1024 * 1024;
	bmp.FileHeader.fileSizeInBytes = bmp.FileHeader.pixelArrayOffsetInBytes + bmp.DIBHeader.bitmapSizeInBytes;
	bmp.DIBHeader.bitmapWidthInPixels = static_cast<uint32_t>(1024);
	bmp.DIBHeader.bitmapHeightInPixels = static_cast<uint32_t>(1024);

	//Write integers to .bmp file
	std::cout << "Writing .BMP...\n";
	{
		std::ofstream writeFile(bitmapFilename, std::ofstream::binary);

		if (!writeFile)
		{
			std::cout << "Error writing " << bitmapFilename << ".\n";
			Exit();
		}

		writeFile.write(reinterpret_cast< char * >(&bmp), sizeof(bmp));
		writeFile.write(reinterpret_cast< char * >(&integers[0]), bmp.DIBHeader.bitmapSizeInBytes);
	}

	//Exit
	Exit();
}