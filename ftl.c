// 주의사항
// 1. blockmap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blockmap.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(blockmap.h에 추가하면 안됨)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "blockmap.h"

void SetSparebuff(char *ptr, int num1, int num2, int num3);

// 필요한 경우 헤더 파일을 추가하시오.

int tb[DATABLKS_PER_DEVICE];
int Free_block_Pos;
//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
//

/****************************************************************************
* 새로쓰기의 경우, 0~15 pbn 중 15번이 freeblock 으로 고정됨.
* 	i lbn 은 i pbn과 match, ex) lbn 4번 == pbn 4번
* 불러오기의 경우, 사용하지 않는 block 중 number가 가장 큰 block 이 freeblock 이된다.
*****************************************************************************/

void ftl_open()
{
	char *pagebuf = (char *)malloc(PAGE_SIZE);
	int lbn;
	//
	// address mapping table 초기화 또는 복구
	// free block's pbn 초기화
	// address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일

	/* FTL 초기화 */
	for (int i = 0; i < DATABLKS_PER_DEVICE; i++)
		tb[i] = -1;

	// Block 을 순회하여, lbn 값을 읽어옴.
	// lbn이 0xFFFFFFFF일 경우 새로쓰기, 아닐경우 tb[lbn]에 해당 pbn을 할당.
	for (int i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		int res = dd_read(i * PAGES_PER_BLOCK, pagebuf);
		if (res != 1)
		{
			fprintf(stderr, "dd_read Error\n");
			exit(1);
		}
		memcpy(&lbn, pagebuf + SECTOR_SIZE, 4);
		// Load
		if (lbn != 0xFFFFFFFF)
		{
			lbn &= 0x000000FF;
			tb[lbn] = i;
		}
		// New
		else
		{
			/* Flash Memory 초기화 */
			dd_erase(i);
		}
	}

	/* 여기서부터 Freeblock에 관한 것임 */
	// pbn 15번 부터 0번까지 순회하면서, free block설정.
	// 새로쓰기의 경우, 0~15 pbn 중 15번이 freeblock
	// 불러오기의 경우, 사용하지 않는 block 중 number가 가장 큰 block 이 freeblock 이된다.
	for (int i = (BLOCKS_PER_DEVICE - 1); i >= 0; i--)
	{
		int res = dd_read(i * PAGES_PER_BLOCK, pagebuf);
		if (res != 1)
		{
			fprintf(stderr, "dd_read Error\n");
			exit(1);
		}
		memcpy(&lbn, pagebuf + SECTOR_SIZE, 4);
		if (lbn == 0xFFFFFFFF)
		{
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
	// memory set
	memset(sectorbuf, 0xFF, SECTOR_SIZE);
	char *pagebuf = (char *)malloc(PAGE_SIZE);
	memset(pagebuf, 0xFF, PAGE_SIZE);

	// ppn = pbn * PAGES_PER_BLOCK + offset
	int res = dd_read(Getppn(lsn), pagebuf);
	if (res != 1)
	{
		fprintf(stderr, "dd_read Error\n");
		exit(1);
	}

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
	char *pagebuf = (char *)malloc(PAGE_SIZE);
	memset(pagebuf, 0xFF, PAGE_SIZE);
	char *sparebuf = (char *)malloc(SPARE_SIZE);
	memset(sparebuf, 0xFF, SPARE_SIZE);

	// 최초 쓰기
	if (Getpbn(lsn) == -1)
	{

		// address mapping table 갱신
		tb[Getlbn(lsn)] = Getlbn(lsn);

		// Table 복구용 LBN 값 삽입.
		memset(sparebuf, Getlbn(lsn), 4);
		memcpy(pagebuf + SECTOR_SIZE, sparebuf, 4);
		dd_write(Getpbn(lsn) * PAGES_PER_BLOCK, pagebuf);

		//갱신여부 LSN + 하위8byte 초기화
		if (Getoffset(lsn) != 0) // offset 이 0이 아니어서 block의 첫번쨰 경우에 써지지 않는 경우
			SetSparebuff(sparebuf, 0xFF, lsn, 0xFF);
		else // offset이 0이여서 block의 첫번째에 써지는 경우
			SetSparebuff(sparebuf, Getlbn(lsn), lsn, 0xFF);

		// Pagebuf에 sectorbuff와 sparebuff를 각각 copy
		memset(pagebuf, 0xFF, PAGE_SIZE);
		memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
		memcpy(pagebuf + SECTOR_SIZE, sparebuf, SPARE_SIZE);

		// flashmemory에 작성
		int res = dd_write(Getppn(lsn), pagebuf);
		if (res != 1)
		{
			fprintf(stderr, "dd_write Error\n");
			exit(1);
		}
		free(sparebuf);
		free(pagebuf);
	}
	// overwrite(해당 ppn에 data가 존재하는 경우) or insert(해당 ppn에 data가 존재하지 않지만, pbn에는 존재하는 경우)
	else
	{
		int pbn = Getpbn(lsn);
		dd_read(Getppn(lsn), pagebuf);
		int lsn_cmp = 0xFFFFFFFF;
		memcpy(&lsn_cmp, pagebuf + SECTOR_SIZE + 4, 4);
		memset(pagebuf, 0xFF, PAGE_SIZE);
		// insert(해당 ppn에 data는 존재하지 않지만, pbn에는 존재하는 경우)
		if (lsn_cmp == 0xFFFFFFFF)
		{
			if (Getoffset(lsn) != 0) // pbn 의 첫번쨰 page에 작성되지 않는 경우
				SetSparebuff(sparebuf, 0xFF, lsn, 0xFF);
			else // pbn 의 첫번쨰 page에 작성되는 경우
				SetSparebuff(sparebuf, Getlbn(lsn), lsn, 0xFF);

			// pagebuf update.
			memcpy(pagebuf + SECTOR_SIZE, sparebuf, SPARE_SIZE);
			memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
			// flash memory에 write
			dd_write(Getppn(lsn), pagebuf);
		}
		// overwrite(해당 ppn에 data가 존재, 덮어쓰기 하는경우 - freeblock 이용)
		else
		{
			// Copy to free block
			for (int i = 0; i < PAGES_PER_BLOCK; i++)
			{
				if (i == Getoffset(lsn))
					continue;
				// Copy useful data
				int res = dd_read(i + pbn * PAGES_PER_BLOCK, pagebuf);
				if (res != 1)
				{
					fprintf(stderr, "dd_read Error\n");
					exit(1);
				}
				// Paste useful data to free block
				res = dd_write(i + Free_block_Pos * PAGES_PER_BLOCK, pagebuf);
				if (res != 1)
				{
					fprintf(stderr, "dd_write Error\n");
					exit(1);
				}
			}
			free(pagebuf);

			// Erase block
			int res = dd_erase(pbn);
			if (res != 1)
			{
				fprintf(stderr, "dd_erase Error\n");
				exit(1);
			}

			// Table 복구용 LBN 값 삽입.
			memset(sparebuf, Getlbn(lsn), 4);
			memcpy(pagebuf + SECTOR_SIZE, sparebuf, 4);
			dd_write(Free_block_Pos * PAGES_PER_BLOCK, pagebuf);

			// 갱신여부 LSN + 하위8byte 초기화
			if (Getoffset(lsn) != 0)
				SetSparebuff(sparebuf, 0xFF, lsn, 0xFF);
			else
				SetSparebuff(sparebuf, Getlbn(lsn), lsn, 0xFF);

			// update pagebuff
			memset(pagebuf, 0xFF, PAGE_SIZE);
			memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
			memcpy(pagebuf + SECTOR_SIZE, sparebuf, SPARE_SIZE);

			// write pagebuff
			res = dd_write(Free_block_Pos * PAGES_PER_BLOCK + Getoffset(lsn), pagebuf);
			if (res != 1)
			{
				fprintf(stderr, "dd_write Error\n");
				exit(1);
			}
			// address mapping table 갱신
			// free block 갱신
			tb[Getlbn(lsn)] = Free_block_Pos;
			Free_block_Pos = pbn;
			free(sparebuf);
			free(pagebuf);
		}
	}
	return;
}

void ftl_print()
{
	printf("lbn\tpbn\n");
	for (int i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		printf("%d\t%d\n", i, tb[i]);
	}
	printf("free block's pbn=%d\n", Free_block_Pos);
	return;
}
int Getpbn(int lsn)
{
	return tb[Getlbn(lsn)];
}
int Getoffset(int lsn)
{
	return lsn % PAGES_PER_BLOCK;
}
int Getppn(int lsn)
{
	return Getpbn(lsn) * PAGES_PER_BLOCK + Getoffset(lsn);
}
int Getlbn(int lsn)
{
	return lsn / PAGES_PER_BLOCK;
}
void SetSparebuff(char *ptr, int num1, int num2, int num3)
{
	// lbn 부분 를 num1으로 초기화
	memset(ptr, num1, 4);
	// lsn 부분 를 num2로 초기화
	memset(ptr + 4, num2, 4);
	// 하위 8byte 를 num3로 초기화
	memset(ptr + 8, num3, 8);
}
