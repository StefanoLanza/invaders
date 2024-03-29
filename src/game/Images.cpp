﻿#include "Images.h"
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
// 9: white
// :  white intense
// ;  light blue
// ;  lightBlueIntense
// <  violet
// =  violetIntense

const uint8_t alien_color[2 + 8 * 4] = 
R"(
11111::
1111111
9999999
9999999
)";

#else
	const uint8_t* alien_color = nullptr;
#endif

#if 0
const wchar_t stageStr[2 + 28 * 5] = 
LR"(
 _____  _                  
|   __|| |_  ___  ___  ___ 
|__   ||  _|| .'|| . || -_|
|_____||_|  |__,||_  ||___|
                 |___|     
)";
#else
const wchar_t stageStr[2 + 21 * 3] = 
LR"(
▄▄▄ ▄▄▄ ▄▄▄  ▄▄  ▄▄▄
█▄▄  █  █▄█ █ ▄▄ █▄ 
▄▄█  █  █ █ ▀▄▄▀ █▄▄
)";
#endif

//const wchar_t scoreStr[2 + 28 * 4] = 
//LR"(
// _____                     
//|   __| ___  ___  ___  ___ 
//|__   ||  _|| . ||  _|| -_|
//|_____||___||___||_|  |___|
//)";
//const wchar_t scoreStr[2 + 16 * 2] = 
//LR"(
// _  _  _   _  _
//_) (_ (_) |  (-
//)";
const wchar_t scoreStr[2 + 20 * 3] = 
LR"(
▄▄▄  ▄▄ ▄▄▄ ▄▄▄ ▄▄▄
█▄▄ █   █ █ █▄█ █▄ 
▄▄█ ▀▄▄ █▄█ █▀▄ █▄▄
)";

#if 0
const wchar_t livesStr[2 + 13 * 2] = 
LR"(
| .     _  _
| | \/ (- _)
)";
#else
const wchar_t livesStr[2 + 18 * 3] = 
LR"(
▄   ▄ ▄ ▄ ▄▄▄ ▄▄▄
█   █ █ █ █▄  █▄▄
█▄▄ █ ▀▄▀ █▄▄ ▄▄█
)";

#endif
#if 1
const wchar_t gameOverStr[2 + 38 * 3] = 
LR"(
 ▄▄  ▄▄▄ ▄   ▄ ▄▄▄   ▄▄▄ ▄  ▄ ▄▄▄ ▄▄▄
█ ▄▄ █▄█ █▀▄▀█ █▄    █ █ █  █ █▄  █▄█
▀▄▄▀ █ █ █   █ █▄▄   █▄█ ▀▄▄▀ █▄▄ █▀▄
)";
#else
const wchar_t gameOverStr[2 + 32 * 3] = 
LR"(
 __                __          
/ _   _   _   _   /  \     _  _
\__) (_| ||| (-   \__/ \/ (- | 
)";
#endif
const wchar_t onePlayerStr[2 + 40 * 3] = 
LR"(
▄▄▄ ▄  ▄ ▄▄▄    ▄▄▄ ▄   ▄▄▄ ▄ ▄ ▄▄▄ ▄▄▄
█ █ █▀▄█ █▄     █▄█ █   █▄█ █▄█ █▄  █▄█
█▄█ █  █ █▄▄    █   █▄▄ █ █   █ █▄▄ █▀▄
)";

const wchar_t twoPlayerStr[2 + 44 * 3] = 
LR"(
▄▄▄ ▄   ▄ ▄▄▄   ▄▄▄ ▄   ▄▄▄ ▄ ▄ ▄▄▄ ▄▄▄ ▄▄▄
 █  █ ▄ █ █ █   █▄█ █   █▄█ █▄█ █▄  █▄█ █▄▄
 █   █ █  █▄█   █   █▄▄ █ █   █ █▄▄ █▀▄ ▄▄█
)";

const wchar_t quitStr[2 + 14 * 3] = 
LR"(
▄▄▄ ▄ ▄ ▄ ▄▄▄
█ █ █ █ █  █ 
▀▀▄ █▄█ █  █ 
)";

