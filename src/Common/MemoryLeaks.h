//=======================================================
// MemoryLeaks.h
// Copyright (C) by Roman Lut
//=======================================================
// ������ ��� ������������ ������ ������.
//
// �������������: ���������� � ������� cpp-���� ������� ����� ���� "#inlude".
// � ������ ������ ������� ������� EnableMemoryLeaksReport(type) (��������, � WinMain())
// ��������� � Release() �� ����� (#ifdef _DEBUG) - ����������� �������������.
//
// ��� ���������� ��������� � Output ���� ��������������� ��������������� ����� ������.
// ������ ���� ����� ���� Id, ��������� � �������.
// ����� �� ����� ���������� ������� ����������� �� ����� ��������� ����� �����, 
// ����� ��������� breakpoint �� ��������� EnableMemoryLeaksReport(), ����� � Evaluate window (Shift-F9),
// ������ _crtBreakAlloc, ��������� ���� ���������� Id �����, � ���������� 
// ���������� ���������. �� ����� new ��������� breakpoint.

// � ������ MEMORYLEAKS_PARANOIC ��� ������������� heap corruption 
// ������� assert _�� ����������_ new ��� delete.
// ����� �������������� ��������� heap �� corruption � ����� ������ �������� CheckHeap();

//������, EnableMemoryLeaksReport(MEMORYLEAKS_BASIC) ����� ������������ ������,
//�.�. ��� �� ��������.
//EnableMemoryLeaksReport(MEMORYLEAKS_PARANOIC) �������� ���� ��� � ����������, �� ���������� �� 
//corruption. �������� ��-���������.

//��� ��������� ������ ����� � heap. ��� �������� ������������ �������� � ����� �����
//�������� Buffer security check:YES � �������� Code generation (�� ��������, ����� �������� � release).

//��������� ����������� ������ � heap corruption ��������� ��� ������ �� �������, �������������
//������������ std::string ��� ���������� strsafe.h �� directx sdk, ���������
//strncopy() �� ��������� ������ �����, ���� ��� ������� ���������� ��������.

//_crtBreakAlloc

#ifndef MEMORYLEAKS_INCLUDED
#define MEMORYLEAKS_INCLUDED

#ifdef _DEBUG
#define MEMORY_LEAKS
#endif

//==========================
// TMEMORYLEAKREPORTTYPE
//==========================
typedef enum
{
 MEMORYLEAKS_BASIC          =  0,  //only dump unfreed blocks at exit
 MEMORYLEAKS_PARANOIC       =  1   //dump unfreed blocks at exit and check heap corruption on each allocation
} TMEMORYLEAKREPORTTYPE; 

#include <malloc.h>
#include "hxplatform.h"           

#ifdef MEMORY_LEAKS              
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <assert.h>
#endif

#include <stdio.h>           

//=============================================
//void __inline EnableMemoryLeaksReport(t) 
//=============================================
void __inline EnableMemoryLeaksReport(TMEMORYLEAKREPORTTYPE t) 
{
#ifdef MEMORY_LEAKS            

 if (t==MEMORYLEAKS_PARANOIC)                
  {                                          
   _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF);  
  }                                          
   else                                      
  {                                          
   _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);  
  }                                          
#endif MEMORY_LEAKS            
}

//=====================================
//void CheckHeap()
//=====================================
void __inline CheckHeap()
{
#ifdef MEMORY_LEAKS            
 assert(_CrtCheckMemory());
#endif MEMORY_LEAKS            
}

//=====================================
//void GetHeapInfo()
//=====================================
void __inline GetHeapInfo(DWORD* usedSize, DWORD* freeSize, 
                          DWORD* usedBlocks, DWORD* freeBlocks)
{
 *usedSize = 0;
 *freeSize = 0;
 *usedBlocks=0;
 *freeBlocks=0;
 
 _HEAPINFO hinfo;
 int heapstatus;
 hinfo._pentry = NULL;

#ifndef _XBOX

 while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )
  { 
    if (hinfo._useflag == _USEDENTRY) 
     {
      (*usedSize)+=hinfo._size;
      (*usedBlocks)++;
     }
      else
     {
      (*freeSize)+=hinfo._size;
      (*freeBlocks)++;
     } 
  }

#endif _XBOX
}


//=====================================
//void DumpHeap()
//=====================================
void __inline DumpHeap()
{
 DWORD usedSize = 0;
 DWORD freeSize = 0;

#ifdef MEMORY_LEAKS            
 
 _HEAPINFO hinfo;
 int heapstatus;
 hinfo._pentry = NULL;
 char msg[1024];

 while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )
  { sprintf(msg, "%6s block at %x08x of size %d bytes\n",
       ( hinfo._useflag == _USEDENTRY ? "USED" : "FREE" ),
         (DWORD)hinfo._pentry, hinfo._size );
    OutputDebugString(msg);     
    
    if (hinfo._useflag == _USEDENTRY) 
     {
      usedSize+=hinfo._size;
     }
      else
     {
      freeSize+=hinfo._size;
     } 
  }
  
 switch( heapstatus )
 {
  case _HEAPEMPTY:
     OutputDebugString( "OK - empty heap\n" );
     break;
  case _HEAPEND:
     OutputDebugString( "OK - end of heap\n" );
     break;
  case _HEAPBADPTR:
     OutputDebugString( "ERROR - bad pointer to heap\n" );
     break;
  case _HEAPBADBEGIN:
     OutputDebugString( "ERROR - bad start of heap\n" );
     break;
  case _HEAPBADNODE:
     OutputDebugString( "ERROR - bad node in heap\n" );
     break;
 }
#endif 
}

#endif MEMORYLEAKS_INCLUDED
