/**
 * @file lv_hal_disp.h
 *
 * @description Display Driver HAL interface header file
 *
 */

#ifndef FOTAHAL_DISP_H
#define FOTAHAL_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
//#include "lv_hal.h"
//#include "lv_color.h"
//#include "lv_area.h"

#include "hal_drv_conf.h"


/**********************
 *      TYPEDEFS
 **********************/

typedef union {
    uint8_t full; /*must be declared first to set all bits of byte via initializer list */
    union {
        uint8_t blue : 1;
        uint8_t green : 1;
        uint8_t red : 1;
    } ch;
} lv_color1_t;

typedef union {
    struct {
        uint8_t blue : 2;
        uint8_t green : 3;
        uint8_t red : 3;
    } ch;
    uint8_t full;
} lv_color8_t;

typedef union {
    struct {
#if LV_COLOR_16_SWAP == 0
        uint16_t blue : 5;
        uint16_t green : 6;
        uint16_t red : 5;
#else
        uint16_t green_h : 3;
        uint16_t red : 5;
        uint16_t blue : 5;
        uint16_t green_l : 3;
#endif
    } ch;
    uint16_t full;
} lv_color16_t;

typedef union {
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    } ch;
    uint32_t full;
} lv_color32_t;

#if LV_COLOR_DEPTH == 1
typedef uint8_t lv_color_int_t;
typedef lv_color1_t lv_color_t;
#define _LV_COLOR_ZERO_INITIALIZER {0x00}
#elif LV_COLOR_DEPTH == 8
typedef uint8_t lv_color_int_t;
typedef lv_color8_t lv_color_t;
#define _LV_COLOR_ZERO_INITIALIZER {{0x00, 0x00, 0x00}}
#elif LV_COLOR_DEPTH == 16
typedef uint16_t lv_color_int_t;
typedef lv_color16_t lv_color_t;
# if LV_COLOR_16_SWAP == 0
#  define _LV_COLOR_ZERO_INITIALIZER {{0x00, 0x00, 0x00}}
# else
#  define _LV_COLOR_ZERO_INITIALIZER {{0x00, 0x00, 0x00, 0x00}}
# endif
#elif LV_COLOR_DEPTH == 32
typedef uint32_t lv_color_int_t;
typedef lv_color32_t lv_color_t;
#define _LV_COLOR_ZERO_INITIALIZER {{0x00, 0x00, 0x00, 0x00}}
#else
#error "Invalid LV_COLOR_DEPTH in lv_conf.h! Set it to 1, 8, 16 or 32!"
#endif

typedef uint8_t lv_opa_t;

/**
 * Represents a point on the screen.
 */
typedef struct {
    lv_coord_t x;
    lv_coord_t y;
} lv_point_t;

/** Represents an area of the screen. */
typedef struct {
    lv_coord_t x1;
    lv_coord_t y1;
    lv_coord_t x2;
    lv_coord_t y2;
} lv_area_t;


/**
 * Get the width of an area
 * @param area_p pointer to an area
 * @return the width of the area (if x1 == x2 -> width = 1)
 */
static inline lv_coord_t lv_area_get_width(const lv_area_t * area_p)
{
    return (lv_coord_t)(area_p->x2 - area_p->x1 + 1);
}

/**
 * Get the height of an area
 * @param area_p pointer to an area
 * @return the height of the area (if y1 == y2 -> height = 1)
 */
static inline lv_coord_t lv_area_get_height(const lv_area_t * area_p)
{
    return (lv_coord_t)(area_p->y2 - area_p->y1 + 1);
}



/*********************
 *      DEFINES
 *********************/
