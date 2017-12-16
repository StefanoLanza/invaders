#include "stdafx.h"
#include "Images.h"
#include "Vector2D.h"
#include <cassert>

namespace
{


const wchar_t alien0_0Str[2 + 8 * 4] = 
LR"(
 ▀▄▄▄▀ 
▄▀▀█▀▀▄
▀▀█▀█▀▀
  ▀ ▀  
)";

const wchar_t alien0_1Str[2 + 8 * 4] = 
LR"(
 ▀▄▄▄▀ 
▄▀▀█▀▀▄
▀▀█▀█▀▀
 ▀   ▀ 
)";

const wchar_t alien1_0Str[2 + 8 * 4] = 
LR"(
 ▄▄█▄▄ 
█▀▀█▀▀█
 ▀█▀█▀ 
  ▀ ▀  
)";

const wchar_t alien1_1Str[2 + 8 * 4] = 
LR"(
 ▄▄█▄▄ 
█▀▀█▀▀█
 ▀█▀█▀ 
 ▀   ▀ 
)";

const wchar_t alien2_0Str[2 + 8 * 4] = 
LR"(
  ▄▄▄  
█▀▀█▀▀█
▀▀█▀█▀▀
  ▀ ▀  
)";

const wchar_t alien2_1Str[2 + 8 * 4] = 
LR"(
  ▄▄▄  
█▀▀█▀▀█
▀▀█▀█▀▀
 ▀   ▀ 
)";

const wchar_t alien3Str[2 + 8 * 4] = 
LR"(
 ▀▄▄▄▀ 
▄▀▀█▀▀▄
▀██▀██▀
  ▀▀▀  
)";

const wchar_t alien4Str[2 + 8 * 4] = 
LR"(
 ▀▄▄▄▀ 
▄▀▀█▀▀▄
▀██▀██▀
  ▀▀▀  
)";


const wchar_t boss0_0Str[] = 
LR"(
  ▀▄   ▄▀  
 ▄█▀▀█▀▀█▄ 
█▀███████▀█
█ █▀▀▀▀▀█ █
   ▀▀ ▀▀   
)";

const wchar_t boss0_1Str[] = 
LR"(
▄ ▀▄   ▄▀ ▄
█▄█▀▀█▀▀█▄█
▀█████████▀
  █▀▀▀▀▀█  
 ▀       ▀ 
)";

const wchar_t boss1Str[] = 
LR"(
   ▄██▄   
 ▄██████▄ 
███▄██▄███
  ▄▀▄▄▀▄  
 ▀ ▀  ▀ ▀ 
)";

const wchar_t boss2Str[] = 
LR"(
 ▄▄████▄▄ 
██████████
██▄▄██▄▄██
 ▄▀▄▀▀▄▀▄ 
▀        ▀
)";

const wchar_t laserStr[] =
LR"(
║
║
║
)";

const wchar_t laserLeftStr[] =
LR"(
▄   
 ▀▄ 
   ▀
)";

const wchar_t laserRightStr[] =
LR"(
   ▄
 ▄▀ 
▀   
)";

const wchar_t playerStr[] =
LR"(
  ▄     ▄  
  ██   ██  
▄█████████▄
)";
/*const wchar_t playerStr[] =
LR"(
    ▲    
  /___\  
//_____\\
)";*/

const wchar_t explosionStr[] = 
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


const Image images[(int)ImageId::count] =
{
	{ nullptr, 0, 0 },
	{ planetStr, 33, 10 },
	{ alien0_0Str, 7, 4 },
	{ alien0_1Str, 7, 4 },
	{ alien1_0Str, 7, 4 },
	{ alien1_1Str, 7, 4 },
	{ alien2_0Str, 7, 4 },
	{ alien2_1Str, 7, 4 },
	{ alien3Str, 7,	4 },
	{ boss0_0Str, 11, 5 },
	{ boss0_1Str, 11, 5 },
	{ boss1Str,	10,	5 },
	{ boss2Str,	10,	5 },
	{ laserStr,	1, 3 },
	{ laserLeftStr,	4, 3 },
	{ laserRightStr, 4,	3 },
	{ laserStr,	1, 3 },
	{ playerStr, 11, 3 },
	{ explosionStr,	7, 3 },
	{ DbonusStr, 5,	3 },
	{ TbonusStr, 5,	3 },
	{ SbonusStr, 5, 3 },
	{ FbonusStr, 5, 3 },
	{ IbonusStr, 5, 3 },
	{ solidWallStr,	4, 2 },
	{ brittleWallStr, 4, 2 },
	{ bombStr, 5, 3 }
};

}


const Image& GetImage(ImageId imageId)
{
	return images[(int)imageId];
}


Vector2D GetImageSize(ImageId imageId)
{
	const Image& image = GetImage(imageId);
	return { (float)image.width, (float)image.height };
}