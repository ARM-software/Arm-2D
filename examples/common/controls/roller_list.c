#include "arm_2d.h"
#include "arm_2d_helper.h"
#include <math.h>

#include "__common.h"

#include "./roller_list.h"
#include "./shape_round_corner_box.h"
/*
 * LTOP_SPACE       +--------+
 * +----+           |        | +----+
 * |    | LM_SPACE  |        | |    |
 * |  L |           |    M   | |  R |
 * +----+           |        | +----+
 *                  +--------+
 */


#define SPEED 5                         /* delta */
#define CONTROL_SCALE        0.67       /* size ratio */
#define CONTROL_SCALE_ALPHA  200        /* alpha */

int8_t roller_switch = 0;               /* roling direction */

/*
 *    roller list 
 */
typedef struct s_tRollerList {
    int16_t lsize;                      /* small box */
    int16_t msize;                      /* big box */
    int16_t tspace;                     /* top space */
    int16_t xspace;                     /* space between boxes*/

    int16_t iX[3];                      /* x offset of boxes    */

    float ratio_sml;                    /* left factor */
    float ratio_smr;                    /* right factor */
    float ratio_aml;                    /* alpha */
    float ratio_amr;
} s_tRollerList;
static s_tRollerList roller_border = {0};

#define ROLLER_BORDER_LSIZE             roller_border.lsize
#define ROLLER_BORDER_MSIZE             roller_border.msize
#define ROLLER_BORDER_TSPACE            roller_border.tspace
#define ROLLER_BORDER_XSPACE            roller_border.xspace
#define ROLLER_BORDER_X1                roller_border.iX[0]
#define ROLLER_BORDER_X2                roller_border.iX[1]
#define ROLLER_BORDER_X3                roller_border.iX[2]
#define ROLLER_RATIO_SIZE_ML            roller_border.ratio_sml
#define ROLLER_RATIO_SIZE_MR            roller_border.ratio_smr
#define ROLLER_RATIO_ALPHA_ML           roller_border.ratio_aml
#define ROLLER_RATIO_ALPHA_MR           roller_border.ratio_amr

/*
 *    box regions
 */
typedef struct roller_region_t {
    float ix;
    float iy;
    uint8_t isize;
    uint8_t Alpha;
} roller_region_t;
roller_region_t s_tRollerRegion[4] = {0};

#define ROLLER_REGION_X(a)              s_tRollerRegion[a].ix
#define ROLLER_REGION_Y(a)              s_tRollerRegion[a].iy
#define ROLLER_REGION_SIZE(a)           s_tRollerRegion[a].isize
#define ROLLER_REGION_ALPHA(a)          s_tRollerRegion[a].Alpha



/*
 *    display content
 */
static uint8_t index_buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
static char data[] = {'a','b','c','d','e','f','g','h','i'};
#define DATA_SIZE sizeof(data)/sizeof(data[0])
uint8_t index_cur = 0;

