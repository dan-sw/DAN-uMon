//------------------------------------------------------------------------
// sed_lut.h: Lookup Table Values
//

uchar sed_lut[256][3] = {
//    RED,  GREEN, BLUE		// Entry
	{ 0x00,  0x00, 0x00, },	// 00  
	{ 0x00,  0x00, 0xA0, },	// 01 
	{ 0x00,  0xA0, 0x00, },	// 02 
	{ 0x00,  0xA0, 0xA0, },	// 03 
	{ 0xA0,  0x00, 0x00, },	// 04 
	{ 0xA0,  0x00, 0xA0, },	// 05 
	{ 0xA0,  0xA0, 0x00, },	// 06 
	{ 0xA0,  0xA0, 0xA0, },	// 07 
	{ 0x50,  0x50, 0x50, },	// 08 
	{ 0x50,  0x50, 0xF0, },	// 09 
	{ 0x50,  0xF0, 0x50, },	// 0A 
	{ 0x50,  0xF0, 0xF0, },	// 0B 
	{ 0xF0,  0x50, 0x50, },	// 0C 
	{ 0xF0,  0x50, 0xF0, },	// 0D 
	{ 0xF0,  0xF0, 0x50, },	// 0E 
	{ 0xF0,  0xF0, 0xF0, },	// 0F 
	{ 0x00,  0x00, 0x00, },	// 10 
	{ 0x10,  0x10, 0x10, },	// 11 
	{ 0x20,  0x20, 0x20, },	// 12 
	{ 0x20,  0x20, 0x20, },	// 13 
	{ 0x30,  0x30, 0x30, },	// 14 
	{ 0x40,  0x40, 0x40, },	// 15 
	{ 0x50,  0x50, 0x50, },	// 16 
	{ 0x60,  0x60, 0x60, },	// 17 
	{ 0x70,  0x70, 0x70, },	// 18 
	{ 0x80,  0x80, 0x80, },	// 19 
	{ 0x90,  0x90, 0x90, },	// 1A 
	{ 0xA0,  0xA0, 0xA0, },	// 1B 
	{ 0xB0,  0xB0, 0xB0, },	// 1C 
	{ 0xC0,  0xC0, 0xC0, },	// 1D 
	{ 0xE0,  0xE0, 0xE0, },	// 1E 
	{ 0xF0,  0xF0, 0xF0, },	// 1F 
	{ 0x00,  0x00, 0xF0, },	// 20 
	{ 0x40,  0x00, 0xF0, },	// 21 
	{ 0x70,  0x00, 0xF0, },	// 22 
	{ 0xB0,  0x00, 0xF0, },	// 23 
	{ 0xF0,  0x00, 0xF0, },	// 24 
	{ 0xF0,  0x00, 0xB0, },	// 25 
	{ 0xF0,  0x00, 0x70, },	// 26 
	{ 0xF0,  0x00, 0x40, },	// 27 
	{ 0xF0,  0x00, 0x00, },	// 28 
	{ 0xF0,  0x40, 0x00, },	// 29 
	{ 0xF0,  0x70, 0x00, },	// 2A 
	{ 0xF0,  0xB0, 0x00, },	// 2B 
	{ 0xF0,  0xF0, 0x00, },	// 2C 
	{ 0xB0,  0xF0, 0x00, },	// 2D 
	{ 0x70,  0xF0, 0x00, },	// 2E 
	{ 0x40,  0xF0, 0x00, },	// 2F 
	{ 0x00,  0xF0, 0x00, },	// 30 
	{ 0x00,  0xF0, 0x40, },	// 31 
	{ 0x00,  0xF0, 0x70, },	// 32 
	{ 0x00,  0xF0, 0xB0, },	// 33 
	{ 0x00,  0xF0, 0xF0, },	// 34 
	{ 0x00,  0xB0, 0xF0, },	// 35 
	{ 0x00,  0x70, 0xF0, },	// 36 
	{ 0x00,  0x40, 0xF0, },	// 37 
	{ 0x70,  0x70, 0xF0, },	// 38 
	{ 0x90,  0x70, 0xF0, },	// 39 
	{ 0xB0,  0x70, 0xF0, },	// 3A 
	{ 0xD0,  0x70, 0xF0, },	// 3B 
	{ 0xF0,  0x70, 0xF0, },	// 3C 
	{ 0xF0,  0x70, 0xD0, },	// 3D 
	{ 0xF0,  0x70, 0xB0, },	// 3E 
	{ 0xF0,  0x70, 0x90, },	// 3F 
	{ 0xF0,  0x70, 0x70, },	// 40 
	{ 0xF0,  0x90, 0x70, },	// 41 
	{ 0xF0,  0xB0, 0x70, },	// 42 
	{ 0xF0,  0xD0, 0x70, },	// 43 
	{ 0xF0,  0xF0, 0x70, },	// 44 
	{ 0xD0,  0xF0, 0x70, },	// 45 
	{ 0xB0,  0xF0, 0x70, },	// 46 
	{ 0x90,  0xF0, 0x70, },	// 47 
	{ 0x70,  0xF0, 0x70, },	// 48 
	{ 0x70,  0xF0, 0x90, },	// 49 
	{ 0x70,  0xF0, 0xB0, },	// 4A 
	{ 0x70,  0xF0, 0xD0, },	// 4B 
	{ 0x70,  0xF0, 0xF0, },	// 4C 
	{ 0x70,  0xD0, 0xF0, },	// 4D 
	{ 0x70,  0xB0, 0xF0, },	// 4E 
	{ 0x70,  0x90, 0xF0, },	// 4F 
	{ 0xB0,  0xB0, 0xF0, },	// 50 
	{ 0xC0,  0xB0, 0xF0, },	// 51 
	{ 0xD0,  0xB0, 0xF0, },	// 52 
	{ 0xE0,  0xB0, 0xF0, },	// 53 
	{ 0xF0,  0xB0, 0xF0, },	// 54 
	{ 0xF0,  0xB0, 0xE0, },	// 55 
	{ 0xF0,  0xB0, 0xD0, },	// 56 
	{ 0xF0,  0xB0, 0xC0, },	// 57 
	{ 0xF0,  0xB0, 0xB0, },	// 58 
	{ 0xF0,  0xC0, 0xB0, },	// 59 
	{ 0xF0,  0xD0, 0xB0, },	// 5A 
	{ 0xF0,  0xE0, 0xB0, },	// 5B 
	{ 0xF0,  0xF0, 0xB0, },	// 5C 
	{ 0xE0,  0xF0, 0xB0, },	// 5D 
	{ 0xD0,  0xF0, 0xB0, },	// 5E 
	{ 0xC0,  0xF0, 0xB0, },	// 5F 
	{ 0xB0,  0xF0, 0xB0, },	// 60 
	{ 0xB0,  0xF0, 0xC0, },	// 61 
	{ 0xB0,  0xF0, 0xD0, },	// 62 
	{ 0xB0,  0xF0, 0xE0, },	// 63 
	{ 0xB0,  0xF0, 0xF0, },	// 64 
	{ 0xB0,  0xE0, 0xF0, },	// 65 
	{ 0xB0,  0xD0, 0xF0, },	// 66 
	{ 0xB0,  0xC0, 0xF0, },	// 67 
	{ 0x00,  0x00, 0x70, },	// 68 
	{ 0x10,  0x00, 0x70, },	// 69 
	{ 0x30,  0x00, 0x70, },	// 6A 
	{ 0x50,  0x00, 0x70, },	// 6B 
	{ 0x70,  0x00, 0x70, },	// 6C 
	{ 0x70,  0x00, 0x50, },	// 6D 
	{ 0x70,  0x00, 0x30, },	// 6E 
	{ 0x70,  0x00, 0x10, },	// 6F 
	{ 0x70,  0x00, 0x00, },	// 70 
	{ 0x70,  0x10, 0x00, },	// 71 
	{ 0x70,  0x30, 0x00, },	// 72 
	{ 0x70,  0x50, 0x00, },	// 73 
	{ 0x70,  0x70, 0x00, },	// 74 
	{ 0x50,  0x70, 0x00, },	// 75 
	{ 0x30,  0x70, 0x00, },	// 76 
	{ 0x10,  0x70, 0x00, },	// 77 
	{ 0x00,  0x70, 0x00, },	// 78 
	{ 0x00,  0x70, 0x10, },	// 79 
	{ 0x00,  0x70, 0x30, },	// 7A 
	{ 0x00,  0x70, 0x50, },	// 7B 
	{ 0x00,  0x70, 0x70, },	// 7C 
	{ 0x00,  0x50, 0x70, },	// 7D 
	{ 0x00,  0x30, 0x70, },	// 7E 
	{ 0x00,  0x10, 0x70, },	// 7F 
	{ 0x30,  0x30, 0x70, },	// 80 
	{ 0x40,  0x30, 0x70, },	// 81 
	{ 0x50,  0x30, 0x70, },	// 82 
	{ 0x60,  0x30, 0x70, },	// 83 
	{ 0x70,  0x30, 0x70, },	// 84 
	{ 0x70,  0x30, 0x60, },	// 85 
	{ 0x70,  0x30, 0x50, },	// 86 
	{ 0x70,  0x30, 0x40, },	// 87 
	{ 0x70,  0x30, 0x30, },	// 88 
	{ 0x70,  0x40, 0x30, },	// 89 
	{ 0x70,  0x50, 0x30, },	// 8A 
	{ 0x70,  0x60, 0x30, },	// 8B 
	{ 0x70,  0x70, 0x30, },	// 8C 
	{ 0x60,  0x70, 0x30, },	// 8D 
	{ 0x50,  0x70, 0x30, },	// 8E 
	{ 0x40,  0x70, 0x30, },	// 8F 
	{ 0x30,  0x70, 0x30, },	// 90 
	{ 0x30,  0x70, 0x40, },	// 91 
	{ 0x30,  0x70, 0x50, },	// 92 
	{ 0x30,  0x70, 0x60, },	// 93 
	{ 0x30,  0x70, 0x70, },	// 94 
	{ 0x30,  0x60, 0x70, },	// 95 
	{ 0x30,  0x50, 0x70, },	// 96 
	{ 0x30,  0x40, 0x70, },	// 97 
	{ 0x50,  0x50, 0x70, },	// 98 
	{ 0x50,  0x50, 0x70, },	// 99 
	{ 0x60,  0x50, 0x70, },	// 9A 
	{ 0x60,  0x50, 0x70, },	// 9B 
	{ 0x70,  0x50, 0x70, },	// 9C 
	{ 0x70,  0x50, 0x60, },	// 9D 
	{ 0x70,  0x50, 0x60, },	// 9E 
	{ 0x70,  0x50, 0x50, },	// 9F 
	{ 0x70,  0x50, 0x50, },	// A0 
	{ 0x70,  0x50, 0x50, },	// A1 
	{ 0x70,  0x60, 0x50, },	// A2 
	{ 0x70,  0x60, 0x50, },	// A3 
	{ 0x70,  0x70, 0x50, },	// A4 
	{ 0x60,  0x70, 0x50, },	// A5 
	{ 0x60,  0x70, 0x50, },	// A6 
	{ 0x50,  0x70, 0x50, },	// A7 
	{ 0x50,  0x70, 0x50, },	// A8 
	{ 0x50,  0x70, 0x50, },	// A9 
	{ 0x50,  0x70, 0x60, },	// AA 
	{ 0x50,  0x70, 0x60, },	// AB 
	{ 0x50,  0x70, 0x70, },	// AC 
	{ 0x50,  0x60, 0x70, },	// AD 
	{ 0x50,  0x60, 0x70, },	// AE 
	{ 0x50,  0x50, 0x70, },	// AF 
	{ 0x00,  0x00, 0x40, },	// B0 
	{ 0x10,  0x00, 0x40, },	// B1 
	{ 0x20,  0x00, 0x40, },	// B2 
	{ 0x30,  0x00, 0x40, },	// B3 
	{ 0x40,  0x00, 0x40, },	// B4 
	{ 0x40,  0x00, 0x30, },	// B5 
	{ 0x40,  0x00, 0x20, },	// B6 
	{ 0x40,  0x00, 0x10, },	// B7 
	{ 0x40,  0x00, 0x00, },	// B8 
	{ 0x40,  0x10, 0x00, },	// B9 
	{ 0x40,  0x20, 0x00, },	// BA 
	{ 0x40,  0x30, 0x00, },	// BB 
	{ 0x40,  0x40, 0x00, },	// BC 
	{ 0x30,  0x40, 0x00, },	// BD 
	{ 0x20,  0x40, 0x00, },	// BE 
	{ 0x10,  0x40, 0x00, },	// BF 
	{ 0x00,  0x40, 0x00, },	// C0 
	{ 0x00,  0x40, 0x10, },	// C1 
	{ 0x00,  0x40, 0x20, },	// C2 
	{ 0x00,  0x40, 0x30, },	// C3 
	{ 0x00,  0x40, 0x40, },	// C4 
	{ 0x00,  0x30, 0x40, },	// C5 
	{ 0x00,  0x20, 0x40, },	// C6 
	{ 0x00,  0x10, 0x40, },	// C7 
	{ 0x20,  0x20, 0x40, },	// C8 
	{ 0x20,  0x20, 0x40, },	// C9 
	{ 0x30,  0x20, 0x40, },	// CA 
	{ 0x30,  0x20, 0x40, },	// CB 
	{ 0x40,  0x20, 0x40, },	// CC 
	{ 0x40,  0x20, 0x30, },	// CD 
	{ 0x40,  0x20, 0x30, },	// CE 
	{ 0x40,  0x20, 0x20, },	// CF 
	{ 0x40,  0x20, 0x20, },	// D0 
	{ 0x40,  0x20, 0x20, },	// D1 
	{ 0x40,  0x30, 0x20, },	// D2 
	{ 0x40,  0x30, 0x20, },	// D3 
	{ 0x40,  0x40, 0x20, },	// D4 
	{ 0x30,  0x40, 0x20, },	// D5 
	{ 0x30,  0x40, 0x20, },	// D6 
	{ 0x20,  0x40, 0x20, },	// D7 
	{ 0x20,  0x40, 0x20, },	// D8 
	{ 0x20,  0x40, 0x20, },	// D9 
	{ 0x20,  0x40, 0x30, },	// DA 
	{ 0x20,  0x40, 0x30, },	// DB 
	{ 0x20,  0x40, 0x40, },	// DC 
	{ 0x20,  0x30, 0x40, },	// DD 
	{ 0x20,  0x30, 0x40, },	// DE 
	{ 0x20,  0x20, 0x40, },	// DF 
	{ 0x20,  0x20, 0x40, },	// E0 
	{ 0x30,  0x20, 0x40, },	// E1 
	{ 0x30,  0x20, 0x40, },	// E2 
	{ 0x30,  0x20, 0x40, },	// E3 
	{ 0x40,  0x20, 0x40, },	// E4 
	{ 0x40,  0x20, 0x30, },	// E5 
	{ 0x40,  0x20, 0x30, },	// E6 
	{ 0x40,  0x20, 0x30, },	// E7 
	{ 0x40,  0x20, 0x20, },	// E8 
	{ 0x40,  0x30, 0x20, },	// E9 
	{ 0x40,  0x30, 0x20, },	// EA 
	{ 0x40,  0x30, 0x20, },	// EB 
	{ 0x40,  0x40, 0x20, },	// EC 
	{ 0x30,  0x40, 0x20, },	// ED 
	{ 0x30,  0x40, 0x20, },	// EE 
	{ 0x30,  0x40, 0x20, },	// EF 
	{ 0x20,  0x40, 0x20, },	// F0 
	{ 0x20,  0x40, 0x30, },	// F1 
	{ 0x20,  0x40, 0x30, },	// F2 
	{ 0x20,  0x40, 0x30, },	// F3 
	{ 0x20,  0x40, 0x40, },	// F4 
	{ 0x20,  0x30, 0x40, },	// F5 
	{ 0x20,  0x30, 0x40, },	// F6 
	{ 0x20,  0x30, 0x40, },	// F7 
	{ 0x00,  0x00, 0x00, },	// F8 
	{ 0x00,  0x00, 0x00, },	// F9 
	{ 0x00,  0x00, 0x00, },	// FA 
	{ 0x00,  0x00, 0x00, },	// FB 
	{ 0x00,  0x00, 0x00, },	// FC 
	{ 0x00,  0x00, 0x00, },	// FD 
	{ 0x00,  0x00, 0x00, },	// FE 
	{ 0x00,  0x00, 0x00, },	// FF 
};


