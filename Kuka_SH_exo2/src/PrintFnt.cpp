#include "PrintFnt.h"
#include "stdafx.h"  
#include <windows.h>
#include <stdio.h>

void PrintFnt::gotoxy( int column, int line )
{
	COORD coord;
	coord.X = column;
	coord.Y = line;
	SetConsoleCursorPosition(
		GetStdHandle( STD_OUTPUT_HANDLE ),
		coord
		);

}

void PrintFnt::ClearScreen(void)
{
	int n;
	for (n = 0; n < 100; n++)
	{gotoxy(1,n); 
	printf( "                                                                                                           " );
	}
	gotoxy(1,1);
}