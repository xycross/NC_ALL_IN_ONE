const unsigned char Code_Confirm_Table[256] = 
{
//   	0	1	2	3	4	5	6	7	8	9	
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 0 	- 	9
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 10	- 	19
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 20	- 	29
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 30 	- 	39
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 40	- 	49
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 50	- 	59
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 60	- 	69
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 70	- 	79
	0,	0,	0,	0,	0,	1,	1,	0,	0,	1,		// 80	- 	89
	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 90 	- 	99
	0,	1,	1,	0,	0,	1,	1,	0,	0,	0,		// 100 	- 	109
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 110 	- 	119
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 120	- 	129
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 130	- 	139
	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,		// 140 	- 	149
	1,	0,	0,	1,	1,	0,	0,	0,	0,	0,		// 150 	- 	159
	0,	0,	0,	0,	0,	1,	1,	0,	0,	1,		// 160 	- 	169
	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 170 	- 	179
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 180 	- 	189
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 190 	-	199
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 200	- 	209
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 210 	- 	219
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 220 	- 	229
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 230 	- 	239
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 240	- 	249
	0,	0,	0,	0,	0,	0						// 250 	-	255
	
};

const unsigned char Code_Gen_Table[16] = 
{
	0x55,		// 0
	0x56,		// 1
	0x59,		// 2
	0x5A,		// 3
	0x65,		// 4
	0x66,		// 5
	0x69,		// 6
	0x6A,		// 7
	0x95,		// 8
	0x96,		// 9
	0x99,		// A
	0x9A,		// B
	0xA5,		// C
	0xA6,		// D
	0xA9,		// E
	0xAA		// F
};

const unsigned char UnCoded_Table[256] = 
{
//   	0	1	2	3	4	5	6	7	8	9	
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 0 	- 	9
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 10	- 	19
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 20	- 	29
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 30 	- 	39
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 40	- 	49
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 50	- 	59
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 60	- 	69
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 70	- 	79
	0,	0,	0,	0,	0,	0,	1,	0,	0,	2,		// 80	- 	89
	3,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 90 	- 	99
	0,	4,	5,	0,	0,	6,	7,	0,	0,	0,		// 100 	- 	109
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 110 	- 	119
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 120	- 	129
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 130	- 	139
	0,	0,	0,	0,	0,	0,	0,	0,	0,	8,		// 140 	- 	149
	9,	0,	0,	0xA,	0xB,	0,	0,	0,	0,	0,		// 150 	- 	159
	0,	0,	0,	0,	0,	0xC,	0xD,	0,	0,	0xE,		// 160 	- 	169
	0xF,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 170 	- 	179
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 180 	- 	189
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 190 	-	199
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 200	- 	209
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 210 	- 	219
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 220 	- 	229
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 230 	- 	239
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 240	- 	249
	0,	0,	0,	0,	0,	0						// 250 	-	255
	
};

#if 0
unsigned short CRC16(unsigned char *puchMsg, unsigned char usDataLen)
{
	volatile unsigned char uchCRCHi = 0xFF ; /* high byte of CRC initialized */
	volatile unsigned char uchCRCLo = 0xFF ; /* low byte of CRC initialized */
	volatile unsigned uIndex ; /* will index into CRC lookup table */

	while (usDataLen--) /* pass through message buffer */
	{
		uIndex = uchCRCHi ^ *puchMsg++ ; /* calculate the CRC */
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
		uchCRCLo = auchCRCLo[uIndex] ;
	}
	return (uchCRCHi << 8 | uchCRCLo) ;
}
#endif

unsigned char Codiz_DataH4bit(unsigned char _data)
{
	unsigned char pri_data;
	
	pri_data = (_data >>4);
	
	return(Code_Gen_Table[pri_data]);
}

unsigned char Codiz_DataL4bit(unsigned char _data)
{
	unsigned char pri_data;
	
	pri_data = (_data & 0x0f);
	
	return(Code_Gen_Table[pri_data]);
}


void Codiz_From8_To16(unsigned char *p, unsigned char src_data)
{
	unsigned char pri_data;
	
	pri_data = (src_data >>4);
	
	*p++ = Code_Gen_Table[pri_data];

	pri_data = (src_data & 0x0f);

	*p = Code_Gen_Table[pri_data];
}

