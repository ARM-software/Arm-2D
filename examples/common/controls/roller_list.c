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
 *	���Ʋ���
 */
#define SPEED 5																	/* ��֡�ٶ� */
#define CONTROL_SCALE			0.67											/* СԲ��/��Բ�� ���� */
#define CONTROL_SCALE_ALPHA		200												/* �м�λ�õĲ�͸���� */

int8_t roller_switch = 0;		/* �������ҹ��� */

/*
 *	�߽����
 */
typedef struct ROLLER_BORDER
{
	int16_t lsize;			/* СԲ�ǳߴ� */
	int16_t msize;			/* ��Բ�ǳߴ� */
	int16_t tspace;			/* СԲ�ǵ������� */
	int16_t xspace;			/* ��СԲ�Ǽ��϶ (����Ϊ��������)*/
	
	int16_t x1;				/* ����Բ�Ǿ��ζ�Ӧ��x���� */
	int16_t x2;
	int16_t x3;
	
	float ratio_sml;			/* �ߴ�����ϵ�� */
	float ratio_smr;			/* �ߴ�����ϵ�� */
	float ratio_aml;			/* ͸���ȱ任 */
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
 *	�ĸ�Բ�Ǿ��εı任����
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
 *	���ݱ任
 */
static uint8_t index_buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
static char data[] = {'a','b','c','d','e','f','g','h','i'};
#define DATA_SIZE sizeof(data)/sizeof(data[0])
uint8_t index_cur = 0;

/*
 *	��λ����Բ�Ǿ��ε�λ��
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
	assert(ptRegion->tSize.iWidth >= ptRegion->tSize.iHeight*2);			/* ȷ����Ϊ������ */
	
	if(bIsNewFrame)
	{
		static uint8_t dtsize = 0;											/* ���ڳߴ罥�� */
		
		if(ROLLER_REGION_X(2) == 0){										/* ��һ��������ʱ��� */
			/* �ߴ���� */
			ROLLER_BORDER_MSIZE = ptRegion->tSize.iHeight;
			ROLLER_BORDER_LSIZE = CONTROL_SCALE * ROLLER_BORDER_MSIZE;
			ROLLER_BORDER_TSPACE = (ROLLER_BORDER_MSIZE-ROLLER_BORDER_LSIZE)>>1;
			ROLLER_BORDER_XSPACE = (ptRegion->tSize.iWidth-ROLLER_BORDER_MSIZE-2*ROLLER_BORDER_LSIZE)>>2;
			/* ������� */
			ROLLER_BORDER_X1 = ptRegion->tLocation.iX + ROLLER_BORDER_XSPACE;
			ROLLER_BORDER_X2 = ptRegion->tLocation.iX + 2*ROLLER_BORDER_XSPACE + ROLLER_BORDER_LSIZE;
			ROLLER_BORDER_X3 = ptRegion->tLocation.iX + 3*ROLLER_BORDER_XSPACE + ROLLER_BORDER_LSIZE + ROLLER_BORDER_MSIZE;
			/* �任ϵ�� */
			ROLLER_RATIO_SIZE_ML = ROLLER_BORDER_TSPACE*1.0/(ROLLER_BORDER_LSIZE+ROLLER_BORDER_XSPACE);	/* ��/�� */
			ROLLER_RATIO_SIZE_MR = ROLLER_BORDER_TSPACE*1.0/(ROLLER_BORDER_MSIZE+ROLLER_BORDER_XSPACE);
			ROLLER_RATIO_ALPHA_ML = (1.0-CONTROL_SCALE)*CONTROL_SCALE_ALPHA*1.0/(ROLLER_BORDER_LSIZE+ROLLER_BORDER_XSPACE);
			ROLLER_RATIO_ALPHA_MR = (1.0-CONTROL_SCALE)*CONTROL_SCALE_ALPHA*1.0/(ROLLER_BORDER_MSIZE+ROLLER_BORDER_XSPACE)*1.0;

			roller_reset(ptRegion);
		}
			
		switch(roller_switch)
		{
			case -1:
				/* ��->�� ����xΪ�ο� */
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
				/* ��<-�� */
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
		 *	������һ�������±�
		 */	
		switch(roller_switch)
		{
			case -1:
			/* ��->�� */
				index_buf[0] = index_buf[1]>0 ? index_buf[1]-1 : DATA_SIZE-1;
			break;
			
			case 1:
			/* ��<-�� */
				index_buf[0] = index_buf[3]+1<DATA_SIZE ? index_buf[3]+1 : 0;
			break;
			
			default:
			break;
		}
		
		if(index_buf[2] != index_cur && roller_switch==0)
		{
			/* �ƶ���� */
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
		 *	Բ�Ǿ���
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
		 *	����
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

