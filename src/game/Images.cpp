#include "Images.h"
#include <engine/Base.h>
#include <engine/Vector2D.h>
#include <engine/Colors.h>
#include <cassert>
#include <fstream>
#include <sys/stat.h>

namespace
{

#if XMAS_EDITION

// 0: red
// 2: green
// 8: white
// 9  white intense
// :  light blue
// ;  lightBlueIntense
// <  violet
// =  violetIntense

const byte alien_color[2 + 8 * 4] = 
R"(
1111199
1111111
8888888
8888888
)";

#else
	#define alien_color nullptr
#endif

const wchar_t stageStr[2 + 21 * 3] = 
LR"(
▄▄▄ ▄▄▄ ▄▄▄  ▄▄  ▄▄▄
█▄▄  █  █▄█ █ ▄▄ █▄ 
▄▄█  █  █ █ ▀▄▄▀ █▄▄
)";

const wchar_t scoreStr[2 + 20 * 3] = 
LR"(
▄▄▄  ▄▄ ▄▄▄ ▄▄▄ ▄▄▄
█▄▄ █   █ █ █▄█ █▄ 
▄▄█ ▀▄▄ █▄█ █▀▄ █▄▄
)";

const wchar_t alien0_0Str[2 + 8 * 4] = 
LR"(
 ▀▄▄▄▀ 
▄▀▀█▀▀▄
▀█▀▀▀█▀
  ▀ ▀  
)";

const wchar_t alien0_1Str[2 + 8 * 4] = 
LR"(
 ▀▄▄▄▀ 
▄▀▀█▀▀▄
█▀▀▀▀▀█
 ▀   ▀ 
)";


const wchar_t alien1_0Str[2 + 8 * 4] = 
LR"(
 █▄▄▄█ 
█▀▀█▀▀█
▀█▀▀▀█▀
  ▀ ▀  
)";

const wchar_t alien1_1Str[2 + 8 * 4] = 
LR"(
 █▄▄▄█ 
█▀▀█▀▀█
█▀▀▀▀▀█
 ▀   ▀ 
)";

const wchar_t alien2_0Str[2 + 8 * 4] = 
LR"(
 ▄ ▄ ▄ 
█▀▀█▀▀█
█▀▀▀▀▀█
 ▀   ▀ 
)";

const wchar_t alien2_1Str[2 + 8 * 4] = 
LR"(
 ▄ ▄ ▄ 
█▀▀█▀▀█
▀█▀▀▀█▀
  ▀ ▀  
)";

const wchar_t alien3_0Str[2 + 8 * 4] = 
LR"(
  ▄ ▄  
▄▀▀█▀▀▄
▀█▀▀▀█▀
  ▀ ▀  
)";

const wchar_t alien3_1Str[2 + 8 * 4] = 
LR"(
  ▄ ▄  
▄▀▀█▀▀▄
█▀▀▀▀▀█
 ▀   ▀ 
)";


const wchar_t boss0_0Str[] = 
LR"(
  ▀▄   ▄▀  
 ▄█▀███▀█▄ 
███▄▄█▄▄███
▄▀  ▀ ▀  ▀▄
 ▀▀     ▀▀ 
)";

const wchar_t boss0_1Str[] = 
LR"(
  ▀▄   ▄▀  
 ▄█▀███▀█▄ 
███▄▄█▄▄███
 ▄▀ ▀ ▀ ▀▄ 
  ▀▀   ▀▀  
)";

const wchar_t boss1_0Str[] = 
LR"(
   ▄███▄   
 ▄▀█████▀▄ 
██▄▄███▄▄██
  ▄▀▀ ▀▀▄  
   ▀   ▀   
)";

const wchar_t boss1_1Str[] = 
LR"(
   ▄███▄   
 ▄▀█████▀▄ 
██▄▄███▄▄██
 ▄▀▀   ▀▀▄ 
  ▀     ▀  
)";

const wchar_t boss2_0Str[] = 
LR"(
 ▄▄██ ██▄▄ 
██▀█████▀██
██▄▄███▄▄██
 ▄▀ ▀ ▀ ▀▄ 
  ▀     ▀  
)";

