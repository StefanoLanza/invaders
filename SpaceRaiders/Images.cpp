#include "stdafx.h"
#include "Images.h"

namespace
{

const wchar_t alien0Str[] = 
LR"(
  ▀▄   ▄▀  
 ▄█▀███▀█▄ 
█▀███████▀█
█ █▀▀▀▀▀█ █
   ▀▀ ▀▀   
)";

const wchar_t alien1Str[] = 
LR"(
▄ ▀▄   ▄▀ ▄
█▄███████▄█
███▄███▄███
▀█████████▀
 ▄▀     ▀▄ 
)";

const wchar_t laserStr[] =
	L"/0xBA";
}

const Image alien0Img =
{
	alien0Str,
	11,
	5
};

const Image alien1Img =
{
	alien1Str,
	11,
	5
};


const Image laserImage =
{
	laserStr,
	1,
	1
};