void Codiz_Data8bit(unsigned char *p, unsigned char data)
{
	*p++ = Codiz_DataH4bit(data);
	*p++ = Codiz_DataL4bit(data);
}

void Codiz_Data12bit(unsigned char *p, unsigned short org_data)
{
	unsigned char i;

	i = org_data >> 8;
	*p++ = Codiz_DataL4bit(i);

	i = org_data & 0x00FF;
	*p++ = Codiz_DataH4bit(i);
	*p++ = Codiz_DataL4bit(i);
}


unsigned char Codiz_Data8bitSum (unsigned char *p, unsigned char data, unsigned char *sum)
{
	unsigned char char_buf0;

	char_buf0 =  (unsigned char)(data  & 0x00ff);

	if (sum) {
		*sum += *p++ = Codiz_DataH4bit(char_buf0);
		*sum += *p++ = Codiz_DataL4bit(char_buf0);
	}
	else {
		*p++ = Codiz_DataH4bit(char_buf0);
		*p++ = Codiz_DataL4bit(char_buf0);
	}
	return 2;
}

unsigned char Codiz_Data12bitSum (unsigned char *p, unsigned int data, unsigned char *sum)
{
	unsigned char char_buf0, char_buf1;

	char_buf0 =  (unsigned char)(data >> 8);
	char_buf1 =  (unsigned char)(data  & 0x00ff);


	if (sum) {
		*sum += *p++ = Codiz_DataL4bit(char_buf0);
		*sum += *p++ = Codiz_DataH4bit(char_buf1);
		*sum += *p++ = Codiz_DataL4bit(char_buf1);
	}
	else {
		*p++ = Codiz_DataL4bit(char_buf0);
		*p++ = Codiz_DataH4bit(char_buf1);
		*p++ = Codiz_DataL4bit(char_buf1);
	}
	return 3;
}


void Codiz_Data16bit(unsigned char *p, unsigned short data)
{
	unsigned char char_buf0, char_buf1;

	char_buf0 =  (unsigned char)(data >> 8);
	char_buf1 =  (unsigned char)(data  & 0x00ff);

	*p++ = Codiz_DataH4bit(char_buf0);
	*p++ = Codiz_DataL4bit(char_buf0);
	*p++ = Codiz_DataH4bit(char_buf1);
	*p++ = Codiz_DataL4bit(char_buf1);
	
}

unsigned char Codiz_Data16bitSum (unsigned char *p, unsigned int data, unsigned char *sum)
{
	unsigned char char_buf0, char_buf1;

	char_buf0 =  (unsigned char)(data >> 8);
	char_buf1 =  (unsigned char)(data  & 0x00ff);

	if (sum) {
		*sum += *p++ = Codiz_DataH4bit(char_buf0);
		*sum += *p++ = Codiz_DataL4bit(char_buf0);
		*sum += *p++ = Codiz_DataH4bit(char_buf1);
		*sum += *p++ = Codiz_DataL4bit(char_buf1);
	}
	else {
		*p++ = Codiz_DataH4bit(char_buf0);
		*p++ = Codiz_DataL4bit(char_buf0);
		*p++ = Codiz_DataH4bit(char_buf1);
		*p++ = Codiz_DataL4bit(char_buf1);
	}
	return 4;
}


unsigned char UnCodiz_From16_To8(unsigned char *src_data)
{
	unsigned char pri_data, sec_data;


	pri_data = UnCoded_Table[*src_data];

	pri_data = pri_data << 4;

	sec_data = *(src_data + 1);

	pri_data += UnCoded_Table[sec_data];

	return pri_data;
}


unsigned char UnCodiz_8bit(unsigned char _data0, unsigned char _data1)
{
	unsigned char out_data;
	
	out_data = UnCoded_Table[_data0];
	out_data = out_data << 4;
	out_data += UnCoded_Table[_data1];
	
	return(out_data);
}

unsigned char UnCodiz_4bit(unsigned char _data0)
{
	unsigned char out_data;

	out_data = UnCoded_Table[_data0];
	
	return(out_data);
	
}

