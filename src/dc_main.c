
#include "dc_main.h"
#include "sq.h"

void Vertical_scroll_test(void);
void Horizontal_scroll_test(void);

// 1 (interlaced) or 0 (progressive)
#define INTERLACED_TEST 1
// 5, 10, 15, 30, 60
#define FPS_TEST 15
#define TIME_SEC 10

// With RGB0888 mode:
// 640x480 progressive maxes out at 20fps with this test with memset to 0xa5000000
// 640x480 progressive maxes out at 30fps with this test with store queues to 0xa5000000
// 640x480 progressive hits 60fps no problem with store queues to 0xb1000000 and LMMODE0 = 1
// At 60fps, 15 loops = 1 second and 150 loops = 10 seconds
// At 30fps, 15/2 loops = 1 second and 75 loops = 10 seconds
// At 15fps, 15/4 loops = 1 second and 38 loops = 10 seconds
// At 10fps, 15/6 loops = 1 second and 25 loops = 10 seconds
// At 5fps, 15/12 loops = 1 second and 12.5 (or 13) loops = 10 seconds
// This approximation is "good enough"
#define LOOP_COUNTER ((FPS_TEST/4) + 1) * TIME_SEC

#define MAX_WIDTH 640
#define MAX_HEIGHT 480
//#define MAX_WIDTH 320
//#define MAX_HEIGHT 240
#define BYTES_PIXEL 4

// standard
#define VBLANK_WAIT 0x01ff
// wide
//#define VBLANK_WAIT 0x01f0

//RGB0888_PIXEL* vram_32 = (RGB0888_PIXEL*)0xa5000000;
RGB0888_PIXEL* vram_32 = (RGB0888_PIXEL*)0xb1000000;

RGB888_PIXEL* vram_24 = (RGB888_PIXEL*)0xa5000000;
RGB565_PIXEL* vram_16 = (RGB565_PIXEL*)0xa5000000;

static inline __attribute__((always_inline)) void * memset_32bit_in(void *dest, const uint32_t val, size_t len)
{
  uint32_t *ptr = (uint32_t*)dest;

  while (len--)
  {
    *ptr++ = val;
  }

  return dest;
}

// main function
int dreamcast_main(void)
{
  // First things first: let's get a more modern color mode up
  // Set up color mode and resolution (cable type and console region are auto-detected by this)
  STARTUP_Init_Video(FB_RGB0888, USE_640x480);
  //  STARTUP_Init_Video(FB_RGB0888, USE_320x240);
//  asm volatile ("sleep");
  // Set LMMODE0 to 32bit
  *(volatile uint32_t*)0xa05F6884 = 0x1;
  // WEEEEEEEEEEEEEEEE
  // Note that framebuffer is only 24-bit in RGB888 mode

  if(STARTUP_dcload_present == DCLOAD_CONSOLE)
  {
    if(DCLOAD_type == DCLOAD_TYPE_IP)
    {
      dcloadsyscall(DCLOAD_WRITE, 1, "IP: Success!\n", 15);
    }
    else if(DCLOAD_type == DCLOAD_TYPE_SER)
    {
      dcloadsyscall(DCLOAD_WRITE, 1, "Serial: Success!\n", 19);
    }
    else
    {
      dcloadsyscall(DCLOAD_WRITE, 1, "Failure!\n", 11);
    }
  }

  printf("Printf test! 0x%x\r\n", STARTUP_dcload_present);

  char test_array[20] = {0};

  printf("%s\n", hex_to_string(1 << 31, test_array));
  printf("%s\n", hex_to_string(1024, test_array));

  printf("%s\n", uint_to_string(0xffffffff, test_array));
  printf("%s\n", uint_to_string(1 << 31, test_array));
  printf("%s\n", uint_to_string(1024, test_array));

  printf("%s\n", int_to_string(1 << 31, test_array));
  printf("%s\n", int_to_string(-2, test_array));
  printf("%s\n", int_to_string(-1, test_array));

  printf("%s\n", float_to_string(0.0f, 1, test_array));
  printf("%s\n", float_to_string(5.0f, 3, test_array));
  printf("%s\n", float_to_string(1.252f, 3, test_array));
  printf("%s\n", float_to_string(1.928401f, 3, test_array));
  printf("%s\n", float_to_string(1.928401f, 2, test_array));

  printf("%s\n", float_to_string(-5.0f, 3, test_array));
/*
  for(float p = 4.0f/3.0f; p <= 100.0f; p += 1.0f)
  {
    printf("%s\n", float_to_string(p, 3, test_array));
  }
*/
  Horizontal_scroll_test();

  // NOTE: Modes accepted over HDMI are not necessarily the same as modes accepted over VGA
  STARTUP_1280x960_VGA(FB_RGB0888);

  Vertical_scroll_test();

  // Set LMMODE0 back to 64 bit
  *(volatile uint32_t*)0xa05F6884 = 0x0;
  // Reset video mode for dcload
  STARTUP_Set_Video(FB_RGB0555, USE_640x480);

  return 0; // startup.S doesn't care what this is
}

