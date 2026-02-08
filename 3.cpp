#include<dos.h>
#include<stdio.h>
#include<stdlib.h>

void videoprint(char* str, int x, int y, unsigned char attribute);
void strconvert(unsigned char temp, char *str);
void print();

static unsigned char attr = 0xCF;

void interrupt (*old_08)(...);// base -> master;
void interrupt (*old_09)(...);
void interrupt (*old_10)(...);
void interrupt (*old_11)(...);
void interrupt (*old_12)(...);
void interrupt (*old_13)(...);
void interrupt (*old_14)(...);
void interrupt (*old_15)(...);

void interrupt (*old_70)(...); // base -> slave
void interrupt (*old_71)(...);
void interrupt (*old_72)(...);
void interrupt (*old_73)(...);
void interrupt (*old_74)(...);
void interrupt (*old_75)(...);
void interrupt (*old_76)(...);
void interrupt (*old_77)(...);

void interrupt new_08(...)
{
	print();
	(*old_08)();
}
void interrupt new_09(...)
{
	print();
	attr++;
	(*old_09)();
}
void interrupt new_10(...)
{
	print();
	(*old_10)();
}
void interrupt new_11(...)
	{
	print();
	(*old_11)();
}
void interrupt new_12(...)
	{
	print();
	(*old_12)();
}
void interrupt new_13(...)
	{
	print();
	(*old_13)();
}
void interrupt new_14(...)
{
	print();
	(*old_14)();
}
void interrupt new_15(...)
{
	print();
	(*old_15)();
}

void interrupt new_70(...)
{
	print();
	(*old_70)();
}
void interrupt new_71(...)
	{
	print();
	(*old_71)();
}
void interrupt new_72(...)
	{
	print();
	(*old_72)();
}
void interrupt new_73(...)
	{
	print();
	(*old_73)();
}
void interrupt new_74(...)
	{
	print();
	(*old_74)();
}
void interrupt new_75(...)
	{
	print();
	(*old_75)();
}
void interrupt new_76(...)
	{
	print();
	(*old_76)();
}
void interrupt new_77(...)
	{
	print();
	(*old_77)();
}

void main()
{		
	unsigned far *fp;

	FP_SEG (fp) = _psp;
	FP_OFF (fp) = 0x2c;
	_dos_freemem(*fp);

	old_08 = getvect(0x08);
	old_09 = getvect(0x09);    
	old_10 = getvect(0x0A);   
	old_11 = getvect(0x0B);    
	old_12 = getvect(0x0C);  
	old_13 = getvect(0x0D);     
	old_14 = getvect(0x0E); 
	old_15 = getvect(0x0F);   

	old_70 = getvect(0x70);
	old_71 = getvect(0x71);    
	old_72 = getvect(0x72); 
	old_73 = getvect(0x73);  
	old_74 = getvect(0x74);   
	old_75 = getvect(0x75);  
	old_76 = getvect(0x76); 
	old_77 = getvect(0x77);  
		
	setvect(0x08, new_08);//таймер
	setvect(0x09, new_09);//клавиатура
	setvect(0x0A, new_10);
	setvect(0x0B, new_11);
	setvect(0x0C, new_12);
	setvect(0x0D, new_13);
	setvect(0x0E, new_14);
	setvect(0x0F, new_15);

	setvect(0xB0, new_70);
	setvect(0xB1, new_71);
	setvect(0xB2, new_72);
	setvect(0xB3, new_73);
	setvect(0xB4, new_74);//мышь
	setvect(0xB5, new_75);
	setvect(0xB6, new_76);
	setvect(0xB7, new_77);

	_disable();

	unsigned char value = inp(0x21);

	outp(0x20, 0x11);
	outp(0x21, 0x08);
	outp(0x21, 0x04)
	outp(0x21, 0x01);
	outp(0x21, value);

	value = inp(0xA1);
	outp(0xA0, 0x11);
	outp(0xA1, 0xB0);
	outp(0xA1, 0x02);
	outp(0xA1, 0x01);
	outp(0xA1, value);
		
	_enable();

	_dos_keep(0,(_DS-_CS)+(_SP/16)+1);
}
	
void strconvert(unsigned char temp, char *str)
{
	int i;
	str[8] = 0;
	i = 7;
	while(temp)
	{
		str[i] = '0' + temp % 2;
		temp = temp / 2;
		i--;
	}
	for(; i > -1; i--)
		str[i] = '0';
}	
void videoprint(char* str, int x, int y, unsigned char attribute)
{
	char far* start = (char far*)0xb8000000;
	start += x + 160 * y;
		
	int i = 0;
	while(str[i] != 0)
	{
		*start = str[i];
		start++;
		*start = attribute;
		start++;
		i++;
	}				
}	
void print()
{
	unsigned char isr_master, isr_slave;
	unsigned char irr_master, irr_slave;
	unsigned char imr_master, imr_slave;
		
	imr_master = inp(0x21); // читаем 1 байт из порта
	imr_slave = inp(0xA1);

	outp(0x20, 0x0A);		//записываем значение в порт
	irr_master = inp(0x20);
	outp(0x20, 0x0B);
	isr_master = inp(0x20);

	outp(0xA0,0x0A);
	irr_slave = inp(0xA0);
	outp(0xA0,0x0B);
	isr_slave = inp(0xA0);

	char str[9];

	videoprint("MASTER  SER: ",0, 0, attr);// обслуживание
	strconvert(isr_master, str);
	videoprint(str, 26, 0, attr);
		
	videoprint("  |  REQ: ",42, 0, attr);// запрос
	strconvert(irr_master, str);
	videoprint(str, 62, 0, attr);	

	videoprint("  |  MAS: ", 78, 0, attr);// маска
	strconvert(imr_master, str);
	videoprint(str, 98, 0, attr);	
		
	videoprint("SLAVE   SER: ", 0, 1, attr);
	strconvert(isr_slave, str);
	videoprint(str, 26, 1, attr);
		
	videoprint("  |  REQ: ", 42, 1, attr);
	strconvert(irr_slave, str);
	videoprint(str, 62, 1, attr);	

	videoprint("  |  MAS: ",78, 1, attr);
	strconvert(imr_slave, str);
	videoprint(str, 98, 1, attr);		
}

