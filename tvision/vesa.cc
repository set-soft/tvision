/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __DJGPP__

/*
  This file is used to save/restore the graphics screen when a
  VESA mode is set.
  I have tested it with all the modes, which are available on my
  et4000 card. They work when the graphic mode is set by the
  libgrx20 library. After testing with the VESA 1 modes from
  the ALLEGRO library I saw, that they NOT work correct, but 
  I don't know why.

0x0100   640 x  400 x 256
0x0101   640 x  480 x 256
0x0102   800 x  600 x  16
0x0103   800 x  600 x 256
0x0104  1024 x  768 x  16
0x0105  1024 x  768 x 256
0x0106  1280 x 1024 x  16

The function save_vesa assumes, that is_vesa_mode called before and
was successfull. There is no other check for validity.
And the function restore_vesa assumes that save_vesa was called
before.
*/
  
#include <stdlib.h>
#include <sys/movedata.h>
#include <sys/farptr.h>
#include <dpmi.h>
#include <go32.h>

int rh_vesa_size=256*1024;

extern int user_mode;

static void *buffer = NULL;
static int buffer_size = 0;
static int saved_size;
static int window_r;
static int window_w;
static int win_size;
static unsigned win_seg_r;
static unsigned win_seg_w;

/*
 * The mode information structure (without padding)

 This struct is taken from libgrx (vesa.h)
 Because I don't want to go into trouble with packing
 this struct I reserve only one block of memory and use
 macros for accessing the members */

static int mode_info_segment = -1;
static int mode_info_selector;

static int GetVesaModeInfo(int mode)
{
  __dpmi_regs r;
  if (mode_info_segment == -1)
    mode_info_segment = __dpmi_allocate_dos_memory(16,&mode_info_selector);
  if (mode_info_segment == -1)
    return 0;
  r.x.ax = 0x4f01;
  r.x.es = mode_info_segment;
  r.x.di = 0;
  r.x.cx = mode;
  __dpmi_int(0x10,&r);
  if (r.x.ax != 0x004f)
    return 0;
  return 1;
}

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
    void	(*WinFuncPtr)()	  PACK;	    pointer to window function
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

static int vesa_state_segment = -1;
static int vesa_state_selector;

static
void save_vesa_state()
{
  __dpmi_regs r;
  if (vesa_state_segment == -1)
  {
    r.x.ax = 0x4f04;
    r.h.dl = 0x00;
    r.x.cx = 0x000f;
    __dpmi_int(0x10,&r);
    if (r.x.ax != 0x004f) return;
    vesa_state_segment =
                  __dpmi_allocate_dos_memory(r.x.bx*4,&vesa_state_selector);
    if (vesa_state_segment == -1) return;
  }
  r.x.ax = 0x4f04;
  r.h.dl = 0x01;
  r.x.es = vesa_state_segment;
  r.x.bx = 0;
  r.x.cx = 0x000f;
  __dpmi_int(0x10,&r);
}

static
void restore_vesa_state()
{
  __dpmi_regs r;
  if (vesa_state_segment == -1) return;
  r.x.ax = 0x4f04;
  r.h.dl = 0x02;
  r.x.es = vesa_state_segment;
  r.x.bx = 0;
  r.x.cx = 0x000f;
  __dpmi_int(0x10,&r);
}

int rh_vesa_supported = 0;

void rh_check_for_vesa()
{
  __dpmi_regs r;

  r.x.ax = 0x4f03;
  __dpmi_int(0x10,&r);

  /* Not supported */
  if (r.h.al != 0x4f) return;

  rh_vesa_supported = 1;
}

int rh_is_vesa_mode()
{
  __dpmi_regs r;

  r.x.ax = 0x4f03;
  __dpmi_int(0x10,&r);

  /* Not supported or not successful */
  if (r.x.ax != 0x004f) return 0;

  /* Because GetVesaModeInfo forces a screen switch under W95, I check
     the most common textmode here without calling that function */
  if (r.x.bx == 0x0003 || r.x.bx == 7)
    return 0;

  if (!GetVesaModeInfo(r.x.bx))
    return 0;

  if (!(ModeAttributes & 1))
    // not supported
    return 0;

  user_mode = r.x.bx;

  return 1;
}

/*
 * Sequencer port and frequently used register indices
 */
#define VGA_SEQUENCER_PORT	0x3c4
#define VGA_SEQUENCER_DATA	0x3c5

#define VGA_WRT_PLANE_ENB_REG	2

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

#include <pc.h>