/*
 *    reset the boxes
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

void roller_list_draw(  const arm_2d_tile_t *ptTarget, 
                        const arm_2d_region_t *ptRegion,
                        bool bIsNewFrame)
{
    /* ensure the width is two times of the height */
    assert(ptRegion->tSize.iWidth >= ptRegion->tSize.iHeight*2);

    if (bIsNewFrame) {
        //static uint8_t dtsize = 0;

        if (ROLLER_REGION_X(2) == 0){
            /* size paramters */
            ROLLER_BORDER_MSIZE = ptRegion->tSize.iHeight;
            ROLLER_BORDER_LSIZE = CONTROL_SCALE * ROLLER_BORDER_MSIZE;
            ROLLER_BORDER_TSPACE = (ROLLER_BORDER_MSIZE-ROLLER_BORDER_LSIZE)>>1;
            ROLLER_BORDER_XSPACE = (ptRegion->tSize.iWidth-ROLLER_BORDER_MSIZE-2*ROLLER_BORDER_LSIZE)>>2;
            /* x offset */
            ROLLER_BORDER_X1 = ptRegion->tLocation.iX + ROLLER_BORDER_XSPACE;
            ROLLER_BORDER_X2 = ptRegion->tLocation.iX + 2*ROLLER_BORDER_XSPACE + ROLLER_BORDER_LSIZE;
            ROLLER_BORDER_X3 = ptRegion->tLocation.iX + 3*ROLLER_BORDER_XSPACE + ROLLER_BORDER_LSIZE + ROLLER_BORDER_MSIZE;
            /* size ratio */
            ROLLER_RATIO_SIZE_ML = ROLLER_BORDER_TSPACE*1.0/(ROLLER_BORDER_LSIZE+ROLLER_BORDER_XSPACE);	/* 高/宽 */
            ROLLER_RATIO_SIZE_MR = ROLLER_BORDER_TSPACE*1.0/(ROLLER_BORDER_MSIZE+ROLLER_BORDER_XSPACE);
            ROLLER_RATIO_ALPHA_ML = (1.0-CONTROL_SCALE)*CONTROL_SCALE_ALPHA*1.0/(ROLLER_BORDER_LSIZE+ROLLER_BORDER_XSPACE);
            ROLLER_RATIO_ALPHA_MR = (1.0-CONTROL_SCALE)*CONTROL_SCALE_ALPHA*1.0/(ROLLER_BORDER_MSIZE+ROLLER_BORDER_XSPACE)*1.0;

            roller_reset(ptRegion);
        }

        switch(roller_switch) {
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

                if(ROLLER_REGION_X(2) == ROLLER_BORDER_X1) {
                    roller_reset(ptRegion);
                    roller_switch = 0;
                    index_cur = index_cur<DATA_SIZE-1 ? index_cur+1 : 0;
                }
                break;

                default:
                break;
        }

        /*
         *  get the next display content
         */
        switch(roller_switch) {
            case -1:
            /* from left to right */
                index_buf[0] = index_buf[1]>0 ? index_buf[1]-1 : DATA_SIZE-1;
            break;

            case 1:
            /* from right to left */
                index_buf[0] = index_buf[3]+1<DATA_SIZE ? index_buf[3]+1 : 0;
            break;

            default:
            break;
        }

        if(index_buf[2] != index_cur && !roller_switch) {
            /* animation complete */
            index_buf[1] = index_cur>0 ? index_cur-1 : DATA_SIZE-1;
            index_buf[2] = index_cur;
            index_buf[3] = index_cur+1<DATA_SIZE ? index_cur+1 : 0;
        }
    }


    uint8_t i=0;
    if(roller_switch==0) {
        i=1;
    } else {
        i=0;
    }

    for(;i<4;i++) {

        draw_round_corner_box( ptTarget,
                               (const arm_2d_region_t []) {{
                                   .tLocation = {
                                   .iX = s_tRollerRegion[i].ix,
                                   .iY = s_tRollerRegion[i].iy,
                                },
                               .tSize = {
                                   .iWidth  = s_tRollerRegion[i].isize,
                                   .iHeight = s_tRollerRegion[i].isize,
                               },
                               }},
                               GLCD_COLOR_DARK_GREY,
                               s_tRollerRegion[i].Alpha,
                               bIsNewFrame);
                               
//        /* draw content for current box */
//        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTarget);
//        arm_lcd_text_set_draw_region( (arm_2d_region_t []) {{
//                                      .tLocation = {
//                                          .iX = ROLLER_REGION_X(i) + 10,
//                                          .iY = ROLLER_REGION_Y(i) + 10,
//                                      },
//                                      .tSize = {
//                                          .iWidth  = 60,
//                                          .iHeight = 40,
//                                      },
//                                    }});
//        arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_WHITE);
//        arm_lcd_text_location(0,0);					  
//        arm_lcd_printf("%c\r\n",data[index_buf[i]]);
    }
}