const wchar_t boss2_1Str[] = 
LR"(
 ▄▄██ ██▄▄ 
██▀█████▀██
██▄▄███▄▄██
▄▀  ▀ ▀  ▀▄
 ▀       ▀ 
)";

const wchar_t playerLaserStr[] =
LR"(
║
║
║
)";

const wchar_t alienLaserStr[] =
LR"(
║
║
1)";

const wchar_t playerShipStr[] =
LR"(
     ▄     
 █   █   █ 
▄█▄█████▄█▄
)";
const byte playerShipColors[] =
R"(
     :     
 :  9:9  : 
99999999999
)";

const wchar_t explosion0_Str[] = 
LR"(
  ▄▄   
 ▀██▀  
       
)";

const wchar_t explosion1_Str[] = 
LR"(
 ▀▄ ▄▀ 
▀▀█▀█▀▀
 ▀   ▀ 
)";

const wchar_t DbonusStr[] = 
LR"(
 ▄▄▄ 
█ D █
 ▀▀▀ 
)";

const wchar_t TbonusStr[] = 
LR"(
 ▄▄▄ 
█ T █
 ▀▀▀ 
)";
 
const wchar_t IbonusStr[] =
LR"(
 ▄▄▄ 
█ I █
 ▀▀▀ 
)";
const wchar_t SbonusStr[] = 
LR"(
 ▄▄▄ 
█ S █
 ▀▀▀ 
)";
const wchar_t FbonusStr[] = 
LR"(
 ▄▄▄ 
█ F █
 ▀▀▀ 
)";
const wchar_t bombStr[] = 
LR"(
 ▄▄█▀
█ B █
 ▀▀▀ 
)";

/*const wchar_t FbonusStr[] = 
LR"(
╭---╮
| f |
╰---╯
)";*/
const wchar_t solidWallStr[] = 
LR"(
████
████
)";
const wchar_t brittleWallStr[] = 
LR"(
░░░░
░░░░
)";
const wchar_t shieldStr[] =
LR"(
▄ ▄ ▄▄▄▄▄▄▄ ▄ ▄
)";

const wchar_t planetStr[] = 
LR"(
     ~+                          
            *       +            
        '                  |     
    ()    .-.,="``"=.    - o -   
          '=/_       \     |     
       *   |  '=._    |          
            \     `=./`,        '
         .   '=.__.=' `='      * 
+                         +      
     O      *        '       .   
)";


const wchar_t _0Img[2 + 4 * 3] =
LR"(
▄▄▄
█ █
█▄█
)";

const wchar_t _1Img[2 + 4 * 3] =
LR"(
  ▄
 ▀█
  █
)";

const wchar_t _2Img[2 + 4 * 3] =
LR"(
▄▄▄
▄▄█
█▄▄
)";

const wchar_t _3Img[2 + 4 * 3] =
LR"(
▄▄▄
▄▄█
▄▄█
)";

const wchar_t _4Img[2 + 4 * 3] =
LR"(
▄ ▄
█▄█
  █
)";

const wchar_t _5Img[2 + 4 * 3] =
LR"(
▄▄▄
█▄▄
▄▄█
)";

const wchar_t _6Img[2 + 4 * 3] =
LR"(
▄▄▄
█▄▄
█▄█
)";

const wchar_t _7Img[2 + 4 * 3] =
LR"(
▄▄▄
  █
  █
)";

const wchar_t _8Img[2 + 4 * 3] =
LR"(
▄▄▄
█▄█
█▄█
)";

const wchar_t _9Img[2 + 4 * 3] =
LR"(
▄▄▄
█▄█
  █
)";

const wchar_t snowFlakeImg[] =
LR"(
*
)";


const wchar_t giftImg[] =
LR"(
            .-" +' "-.  
           /.'.'A_'*`.\ 
          |:.*'/\-\. ':|
          |:.'.||"|.'*:|
           \:~^~^~^~^:/ 
            /`-....-'\  
           /          \ 
           `-.,____,.-' 
)";

const wchar_t happyHolImg[] =
LR"(
 __,  ,___,                   