void rh_save_vesa()
{
  int offset = 0, off_in_page=0;
  int seg = 0;
  __dpmi_regs r;
  save_vesa_state();

  if (WinBAttributes & 0x3) /* exist and readable */
  {
    window_r = 1;
    win_seg_r = WinBSegment;
    /* I assume here, that now winb is writable. Is that correct?? */
    window_w = 0;
    win_seg_w = WinASegment;
  }
  else
  {
    window_w = window_r = 0;
    win_seg_r = win_seg_w = WinASegment;
  }
  win_size = WinSize * 1024;

  win_seg_r <<= 4;
  win_seg_w <<= 4;

  saved_size = rh_vesa_size;

  if (saved_size < win_size)
    win_size = saved_size;

  if (saved_size > buffer_size)
  {
    buffer = realloc(buffer,saved_size);
    buffer_size = saved_size;
  }

  while (offset < saved_size)
  {
    if (ModeAttributes & 0x10) // graphic ?
    {
      r.x.ax = 0x4f05;
      r.x.bx = window_r;
      r.x.dx = seg;
      __dpmi_int(0x10,&r);

      if (NumberOfPlanes > 1)
      {
        int i;
        int bs = win_size / 4;

        /* now set to read mode 0 */
        outportw(VGA_GR_CTRL_PORT,((0x00 << 8) | VGA_MODE_REG));
        for (i=0;i<4;i++)
        {
          /* read from plane 'i' */
          outportw(VGA_GR_CTRL_PORT,((i << 8) | VGA_RD_PLANE_SEL_REG));

          movedata(_dos_ds,win_seg_r+off_in_page,_my_ds(),(int)buffer+offset,bs);
          offset += bs;
        }
        off_in_page+=bs;
        if (off_in_page>=win_size)
          {
           off_in_page=0;
           seg++;
          }
      }
    }
    // SET: Text modes are 1 plane but some BIOS like the S3 ones reports it
    // as 4 planes because the fonts are located in the second pair of banks.
    // S3 BIOSes are normally buggy so I guess that's just another bug by S3.
    if (NumberOfPlanes==1 || !(ModeAttributes & 0x10))
    {
      movedata(_dos_ds,win_seg_r,_my_ds(),(int)buffer+offset,win_size);
      offset += win_size;
      seg++;
    }
  }
//  restore_vesa_state();
}

__inline__ void
_farnsandl(unsigned long offset,
	 unsigned long value)
{
  __asm__ __volatile__ (".byte 0x64\n"
      "	andl %k0,(%k1)"
      :
      : "ri" (value), "r" (offset));
}

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

void rh_restore_vesa()
{
  int offset = 0, off_in_page = 0, off;
  int seg = 0;
  __dpmi_regs r;
  r.x.ax = 0x4f02;
  r.x.bx = user_mode | 0x8000;
  __dpmi_int(0x10,&r);
  restore_vesa_state();
  while (offset < saved_size)
  {
    if (ModeAttributes & 0x10) // graphic ??
    {
      r.x.ax = 0x4f05;
      r.x.bx = window_w;
      r.x.dx = seg;
      __dpmi_int(0x10,&r);

      r.x.ax = 0x4f05;
      r.x.bx = window_r;
      r.x.dx = seg;
      __dpmi_int(0x10,&r);

      if (NumberOfPlanes > 1)
      {
        int i,j;
        int bs = win_size / 4;

        outportw(VGA_GR_CTRL_PORT,(VGA_FUNC_SET << 8) | VGA_ROT_FN_SEL_REG);
        /* enable all planes for writing */
        outportw(VGA_SEQUENCER_PORT,((0x0f << 8) | VGA_WRT_PLANE_ENB_REG));
        /* eanable all planes for set/reset */
        outportw(VGA_GR_CTRL_PORT,((0x00 << 8) | VGA_SET_RESET_ENB_REG));
        /* set all planes for reset */
        outportw(VGA_GR_CTRL_PORT,((0x00 << 8) | VGA_SET_RESET_REG));
#if 0
        /* set write mode 3 */
        outportw(VGA_GR_CTRL_PORT,((8+3) << 8) | VGA_MODE_REG);
#endif
        /* clear all */
        _farfill_l(_dos_ds,win_seg_w+off_in_page,bs/4,0xffffffffL);
        unsigned long *_l = (unsigned long *)(((char *)buffer)+offset);
        for (i=0;i<4;i++)
        {
#if 1
          /* enable plane 'i' for writing */
          outportw(VGA_SEQUENCER_PORT,(((1 << i) << 8) | VGA_WRT_PLANE_ENB_REG));
#endif
          /* eanable plane 'i' for set/reset */
          outportw(VGA_GR_CTRL_PORT,(((1 << i) << 8) | VGA_SET_RESET_ENB_REG));
          /* enable plane 'i' for set */
          outportw(VGA_GR_CTRL_PORT,(((1 << i) << 8) | VGA_SET_RESET_REG));
#if 0
          /* set write mode 3 */
          outportw(VGA_GR_CTRL_PORT,((8+3) << 8) | VGA_MODE_REG);
#endif
          _farsetsel(_dos_ds);
          off=win_seg_w+off_in_page;
          for (j=0;j<bs;j+=4)
          {
            _farnsandl(j+off,*_l++);
          }
          offset+=bs;
        }
       off_in_page+=bs;
       if (off_in_page>=win_size)
         {
          off_in_page=0;
          seg++;
         }
      }
    }
    if (NumberOfPlanes==1 || !(ModeAttributes & 0x10))
    {
      movedata(_my_ds(),(int)buffer+offset,_dos_ds,win_seg_w,win_size);
      offset += win_size;
      seg++;
    }
  }
  /* restore again, because of the change of the registers and the bank */
  restore_vesa_state();
}

