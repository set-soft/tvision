/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/****************************** VGA REGISTERS *******************************/
/*
 * Sequencer port and frequently used register indices
 */
#define VGA_SEQUENCER_PORT	0x3c4
#define VGA_SEQUENCER_DATA	0x3c5

#define VGA_WRT_PLANE_ENB_REG	2
#define VGA_MEMORY_MODE_REG     4

/*
 * color plane operations
 */
#define VGA_FUNC_SET	0
#define VGA_FUNC_AND	8
#define VGA_FUNC_OR	16
#define VGA_FUNC_XOR	24

/*
 * Graphics controller port and frequently used registers
 */
#define VGA_GR_CTRL_PORT	0x3ce
#define VGA_GR_CTRL_DATA	0x3cf

#define VGA_SET_RESET_REG	0
#define VGA_SET_RESET_ENB_REG	1
#define VGA_COLOR_COMP_REG	2
#define VGA_ROT_FN_SEL_REG	3
#define VGA_RD_PLANE_SEL_REG	4
#define VGA_MODE_REG		5
#define VGA_MISC_REG		6
#define VGA_COLOR_DONTC_REG	7
#define VGA_BIT_MASK_REG	8

/*************************** VESA STRUCTURE *********************************/

/*
 * The mode information structure (without padding)

 This struct is taken from libgrx (vesa.h)
 Because I don't want to go into trouble with packing
 this struct I reserve only one block of memory and use
 macros for accessing the members */

#define _BYTE(_index) _farpeekb(mode_info_selector,_index)
#define _WORD(_index) _farpeekw(mode_info_selector,_index)

/*
typedef struct {
    short	ModeAttributes	  PACK;	    mode attributes
*/
#define ModeAttributes _WORD(0)
/*
    char	WinAAttributes	  PACK;	    Window A attributes
    char	WinBAttributes	  PACK;	    Window B attributes
*/
#define WinAAttributes _BYTE(2)
#define WinBAttributes _BYTE(3)
/*
    short	WinGranularity	  PACK;	    window granularity
*/
#define WinGranularity _WORD(4)
/*
    short	WinSize		  PACK;	    window size
*/
#define WinSize _WORD(6)
/*
    short	WinASegment	  PACK;	    Window A start segment
    short	WinBSegment	  PACK;	    Window B start segment
*/
#define WinASegment _WORD(8)
#define WinBSegment _WORD(10)
/*
    void far  (*WinFuncPtr)()	  PACK;	    pointer to window function
    short	BytesPerScanLine  PACK;	    bytes per scan line
      ==== extended and optional information ====
    short	XResolution	  PACK;	    horizontal resolution
    short	YResolution	  PACK;	    vertical resolution
    char	XCharSize	  PACK;	    character cell width
    char	YCharSize	  PACK;	    character cell height
    char	NumberOfPlanes	  PACK;	    number of memory planes
*/
#define NumberOfPlanes _BYTE(0x18)
/*
    char	BitsPerPixel	  PACK;	    bits per pixel
    char	NumberOfBanks	  PACK;	    number of banks
    char	MemoryModel	  PACK;	    memory model type
    char	BankSize	  PACK;	    bank size in K
    char	NumImagePages	  PACK;	    number of image pages
    char	reserved[1]	  PACK;
       ==== VESA 1.2 and later ====
    char	RedMaskSize	  PACK;	    number of bits in red mask
    char	RedMaskPos	  PACK;	    starting bit for red mask
    char	GreenMaskSize	  PACK;
    char	GreenMaskPos	  PACK;
    char	BlueMaskSize	  PACK;
    char	BlueMaskPos	  PACK;
    char	ReservedMaskSize  PACK;	    reserved bits in pixel
    char	ReservedMaskPos	  PACK;
    char	DirectScreenMode  PACK;
} VESAmodeInfoBlock;
*/

/************************** Inline Helpers **********************************/
__inline__ void
_farfill_l(unsigned short sel,unsigned long offset,unsigned long count,
	 unsigned long value)
{
  __asm__ __volatile__ (
      "pushw %%es\n"
      "movw %w0,%%es\n"
      "cld\n"
      "rep\n"
      "stosl\n"
      "popw %%es"
      :
      : "rm" (sel), "a" (value), "D" (offset), "c" (count));
}

#define RESTORE_VESA_HARD   1
#define RESTORE_VESA_BIOS   2
#define RESTORE_VESA_COLORS 4
#define RESTORE_VESA_SVGA   8
#define RESTORE_VESA_ALL    0xF

#define RESTORE_VGA_HARD   1
#define RESTORE_VGA_BIOS   2
#define RESTORE_VGA_COLORS 4
#define RESTORE_VGA_ALL    7