const wchar_t cursorStr[2 + 4 * 3] = 
LR"(
▄  
██▄
█▀ 
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
 ▄ █ ▄ 
█▀▀█▀▀█
█▀▀▀▀▀█
 ▀   ▀ 
)";

const wchar_t alien2_1Str[2 + 8 * 4] = 
LR"(
 ▄ █ ▄ 
█▀▀█▀▀█
▀█▀▀▀█▀
  ▀ ▀  
)";

const wchar_t alien3_0Str[2 + 8 * 4] = 
LR"(
█▄ ▄ ▄█
█▀▀█▀▀█
▀█▀▀▀█▀
  ▀ ▀  
)";

const wchar_t alien3_1Str[2 + 8 * 4] = 
LR"(
█▄ ▄ ▄█
█▀▀█▀▀█
█▀▀▀▀▀█
 ▀   ▀ 
)";

const wchar_t alien4_0Str[2 + 8 * 4] = 
LR"(
█▄   ▄█
▄▀▀█▀▀▄
▀█▀▀▀█▀
  ▀ ▀  
)";

const wchar_t alien4_1Str[2 + 8 * 4] = 
LR"(
█▄   ▄█
▄▀▀█▀▀▄
█▀▀▀▀▀█
 ▀   ▀ 
)";

const wchar_t alien5_1Str[2 + 10 * 4] = 
LR"(
 █ ▄ ▄ █ 
▄█▀▀█▀▀█▄
█▀▀█▄█▀▀█
▀▄  ▀  ▄▀
)";

const wchar_t alien5_0Str[2 + 10 * 4] = 
LR"(
 █ ▄ ▄ █ 
▄█▀▀█▀▀█▄
█▀▀█▄█▀▀█
 ▀▄ ▀ ▄▀ 
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
  ▄▀▀ ▀▀▄  
   ▀▀ ▀▀   
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
▄▀▀     ▀▀▄
 ▀       ▀ 
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
const uint8_t playerShip1_Colors[] =
R"(
     :     
 :  9:9  : 
99999999999
)";
const uint8_t playerShip2_Colors[] =
R"(
     :     
 :  ;:;  : 
;;;;;;;;;;;
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

const wchar_t asteroid0_Str[] = 
LR"(
  ▄▄▄▄   
▄██████▄ 
█████████
 ███████▀
  ▀▀▀▀▀  
)";

const wchar_t asteroid1_Str[] = 
LR"(
   ▄▄▄▄  
▄███████▄
█████████
 ▀██████ 
   ▀▀▀   
)";

const wchar_t asteroid2_Str[] = 
LR"(
   ▄▄▄  
▄██████▄
▀██████▀
  ▀▀▀   
)";

const wchar_t DbonusStr[] = 
LR"(
╭───╮
| D |
╰───╯
)";

const wchar_t TbonusStr[] = 
LR"(
╭───╮
| T |
╰───╯
)";
 
const wchar_t IbonusStr[] =
LR"(
╭───╮
| I |
╰───╯
)";
const wchar_t SbonusStr[] = 
LR"(
╭───╮
| S |
╰───╯
)";
const wchar_t FbonusStr[] = 
LR"(
╭───╮
| F |
╰───╯
)";
const wchar_t bombStr[] = 
LR"(
╭───╮
| B |
╰───╯
)";

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
■ ■ ■■■■■■■ ■ ■
)";

const wchar_t particleStr[] =
LR"(
.
)";

const wchar_t asteroidParticleStr[] =
LR"(
■
)";

const wchar_t starStr[] =
LR"(
.
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

#if 1
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
#else
const wchar_t _0Img[2 + 6 * 3] =
LR"(
  __ 
 /  \
 \__/
)";

const wchar_t _1Img[2 + 3 * 3] =
LR"(
  
/|
 |
)";

const wchar_t _2Img[2 + 5 * 3] =
LR"(
 __ 
  _)
 (__
)";

const wchar_t _3Img[2 + 5 * 3] =
LR"(
 __ 
  _)
 __)
)";