void Vertical_scroll_test(void)
{
  volatile uint32_t* vblank = (volatile uint32_t*)0xa05f810c;
  // At 60fps, 15 loops = 1 second and 150 loops = 10 seconds.
  // At 30fps, 15/2 loops = 1 second and 75 loops = 10 seconds
  // At 10fps, 15/6 loops = 1 second and 25 loops = 10 seconds
  // At 5fps, 15/12 loops = 1 second and 12.5 (or 13) loops = 10 seconds
  uint32_t counter = LOOP_COUNTER;
  while(counter--) // each loop contains 4 frames
  {
    //sq_set32(vram_32, 0x00FF0000, 640 * 480 * 4);
    //memset_32bit_in(vram_32, 0x00FF0000, 640*480);

//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*0), 0x000000FF, MAX_WIDTH*MAX_HEIGHT/4);
//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*1), 0x00FF0000, MAX_WIDTH*MAX_HEIGHT/4);
//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*2), 0x0000FF00, MAX_WIDTH*MAX_HEIGHT/4);
//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*3), 0x00000000, MAX_WIDTH*MAX_HEIGHT/4);

    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*0), 0x000000FF, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);
    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*1), 0x00FF0000, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);
    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*2), 0x0000FF00, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);
    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*3), 0x00000000, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);

#if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#else
// uncapped
#endif

#if INTERLACED_TEST == 1
// interlaced, double everything
  #if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #else
  // uncapped
  #endif
#else
// progressive
#endif

    //sq_set32(vram_32, 0x000000FF, 640 * 480 * 4);
    //memset_32bit_in(vram_32, 0x000000FF, 640*480);

//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*1), 0x000000FF, MAX_WIDTH*MAX_HEIGHT/4);
//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*2), 0x00FF0000, MAX_WIDTH*MAX_HEIGHT/4);
//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*3), 0x0000FF00, MAX_WIDTH*MAX_HEIGHT/4);
//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*0), 0x00000000, MAX_WIDTH*MAX_HEIGHT/4);

    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*1), 0x000000FF, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);
    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*2), 0x00FF0000, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);
    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*3), 0x0000FF00, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);
    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*0), 0x00000000, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);

#if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#else
// uncapped
#endif

#if INTERLACED_TEST == 1
// interlaced, double everything
  #if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #else
  // uncapped
  #endif
#else
// progressive
#endif

    //sq_set32(vram_32, 0x0000FF00, 640 * 480 * 4);
    //memset_32bit_in(vram_32, 0x0000FF00, 640*480);

//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*2), 0x000000FF, MAX_WIDTH*MAX_HEIGHT/4);
//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*3), 0x00FF0000, MAX_WIDTH*MAX_HEIGHT/4);
//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*0), 0x0000FF00, MAX_WIDTH*MAX_HEIGHT/4);
//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*1), 0x00000000, MAX_WIDTH*MAX_HEIGHT/4);

    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*2), 0x000000FF, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);
    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*3), 0x00FF0000, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);
    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*0), 0x0000FF00, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);
    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*1), 0x00000000, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);

#if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#else
// uncapped
#endif

#if INTERLACED_TEST == 1
// interlaced, double everything
  #if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #else
  // uncapped
  #endif
#else
// progressive
#endif

    //sq_set32(vram_32, 0x00000000, 640 * 480 * 4);
    //memset_32bit_in(vram_32, 0x00000000, 640*480);

//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*3), 0x000000FF, MAX_WIDTH*MAX_HEIGHT/4);
//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*0), 0x00FF0000, MAX_WIDTH*MAX_HEIGHT/4);
//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*1), 0x0000FF00, MAX_WIDTH*MAX_HEIGHT/4);
//    memset_32bit_in(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*2), 0x00000000, MAX_WIDTH*MAX_HEIGHT/4);

    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*3), 0x000000FF, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);
    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*0), 0x00FF0000, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);
    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*1), 0x0000FF00, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);
    sq_set32(vram_32 + (MAX_WIDTH*MAX_HEIGHT/4*2), 0x00000000, BYTES_PIXEL*MAX_WIDTH*MAX_HEIGHT/4);

#if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#else
// uncapped
#endif

#if INTERLACED_TEST == 1
// interlaced, double everything
  #if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #else
  // uncapped
  #endif
#else
// progressive
#endif
  }
}