#define LV_COLOR_WHITE LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)
#define LV_COLOR_SILVER LV_COLOR_MAKE(0xC0, 0xC0, 0xC0)
#define LV_COLOR_GRAY LV_COLOR_MAKE(0x80, 0x80, 0x80)
#define LV_COLOR_BLACK LV_COLOR_MAKE(0x00, 0x00, 0x00)
#define LV_COLOR_RED LV_COLOR_MAKE(0xFF, 0x00, 0x00)
#define LV_COLOR_MAROON LV_COLOR_MAKE(0x80, 0x00, 0x00)
#define LV_COLOR_YELLOW LV_COLOR_MAKE(0xFF, 0xFF, 0x00)
#define LV_COLOR_OLIVE LV_COLOR_MAKE(0x80, 0x80, 0x00)
#define LV_COLOR_LIME LV_COLOR_MAKE(0x00, 0xFF, 0x00)
#define LV_COLOR_GREEN LV_COLOR_MAKE(0x00, 0x80, 0x00)
#define LV_COLOR_CYAN LV_COLOR_MAKE(0x00, 0xFF, 0xFF)
#define LV_COLOR_AQUA LV_COLOR_CYAN
#define LV_COLOR_TEAL LV_COLOR_MAKE(0x00, 0x80, 0x80)
#define LV_COLOR_BLUE LV_COLOR_MAKE(0x00, 0x00, 0xFF)
#define LV_COLOR_NAVY LV_COLOR_MAKE(0x00, 0x00, 0x80)
#define LV_COLOR_MAGENTA LV_COLOR_MAKE(0xFF, 0x00, 0xFF)
#define LV_COLOR_PURPLE LV_COLOR_MAKE(0x80, 0x00, 0x80)
#define LV_COLOR_ORANGE LV_COLOR_MAKE(0xFF, 0xA5, 0x00)


#ifndef _LV_COLOR_HAS_MODERN_CPP
#define _LV_COLOR_HAS_MODERN_CPP 0
#endif

#if _LV_COLOR_HAS_MODERN_CPP
/* Fix msvc compiler error C4576 inside C++ code */
#define _LV_COLOR_MAKE_TYPE_HELPER lv_color_t
#else
#define _LV_COLOR_MAKE_TYPE_HELPER (lv_color_t)
#endif

/* The most simple macro to create a color from R,G and B values */
#if LV_COLOR_DEPTH == 1
#define LV_COLOR_MAKE(r8, g8, b8) (_LV_COLOR_MAKE_TYPE_HELPER{(uint8_t)((b8 >> 7) | (g8 >> 7) | (r8 >> 7))})
#elif LV_COLOR_DEPTH == 8
#define LV_COLOR_MAKE(r8, g8, b8) (_LV_COLOR_MAKE_TYPE_HELPER{{(uint8_t)((b8 >> 6) & 0x3U), (uint8_t)((g8 >> 5) & 0x7U), (uint8_t)((r8 >> 5) & 0x7U)}})
#elif LV_COLOR_DEPTH == 16
#if LV_COLOR_16_SWAP == 0
#define LV_COLOR_MAKE(r8, g8, b8) (_LV_COLOR_MAKE_TYPE_HELPER{{(uint16_t)((b8 >> 3) & 0x1FU), (uint16_t)((g8 >> 2) & 0x3FU), (uint16_t)((r8 >> 3) & 0x1FU)}})
#else
#define LV_COLOR_MAKE(r8, g8, b8) (_LV_COLOR_MAKE_TYPE_HELPER{{(uint16_t)((g8 >> 5) & 0x7U), (uint16_t)((r8 >> 3) & 0x1FU), (uint16_t)((b8 >> 3) & 0x1FU), (uint16_t)((g8 >> 2) & 0x7U)}})
#endif
#elif LV_COLOR_DEPTH == 32
#define LV_COLOR_MAKE(r8, g8, b8) (_LV_COLOR_MAKE_TYPE_HELPER{{b8, g8, r8, 0xff}}) /*Fix 0xff alpha*/
#endif












/*Protect some attributes (max. 8 bit)*/
enum {
    LV_PROTECT_NONE      = 0x00,
    LV_PROTECT_CHILD_CHG = 0x01,   /**< Disable the child change signal. Used by the library*/
    LV_PROTECT_PARENT    = 0x02,   /**< Prevent automatic parent change (e.g. in lv_page)*/
    LV_PROTECT_POS       = 0x04,   /**< Prevent automatic positioning (e.g. in lv_cont layout)*/
    LV_PROTECT_FOLLOW    = 0x08,   /**< Prevent the object be followed in automatic ordering (e.g. in
                                      lv_cont PRETTY layout)*/
    LV_PROTECT_PRESS_LOST = 0x10,  /**< If the `indev` was pressing this object but swiped out while
                                      pressing do not search other object.*/
    LV_PROTECT_CLICK_FOCUS = 0x20, /**< Prevent focusing the object by clicking on it*/
};
typedef uint8_t lv_protect_t;

