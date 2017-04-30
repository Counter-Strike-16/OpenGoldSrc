#include <dpmi.h>

//#include "types.h"
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

//#include "mgenord.h"
#define MGENVXD_REGISTER_ORD       1
#define MGENVXD_GETMEM_ORD         2
#define MGENVXD_DEREGISTER_ORD     3
#define MGENVXD_WAKEUP_ORD         4
#define MGENVXD_MAKEDQS_ORD        5


// Virtual 8086 API Ordinals
#define V86API_GETSELECTOR16_ORD (1)
#define V86API_GETSELECTOR32_ORD (2)
#define V86API_GETFLAT32_ORD (3)
#define V86API_MOVERP_ORD (6)
#define V86API_MOVEPR_ORD (7)
#define V86API_POST_ORD   (8)
#define V86API_INIT_ORD    (9)
#define V86API_UNINIT_ORD  (10)
#define V86API_INSERTKEY_ORD (11)
#define V86API_REMOVEHOTKEY_ORD  (12)
#define V86API_INSTALLHOTKEY_ORD (13)
#define V86API_HOOKINT48_ORD  (14)
#define V86API_WAKEUPDLL_ORD (15)

// these are DPMI functions.  Make sure they don't clash with the
// other MGENVXD_XXXX functions above, or the DPMI functions!
#define MGENVXD_GETQUEUECTR_ORD    6
#define MGENVXD_MOVENODE_ORD       7
#define MGENVXD_GETNODE_ORD        8
#define MGENVXD_FLUSHNODE_ORD      9
#define MGENVXD_MCOUNT_ORD         10
#define MGENVXD_MASTERNODE_ORD     11
#define MGENVXD_SANITYCHECK_ORD    12
#define MGENVXD_WAKEUPDLL_ORD       13
#define MGENVXD_WAIT_ORD           14

//
#define HWND_OFFSET (0)
#define UMSG_OFFSET (1)
#define SIZEREQUEST_OFFSET (2)
#define HVXD_OFFSET (3)
#define DATUM_OFFSET (4)
#define SLOT_OFFSET (5)
#define SIZEGIVEN_OFFSET (6)
#define SELECTOR32_OFFSET (7)
#define SELECTOR16_OFFSET (8)

//#include "magic.h"
#define MGENVXD_DEVICE_ID 0x18AA

//#include "rtq.h"
#define RTQ_NODE struct rtq_node

RTQ_NODE
   {
      RTQ_NODE *self; // Ring zero address of this node
      RTQ_NODE *left; // Ring zero address of preceding node
      RTQ_NODE *right; // Ring zero address of succeding node
      BYTE *      rtqDatum;  // Ring 3 Datum of Buffer (start of preface)
      BYTE *      rtqInsert; // Ring 3 insertion position
      WORD     rtqLen; // Length of buffer, excluding preface
      WORD     rtqUpCtr;  // Up Counter of bytes used so far
      WORD     rtqQCtr;   // number of nodes attached
      WORD     padding;   // DWORD alignment
   };

#define RTQ_PARAM_MOVENODE struct rtq_param_movenode
RTQ_PARAM_MOVENODE
   {
      WORD     rtqFromDQ;
      WORD     rtqToDQ;
   };

RTQ_NODE* rtq_fetch(RTQ_NODE*, RTQ_NODE*); // To, From

#define CHUNNEL_INT 0x48

#define int386		_int86

void
Yield(void)
{
	__dpmi_yield();
}