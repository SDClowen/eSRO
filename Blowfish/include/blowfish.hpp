#ifndef __BLOWFISH_HPP__
#define __BLOWFISH_HPP__

/*
	Blowfish
	By: Jim Conger (original Bruce Schneier)
	Url: http://www.schneier.com/blowfish-download.html
*/

#define MAXKEYBYTES 	56		// 448 bits max
#define NPASS           16		// SBox passes

#define DWORD  		unsigned long
#define WORD  		unsigned short
#define BYTE  		unsigned char
typedef BYTE* LPBYTE;
typedef WORD* LPWORD;
typedef DWORD* LPDWORD;

#include <sys/socket.h>
#include <string>

class cBlowFish
{
private:
	DWORD 		* PArray;
	DWORD		(* SBoxes)[256];
	void 		Blowfish_encipher(DWORD *xl, DWORD *xr);
	void 		Blowfish_decipher(DWORD *xl, DWORD *xr);

public:
	cBlowFish();
	~cBlowFish();
	void 		Initialize(BYTE key[], int keybytes);
	DWORD		GetOutputLength(DWORD lInputLong);
	DWORD		Encode(BYTE * pInput, BYTE * pOutput, DWORD lSize);
	void		Decode(BYTE * pInput, BYTE * pOutput, DWORD lSize);
};

#endif //__BLOWFISH_HPP__