void Horizontal_scroll_test(void)
{
  uint32_t height = 0;
  volatile uint32_t* vblank = (volatile uint32_t*)0xa05f810c;
  // At 60fps, 15 loops = 1 second and 150 loops = 10 seconds.
  // At 30fps, 15/2 loops = 1 second and 75 loops = 10 seconds
  // At 10fps, 15/6 loops = 1 second and 25 loops = 10 seconds
  // At 5fps, 15/12 loops = 1 second and 12.5 (or 13) loops = 10 seconds
  uint32_t counter = LOOP_COUNTER;
  while(counter--) // each loop contains 4 frames
  {
    for(height = 0; height < MAX_HEIGHT; height++)
    {
//      memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 0 + height*MAX_WIDTH, 0x000000FF, MAX_WIDTH / 4);
//      memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 1 + height*MAX_WIDTH, 0x00FF0000, MAX_WIDTH / 4);
//      memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 2 + height*MAX_WIDTH, 0x0000FF00, MAX_WIDTH / 4);
//      memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 3 + height*MAX_WIDTH, 0x00000000, MAX_WIDTH / 4);

      sq_set32(vram_32 + (MAX_WIDTH / 4) * 0 + height*MAX_WIDTH, 0x000000FF, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
      sq_set32(vram_32 + (MAX_WIDTH / 4) * 1 + height*MAX_WIDTH, 0x00FF0000, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
      sq_set32(vram_32 + (MAX_WIDTH / 4) * 2 + height*MAX_WIDTH, 0x0000FF00, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
      sq_set32(vram_32 + (MAX_WIDTH / 4) * 3 + height*MAX_WIDTH, 0x00000000, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
    }

#if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#else
// uncapped
#endif

#if INTERLACED_TEST == 1
// interlaced, double everything
  #if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #else
  // uncapped
  #endif
#else
// progressive
#endif

    for(height = 0; height < MAX_HEIGHT; height++)
    {
      //memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 1 + height*MAX_WIDTH, 0x000000FF, MAX_WIDTH / 4);
      //memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 2 + height*MAX_WIDTH, 0x00FF0000, MAX_WIDTH / 4);
      //memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 3 + height*MAX_WIDTH, 0x0000FF00, MAX_WIDTH / 4);
      //memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 0 + height*MAX_WIDTH, 0x00000000, MAX_WIDTH / 4);

      sq_set32(vram_32 + (MAX_WIDTH / 4) * 1 + height*MAX_WIDTH, 0x000000FF, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
      sq_set32(vram_32 + (MAX_WIDTH / 4) * 2 + height*MAX_WIDTH, 0x00FF0000, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
      sq_set32(vram_32 + (MAX_WIDTH / 4) * 3 + height*MAX_WIDTH, 0x0000FF00, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
      sq_set32(vram_32 + (MAX_WIDTH / 4) * 0 + height*MAX_WIDTH, 0x00000000, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
    }

#if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#else
// uncapped
#endif

#if INTERLACED_TEST == 1
// interlaced, double everything
  #if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #else
  // uncapped
  #endif
#else
// progressive
#endif

    for(height = 0; height < MAX_HEIGHT; height++)
    {
      //memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 2 + height*MAX_WIDTH, 0x000000FF, MAX_WIDTH / 4);
      //memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 3 + height*MAX_WIDTH, 0x00FF0000, MAX_WIDTH / 4);
      //memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 0 + height*MAX_WIDTH, 0x0000FF00, MAX_WIDTH / 4);
      //memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 1 + height*MAX_WIDTH, 0x00000000, MAX_WIDTH / 4);

      sq_set32(vram_32 + (MAX_WIDTH / 4) * 2 + height*MAX_WIDTH, 0x000000FF, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
      sq_set32(vram_32 + (MAX_WIDTH / 4) * 3 + height*MAX_WIDTH, 0x00FF0000, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
      sq_set32(vram_32 + (MAX_WIDTH / 4) * 0 + height*MAX_WIDTH, 0x0000FF00, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
      sq_set32(vram_32 + (MAX_WIDTH / 4) * 1 + height*MAX_WIDTH, 0x00000000, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
    }

#if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#else
// uncapped
#endif

#if INTERLACED_TEST == 1
// interlaced, double everything
  #if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #else
  // uncapped
  #endif
#else
// progressive
#endif

    for(height = 0; height < MAX_HEIGHT; height++)
    {
      //memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 3 + height*MAX_WIDTH, 0x000000FF, MAX_WIDTH / 4);
      //memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 0 + height*MAX_WIDTH, 0x00FF0000, MAX_WIDTH / 4);
      //memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 1 + height*MAX_WIDTH, 0x0000FF00, MAX_WIDTH / 4);
      //memset_32bit_in(vram_32 + (MAX_WIDTH / 4) * 2 + height*MAX_WIDTH, 0x00000000, MAX_WIDTH / 4);

      sq_set32(vram_32 + (MAX_WIDTH / 4) * 3 + height*MAX_WIDTH, 0x000000FF, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
      sq_set32(vram_32 + (MAX_WIDTH / 4) * 0 + height*MAX_WIDTH, 0x00FF0000, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
      sq_set32(vram_32 + (MAX_WIDTH / 4) * 1 + height*MAX_WIDTH, 0x0000FF00, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
      sq_set32(vram_32 + (MAX_WIDTH / 4) * 2 + height*MAX_WIDTH, 0x00000000, BYTES_PIXEL*MAX_WIDTH / 4); //sq_set32 cuts off excess
    }

#if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
#else
// uncapped
#endif

#if INTERLACED_TEST == 1
// interlaced, double everything
  #if FPS_TEST == 60
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 30
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 15
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 10
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #elif FPS_TEST == 5
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
      while(!(*vblank & VBLANK_WAIT));
      while(*vblank & VBLANK_WAIT);
  #else
  // uncapped
  #endif
#else
// progressive
#endif
  }
}