(--|__| _,,_ ,_               
  _|  |(_||_)|_)\/            
          |  | _/             
        __,  ,___,            
       (--|__| _ |' _| _,   , 
         _|  |(_)||(_|(_|\//_)
        (               _/    
)";

const wchar_t leafImg[] =
LR"(
      _/\_     __/\__
      )   (_  _) .' (
      `) '.( ) .'  (`
       `-._\()/__(~` 
           ()()      
          / |`\      
          ) : (      
          `)_/`      
)";

constexpr int numGameImages = static_cast<int>(GameImageId::_9) + 1;
const Image gameImages[numGameImages] =
{
	{ planetStr, nullptr, 33, 10 },
	{ alien0_0Str, alien_color, 7, 4 },
	{ alien0_1Str, alien_color, 7, 4 },
	{ alien1_0Str, alien_color, 7, 4 },
	{ alien1_1Str, alien_color, 7, 4 },
	{ alien2_0Str, alien_color, 7, 4 },
	{ alien2_1Str, alien_color, 7, 4 },
	{ alien3_0Str, alien_color, 7,	4 },
	{ alien3_1Str, alien_color, 7,	4 },
	{ boss0_0Str, nullptr, 11, 5 },
	{ boss0_1Str, nullptr, 11, 5 },
	{ boss1_0Str, nullptr, 11, 5 },
	{ boss1_1Str, nullptr, 11, 5 },
	{ boss2_0Str, nullptr, 11,	5 },
	{ boss2_1Str, nullptr, 11,	5 },
	{ playerLaserStr,	nullptr, 1, 3 },
	{ alienLaserStr,	nullptr, 1, 2 },
	{ playerShipStr, playerShipColors, 11, 3 },
	{ explosion0_Str,	nullptr, 7, 3 },
	{ explosion1_Str,	nullptr, 7, 3 },
	{ DbonusStr, nullptr, 5,	3 },
	{ TbonusStr, nullptr, 5,	3 },
	{ SbonusStr, nullptr, 5, 3 },
	{ FbonusStr, nullptr, 5, 3 },
	{ IbonusStr, nullptr, 5, 3 },
	{ shieldStr, nullptr, 15, 1 },
	{ solidWallStr,	nullptr, 4, 2 },
	{ brittleWallStr, nullptr, 4, 2 },
	{ bombStr, nullptr, 5, 3 },
	{ snowFlakeImg, nullptr, 1, 1 },
	{ giftImg, nullptr, 24, 8 },
	{ happyHolImg, nullptr, 30, 8 },
	{ leafImg, nullptr, 21, 8 },
	{ stageStr, nullptr, 20, 3},
	{ scoreStr, nullptr, 19, 3 },
	{ _0Img, nullptr, 3, 3 },
	{ _1Img, nullptr, 3, 3 },
	{ _2Img, nullptr, 3, 3 },
	{ _3Img, nullptr, 3, 3 },
	{ _4Img, nullptr, 3, 3 },
	{ _5Img, nullptr, 3, 3 },
	{ _6Img, nullptr, 3, 3 },
	{ _7Img, nullptr, 3, 3 },
	{ _8Img, nullptr, 3, 3 },
	{ _9Img, nullptr, 3, 3 },
};

long GetFileSize(const char* fileName)
{
	struct stat stat_buf;
	int rc = stat(fileName, &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

}


void InitGameImages() 
{
	SetImages(GameImageId::planet, numGameImages, gameImages);
}

bool LoadTxtImage(ImageA& image, int width, int height, const char* fileName)
{
	const long fileSize = GetFileSize(fileName);
	if (fileSize == -1)
	{
		return false;
	}
	const size_t imgSize = width * height;
	if (fileSize != (width + 2) * height) // +2: CR LF
	{
		return false;
	}

	std::ifstream inFile;
	inFile.open(fileName);
    if (!inFile) 
	{
		return false;
    }
    
	char* img = static_cast<char*>( malloc(imgSize * sizeof(char)) );
	char* dst = img;
	for (int row = 0; row < height; ++row)
	{
		inFile.read(dst, width);
		// skip CR/LF
		char CRLF;
		inFile.read(&CRLF, 1);
		dst += width;
	}
	image.img = img;
	image.width = width;
	image.height = height;
	image.freeMemory = true;
    
    inFile.close();
	return true;
}


