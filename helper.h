#ifndef _HELPER
#define _HELPER
///////////////////////////////////////////////////////////////////////

#ifndef _INC_DIRECT
 #include <direct.h>
#endif

#ifndef _INC_STDIO
 #include <stdio.h>
#endif

#ifndef _INC_STDLIB
 #include <stdlib.h>
#endif

#ifndef _INC_TIME
 #include <time.h>
#endif

#ifndef _INC_STDARG
 #include <stdarg.h>
#endif

//////////////////////////////////
///// MOJE FUNKCJE
//////////////////////////////////
//
int DiskSize(int i)
{
	_diskfree_t d;
	_getdiskfree(i,&d);
	unsigned a;
	a=d.total_clusters*d.sectors_per_cluster*d.bytes_per_sector;
	if (a!=1168231104)
	{
		return a;
	}
	else
	{
		return -1;
	}
}

int DiskFree(int i)
{
	_diskfree_t d;
	_getdiskfree(i,&d);
	unsigned a;
	a=d.avail_clusters*d.sectors_per_cluster*d.bytes_per_sector;
	if (a!=1168231104)
	{
		return a;
	}
	else
	{
		return -1;
	}
}

bool IsDrive(int i)
{
	_diskfree_t d;
	_getdiskfree(i,&d);
	unsigned a;
	a=d.total_clusters*d.sectors_per_cluster*d.bytes_per_sector;
	if (a!=1168231104)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int Random(int min,int max)
{
	int i;
	i=rand()%(max-min+1)+min;
	return i;
}

void randomize()
{
	tm timer;
	int i=0,a=0,b;
	_getsystime(&timer);
	i=timer.tm_sec+100;
	while (a!=i)
	{
		b=rand()%100;
		a++;
	}
}

bool IsFile(char* nazwa)
{
	FILE* plik;
	if ((plik=fopen(nazwa,"r"))!=NULL)
	{
		fclose(plik);
		return true;
	}
	else
	{
		return false;
	}
}

char* makestring(char* control, ...)
{
    char *buf;
    va_list args;
    va_start(args, control);
		vsprintf(buf,control,args);
    va_end(args);
    return buf;
}



///////////////////////////////////////////////////////////////////////
#endif