enum {
    LV_STATE_DEFAULT   =  0x00,
    LV_STATE_CHECKED  =  0x01,
    LV_STATE_FOCUSED  =  0x02,
    LV_STATE_EDITED   =  0x04,
    LV_STATE_HOVERED  =  0x08,
    LV_STATE_PRESSED  =  0x10,
    LV_STATE_DISABLED =  0x20,
};

typedef uint8_t lv_state_t;

typedef struct _lv_obj_t {
    struct _lv_obj_t * parent; /**< Pointer to the parent object*/
    //lv_ll_t child_ll;       /**< Linked list to store the children objects*/

    lv_area_t coords; /**< Coordinates of the object (x1, y1, x2, y2)*/

    //lv_event_cb_t event_cb; /**< Event callback function */
    //lv_signal_cb_t signal_cb; /**< Object type specific signal function*/
    //lv_design_cb_t design_cb; /**< Object type specific design function*/

    void * ext_attr;            /**< Object type specific extended data*/
    //lv_style_list_t  style_list;

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    uint8_t ext_click_pad_hor; /**< Extra click padding in horizontal direction */
    uint8_t ext_click_pad_ver; /**< Extra click padding in vertical direction */
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    lv_area_t ext_click_pad;   /**< Extra click padding area. */
#endif

    lv_coord_t ext_draw_pad; /**< EXTend the size in every direction for drawing. */

    /*Attributes and states*/
    uint8_t click           : 1; /**< 1: Can be pressed by an input device*/
    uint8_t drag            : 1; /**< 1: Enable the dragging*/
    uint8_t drag_throw      : 1; /**< 1: Enable throwing with drag*/
    uint8_t drag_parent     : 1; /**< 1: Parent will be dragged instead*/
    uint8_t hidden          : 1; /**< 1: Object is hidden*/
    uint8_t top             : 1; /**< 1: If the object or its children is clicked it goes to the foreground*/
    uint8_t parent_event    : 1; /**< 1: Send the object's events to the parent too. */
    uint8_t adv_hittest     : 1; /**< 1: Use advanced hit-testing (slower) */
    uint8_t gesture_parent  : 1; /**< 1: Parent will be gesture instead*/
    uint8_t focus_parent    : 1; /**< 1: Parent will be focused instead*/

    //lv_drag_dir_t drag_dir  : 3; /**<  Which directions the object can be dragged in */
    //lv_bidi_dir_t base_dir  : 2; /**< Base direction of texts related to this object */

#if LV_USE_GROUP != 0
    void * group_p;
#endif

    uint8_t protect;            /**< Automatically happening actions can be prevented.
                                     'OR'ed values from `lv_protect_t`*/
    lv_state_t state;

#if LV_USE_OBJ_REALIGN
    //lv_realign_t realign;       /**< Information about the last call to ::lv_obj_align. */
#endif

#if LV_USE_USER_DATA
    lv_obj_user_data_t user_data; /**< Custom user data for object. */
#endif

} lv_obj_t;

#define LV_PM_PART_TOUCH   (1 << 2)
#define LV_PM_PART_DISPLAY (1 << 1)
#define LV_PM_PART_MISC    (1 << 0)


typedef void (*lv_pm_cb_t)(void *);

typedef struct {
    lv_pm_cb_t suspend;
    lv_pm_cb_t resume;
    void *data;
    int part;
} lv_pm_info_t;



int lv_pm_register(lv_pm_info_t *cb);
/**
 * Mark an area of an object as invalid.
 * This area will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 * @param area the area to redraw
 */
