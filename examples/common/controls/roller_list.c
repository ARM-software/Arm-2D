#include "roller_list.h"


//
// LTOP_SPACE       **********
// ******           *        * ******
// *    * LM_SPACE  *        * *    *
// *  L *           *    M   * *  R *
// ******           *        * ******
//                  **********
//

/*
 *	控制参数
 */
#define SPEED 5																	/* 跳帧速度 */
#define CONTROL_SCALE			0.67											/* 小圆角/大圆角 比例 */
#define CONTROL_SCALE_ALPHA		200												/* 中间位置的不透明度 */

int8_t roller_switch = 0;		/* 控制左右滚动 */

/*
 *	边界参数
 */
typedef struct ROLLER_BORDER
{
	int16_t lsize;			/* 小圆角尺寸 */
	int16_t msize;			/* 大圆角尺寸 */
	int16_t tspace;			/* 小圆角到顶距离 */
	int16_t xspace;			/* 大小圆角间间隙 (允许为负，叠加)*/
	
	int16_t x1;				/* 三个圆角矩形对应的x坐标 */
	int16_t x2;
	int16_t x3;
	
	float ratio_sml;			/* 尺寸左中系数 */
	float ratio_smr;			/* 尺寸右中系数 */
	float ratio_aml;			/* 透明度变换 */
	float ratio_amr;	
}ROLLER_BORDER;
ROLLER_BORDER roller_border = {0};

#define ROLLER_BORDER_LSIZE			roller_border.lsize
#define ROLLER_BORDER_MSIZE			roller_border.msize
#define ROLLER_BORDER_TSPACE		roller_border.tspace
#define ROLLER_BORDER_XSPACE		roller_border.xspace
#define ROLLER_BORDER_X1			roller_border.x1
#define ROLLER_BORDER_X2			roller_border.x2
#define ROLLER_BORDER_X3			roller_border.x3
#define ROLLER_RATIO_SIZE_ML		roller_border.ratio_sml
#define ROLLER_RATIO_SIZE_MR		roller_border.ratio_smr
#define ROLLER_RATIO_ALPHA_ML		roller_border.ratio_aml
#define ROLLER_RATIO_ALPHA_MR		roller_border.ratio_amr

/*
 *	四个圆角矩形的变换参数
 */
typedef struct ROLLER_REGION
{
	float ix;
	float iy;
	uint8_t isize;
	uint8_t Alpha;
}ROLLER_REGION;
ROLLER_REGION roller_region[4] = {0};

#define	ROLLER_REGION_X(a)		roller_region[a].ix
#define	ROLLER_REGION_Y(a)		roller_region[a].iy
#define	ROLLER_REGION_SIZE(a)	roller_region[a].isize
#define	ROLLER_REGION_ALPHA(a)	roller_region[a].Alpha



/*
 *	内容变换
 */
static uint8_t index_buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
static char data[] = {'a','b','c','d','e','f','g','h','i'};
#define DATA_SIZE sizeof(data)/sizeof(data[0])
uint8_t index_cur = 0;

/*
 *	复位三个圆角矩形的位置
 */
static void roller_reset(const arm_2d_region_t *ptRegion)
{
	ROLLER_REGION_X(0) = ROLLER_BORDER_X1 - ROLLER_BORDER_XSPACE - ROLLER_BORDER_LSIZE;
	ROLLER_REGION_Y(0) = ptRegion->tLocation.iY + ROLLER_BORDER_TSPACE;
	ROLLER_REGION_SIZE(0) = ROLLER_BORDER_LSIZE;
	ROLLER_REGION_ALPHA(0) = CONTROL_SCALE*CONTROL_SCALE_ALPHA;
	
	ROLLER_REGION_X(1) = ROLLER_BORDER_X1;
	ROLLER_REGION_Y(1) = ptRegion->tLocation.iY + ROLLER_BORDER_TSPACE;
	ROLLER_REGION_SIZE(1) = ROLLER_BORDER_LSIZE;
	ROLLER_REGION_ALPHA(1) = CONTROL_SCALE*CONTROL_SCALE_ALPHA;
	
	ROLLER_REGION_X(2) = ROLLER_BORDER_X2;
	ROLLER_REGION_Y(2) = ptRegion->tLocation.iY;
	ROLLER_REGION_SIZE(2) = ROLLER_BORDER_MSIZE;
	ROLLER_REGION_ALPHA(2) = CONTROL_SCALE_ALPHA;
	
	ROLLER_REGION_X(3) = ROLLER_BORDER_X3;
	ROLLER_REGION_Y(3) = ptRegion->tLocation.iY + ROLLER_BORDER_TSPACE;
	ROLLER_REGION_SIZE(3) = ROLLER_BORDER_LSIZE;
	ROLLER_REGION_ALPHA(3) = CONTROL_SCALE*CONTROL_SCALE_ALPHA;

}

