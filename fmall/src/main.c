/**
 * FujiNet Tools for CLI
 *
 * fmall - Mount all disk slots
 *
 * usage:
 *  fmall
 *
 * Author: Thomas Cherryhomes
 *  <thom.cherryhomes@gmail.com>
 *
 * Released under GPL, see COPYING
 * for details
 */

#include <atari.h>
#include <string.h>
#include <stdlib.h>
#include <peekpoke.h>
#include "sio.h"
#include "conio.h"
#include "err.h"

unsigned char buf[8];

union
{
  unsigned char host[8][32];
  unsigned char rawData[256];
} hostSlots;

union
{
  struct
  {
    unsigned char hostSlot;
    unsigned char mode;
    unsigned char file[36];
  } slot[8];
  unsigned char rawData[304];
} deviceSlots;

/**
 * Remount all disk slots
 */
void remount_all(void)
{
  unsigned char c;

  OS.dcb.ddevic=0x70;
  OS.dcb.dunit=1;
  OS.dcb.dcomnd=0xF4; // Get host slots
  OS.dcb.dstats=0x40;
  OS.dcb.dbuf=&hostSlots.rawData;
  OS.dcb.dtimlo=0x0f;
  OS.dcb.dbyt=256;
  OS.dcb.daux=0;
  siov();

  // Read Device slots
  OS.dcb.ddevic=0x70;
  OS.dcb.dunit=1;
  OS.dcb.dcomnd=0xF2;
  OS.dcb.dstats=0x40;
  OS.dcb.dbuf=&deviceSlots.rawData;
  OS.dcb.dtimlo=0x0f;
  OS.dcb.dbyt=sizeof(deviceSlots.rawData);
  OS.dcb.daux=0;
  siov();
  
  for (c=0;c<8;c++)
    {
      if (deviceSlots.slot[c].hostSlot!=0xFF)
	{
	  // Mount host slot
	  OS.dcb.ddevic=0x70;
	  OS.dcb.dunit=1;
	  OS.dcb.dcomnd=0xF9;
	  OS.dcb.dstats=0x00;
	  OS.dcb.dbuf=NULL;
	  OS.dcb.dtimlo=0x01;
	  OS.dcb.dbyt=0;
	  OS.dcb.daux=deviceSlots.slot[c].hostSlot;
	  siov();

	  // Mount device slot
	  OS.dcb.ddevic=0x70;
	  OS.dcb.dunit=1;
	  OS.dcb.dcomnd=0xF8;
	  OS.dcb.dstats=0x00;
	  OS.dcb.dbuf=NULL;
	  OS.dcb.dtimlo=0x01;
	  OS.dcb.dbyt=0;
	  OS.dcb.daux1=c;
	  OS.dcb.daux2=deviceSlots.slot[c].mode;
	  siov();
	}
    }
}

/**
 * main
 */
int main(void)
{

  OS.lmargn=2;
  
  print("\x9b");

  print("MOUNTING ALL DEVICE SLOTS...");
  remount_all();

  if (OS.dcb.dstats==1)
    {
      print("OK");
    }
  else
    {
      print("ERROR");
    }
  
  if (!_is_cmdline_dos())
    {
      print("\x9bPRESS \xD2\xC5\xD4\xD5\xD2\xCE TO CONTINUE.\x9b");
      get_line(buf,sizeof(buf));
    }
  
  return(0);
}
