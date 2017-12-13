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
▀█████▀
  ▀▀▀  
)";

const wchar_t alien4Str[2 + 8 * 4] = 
LR"(
 ▀▄▄▄▀ 
▄▀▀█▀▀▄
▀█████▀
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
▄ ▀▄   ▄▀ ▄
█▄███████▄█
███▄███▄███
▀█████████▀
 ▄▀     ▀▄ 
)";

const wchar_t boss2Str[] = 
LR"(
   ▄██▄   
 ▄██████▄ 
███▄██▄███
  ▄▀▄▄▀▄  
 ▀ ▀  ▀ ▀ 
)";

const wchar_t boss3Str[] = 
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

const Image planetImg =
{
	planetStr,
	33,
	10
};

const Image alien0_0Img =
{
	alien0_0Str,
	7,
	4
};
const Image alien0_1Img =
{
	alien0_1Str,
	7,
	4
};

const Image alien1_0Img =
{
	alien1_0Str,
	7,
	4
};

const Image alien1_1Img =
{
	alien1_1Str,
	7,
	4
};

const Image alien2_0Img =
{
	alien2_0Str,
	7,
	4
};

const Image alien2_1Img =
{
	alien2_1Str,
	7,
	4
};

const Image alien3Img =
{
	alien3Str,
	7,
	4
};


const Image boss00Img =
{
	boss0_0Str,
	11,
	5
};

const Image boss01Img =
{
	boss0_1Str,
	11,
	5
};

const Image boss1Img =
{
	boss1Str,
	11,
	5
};

const Image boss2Img =
{
	boss2Str,
	10,
	5
};

const Image boss3Img =
{
	boss3Str,
	10,
	5
};

const Image laserImage =
{
	laserStr,
	1,
	3
};

const Image laserLeftImage =
{
	laserLeftStr,
	4,
	3
};

const Image laserRightImage =
{
	laserRightStr,
	4,
	3
};

const Image playerImage =
{
	playerStr,
	11,
	3
};

const Image explosionImage =
{
	explosionStr,
	7,
	3
};

const Image TpowerUpImage =
{
	TbonusStr,
	5,
	3
};

const Image SpowerUpImage =
{
	SbonusStr,
	5,
	3
};

const Image FpowerUpImage =
{
	FbonusStr,
	5,
	3
};

const Image IpowerUpImage =
{
	IbonusStr,
	5,
	3
};

const Image solidWallImage =
{
	solidWallStr,
	4,
	2
};

const Image brittleWallImage =
{
	brittleWallStr,
	4,
	2
};

const Image* images[(int)ImageId::count] =
{
	nullptr,
	&planetImg,
	&alien0_0Img,
	&alien0_1Img,
	&alien1_0Img,
	&alien1_1Img,
	&alien2_0Img,
	&alien2_1Img,
	&alien3Img,
	&boss00Img,
	&boss01Img,
	&boss1Img,
	&boss2Img,
	&boss3Img,
	&laserImage,
	&laserLeftImage,
	&laserRightImage,
	&laserImage,
	&playerImage,
	&explosionImage,
	&TpowerUpImage,
	&SpowerUpImage,
	&FpowerUpImage,
	&IpowerUpImage,
	&solidWallImage,
	&brittleWallImage,
};

}


const Image* GetImage(ImageId imageId)
{
	return images[(int)imageId];
}


Vector2D GetImageSize(ImageId imageId)
{
	const Image* image = GetImage(imageId);
	assert(image);
	return { (float)image->width, (float)image->height };
}