void roller_list_draw(	const arm_2d_tile_t *ptTarget, 
						const arm_2d_region_t *ptRegion,
						bool bIsNewFrame)
{	
	assert(ptRegion->tSize.iWidth >= ptRegion->tSize.iHeight*2);			/* 确保宽为两倍高 */
	
	if(bIsNewFrame)
	{
		static uint8_t dtsize = 0;											/* 用于尺寸渐变 */
		
		if(ROLLER_REGION_X(2) == 0){										/* 第一次无数据时填充 */
			/* 尺寸参数 */
			ROLLER_BORDER_MSIZE = ptRegion->tSize.iHeight;
			ROLLER_BORDER_LSIZE = CONTROL_SCALE * ROLLER_BORDER_MSIZE;
			ROLLER_BORDER_TSPACE = (ROLLER_BORDER_MSIZE-ROLLER_BORDER_LSIZE)>>1;
			ROLLER_BORDER_XSPACE = (ptRegion->tSize.iWidth-ROLLER_BORDER_MSIZE-2*ROLLER_BORDER_LSIZE)>>2;
			/* 坐标参数 */
			ROLLER_BORDER_X1 = ptRegion->tLocation.iX + ROLLER_BORDER_XSPACE;
			ROLLER_BORDER_X2 = ptRegion->tLocation.iX + 2*ROLLER_BORDER_XSPACE + ROLLER_BORDER_LSIZE;
			ROLLER_BORDER_X3 = ptRegion->tLocation.iX + 3*ROLLER_BORDER_XSPACE + ROLLER_BORDER_LSIZE + ROLLER_BORDER_MSIZE;
			/* 变换系数 */
			ROLLER_RATIO_SIZE_ML = ROLLER_BORDER_TSPACE*1.0/(ROLLER_BORDER_LSIZE+ROLLER_BORDER_XSPACE);	/* 高/宽 */
			ROLLER_RATIO_SIZE_MR = ROLLER_BORDER_TSPACE*1.0/(ROLLER_BORDER_MSIZE+ROLLER_BORDER_XSPACE);
			ROLLER_RATIO_ALPHA_ML = (1.0-CONTROL_SCALE)*CONTROL_SCALE_ALPHA*1.0/(ROLLER_BORDER_LSIZE+ROLLER_BORDER_XSPACE);
			ROLLER_RATIO_ALPHA_MR = (1.0-CONTROL_SCALE)*CONTROL_SCALE_ALPHA*1.0/(ROLLER_BORDER_MSIZE+ROLLER_BORDER_XSPACE)*1.0;

			roller_reset(ptRegion);
		}
			
		switch(roller_switch)
		{
			case -1:
				/* 左->右 都以x为参考 */
				ROLLER_REGION_X(1) = ROLLER_REGION_X(1)+SPEED<ROLLER_BORDER_X2 ? ROLLER_REGION_X(1)+SPEED: ROLLER_BORDER_X2;
				ROLLER_REGION_Y(1) = ptRegion->tLocation.iY + ROLLER_BORDER_TSPACE - (ROLLER_REGION_X(1)-ROLLER_BORDER_X1)*ROLLER_RATIO_SIZE_ML;
				ROLLER_REGION_SIZE(1) = ROLLER_BORDER_LSIZE+(ROLLER_REGION_X(1)-ROLLER_BORDER_X1)*ROLLER_RATIO_SIZE_ML*2;
				ROLLER_REGION_ALPHA(1) = CONTROL_SCALE*CONTROL_SCALE_ALPHA + (ROLLER_REGION_X(1)-ROLLER_BORDER_X1)*ROLLER_RATIO_ALPHA_ML;
			
				ROLLER_REGION_X(2) = ROLLER_REGION_X(1)+ROLLER_REGION_SIZE(1)+ROLLER_BORDER_XSPACE;
				ROLLER_REGION_Y(2) = ptRegion->tLocation.iY + (ROLLER_REGION_X(2)-ROLLER_BORDER_X2)*ROLLER_RATIO_SIZE_MR;
				ROLLER_REGION_SIZE(2) = ROLLER_BORDER_MSIZE-(ROLLER_REGION_X(2)-ROLLER_BORDER_X2)*ROLLER_RATIO_SIZE_MR*2;
				ROLLER_REGION_ALPHA(2) = CONTROL_SCALE_ALPHA-(ROLLER_REGION_X(2)-ROLLER_BORDER_X2)*ROLLER_RATIO_ALPHA_MR;

				ROLLER_REGION_X(3) = ROLLER_REGION_X(2) + ROLLER_REGION_SIZE(2) + ROLLER_BORDER_XSPACE;
				ROLLER_REGION_X(0) = ROLLER_REGION_X(1) - ROLLER_BORDER_LSIZE - ROLLER_BORDER_XSPACE;
			
				if(ROLLER_REGION_X(1) == ROLLER_BORDER_X2){			
					roller_reset(ptRegion);
					
					roller_switch = 0;
					index_cur = index_cur>0 ? index_cur-1 : DATA_SIZE-1;
				}

			break;
				
			case 1:
				/* 左<-右 */
				ROLLER_REGION_X(2) = ROLLER_REGION_X(2)-SPEED>ROLLER_BORDER_X1 ? ROLLER_REGION_X(2)-SPEED: ROLLER_BORDER_X1;
				ROLLER_REGION_Y(2) = ptRegion->tLocation.iY + (ROLLER_BORDER_X2-ROLLER_REGION_X(2))*ROLLER_RATIO_SIZE_ML;
				ROLLER_REGION_SIZE(2) = ROLLER_BORDER_MSIZE-(ROLLER_BORDER_X2-ROLLER_REGION_X(2))*ROLLER_RATIO_SIZE_ML*2;
				ROLLER_REGION_ALPHA(2) = CONTROL_SCALE_ALPHA - (ROLLER_BORDER_X2-ROLLER_REGION_X(2))*ROLLER_RATIO_ALPHA_ML;
			
				ROLLER_REGION_X(3) = ROLLER_REGION_X(2)+ROLLER_REGION_SIZE(2)+ROLLER_BORDER_XSPACE;
				ROLLER_REGION_Y(3) = ptRegion->tLocation.iY + ROLLER_BORDER_TSPACE - (ROLLER_BORDER_X3-ROLLER_REGION_X(3))*ROLLER_RATIO_SIZE_MR;
				ROLLER_REGION_SIZE(3) = ROLLER_BORDER_LSIZE+(ROLLER_BORDER_X3-ROLLER_REGION_X(3))*ROLLER_RATIO_SIZE_MR*2;
				ROLLER_REGION_ALPHA(3) = CONTROL_SCALE_ALPHA*CONTROL_SCALE+(ROLLER_BORDER_X3-ROLLER_REGION_X(3))*ROLLER_RATIO_ALPHA_MR;
			
				ROLLER_REGION_X(1) = ROLLER_REGION_X(2) - ROLLER_BORDER_XSPACE - ROLLER_BORDER_LSIZE;
				ROLLER_REGION_X(0) = ROLLER_REGION_X(3) + ROLLER_BORDER_XSPACE + ROLLER_REGION_SIZE(3);
			
				if(ROLLER_REGION_X(2) == ROLLER_BORDER_X1){			
					roller_reset(ptRegion);
					
					roller_switch = 0;
					index_cur = index_cur<DATA_SIZE-1 ? index_cur+1 : 0;
				}
			break;
					
			default:
				
			break;
		}

		/*
		 *	计算下一条内容下标
		 */	
		switch(roller_switch)
		{
			case -1:
			/* 左->右 */
				index_buf[0] = index_buf[1]>0 ? index_buf[1]-1 : DATA_SIZE-1;
			break;
			
			case 1:
			/* 左<-右 */
				index_buf[0] = index_buf[3]+1<DATA_SIZE ? index_buf[3]+1 : 0;
			break;
			
			default:
			break;
		}
		
		if(index_buf[2] != index_cur && roller_switch==0)
		{
			/* 移动完成 */
			index_buf[1] = index_cur>0 ? index_cur-1 : DATA_SIZE-1;
			index_buf[2] = index_cur;
			index_buf[3] = index_cur+1<DATA_SIZE ? index_cur+1 : 0;
		}
	}
	
	
	
	uint8_t i=0;
	if(roller_switch==0)
		i=1;
	else
		i=0;
	
	for(;i<4;i++)
	{
		/* 
		 *	圆角矩形
		 */
		draw_round_corner_box(  ptTarget,
							(const arm_2d_region_t []) {{
								  .tLocation = {
									  .iX = roller_region[i].ix,
									  .iY =	roller_region[i].iy,
								  },
								  .tSize = {
									  .iWidth  = roller_region[i].isize,
									  .iHeight = roller_region[i].isize,
								  },
							  }},
							GLCD_COLOR_DARK_GREY,
							roller_region[i].Alpha,
							bIsNewFrame);
		/*
		 *	内容
		 */					  
		arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTarget);
		arm_lcd_text_set_draw_region((arm_2d_region_t []) {{
											  .tLocation = {
												  .iX = ROLLER_REGION_X(i) + 10,
												  .iY = ROLLER_REGION_Y(i) + 10,
											  },
											  .tSize = {
												  .iWidth  = 60,
												  .iHeight = 40,
											  },
										  }});
		arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_WHITE);
		arm_lcd_text_location(0,0);					  
		arm_lcd_printf("%c\r\n",data[index_buf[i]]);
	}	
}