unsigned char UnCodiz_8bitSum (unsigned char *p, unsigned char *data, unsigned char *sum)
{

	if (sum) {
		*sum += *p;
		*sum += *(p+1);
	}

	*data = UnCodiz_8bit(*(p+0), *(p+1));

	return 2;
}

unsigned char UnCodiz_12bitSum (unsigned char *p, unsigned int *data, unsigned char *sum)
{
	unsigned dath, datl;

	if (sum) {
		*sum += *(p+0);
		*sum += *(p+1);
		*sum += *(p+2);
	}
	dath = UnCodiz_8bit(    0, *(p+0));
	datl = UnCodiz_8bit(*(p+1), *(p+2));

	*data = (dath << 8) | (datl & 0x00ff);
	return 3;
}


unsigned short UnCodiz_12bit(unsigned char *p)
{
	unsigned short out_data;
	out_data = UnCoded_Table[*p];
	out_data = out_data << 4;
	p++;

	out_data += UnCoded_Table[*p];
	out_data = out_data << 4;
	p++;

	out_data += UnCoded_Table[*p];

	return out_data;
}

void UnCodiz_16bit(unsigned char *p, unsigned short *data)
{
	unsigned char dath, datl;
	
	dath = UnCodiz_8bit(*(p+0), *(p+1));
	datl = UnCodiz_8bit(*(p+2), *(p+3));
	*data = (dath << 8) | (datl & 0x00ff);
}

unsigned char UnCodiz_16bitSum (unsigned char *p, unsigned int *data, unsigned char *sum)
{
	unsigned dath, datl;

	if (sum) {
		*sum += *(p+0);
		*sum += *(p+1);
		*sum += *(p+2);
		*sum += *(p+3);
	}

	dath = UnCodiz_8bit(*(p+0), *(p+1));
	datl = UnCodiz_8bit(*(p+2), *(p+3));
	*data = (dath << 8) | (datl & 0x00ff);
	return 4;
}

unsigned char Conv_2ASC_1Byte(unsigned char first_byte, unsigned char second_byte)
{
	unsigned char r_value;

	if(first_byte < 0x3A)
	{
		r_value = (first_byte - '0') << 4;
	}
	else
	{
		r_value = (first_byte -'A' + 10) << 4;
	}

	if(second_byte < 0x3A)
	{
		r_value += (second_byte - '0');
	}
	else
	{
		r_value += (second_byte - 'A' + 10);
	}

	return r_value;
}

unsigned short Conv_4ASC_2Byte(unsigned char *pos)
{
	unsigned short r_value = 0;
	unsigned char i;

	for(i=0;i<4;i++)
	{
		r_value *= 10;
		if(*pos < 0x3A)
		{
			r_value += (*pos - '0');
		}
		else
		{
			r_value += (*pos -'A' + 10);
		}
		pos++;
	}

	return r_value;
}

unsigned short Conv_4ASC_2Hex(unsigned char *pos)
{
	unsigned short r_value = 0;
	unsigned char i;

	for(i=0;i<4;i++)
	{
		r_value *= 16;
		if(*pos < 0x3A)
		{
			r_value += (*pos - '0');
		}
		else
		{
			r_value += (*pos -'A' + 10);
		}
		pos++;
	}

	return r_value;
}


unsigned int Conv_5ASC_2Byte(unsigned char *pos)
{
	unsigned int r_value = 0;
	unsigned char i;

	for(i=0;i<5;i++)
	{
		r_value *= 10;
		if(*pos < 0x3A)
		{
			r_value += (*pos - '0');
		}
		else
		{
			r_value += (*pos -'A' + 10);
		}
		pos++;
	}

	return r_value;
}

unsigned int Conv_4ASC_2Byte_10(unsigned char *pos)
{
	unsigned int r_value = 0;
	unsigned char i;

	for(i=0;i<4;i++)
	{
		r_value *= 10;
		if(*pos < 0x3A)
		{
			r_value += (*pos - '0');
		}
		else
		{
			r_value += (*pos -'A' + 10);
		}
		pos++;
	}

	return r_value * 10;
}


unsigned int Conv_5ASC_2Hex(unsigned char *pos)
{
	unsigned int r_value = 0;
	unsigned char i;

	for(i=0;i<5;i++)
	{
		r_value *= 16;
		if(*pos < 0x3A)
		{
			r_value += (*pos - '0');
		}
		else
		{
			r_value += (*pos - 'A' + 10);
		}
		pos++;
	}

	return r_value;
}



