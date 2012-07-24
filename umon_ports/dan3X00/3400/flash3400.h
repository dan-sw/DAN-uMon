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