void lv_obj_invalidate_area(const lv_obj_t * obj, const lv_area_t * area);

/**
 * Mark the object as invalid therefore its current position will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 */
void lv_obj_invalidate(const lv_obj_t * obj);




/*********************
 *      DEFINES
 *********************/
#ifndef LV_INV_BUF_SIZE
#define LV_INV_BUF_SIZE 32 /*Buffer size for invalid areas */
#endif

#ifndef LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_FLUSH_READY
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct _disp_t;
struct _disp_drv_t;

/**
 * Structure for holding display buffer information.
 */
typedef struct {
    void * buf1; /**< First display buffer. */
    void * buf2; /**< Second display buffer. */

    /*Internal, used by the library*/
    void * buf_act;
    uint32_t size; /*In pixel count*/
    lv_area_t area;
    /*1: flushing is in progress. (It can't be a bit field because when it's cleared from IRQ Read-Modify-Write issue might occur)*/
    volatile int flushing;
    /*1: It was the last chunk to flush. (It can't be a bi tfield because when it's cleared from IRQ Read-Modify-Write issue might occur)*/
    volatile int flushing_last;
    volatile uint32_t last_area         : 1; /*1: the last area is being rendered*/
    volatile uint32_t last_part         : 1; /*1: the last part of the current area is being rendered*/
} lv_disp_buf_t;

/**
 * Display Driver structure to be registered by HAL
 */