#ifdef TEST

//#define USE_TV

#include <grx20.h>
#include <stdio.h>
#include <conio.h>
#include <pc.h>

#ifdef USE_TV

#define getch()

#define Uses_TScreen
#if 1
#define Uses_TEditorApp
#include <tv.h>

class IDE : public TEditorApp
{
public:
  IDE() : TProgInit( &IDE::initStatusLine,
                     &IDE::initMenuBar,
		     &IDE::initDeskTop),
	  TEditorApp()
  {}
  void Suspend() { suspend(); }
  void Resume() { resume(); Redraw(); }
};

IDE *app;

#define INIT() app = new IDE(); SUSPEND()
#define SUSPEND() app->Suspend()
#define RESUME() app->Resume()
char *ExpandFileNameToThePointWhereTheProgramWasLoaded(char *x)
{
  return x;
}

#else
#include <tv.h>

#define INIT() SUSPEND()
#define SUSPEND() TScreen::suspend()
#define RESUME() TScreen::resume()
#endif

#else

#define INIT() textmode(3)
#define RESUME() if (!rh_is_vesa_mode()) { printf("Isn't VESA\n"); exit(1); }; rh_save_vesa(); textmode(0x3)
#define SUSPEND() rh_restore_vesa()

#endif
int user_mode;

int main(int argc, char *argv[])
{
  int i;
  char *end;
#ifdef USE_TV
  extern int screen_saving;
  screen_saving = 1;
#endif
  if (argc!=2)
    {
     printf("Error: you must pass to the program the amount of memory to save\n");
     return 1;
    }
  rh_vesa_size=strtol(argv[1],&end,0);
  INIT();
  printf("We are in text mode\n");
  getch();
  GrSetMode(GR_width_height_color_graphics,640,480,256);
  GrSetMode(GR_width_height_color_graphics,1024,768,256);
  GrClearScreen(GrBlack());
  for (i=0;i<16;i++)
  {
    GrFilledBox(i*21,0,i*21+20,15,i);
  }
  GrFilledEllipse(GrMaxX()-100,GrMaxY()-100,50,50,GrAllocColor(0,0,255));
  getch();
  RESUME();
  printf("Now we are in text mode again but the graphic mode was saved\n");
  getch();
  SUSPEND();
  getch();
  RESUME();
  printf("Was restored OK?\n");
  getch();
/*  SUSPEND();
  getch();
  for (i=0;i<16;i++)
  {
    GrFilledBox(i*21,0,i*21+20,15,i);
  }
  GrFilledEllipse(300,200,50,50,GrAllocColor(0,0,255));
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  RESUME();
  getch();
  SUSPEND();
  GrEllipseArc(300,200,50,50,10,10,GR_ARC_STYLE_CLOSE2,GrWhite());
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  GrClearScreen(GrBlack());
  GrFilledEllipse(300,200,50,50,GrAllocColor(0,0,255));
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  GrEllipseArc(300,200,50,50,10,10,GR_ARC_STYLE_CLOSE2,GrWhite());
  GrFilledEllipse(300,200,50,50,GrAllocColor(0,0,255));
  GrFilledEllipse(300,200,50,50,GrWhite());
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  GrTextXY(0,0,"This is a sample text",GrWhite(),GrNOCOLOR);
  RESUME();
  getch();
  SUSPEND();
  getch();
  textmode(3);*/
  return 0;
}
#endif

#endif // __DJGPP__