ushort sed_lut_16bit[256][3] = {
//    RED,  GREEN, BLUE		// Entry
	{ 0x0000,  0x0000, 0x0000, },	// 00  
	{ 0x0000,  0x0000, 0x00A0, },	// 01 
	{ 0x0000,  0x00A0, 0x0000, },	// 02 
	{ 0x0000,  0x00A0, 0x00A0, },	// 03 
	{ 0x00A0,  0x0000, 0x0000, },	// 04 
	{ 0x00A0,  0x0000, 0x00A0, },	// 05 
	{ 0x00A0,  0x00A0, 0x0000, },	// 06 
	{ 0x00A0,  0x00A0, 0x00A0, },	// 07 
	{ 0x0050,  0x0050, 0x0050, },	// 08 
	{ 0x0050,  0x0050, 0x00F0, },	// 09 
	{ 0x0050,  0x00F0, 0x0050, },	// 0A 
	{ 0x0050,  0x00F0, 0x00F0, },	// 0B 
	{ 0x00F0,  0x0050, 0x0050, },	// 0C 
	{ 0x00F0,  0x0050, 0x00F0, },	// 0D 
	{ 0x00F0,  0x00F0, 0x0050, },	// 0E 
	{ 0x00F0,  0x00F0, 0x00F0, },	// 0F 
	{ 0x0000,  0x0000, 0x0000, },	// 10 
	{ 0x0010,  0x0010, 0x0010, },	// 11 
	{ 0x0020,  0x0020, 0x0020, },	// 12 
	{ 0x0020,  0x0020, 0x0020, },	// 13 
	{ 0x0030,  0x0030, 0x0030, },	// 14 
	{ 0x0040,  0x0040, 0x0040, },	// 15 
	{ 0x0050,  0x0050, 0x0050, },	// 16 
	{ 0x0060,  0x0060, 0x0060, },	// 17 
	{ 0x0070,  0x0070, 0x0070, },	// 18 
	{ 0x0080,  0x0080, 0x0080, },	// 19 
	{ 0x0090,  0x0090, 0x0090, },	// 1A 
	{ 0x00A0,  0x00A0, 0x00A0, },	// 1B 
	{ 0x00B0,  0x00B0, 0x00B0, },	// 1C 
	{ 0x00C0,  0x00C0, 0x00C0, },	// 1D 
	{ 0x00E0,  0x00E0, 0x00E0, },	// 1E 
	{ 0x00F0,  0x00F0, 0x00F0, },	// 1F 
	{ 0x0000,  0x0000, 0x00F0, },	// 20 
	{ 0x0040,  0x0000, 0x00F0, },	// 21 
	{ 0x0070,  0x0000, 0x00F0, },	// 22 
	{ 0x00B0,  0x0000, 0x00F0, },	// 23 
	{ 0x00F0,  0x0000, 0x00F0, },	// 24 
	{ 0x00F0,  0x0000, 0x00B0, },	// 25 
	{ 0x00F0,  0x0000, 0x0070, },	// 26 
	{ 0x00F0,  0x0000, 0x0040, },	// 27 
	{ 0x00F0,  0x0000, 0x0000, },	// 28 
	{ 0x00F0,  0x0040, 0x0000, },	// 29 
	{ 0x00F0,  0x0070, 0x0000, },	// 2A 
	{ 0x00F0,  0x00B0, 0x0000, },	// 2B 
	{ 0x00F0,  0x00F0, 0x0000, },	// 2C 
	{ 0x00B0,  0x00F0, 0x0000, },	// 2D 
	{ 0x0070,  0x00F0, 0x0000, },	// 2E 
	{ 0x0040,  0x00F0, 0x0000, },	// 2F 
	{ 0x0000,  0x00F0, 0x0000, },	// 30 
	{ 0x0000,  0x00F0, 0x0040, },	// 31 
	{ 0x0000,  0x00F0, 0x0070, },	// 32 
	{ 0x0000,  0x00F0, 0x00B0, },	// 33 
	{ 0x0000,  0x00F0, 0x00F0, },	// 34 
	{ 0x0000,  0x00B0, 0x00F0, },	// 35 
	{ 0x0000,  0x0070, 0x00F0, },	// 36 
	{ 0x0000,  0x0040, 0x00F0, },	// 37 
	{ 0x0070,  0x0070, 0x00F0, },	// 38 
	{ 0x0090,  0x0070, 0x00F0, },	// 39 
	{ 0x00B0,  0x0070, 0x00F0, },	// 3A 
	{ 0x00D0,  0x0070, 0x00F0, },	// 3B 
	{ 0x00F0,  0x0070, 0x00F0, },	// 3C 
	{ 0x00F0,  0x0070, 0x00D0, },	// 3D 
	{ 0x00F0,  0x0070, 0x00B0, },	// 3E 
	{ 0x00F0,  0x0070, 0x0090, },	// 3F 
	{ 0x00F0,  0x0070, 0x0070, },	// 40 
	{ 0x00F0,  0x0090, 0x0070, },	// 41 
	{ 0x00F0,  0x00B0, 0x0070, },	// 42 
	{ 0x00F0,  0x00D0, 0x0070, },	// 43 
	{ 0x00F0,  0x00F0, 0x0070, },	// 44 
	{ 0x00D0,  0x00F0, 0x0070, },	// 45 
	{ 0x00B0,  0x00F0, 0x0070, },	// 46 
	{ 0x0090,  0x00F0, 0x0070, },	// 47 
	{ 0x0070,  0x00F0, 0x0070, },	// 48 
	{ 0x0070,  0x00F0, 0x0090, },	// 49 
	{ 0x0070,  0x00F0, 0x00B0, },	// 4A 
	{ 0x0070,  0x00F0, 0x00D0, },	// 4B 
	{ 0x0070,  0x00F0, 0x00F0, },	// 4C 
	{ 0x0070,  0x00D0, 0x00F0, },	// 4D 
	{ 0x0070,  0x00B0, 0x00F0, },	// 4E 
	{ 0x0070,  0x0090, 0x00F0, },	// 4F 
	{ 0x00B0,  0x00B0, 0x00F0, },	// 50 
	{ 0x00C0,  0x00B0, 0x00F0, },	// 51 
	{ 0x00D0,  0x00B0, 0x00F0, },	// 52 
	{ 0x00E0,  0x00B0, 0x00F0, },	// 53 
	{ 0x00F0,  0x00B0, 0x00F0, },	// 54 
	{ 0x00F0,  0x00B0, 0x00E0, },	// 55 
	{ 0x00F0,  0x00B0, 0x00D0, },	// 56 
	{ 0x00F0,  0x00B0, 0x00C0, },	// 57 
	{ 0x00F0,  0x00B0, 0x00B0, },	// 58 
	{ 0x00F0,  0x00C0, 0x00B0, },	// 59 
	{ 0x00F0,  0x00D0, 0x00B0, },	// 5A 
	{ 0x00F0,  0x00E0, 0x00B0, },	// 5B 
	{ 0x00F0,  0x00F0, 0x00B0, },	// 5C 
	{ 0x00E0,  0x00F0, 0x00B0, },	// 5D 
	{ 0x00D0,  0x00F0, 0x00B0, },	// 5E 
	{ 0x00C0,  0x00F0, 0x00B0, },	// 5F 
	{ 0x00B0,  0x00F0, 0x00B0, },	// 60 
	{ 0x00B0,  0x00F0, 0x00C0, },	// 61 
	{ 0x00B0,  0x00F0, 0x00D0, },	// 62 
	{ 0x00B0,  0x00F0, 0x00E0, },	// 63 
	{ 0x00B0,  0x00F0, 0x00F0, },	// 64 
	{ 0x00B0,  0x00E0, 0x00F0, },	// 65 
	{ 0x00B0,  0x00D0, 0x00F0, },	// 66 
	{ 0x00B0,  0x00C0, 0x00F0, },	// 67 
	{ 0x0000,  0x0000, 0x0070, },	// 68 
	{ 0x0010,  0x0000, 0x0070, },	// 69 
	{ 0x0030,  0x0000, 0x0070, },	// 6A 
	{ 0x0050,  0x0000, 0x0070, },	// 6B 
	{ 0x0070,  0x0000, 0x0070, },	// 6C 
	{ 0x0070,  0x0000, 0x0050, },	// 6D 
	{ 0x0070,  0x0000, 0x0030, },	// 6E 
	{ 0x0070,  0x0000, 0x0010, },	// 6F 
	{ 0x0070,  0x0000, 0x0000, },	// 70 
	{ 0x0070,  0x0010, 0x0000, },	// 71 
	{ 0x0070,  0x0030, 0x0000, },	// 72 
	{ 0x0070,  0x0050, 0x0000, },	// 73 
	{ 0x0070,  0x0070, 0x0000, },	// 74 
	{ 0x0050,  0x0070, 0x0000, },	// 75 
	{ 0x0030,  0x0070, 0x0000, },	// 76 
	{ 0x0010,  0x0070, 0x0000, },	// 77 
	{ 0x0000,  0x0070, 0x0000, },	// 78 
	{ 0x0000,  0x0070, 0x0010, },	// 79 
	{ 0x0000,  0x0070, 0x0030, },	// 7A 
	{ 0x0000,  0x0070, 0x0050, },	// 7B 
	{ 0x0000,  0x0070, 0x0070, },	// 7C 
	{ 0x0000,  0x0050, 0x0070, },	// 7D 
	{ 0x0000,  0x0030, 0x0070, },	// 7E 
	{ 0x0000,  0x0010, 0x0070, },	// 7F 
	{ 0x0030,  0x0030, 0x0070, },	// 80 
	{ 0x0040,  0x0030, 0x0070, },	// 81 
	{ 0x0050,  0x0030, 0x0070, },	// 82 
	{ 0x0060,  0x0030, 0x0070, },	// 83 
	{ 0x0070,  0x0030, 0x0070, },	// 84 
	{ 0x0070,  0x0030, 0x0060, },	// 85 
	{ 0x0070,  0x0030, 0x0050, },	// 86 
	{ 0x0070,  0x0030, 0x0040, },	// 87 
	{ 0x0070,  0x0030, 0x0030, },	// 88 
	{ 0x0070,  0x0040, 0x0030, },	// 89 
	{ 0x0070,  0x0050, 0x0030, },	// 8A 
	{ 0x0070,  0x0060, 0x0030, },	// 8B 
	{ 0x0070,  0x0070, 0x0030, },	// 8C 
	{ 0x0060,  0x0070, 0x0030, },	// 8D 
	{ 0x0050,  0x0070, 0x0030, },	// 8E 
	{ 0x0040,  0x0070, 0x0030, },	// 8F 
	{ 0x0030,  0x0070, 0x0030, },	// 90 
	{ 0x0030,  0x0070, 0x0040, },	// 91 
	{ 0x0030,  0x0070, 0x0050, },	// 92 
	{ 0x0030,  0x0070, 0x0060, },	// 93 
	{ 0x0030,  0x0070, 0x0070, },	// 94 
	{ 0x0030,  0x0060, 0x0070, },	// 95 
	{ 0x0030,  0x0050, 0x0070, },	// 96 
	{ 0x0030,  0x0040, 0x0070, },	// 97 
	{ 0x0050,  0x0050, 0x0070, },	// 98 
	{ 0x0050,  0x0050, 0x0070, },	// 99 
	{ 0x0060,  0x0050, 0x0070, },	// 9A 
	{ 0x0060,  0x0050, 0x0070, },	// 9B 
	{ 0x0070,  0x0050, 0x0070, },	// 9C 
	{ 0x0070,  0x0050, 0x0060, },	// 9D 
	{ 0x0070,  0x0050, 0x0060, },	// 9E 
	{ 0x0070,  0x0050, 0x0050, },	// 9F 
	{ 0x0070,  0x0050, 0x0050, },	// A0 
	{ 0x0070,  0x0050, 0x0050, },	// A1 
	{ 0x0070,  0x0060, 0x0050, },	// A2 
	{ 0x0070,  0x0060, 0x0050, },	// A3 
	{ 0x0070,  0x0070, 0x0050, },	// A4 
	{ 0x0060,  0x0070, 0x0050, },	// A5 
	{ 0x0060,  0x0070, 0x0050, },	// A6 
	{ 0x0050,  0x0070, 0x0050, },	// A7 
	{ 0x0050,  0x0070, 0x0050, },	// A8 
	{ 0x0050,  0x0070, 0x0050, },	// A9 
	{ 0x0050,  0x0070, 0x0060, },	// AA 
	{ 0x0050,  0x0070, 0x0060, },	// AB 
	{ 0x0050,  0x0070, 0x0070, },	// AC 
	{ 0x0050,  0x0060, 0x0070, },	// AD 
	{ 0x0050,  0x0060, 0x0070, },	// AE 
	{ 0x0050,  0x0050, 0x0070, },	// AF 
	{ 0x0000,  0x0000, 0x0040, },	// B0 
	{ 0x0010,  0x0000, 0x0040, },	// B1 
	{ 0x0020,  0x0000, 0x0040, },	// B2 
	{ 0x0030,  0x0000, 0x0040, },	// B3 
	{ 0x0040,  0x0000, 0x0040, },	// B4 
	{ 0x0040,  0x0000, 0x0030, },	// B5 
	{ 0x0040,  0x0000, 0x0020, },	// B6 
	{ 0x0040,  0x0000, 0x0010, },	// B7 
	{ 0x0040,  0x0000, 0x0000, },	// B8 
	{ 0x0040,  0x0010, 0x0000, },	// B9 
	{ 0x0040,  0x0020, 0x0000, },	// BA 
	{ 0x0040,  0x0030, 0x0000, },	// BB 
	{ 0x0040,  0x0040, 0x0000, },	// BC 
	{ 0x0030,  0x0040, 0x0000, },	// BD 
	{ 0x0020,  0x0040, 0x0000, },	// BE 
	{ 0x0010,  0x0040, 0x0000, },	// BF 
	{ 0x0000,  0x0040, 0x0000, },	// C0 
	{ 0x0000,  0x0040, 0x0010, },	// C1 
	{ 0x0000,  0x0040, 0x0020, },	// C2 
	{ 0x0000,  0x0040, 0x0030, },	// C3 
	{ 0x0000,  0x0040, 0x0040, },	// C4 
	{ 0x0000,  0x0030, 0x0040, },	// C5 
	{ 0x0000,  0x0020, 0x0040, },	// C6 
	{ 0x0000,  0x0010, 0x0040, },	// C7 
	{ 0x0020,  0x0020, 0x0040, },	// C8 
	{ 0x0020,  0x0020, 0x0040, },	// C9 
	{ 0x0030,  0x0020, 0x0040, },	// CA 
	{ 0x0030,  0x0020, 0x0040, },	// CB 
	{ 0x0040,  0x0020, 0x0040, },	// CC 
	{ 0x0040,  0x0020, 0x0030, },	// CD 
	{ 0x0040,  0x0020, 0x0030, },	// CE 
	{ 0x0040,  0x0020, 0x0020, },	// CF 
	{ 0x0040,  0x0020, 0x0020, },	// D0 
	{ 0x0040,  0x0020, 0x0020, },	// D1 
	{ 0x0040,  0x0030, 0x0020, },	// D2 
	{ 0x0040,  0x0030, 0x0020, },	// D3 
	{ 0x0040,  0x0040, 0x0020, },	// D4 
	{ 0x0030,  0x0040, 0x0020, },	// D5 
	{ 0x0030,  0x0040, 0x0020, },	// D6 
	{ 0x0020,  0x0040, 0x0020, },	// D7 
	{ 0x0020,  0x0040, 0x0020, },	// D8 
	{ 0x0020,  0x0040, 0x0020, },	// D9 
	{ 0x0020,  0x0040, 0x0030, },	// DA 
	{ 0x0020,  0x0040, 0x0030, },	// DB 
	{ 0x0020,  0x0040, 0x0040, },	// DC 
	{ 0x0020,  0x0030, 0x0040, },	// DD 
	{ 0x0020,  0x0030, 0x0040, },	// DE 
	{ 0x0020,  0x0020, 0x0040, },	// DF 
	{ 0x0020,  0x0020, 0x0040, },	// E0 
	{ 0x0030,  0x0020, 0x0040, },	// E1 
	{ 0x0030,  0x0020, 0x0040, },	// E2 
	{ 0x0030,  0x0020, 0x0040, },	// E3 
	{ 0x0040,  0x0020, 0x0040, },	// E4 
	{ 0x0040,  0x0020, 0x0030, },	// E5 
	{ 0x0040,  0x0020, 0x0030, },	// E6 
	{ 0x0040,  0x0020, 0x0030, },	// E7 
	{ 0x0040,  0x0020, 0x0020, },	// E8 
	{ 0x0040,  0x0030, 0x0020, },	// E9 
	{ 0x0040,  0x0030, 0x0020, },	// EA 
	{ 0x0040,  0x0030, 0x0020, },	// EB 
	{ 0x0040,  0x0040, 0x0020, },	// EC 
	{ 0x0030,  0x0040, 0x0020, },	// ED 
	{ 0x0030,  0x0040, 0x0020, },	// EE 
	{ 0x0030,  0x0040, 0x0020, },	// EF 
	{ 0x0020,  0x0040, 0x0020, },	// F0 
	{ 0x0020,  0x0040, 0x0030, },	// F1 
	{ 0x0020,  0x0040, 0x0030, },	// F2 
	{ 0x0020,  0x0040, 0x0030, },	// F3 
	{ 0x0020,  0x0040, 0x0040, },	// F4 
	{ 0x0020,  0x0030, 0x0040, },	// F5 
	{ 0x0020,  0x0030, 0x0040, },	// F6 
	{ 0x0020,  0x0030, 0x0040, },	// F7 
	{ 0x0000,  0x0000, 0x0000, },	// F8 
	{ 0x0000,  0x0000, 0x0000, },	// F9 
	{ 0x0000,  0x0000, 0x0000, },	// FA 
	{ 0x0000,  0x0000, 0x0000, },	// FB 
	{ 0x0000,  0x0000, 0x0000, },	// FC 
	{ 0x0000,  0x0000, 0x0000, },	// FD 
	{ 0x0000,  0x0000, 0x0000, },	// FE 
	{ 0x0000,  0x0000, 0x0000, },	// FF 
};