typedef struct _disp_drv_t {

    lv_coord_t hor_res; /**< Horizontal resolution. */
    lv_coord_t ver_res; /**< Vertical resolution. */

    /** Pointer to a buffer initialized with `haldisp_buf_init()`.
     * LVGL will use this buffer(s) to draw the screens contents */
    lv_disp_buf_t * buffer;

#if LV_ANTIALIAS
    uint32_t antialiasing : 1; /**< 1: antialiasing is enabled on this display. */
#endif
    uint32_t rotated : 1; /**< 1: turn the display by 90 degree. @warning Does not update coordinates for you!*/

#if LV_COLOR_SCREEN_TRANSP
    /**Handle if the the screen doesn't have a solid (opa == LV_OPA_COVER) background.
     * Use only if required because it's slower.*/
    uint32_t screen_transp : 1;
#endif

    /** DPI (dot per inch) of the display.
     * Set to `LV_DPI` from `lv_Conf.h` by default.
     */
    uint32_t dpi : 10;

    /** MANDATORY: Write the internal buffer (VDB) to the display. 'lv_disp_flush_ready()' has to be
     * called when finished */
    void (*flush_cb)(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

    /** OPTIONAL: Extend the invalidated areas to match with the display drivers requirements
     * E.g. round `y` to, 8, 16 ..) on a monochrome display*/
    void (*rounder_cb)(struct _disp_drv_t * disp_drv, lv_area_t * area);

    /** OPTIONAL: Set a pixel in a buffer according to the special requirements of the display
     * Can be used for color format not supported in LittelvGL. E.g. 2 bit -> 4 gray scales
     * @note Much slower then drawing with supported color formats. */
    void (*set_px_cb)(struct _disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                      lv_color_t color, lv_opa_t opa);

    /** OPTIONAL: Called after every refresh cycle to tell the rendering and flushing time + the
     * number of flushed pixels */
    void (*monitor_cb)(struct _disp_drv_t * disp_drv, uint32_t time, uint32_t px);

    /** OPTIONAL: Called periodically while lvgl waits for operation to be completed.
     * For example flushing or GPU
     * User can execute very simple tasks here or yield the task */
    void (*wait_cb)(struct _disp_drv_t * disp_drv);

    /** OPTIONAL: Called when lvgl needs any CPU cache that affects rendering to be cleaned */
    void (*clean_dcache_cb)(struct _disp_drv_t * disp_drv);

    /** OPTIONAL: called to wait while the gpu is working */
    void (*gpu_wait_cb)(struct _disp_drv_t * disp_drv);

#if LV_USE_GPU

    /** OPTIONAL: Blend two memories using opacity (GPU only)*/
    void (*gpu_blend_cb)(struct _disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length,
                         lv_opa_t opa);

    /** OPTIONAL: Fill a memory with a color (GPU only)*/
    void (*gpu_fill_cb)(struct _disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
                        const lv_area_t * fill_area, lv_color_t color);
#endif

    /** On CHROMA_KEYED images this color will be transparent.
     * `LV_COLOR_TRANSP` by default. (lv_conf.h)*/
    lv_color_t color_chroma_key;

#if LV_USE_USER_DATA
    lv_disp_drv_user_data_t user_data; /**< Custom display driver user data */
#endif

} lv_disp_drv_t;

struct _lv_obj_t;

/**
 * Display structure.
 * @note `lv_disp_drv_t` should be the first member of the structure.
 */
typedef struct _disp_t {
    /**< Driver to the display*/
    lv_disp_drv_t driver;

    /**< A task which periodically checks the dirty areas and refreshes them*/
    //lv_task_t * refr_task;

    /** Screens of the display*/
    //lv_ll_t scr_ll;
    struct _lv_obj_t * act_scr;         /**< Currently active screen on this display */
    struct _lv_obj_t * prev_scr;        /**< Previous screen. Used during screen animations */
#if LV_USE_ANIMATION
    struct _lv_obj_t * scr_to_load;     /**< The screen prepared to load in lv_scr_load_anim*/
#endif
    struct _lv_obj_t * top_layer; /**< @see lv_disp_get_layer_top */
    struct _lv_obj_t * sys_layer; /**< @see lv_disp_get_layer_sys */

uint8_t del_prev  :
    1;        /**< 1: Automatically delete the previous screen when the screen load animation is ready */

    lv_color_t bg_color;          /**< Default display color when screens are transparent*/
    const void * bg_img;       /**< An image source to display as wallpaper*/
    lv_opa_t bg_opa;              /**<Opacity of the background color or wallpaper */

    /** Invalidated (marked to redraw) areas*/
    lv_area_t inv_areas[LV_INV_BUF_SIZE];
    uint8_t inv_area_joined[LV_INV_BUF_SIZE];
    uint32_t inv_p : 10;

    /*Miscellaneous data*/
    uint32_t last_activity_time; /**< Last time there was activity on this display */
} lv_disp_t;


typedef enum {
    LV_DISP_SIZE_SMALL,
    LV_DISP_SIZE_MEDIUM,
    LV_DISP_SIZE_LARGE,
    LV_DISP_SIZE_EXTRA_LARGE,
} lv_disp_size_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a display driver with default values.
 * It is used to have known values in the fields and not junk in memory.
 * After it you can safely set only the fields you need.
 * @param driver pointer to driver variable to initialize
 */
void hal_disp_drv_init(lv_disp_drv_t * driver);

/**
 * Initialize a display buffer
 * @param disp_buf pointer `lv_disp_buf_t` variable to initialize
 * @param buf1 A buffer to be used by LVGL to draw the image.
 *             Always has to specified and can't be NULL.
 *             Can be an array allocated by the user. E.g. `static lv_color_t disp_buf1[1024 * 10]`
 *             Or a memory address e.g. in external SRAM
 * @param buf2 Optionally specify a second buffer to make image rendering and image flushing
 *             (sending to the display) parallel.
 *             In the `disp_drv->flush` you should use DMA or similar hardware to send
 *             the image to the display in the background.
 *             It lets LVGL to render next frame into the other buffer while previous is being
 * sent. Set to `NULL` if unused.
 * @param size_in_px_cnt size of the `buf1` and `buf2` in pixel count.
 */
void haldisp_buf_init(lv_disp_buf_t * disp_buf, void * buf1, void * buf2, uint32_t size_in_px_cnt);

/**
 * Register an initialized display driver.
 * Automatically set the first display as active.
 * @param driver pointer to an initialized 'lv_disp_drv_t' variable (can be local variable)
 * @return pointer to the new display or NULL on error
 */
lv_disp_t * lv_disp_drv_register(lv_disp_drv_t * driver);

/**
 * Update the driver in run time.
 * @param disp pointer to a display. (return value of `lv_disp_drv_register`)
 * @param new_drv pointer to the new driver
 */
void lv_disp_drv_update(lv_disp_t * disp, lv_disp_drv_t * new_drv);

/**
 * Remove a display
 * @param disp pointer to display
 */
void lv_disp_remove(lv_disp_t * disp);

/**
 * Set a default screen. The new screens will be created on it by default.
 * @param disp pointer to a display
 */
void lv_disp_set_default(lv_disp_t * disp);

/**
 * Get the default display
 * @return pointer to the default display
 */
lv_disp_t * lv_disp_get_default(void);

/**
 * Get the horizontal resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the horizontal resolution of the display
 */
lv_coord_t lv_disp_get_hor_res(lv_disp_t * disp);

/**
 * Get the vertical resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the vertical resolution of the display
 */
lv_coord_t lv_disp_get_ver_res(lv_disp_t * disp);

/**
 * Get if anti-aliasing is enabled for a display or not
 * @param disp pointer to a display (NULL to use the default display)
 * @return true: anti-aliasing is enabled; false: disabled
 */
bool lv_disp_get_antialiasing(lv_disp_t * disp);

/**
 * Get the DPI of the display
 * @param disp pointer to a display (NULL to use the default display)
 * @return dpi of the display
 */
lv_coord_t lv_disp_get_dpi(lv_disp_t * disp);

/**
 * Get the size category of the display based on it's hor. res. and dpi.
 * @param disp pointer to a display (NULL to use the default display)
 * @return LV_DISP_SIZE_SMALL/MEDIUM/LARGE/EXTRA_LARGE
 */
lv_disp_size_t lv_disp_get_size_category(lv_disp_t * disp);

//! @cond Doxygen_Suppress

/**
 * Call in the display driver's `flush_cb` function when the flushing is finished
 * @param disp_drv pointer to display driver in `flush_cb` where this function is called
 */
LV_ATTRIBUTE_FLUSH_READY void lv_disp_flush_ready(lv_disp_drv_t * disp_drv);

/**
 * Tell if it's the last area of the refreshing process.
 * Can be called from `flush_cb` to execute some special display refreshing if needed when all areas area flushed.
 * @param disp_drv pointer to display driver
 * @return true: it's the last area to flush; false: there are other areas too which will be refreshed soon
 */
LV_ATTRIBUTE_FLUSH_READY bool lv_disp_flush_is_last(lv_disp_drv_t * disp_drv);

//! @endcond

/**
 * Get the next display.
 * @param disp pointer to the current display. NULL to initialize.
 * @return the next display or NULL if no more. Give the first display when the parameter is NULL
 */
lv_disp_t * lv_disp_get_next(lv_disp_t * disp);

/**
 * Get the internal buffer of a display
 * @param disp pointer to a display
 * @return pointer to the internal buffers
 */
lv_disp_buf_t * lv_disp_get_buf(lv_disp_t * disp);

/**
 * Get the number of areas in the buffer
 * @return number of invalid areas
 */
uint16_t lv_disp_get_inv_buf_size(lv_disp_t * disp);

/**
 * Pop (delete) the last 'num' invalidated areas from the buffer
 * @param num number of areas to delete
 */
void _lv_disp_pop_from_inv_buf(lv_disp_t * disp, uint16_t num);

/**
 * Check the driver configuration if it's double buffered (both `buf1` and `buf2` are set)
 * @param disp pointer to to display to check
 * @return true: double buffered; false: not double buffered
 */
bool lv_disp_is_double_buf(lv_disp_t * disp);

/**
 * Check the driver configuration if it's TRUE double buffered (both `buf1` and `buf2` are set and
 * `size` is screen sized)
 * @param disp pointer to to display to check
 * @return true: double buffered; false: not double buffered
 */
bool lv_disp_is_true_double_buf(lv_disp_t * disp);


lv_obj_t * lv_disp_get_scr_act(lv_disp_t * disp);
void lv_refr_now(lv_disp_t * disp);

lv_disp_t * _lv_refr_get_disp_refreshing(void);




/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
