#ifndef _EINK_H_
#define _EINK_H_


void einkClearData(void);
void einkUpdate(void);
void einkPuts(const char* str,char l);

extern unsigned char display_data[20];
extern const unsigned int font_data[];
extern unsigned char displayInverse;



#endif