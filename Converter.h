

#ifndef __CONVERTER_H_
#define	__CONVERTER_H_




unsigned short CRC16(unsigned char *, unsigned char);

unsigned char Codiz_DataH4bit(unsigned char);
unsigned char Codiz_DataL4bit(unsigned char);

void Codiz_Data8bit(unsigned char *, unsigned char);
void Codiz_Data12bit(unsigned char *p, unsigned short);
void Codiz_Data16bit(unsigned char *, unsigned short);

void Codiz_From8_To16(unsigned char *, unsigned char);
	
unsigned char Codiz_Data8bitSum(unsigned char *, unsigned char, unsigned char *);
unsigned char Codiz_Data12bitSum(unsigned char *, unsigned int, unsigned char *);
unsigned char Codiz_Data16bitSum(unsigned char *, unsigned int, unsigned char *);

unsigned char UnCodiz_From16_To8(unsigned char *);

unsigned char UnCodiz_8bit(unsigned char, unsigned char);
unsigned char UnCodiz_8bitSum(unsigned char *, unsigned char *, unsigned char *);
unsigned char UnCodiz_12bitSum(unsigned char *, unsigned int *, unsigned char *);
unsigned char UnCodiz_16bitSum(unsigned char *, unsigned int *, unsigned char *);

unsigned short UnCodiz_12bit(unsigned char *p);
void UnCodiz_16bit(unsigned char *, unsigned short *);

unsigned char UnCodiz_4bit(unsigned char);

unsigned char Conv_2ASC_1Byte(unsigned char, unsigned char);
unsigned short Conv_4ASC_2Byte(unsigned char *);
unsigned short Conv_4ASC_2Hex(unsigned char *);

unsigned int Conv_5ASC_2Byte(unsigned char *);
unsigned int Conv_4ASC_2Byte_10(unsigned char *);
unsigned int Conv_5ASC_2Hex(unsigned char *);
unsigned char Conv_1ASC_1Byte(unsigned char);
unsigned char Conv_1Byte_Nibble_Asc(unsigned char);
unsigned short Conv_1Byte_2ASC(unsigned char pos);
unsigned short Conv_2Byte_1short(unsigned char first, unsigned char second);
unsigned int Conv_1Word_4ASC(unsigned short pos);
void Conv_Short_4Byte_Asc(unsigned char *, unsigned short);
void Conv_Short_4Byte_2Asc(unsigned char *, unsigned short);


unsigned short Conv_Int_2ASC(unsigned char pos);





#endif	/* CONVERTER_H */

