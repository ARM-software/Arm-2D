/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*============================ INCLUDES ======================================*/

#define __USER_SCENE_TEXT_READER_IMPLEMENT__
#include "arm_2d_scene_text_reader.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#include <stdlib.h>
#include <string.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion" 
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 64,177
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe177,Pe174
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#   pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileCMSISLogo          c_tileCMSISLogoGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_Arial14_A1,
  ARM_2D_FONT_Arial14_A2,
  ARM_2D_FONT_Arial14_A4,
  ARM_2D_FONT_Arial14_A8;
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*
 * \brief This text sample is designed to test a GUI textbox's ability to handle 
 *        complex content. It features exceptionally long and compound words 
 *        (e.g., pneumonoultramicroscopicsilicovolcanoconiosis, lexicon.logic), 
 *        multiple ASCII punctuation symbols ("", [], (), {}), and varied 
 *        sentence structures with unusual line breaks and spacing. The text 
 *        challenges the control's word-wrapping, scrolling, and rendering of 
 *        intricate formatting.
 */
const char c_chStory[] = {
    "once upon the time, in a realm known as Lexiconia, where verbosity reigned supreme and language was celebrated in its most extravagant form, there existed a venerable scholar named Bartholomew.Quintessential. He resided in an ancient citadel called BibliothecaMagnificus, a fortress-like edifice filled with towering shelves of dusty tomes and manuscripts chronicling the history of extraordinary words such as pseudopseudohypoparathyroidism, antidisestablishmentarianism, floccinaucinihilipilification, and supercalifragilisticexpialidocious. The corridors of BibliothecaMagnificus were adorned with elaborate inscriptions and mysterious symbols (\"\", \"[]\", \"()\", \"{}\") that lent an air of enigmatic sophistication to every chamber.\n"
    "Bartholomew's insatiable curiosity led him to pursue the legendary IncomprehensibilityCodex, a manuscript reputed to contain secrets of linguistic power and the arcane art of sesquipedalian mastery. According to ancient lore, the codex was secured within the ImpenetrableVault, hidden beneath the venerable OrthographicPalace. This vault was protected by barriers imbued with pneumonoultramicroscopicsilicovolcanoconiosis-infused enchantments and guarded by sentinels whose duty was to summon supercalifragilisticexpialidocious spells in defense of the realm.\n"
    "One fateful night, as a storm of meteorologically.catastrophic intensity battered Lexiconia, Bartholomew discovered a cryptic passage in one of his leather-bound volumes. The passage, written in an elegantly swirling script, read: \"By the power of honorificabilitudinitatibus, let the lexicographical enigmas be unveiled!\" With trembling hands, he clutched his phosphorescent.lantern and embarked on a journey across serpentine causeways and labyrinthine corridors that twisted like the spirals of a double helix. His path was illuminated by bioluminescent flora and the soft glow of starlight that filtered through the grand stained glass windows of ancient archways.\n"
    "As he traversed the shadowy passageways, Bartholomew encountered numerous obstacles that tested his resolve. In one vast chamber, he found a giant, intricately carved stone door with the inscription: {antidisestablishmentarianism} [floccinaucinihilipilification]. The door's design was a marvel of engineering, complete with elaborate brackets and swirling patterns resembling the structure of the most complex molecules. Before he could decipher its hidden mechanism, a spectral figure materialized, exclaiming in a voice that resonated like a hundred echoes, \"WHO DARES DISTURB THE LEXICONIC SANCTUM?\"\n"
    "Bartholomew, undaunted by the apparition, replied in a steady tone, \"I, Bartholomew.Quintessential, seek only the illumination of knowledge and the revelation of the IncomprehensibilityCodex!\" His declaration was punctuated by a flourish of symbols - (\"parenthetical expressions\"), [square brackets], and {curly braces} - which danced around his words as though animated by their own magical properties. The spectral guardian paused, as if weighing his words with the precision of an ancient metronome, and then allowed him passage with a nod and the utterance, \"PROCEED WITH CAUTION.\"\n"
    "Stepping through the threshold, Bartholomew found himself within a vast, echoing vault. The air was thick with the musky scent of antiquity and the soft hum of energy that seemed to vibrate from the very walls. Here, suspended in mid-air by streams of shimmering light, floated the IncomprehensibilityCodex. Its pages were inscribed with a mesmerizing mixture of archaic symbols and sprawling words such as hippopotomonstrosesquipedaliophobia, counterdemonstrations, and spectrophotofluorometrically. Every so often, he noticed peculiar constructions where two words were united by a period - words like linguistically.elevated and metaphysically.profound - each pair hinting at deeper, dual meanings.\n"
    "Just as Bartholomew reached out to claim the codex, the vault was plunged into darkness by the sudden appearance of a rival seeker, Draconian.Dictum. Cloaked in a robe adorned with cryptic runes and mysterious emblems (including \"\", \"[]\", \"()\", \"{}\" interwoven into the fabric), Draconian.Dictum was notorious for his ruthless ambition to harness the codex's power for his own malevolent designs. \"Your journey ends here,\" he declared, his voice echoing ominously off the vaulted ceiling. \"I shall claim the IncomprehensibilityCodex and unleash a new era of linguistic tyranny!\"\n"
    "In that charged moment, a fierce battle of words ensued. Bartholomew and Draconian exchanged a barrage of incantations composed of some of the longest, most convoluted words ever uttered by mortal tongues. The air vibrated with phrases like pneumonoultramicroscopicsilicovolcanoconiosis-induced, counterdemonstrationally.inquisitive, and honorificabilitudinitatibus-powered. Sparks of energy flew as the two clashed, their voices intertwining like threads in an elaborate tapestry of linguistic prowess.\n"
    "At the climax of their duel, Bartholomew recalled the cryptic instruction from his tome and boldly chanted, \"By the inimitable power of pseudopseudohypoparathyroidism, let clarity conquer confusion!\" In that very instant, the vault trembled and a dazzling light erupted from the codex. The brilliance was so intense that both adversaries were momentarily blinded. When their vision returned, Draconian.Dictum had been subdued, his ambition extinguished by the overwhelming force of pure, unadulterated lexiconic wisdom.\n"
    "With a calm determination, Bartholomew approached the IncomprehensibilityCodex. He carefully retrieved the ancient manuscript and held it close to his heart, feeling an immense surge of knowledge flow through him like a torrent of iridescent energy. The codex seemed to pulse with life, its pages fluttering open to reveal an intricate diagram of words and symbols - a veritable roadmap to understanding the fabric of language itself. Among these symbols were recurring patterns such as \"syntax.semantics\", \"etymology.mystery\", and \"grammar.infinity\", each pair a testament to the interwoven nature of linguistic power.\n"
    "Overwhelmed yet resolute, Bartholomew decided that such formidable power must be used to enlighten, not to dominate. With the codex in his possession, he vowed to establish a grand academy dedicated to the study and preservation of language - a sanctuary where scholars from all corners of Lexiconia could come together and explore the vast, uncharted territories of sesquipedalian expression. He named this new institution the PolyglotticSanctuary, a beacon of hope and enlightenment in a world too often marred by linguistic ignorance.\n"
    "In the days that followed, Bartholomew traveled far and wide, disseminating the wisdom of the IncomprehensibilityCodex to eager minds. He organized symposia where participants debated the merits of words like supercalifragilisticexpialidocious and antidisestablishmentarianism, often using intricate sentence structures featuring elements like (parenthetical expressions), [clarifying brackets], and {emphasized segments} to illustrate their points. One memorable session featured a discourse titled \"The Duality of lexicon.logic: Understanding linguistically.elevated constructs in modern discourse\", a lecture that left the audience both awed and inspired.\n"
    "The PolyglotticSanctuary soon became a hub for linguistic experimentation and intellectual adventure. Scholars and enthusiasts gathered there, exchanging ideas through lively debates, written treatises, and even theatrical performances where words were personified in elaborate narratives. At one such performance, a troupe of actors reenacted the legendary battle between Bartholomew.Quintessential and Draconian.Dictum, complete with dazzling effects that simulated the luminous bursts of energy generated by long, compound words. The performance was punctuated by dramatic interludes featuring sequences like \"philosophy.science\", \"art.music\", and \"history.mystery\", each underscored by the resonant cadence of eloquent prose.\n"
    "As the years passed, the influence of Bartholomew.Quintessential and his PolyglotticSanctuary grew, and Lexiconia transformed into a realm where language was not merely a tool for communication but a living, breathing art form. The citizens of Lexiconia, inspired by the legacy of their great scholar, embraced even the most formidable words with pride and creativity. They marveled at the beauty of a well-crafted sentence and celebrated the complexity of expressions that many had once deemed impenetrable.\n"
    "Even the natural world seemed to echo this newfound appreciation for language. In the verdant groves surrounding the sanctuary, the wind whispered through the leaves, carrying with it fragments of long-forgotten words like counterdemonstrational, spectrophotofluorometrically, and interdimensional.transcendence. Streams babbled in rhythmic cadences reminiscent of syntactically.perfected verses, while birds chirped melodious notes that formed spontaneous ballads of literary brilliance.\n"
    "In time, the story of Bartholomew.Quintessential and the IncomprehensibilityCodex became the stuff of legend - a timeless reminder that even in a world filled with challenges and adversaries, the power of words could illuminate the darkest corners of existence. The legacy of the PolyglotticSanctuary, with its ever-present symbols such as \"\", \"[]\", \"()\", \"{}\" and its celebrated dual-word constructions like lexicon.logic and syntax.semantics, continued to inspire future generations to seek knowledge, embrace complexity, and cherish the art of language in all its magnificent, unbounded glory.\n"
    "Thus, in the annals of Lexiconia, the memory of that fateful night lived on as a beacon of intellectual triumph and the enduring magic of words, reminding all who encountered it that even the longest, most intricate expressions have the power to unite hearts and minds in the eternal quest for enlightenment."
};

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_text_reader_load(arm_2d_scene_t *ptScene)
{
    user_scene_text_reader_t *ptThis = (user_scene_text_reader_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    text_box_on_load(&this.tTextPanel);
}

static void __after_scene_text_reader_switching(arm_2d_scene_t *ptScene)
{
    user_scene_text_reader_t *ptThis = (user_scene_text_reader_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_text_reader_depose(arm_2d_scene_t *ptScene)
{
    user_scene_text_reader_t *ptThis = (user_scene_text_reader_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    text_box_depose(&this.tTextPanel);

    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene text_reader                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_text_reader_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_text_reader_t *ptThis = (user_scene_text_reader_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_text_reader_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_text_reader_t *ptThis = (user_scene_text_reader_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_text_reader_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_text_reader_t *ptThis = (user_scene_text_reader_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    if (arm_2d_helper_is_time_out(100, &this.lTimestamp[0])) {

        if (++this.iLineNumber >= 200) {
            this.iLineNumber = 0;
        }
        text_box_set_start_line(&this.tTextPanel, this.iLineNumber);
    }

    text_box_on_frame_start(&this.tTextPanel);
}

static void __on_scene_text_reader_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_text_reader_t *ptThis = (user_scene_text_reader_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    text_box_on_frame_complete(&this.tTextPanel);
}

static void __before_scene_text_reader_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_text_reader_t *ptThis = (user_scene_text_reader_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_text_reader_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_text_reader_t *ptThis = (user_scene_text_reader_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/

        arm_2d_dock(__top_canvas, 16) {

        #if 1
            draw_round_corner_border(   ptTile, 
                &__dock_region, 
                GLCD_COLOR_BLACK, 
                (arm_2d_border_opacity_t)
                    {32, 32, 255-64, 255-64},
                (arm_2d_corner_opacity_t)
                    {0, 128, 128, 128});
        #endif

            arm_2d_dock_with_margin(__dock_region, 10) {

                arm_lcd_text_set_char_spacing(1);
                arm_lcd_text_set_line_spacing(4);

                text_box_show(  &this.tTextPanel, 
                                ptTile, 
                                &__dock_region,
                                (__arm_2d_color_t) {GLCD_COLOR_BLACK},
                                255,
                                bIsNewFrame);
                
                //arm_2d_helper_draw_box(ptTile, &__dock_region, 1, GLCD_COLOR_BLUE, 32);

            }

        }

    /*-----------------------draw the foreground end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_text_reader_t *__arm_2d_scene_text_reader_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_text_reader_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_text_reader_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_text_reader_t),
                                                        __alignof__(user_scene_text_reader_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_text_reader_t));

    *ptThis = (user_scene_text_reader_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_WHITE}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_text_reader_load,
            .fnScene        = &__pfb_draw_scene_text_reader_handler,
            //.fnAfterSwitch  = &__after_scene_text_reader_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_text_reader_background_start,
            //.fnOnBGComplete = &__on_scene_text_reader_background_complete,
            .fnOnFrameStart = &__on_scene_text_reader_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_text_reader_switching_out,
            .fnOnFrameCPL   = &__on_scene_text_reader_frame_complete,
            .fnDepose       = &__on_scene_text_reader_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_text_reader_t begin ---------------*/

    /* initialize textbox */
    do {
        text_box_c_str_reader_init( &this.tStringReader,
                                    c_chStory,
                                    sizeof(c_chStory));

        text_box_cfg_t tCFG = {
            .ptFont = (arm_2d_font_t *)&ARM_2D_FONT_Arial14_A8,
            .tStreamIO = {
                .ptIO       = &TEXT_BOX_IO_C_STRING_READER,
                .pTarget    = (uintptr_t)&this.tStringReader,
            },
            .u2LineAlign = TEXT_BOX_LINE_ALIGN_JUSTIFIED,
            //.fScale = 1.0f,
            .chSpaceBetweenParagraph = 20,

            .ptScene = (arm_2d_scene_t *)ptThis,
            .bUseDirtyRegions = true,
        };

        text_box_init(&this.tTextPanel, &tCFG);

        
    } while(0);

    /* ------------   initialize members of user_scene_text_reader_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