unsigned char Conv_1ASC_1Byte(unsigned char value)
{
	if(value < 0x3A)
	{
		return (value - '0');
	}
	else
	{
		return (value - 'A' + 10);
	}
}

unsigned char Conv_1Byte_Nibble_Asc(unsigned char value)
{
	value = value & 0x0F;
	
	if(value >= 0x0A)
	{
		return(value - 10 + 'A');
	}
	else
	{
		return(value + '0');
	}
}


unsigned short Conv_1Byte_2ASC(unsigned char pos)
{
	unsigned char first, second;
	unsigned short asc;
	
	first 	= pos >> 4;
	second	= (pos & 0x0f);
	
	if(first >  9){ first += 55; }
	else{ first += 48; }
	if(second >  9){ second += 55; }
	else{ second += 48; }
	
	asc = first;
	asc <<= 8;
	asc |= second;

	return asc;	
	
}

unsigned short Conv_Int_2ASC(unsigned char pos)
{
	unsigned char first, second;
	unsigned short asc;

	first = pos / 10;
	second = pos % 10;
	if(first < 9)
		first += 0x30;
	if(second < 9)
		second += 0x30;
	asc = first;
	asc <<= 8;
	asc |= second;

	return asc;
}

unsigned short Conv_2Byte_1short(unsigned char first, unsigned char second)
{
	unsigned short value;
	
	value = first;
	value <<= 8;
	value |= second;

	return value;

	
}

unsigned int Conv_1Word_4ASC(unsigned short pos)
{
	unsigned char first, second, third, fourth;
	unsigned int asc;
	
	first	 = (unsigned char)((pos >> 12)& 0x000f);
	second	 = (unsigned char)((pos >> 8) & 0x000f);
	third	 = (unsigned char)((pos >> 4) & 0x000f);
	fourth	 = (unsigned char)(pos& 0x000f);
	
	if(first > 9){ first += 55; }
	else{ first += 48; }
	
	if(second > 9){ second += 55; }
	else{ second += 48; }
	
	if(third > 9){ third += 55; }
	else{ third += 48; }
	
	if(fourth > 9){ fourth += 55; }
	else{ fourth += 48; }	
	
	
	asc = first;
	asc <<= 8;
	
	asc |= second;
	asc <<= 8;
	
	asc |= third;
	asc <<= 8;
	
	asc |= fourth;
	
	return asc;
}


void Conv_Short_4Byte_Asc(unsigned char *pos, unsigned short value)
{
	unsigned char temp;

	temp = value & 0x000F;

	if(temp > 0x09) *(pos + 3) = temp - 10 + 'A';
	else *(pos + 3) = temp + '0';

	value = value >> 4;
	temp = value & 0x000F;

	if(temp > 0x09) *(pos + 2) = temp - 10 + 'A';
	else *(pos + 2) = temp + '0';

	value = value >> 4;
	temp = value & 0x000F;

	if(temp > 0x09) *(pos + 1) = temp - 10 + 'A';
	else *(pos + 1) = temp + '0';

	value = value >> 4;
	temp = value & 0x000F;

	if(temp > 0x09) *pos = temp - 10 + 'A';
	else *pos = temp + '0';

	
}



void Conv_Short_4Byte_2Asc(unsigned char *pos, unsigned short _data)
{
	unsigned char temp1, temp2;

	temp1 = _data >> 8;

	temp2 = temp1 >> 4;
	
	if(temp2 > 0x09) *pos = temp2 - 10 + 'A';
	else *pos = temp2 + '0';

	temp2 = temp1 & 0x0F;

	if(temp2 > 0x09) *(pos + 1) = temp2 - 10 + 'A';
	else *(pos + 1) = temp2 + '0';

	temp1 = _data & 0x00FF;


	temp2 = temp1 >> 4;

	if(temp2 > 0x09) *(pos + 2) = temp2 - 10 + 'A';
	else *(pos + 2) = temp2 + '0';
	

	temp2 = temp1 & 0x0F;

	if(temp2 > 0x09) *(pos + 3) = temp2 - 10 + 'A';
	else *(pos + 3) = temp2 + '0';

}