const wchar_t _4Img[2 + 5 * 3] =
LR"(
    
|__|
   |
)";

const wchar_t _5Img[2 + 4 * 3] =
LR"(
 __
|_ 
__)
)";

const wchar_t _6Img[2 + 5 * 3] =
LR"(
 __ 
/__ 
(__)
)";

const wchar_t _7Img[2 + 4 * 3] =
LR"(
___
  /
 / 
)";

const wchar_t _8Img[2 + 5 * 3] =
LR"(
 __ 
(__)
(__)
)";

const wchar_t _9Img[2 + 5 * 3] =
LR"(
 __ 
(__)
 __/
)";
#endif
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
	{ alien4_0Str, alien_color, 7,	4 },
	{ alien4_1Str, alien_color, 7,	4 },
	{ alien5_0Str, alien_color, 9,	4 },
	{ alien5_1Str, alien_color, 9,	4 },
	{ boss0_0Str, nullptr, 11, 5 },
	{ boss0_1Str, nullptr, 11, 5 },
	{ boss1_0Str, nullptr, 11, 5 },
	{ boss1_1Str, nullptr, 11, 5 },
	{ boss2_0Str, nullptr, 11,	5 },
	{ boss2_1Str, nullptr, 11,	5 },
	{ playerLaserStr,	nullptr, 1, 3 },
	{ alienLaserStr,	nullptr, 1, 2 },
	{ playerShipStr, playerShip1_Colors, 11, 3 },
	{ playerShipStr, playerShip2_Colors, 11, 3 },
	{ explosion0_Str,	nullptr, 7, 3 },
	{ explosion1_Str,	nullptr, 7, 3 },
	{ asteroid0_Str,	nullptr, 9, 5 },
	{ asteroid1_Str,	nullptr, 9, 5 },
	{ asteroid2_Str,	nullptr, 8, 4 },
	{ DbonusStr, nullptr, 5,	3 },
	{ TbonusStr, nullptr, 5,	3 },
	{ SbonusStr, nullptr, 5, 3 },
	{ FbonusStr, nullptr, 5, 3 },
	{ IbonusStr, nullptr, 5, 3 },
	{ shieldStr, nullptr, 15, 1 },
	{ particleStr, nullptr, 1, 1 },
	{ asteroidParticleStr, nullptr, 1, 1 },
	{ starStr, nullptr, 1, 1 },
	{ solidWallStr,	nullptr, 4, 2 },
	{ brittleWallStr, nullptr, 4, 2 },
	{ bombStr, nullptr, 5, 3 },
	{ snowFlakeImg, nullptr, 1, 1 },
	{ giftImg, nullptr, 24, 8 },
	{ happyHolImg, nullptr, 30, 8 },
	{ leafImg, nullptr, 21, 8 },
	//{ stageStr, nullptr, 27, 5},
	{ stageStr, nullptr, 20, 3},
	//{ scoreStr, nullptr, 27, 4 }, // 19, 3
	{ scoreStr, nullptr, 19, 3 },
	//{ livesStr, nullptr, 12, 2 }, // 19, 3
	{ livesStr, nullptr, 17, 3 },
	//{ gameOverStr, nullptr, 31, 3 },
	{ gameOverStr, nullptr, 37, 3 },
	{ onePlayerStr, nullptr, 39, 3 },
	{ twoPlayerStr, nullptr, 43, 3 },
	{ quitStr, nullptr, 13, 3 },
	{ cursorStr, nullptr, 3, 3},
	#if 1
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
#else
	{ _0Img, nullptr, 5, 3 },
	{ _1Img, nullptr, 2, 3 },
	{ _2Img, nullptr, 4, 3 },
	{ _3Img, nullptr, 4, 3 },
	{ _4Img, nullptr, 4, 3 },
	{ _5Img, nullptr, 3, 3 },
	{ _6Img, nullptr, 4, 3 },
	{ _7Img, nullptr, 3, 3 },
	{ _8Img, nullptr, 4, 3 },
	{ _9Img, nullptr, 4, 3 },
#endif
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


