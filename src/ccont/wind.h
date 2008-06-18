#ifndef __rcs_id__
#ifndef __rcs_id_ccont_wind_h__
#define __rcs_id_ccont_wind_h__
static const char __rcs_id_ccont_wind_h[] = "$Id: wind.h,v 1.2 1999/02/19 09:25:27 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

#ifndef WIND1
#define WIND1(x) WIND(x)
#endif
WIND1(0x8000)
WIND(0x4000)
WIND(0x2000)
WIND(0x1000)
WIND(0x0800)
WIND(0x0400)
WIND(0x0200)
WIND(0x0100)
WIND(0x0080)
WIND(0x0040)
WIND(0x0020)
WIND(0x0010)
WIND(0x0008)
WIND(0x0004)
WIND(0x0002)
WIND(0x0001)
#undef WIND1
#undef WIND
