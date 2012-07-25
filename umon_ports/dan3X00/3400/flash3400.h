/*
All files except if stated otherwise in the begining of the file are under the GPLv2 license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/
/**********************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  flash3400.h
 Purpose    :  Flash driver port for DAN3400
 Owner		:  Sergey Krasnitsky
 Created    :  2.11.2010
\**********************************************************************/

extern int  Flash3400_erase (struct flashinfo *fdev, int snum);
extern void EndFlash3400_erase (void);

extern int  Flash3400_write (struct flashinfo *fdev, unsigned char *dest, unsigned char *src, long bytecnt);
extern void EndFlash3400_write (void);

extern int  Flash3400_ewrite (struct flashinfo *fdev, unsigned char *dest, unsigned char *src, long bytecnt);
extern void EndFlash3400_ewrite (void);

extern int  Flash3400_lock (struct flashinfo *fdev, int snum, int operation);
extern void EndFlash3400_lock (void);

extern int  Flash3400_type (struct flashinfo *fdev);
extern void EndFlash3400_type (void);

extern void FLASH_Read(uint8 bank, uint8 *data, unsigned addr, unsigned len);
extern int FLASH_Write(uint8 bank, unsigned addr, uint8* data, unsigned len);
extern int FLASH_EraseSector(uint8 bank, unsigned secnum);
extern void FLASH_Read_ID(uint8 bank, uint32 *jedec, uint16 *ext_jedec);
