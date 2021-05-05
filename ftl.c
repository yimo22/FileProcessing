// 주의사항
// 1. blockmap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blockmap.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(blockmap.h에 추가하면 안됨)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "blockmap.h"
//#define DEBUG 0
// 필요한 경우 헤더 파일을 추가하시오.

int tb[DATABLKS_PER_DEVICE];
int Free_block_Pos;
//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
//


// 불러오기 기능이 되는지 check
void ftl_open()
{
	//
	// address mapping table 초기화 또는 복구
	// free block's pbn 초기화
  // address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일
	for(int i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		memset(tb+i,0xFFFFFFFF,sizeof(int));
	}
	char *pagebuf = (char *)malloc(PAGE_SIZE);
	int lbn;
	for(int i=0;i<DATABLKS_PER_DEVICE;i++){
		dd_read(i*PAGES_PER_BLOCK,pagebuf);
		memmove(&lbn,pagebuf+SECTOR_SIZE,4);
		if(lbn == 0xFFFFFFFF){
			tb[DATABLKS_PER_DEVICE - i] = -1;
		}
		else{
			tb[lbn] = i;
		}
	}
	for(int i=0;i<DATABLKS_PER_DEVICE;i++){
		if(tb[i] == -1){
			Free_block_Pos = i;
			break;
		}
	}
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_read(int lsn, char *sectorbuf)
{
	memset(sectorbuf,0xFF,SECTOR_SIZE);
	char *pagebuf = (char *)malloc(PAGE_SIZE);
	memset(pagebuf,0xFF,PAGE_SIZE);
	// ppn = pbn * PAGES_PER_BLOCK + offset
	dd_read(Getppn(lsn), pagebuf);
	// Sector 영역 buff로 copy.
	memcpy(sectorbuf, pagebuf, SECTOR_SIZE);
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_write(int lsn, char *sectorbuf)
{
	// pagebuf의 sector영역에 저장, 필요에 따라 pagebuf의 spare영역에 메타 데이터(lsn,lbn등) 저장
	// 이 pagebuf를 인자값으로 dd_write()에 전달. (physical page number 전달)
		#ifdef DEBUG 
		printf("=====================ftl_write===================\n");
		printf("secotbuf : %s\n",sectorbuf);
		#endif
		char *pagebuf = (char *)malloc(PAGE_SIZE);
		char * sparebuf = (char *)malloc(SPARE_SIZE);
	
		memcpy(pagebuf,sectorbuf,SECTOR_SIZE);
		// 최초 쓰기
	if(Getpbn(lsn) == -1){
		#ifdef DEBUG 
		printf("=====================New===================\n");
		#endif

		// Table 복구용 LBN + 갱신여부 LSN + 하위8byte 초기화
		memset(sparebuf,Getlbn(lsn),4);
		memset(sparebuf+4,lsn,4);
		memset(sparebuf+8,0xFF,8);

		// Pagebuf에 copy
		memcpy(pagebuf+SECTOR_SIZE,sparebuf,SPARE_SIZE);

		// address mapping table 갱신
		tb[Getlbn(lsn)] = DATABLKS_PER_DEVICE - Getlbn(lsn);
		#ifdef DEBUG 
		printf("=====================New END===================\n");
		#endif
	}
	// update
	else{
		#ifdef DEBUG 
		printf("=====================update===================\n");
		#endif
		char * temp_page = (char *)malloc(PAGE_SIZE);
		int pbn = Getpbn(lsn);

		// Copy to free block
		for(int i=0;i<PAGES_PER_BLOCK;i++){
			if(i == Getoffset(lsn))
				continue;
			dd_read(i+Getpbn(lsn)*PAGES_PER_BLOCK,temp_page);
			dd_write(i+Free_block_Pos*PAGES_PER_BLOCK,temp_page);
		}
		free(temp_page);

		// Erase block
		dd_erase(pbn);

		// update data
		memset(sparebuf,Getlbn(lsn),4);
		memset(sparebuf+4,lsn,4);
		memcpy(pagebuf+SECTOR_SIZE,sparebuf,SPARE_SIZE);

		// address mapping table 갱신
		// free block 갱신
		tb[Getlbn(lsn)] = Free_block_Pos;
		Free_block_Pos = pbn;


		#ifdef DEBUG 
		printf("=====================update END===================\n");
		#endif
	}
	dd_write(Getppn(lsn),pagebuf);
	free(sparebuf);
	free(pagebuf);
	return;
}

void ftl_print()
{
	printf("lbn\tpbn\n");
	for(int i=0;i<DATABLKS_PER_DEVICE;i++){
		printf("%d\t%d\n",i,tb[i]);
	}
	printf("free block's pbn=%d\n",Free_block_Pos);
	return;
}
int Getpbn(int lsn){
	return tb[Getlbn(lsn)];
}
int Getoffset(int lsn){
	return lsn % PAGES_PER_BLOCK;
}
int Getppn(int lsn){
	return Getpbn(lsn) * PAGES_PER_BLOCK + Getoffset(lsn);
}
int Getlbn(int lsn){
	return lsn / PAGES_PER_BLOCK;
}

