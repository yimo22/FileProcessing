#include <stdio.h>
#include "person.h"
#include <stdlib.h>
#include <string.h>
//필요한 경우 헤더 파일과 함수를 추가할 수 있음

// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제 레코드를 수정할 때나
// 위의 readPage() 함수를 호출하여 pagebuf에 저장한 후, 여기에 필요에 따라서 새로운 레코드를 저장하거나
// 삭제 레코드 관리를 위한 메타데이터를 저장합니다. 그리고 난 후 writePage() 함수를 호출하여 수정된 pagebuf를
// 레코드 파일에 저장합니다. 반드시 페이지 단위로 읽거나 써야 합니다.
//
// 주의: 데이터 페이지로부터 레코드(삭제 레코드 포함)를 읽거나 쓸 때 페이지 단위로 I/O를 처리해야 하지만,
// 헤더 레코드의 메타데이터를 저장하거나 수정하는 경우 페이지 단위로 처리하지 않고 직접 레코드 파일을 접근해서 처리한다.
#define HEADER_RECORD_SIZE 16
#define HEADER_AREA_OFFSET_SIZE 4
#define HEADER_AREA_totalRcrd_SIZE 4
#define HEADER_AREA_len 4
#define HEADER_AREA_METADATA_SIZE (HEADER_AREA_OFFSET_SIZE + HEADER_AREA_len)
#define IDX_HEADER_SIZE 4
#define IDX_DATA_SIZE 21
//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void readPage(FILE *fp, char *pagebuf, int pagenum)
{
  if (fp == NULL)
  {
    fprintf(stderr, "Error - FILE *\n");
    exit(1);
  }
  // 해당 page로 offset을 이동
  fseek(fp, HEADER_RECORD_SIZE + pagenum * PAGE_SIZE, SEEK_SET);
  // 해당 page를 pagebuf로 읽는다.
  if (fread(pagebuf, sizeof(char), PAGE_SIZE, fp) < 0)
  {
    fprintf(stderr, "Error - fread\n");
    exit(1);
  }
}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 레코드 파일의 위치에 저장한다.
// 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
  if (fp == NULL)
  {
    fprintf(stderr, "Error - FILE *\n");
    exit(1);
  }
  // 해당 page로 offset을 이동
  fseek(fp, HEADER_RECORD_SIZE + pagenum * PAGE_SIZE, SEEK_SET);
  // 해당 page를 pagebuf로 읽는다.
  if (fwrite(pagebuf, sizeof(char), PAGE_SIZE, fp) < 0)
  {
    fprintf(stderr, "Error - fwrite\n");
    exit(1);
  }
}

//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다.
//
void pack(char *recordbuf, const Person *p)
{
  sprintf(recordbuf, "%s#%s#%s#%s#%s#%s#", p->id, p->name, p->age, p->addr, p->phone, p->email);
  return;
}

//
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때 사용한다.
//
void unpack(char *recordbuf, Person *p)
{
  char *ptr;
  ptr = strtok(recordbuf, "#");

  strcpy(p->id, ptr);
  for (int i = strlen(p->id); i < 13; i++)
    p->id[i] = '\0';

  ptr = strtok(NULL, "#");
  strcpy(p->name, ptr);
  for (int i = strlen(p->name); i < 13; i++)
    p->name[i] = '\0';
  ptr = strtok(NULL, "#");

  strcpy(p->age, ptr);
  for (int i = strlen(p->age); i < 13; i++)
    p->age[i] = '\0';
  ptr = strtok(NULL, "#");

  strcpy(p->addr, ptr);
  for (int i = strlen(p->addr); i < 13; i++)
    p->addr[i] = '\0';
  ptr = strtok(NULL, "#");

  strcpy(p->phone, ptr);
  for (int i = strlen(p->phone); i < 13; i++)
    p->phone[i] = '\0';
  ptr = strtok(NULL, "#");

  strcpy(p->email, ptr);
  for (int i = strlen(p->email); i < 13; i++)
    p->email[i] = '\0';
  return;
}

//
// 주어진 레코드 파일(recordfp)을 이용하여 심플 인덱스 파일(idxfp)을 생성한다.
//
void createIndex(FILE *idxfp, FILE *recordfp)
{
  if (idxfp == NULL)
  {
    fprintf(stderr, "Error - FILE *\n");
    exit(1);
  }
  Person *ptr = (Person *)malloc(sizeof(Person));
  char *idxptr = (char *)malloc(IDX_HEADER_SIZE);
  char *idx_data_buff = (char *)malloc(IDX_DATA_SIZE);
  char *pagebuff = (char *)malloc(PAGE_SIZE);
  int total_record_in_idx = 0;
  int Maxpage_in_rcrdFile;

  // Get total_page in rcrdfile
  fseek(recordfp, 0, SEEK_SET);
  fread(&Maxpage_in_rcrdFile, sizeof(int), 1, recordfp);
  // record 파일을 전부 읽어 index 파일을 생성
  for (int pagenumber = 0; pagenumber < Maxpage_in_rcrdFile; pagenumber++)
  {
    // 해당 page read
    readPage(recordfp, pagebuff, pagenumber);
    // read한 page에 대하여 주민번호, record 주소(페이지 번호 + 페이지 내 레코드 번호)
    int total_records_page_i;
    memcpy(&total_records_page_i, pagebuff, HEADER_AREA_totalRcrd_SIZE);
    for (int i = 0; i < total_records_page_i; i++)
    {
      int i_meta_offset = HEADER_AREA_totalRcrd_SIZE + i * HEADER_AREA_METADATA_SIZE;
      int data_offset, data_len;
      memcpy(&data_offset, pagebuff + i_meta_offset, HEADER_AREA_OFFSET_SIZE);
      memcpy(&data_len, pagebuff + i_meta_offset + 4, HEADER_AREA_len);

      // 해당 rcrd를 memory로부터 가져와서 Person객체로 만든다.
      char *rcrdbuff = (char *)malloc(data_len);
      memcpy(rcrdbuff, pagebuff + HEADER_AREA_SIZE + data_offset, data_len);
      unpack(rcrdbuff, ptr);
      // 해당 record로 이동하여 deleted 여부 확인
      // deleted 됐을 경우 제외한다.
      if (ptr->id[0] == '*')
      {
        // Deleted 된 경우
        continue;
      }
      else
      {
        // Deleted 되지 않은 경우
        total_record_in_idx++;

        memcpy(rcrdbuff, ptr->id, 13);
        memcpy(rcrdbuff + 13, &pagenumber, 4);
        memcpy(rcrdbuff + 13 + 4, &i, 4);

        idxptr = (char *)realloc(idxptr, IDX_HEADER_SIZE + total_record_in_idx * IDX_DATA_SIZE);
        if (idxptr == NULL)
        {
          fprintf(stderr, "Error - realloc\n");
          exit(1);
        }
        memcpy(idxptr + IDX_HEADER_SIZE + IDX_DATA_SIZE * (total_record_in_idx - 1), rcrdbuff, IDX_DATA_SIZE);
      }
    }
  }
  // 생성된 index 파일에 대하여, 주민번호 오름차순으로 정렬
  fseek(idxfp, 0, SEEK_SET);
  fwrite(&total_record_in_idx, sizeof(int), 1, idxfp);
  for (int i = 0; i < total_record_in_idx; i++)
  {
    int record_i_offset = IDX_HEADER_SIZE + i * IDX_DATA_SIZE;
    char *record_i = (char *)malloc(IDX_DATA_SIZE);
    char record_i_id[14];
    memcpy(record_i, idxptr + record_i_offset, IDX_DATA_SIZE);
    memcpy(record_i_id, idxptr + record_i_offset, 13);
    record_i_id[13] = '\0';
    for (int j = i + 1; j < total_record_in_idx; j++)
    {
      memcpy(record_i, idxptr + record_i_offset, IDX_DATA_SIZE);
      memcpy(record_i_id, idxptr + record_i_offset, 13);
      record_i_id[13] = '\0';

      int record_j_offset = IDX_HEADER_SIZE + j * IDX_DATA_SIZE;
      char *record_j = (char *)malloc(IDX_DATA_SIZE);
      char record_j_id[14];
      memcpy(record_j, idxptr + record_j_offset, IDX_DATA_SIZE);
      memcpy(record_j_id, idxptr + record_j_offset, 13);
      record_j_id[13] = '\0';
      if (atoll(record_i_id) > atoll(record_j_id))
      {
        // record_j_id가 더 빠른 숫자임 - change
        memcpy(idxptr + record_i_offset, record_j, IDX_DATA_SIZE);
        memcpy(idxptr + record_j_offset, record_i, IDX_DATA_SIZE);
      }
      else if (atoll(record_i_id) == atoll(record_j_id))
      {
        // record_i = record_j 인경우 (error)
        fprintf(stderr, "Error - Same id\n");
        exit(1);
      }
    }
    // Write record to idxFile
    fwrite(idxptr + record_i_offset, sizeof(char), IDX_DATA_SIZE, idxfp);
  }
}

//
// 주어진 심플 인덱스 파일(idxfp)을 이용하여 주민번호 키값과 일치하는 레코드의 주소, 즉 페이지 번호와 레코드 번호를 찾는다.
// 이때, 반드시 이진 검색 알고리즘을 사용하여야 한다.
//
void binarysearch(FILE *idxfp, const char *id, int *pageNum, int *recordNum)
{
  if (strlen(id) > 13 || strlen(id) <= 0)
  {
    fprintf(stderr, "Error - len(id) error\n");
  }

  // read idxFile header and Get Total #records
  int total_record_in_idx = 0;
  int total_num_reads = 0;
  fread(&total_record_in_idx, sizeof(int), 1, idxfp);
  total_num_reads++;

  char *rcrdbuff = (char *)malloc(IDX_DATA_SIZE);
  char temp_id[14];
  int Left_bound = 0;
  int Right_bound = total_record_in_idx;
  int index = (Left_bound + Right_bound) / 2;

  // Start binary search : start point is total_record / 2
  while (1)
  {
    if (Right_bound == Left_bound)
    {
      // case - there's no record
      *pageNum = -1;
      *recordNum = -1;
      return;
    }

    // read N's record to rcrdbuff
    fseek(idxfp, IDX_HEADER_SIZE + IDX_DATA_SIZE * index, SEEK_SET);
    fread(rcrdbuff, sizeof(char), IDX_DATA_SIZE, idxfp);
    total_num_reads++;

    memcpy(temp_id, rcrdbuff, 13);
    temp_id[13] = '\0';
    // check id
    if (atoll(temp_id) == atoll(id))
    {
      // Found
      // return the pageNum, recordNum
      memcpy(pageNum, rcrdbuff + 13, 4);
      memcpy(recordNum, rcrdbuff + 17, 4);
      printf("#reads : %d\n", total_num_reads);
      return;
    }
    else if (atoll(temp_id) - atoll(id) > 0)
    {
      // temp_id > id
      // move forward
      Right_bound = index;
      index = (Left_bound + Right_bound) / 2;
    }
    else
    {
      // temp_id < id
      // move backward
      Left_bound = index + 1;
      index = (Left_bound + Right_bound) / 2;
    }
  }
}
void Interface_binarysearch(FILE *rcrd, FILE *idx, char *id)
{
  int pagenum, recordnum;
  binarysearch(idx, id, &pagenum, &recordnum);
  if (pagenum == -1 && recordnum == -1)
  {
    printf("no persons\n");
  }
  else
  {
    // 해당 정보를 받아올 포인터/버퍼 선언.
    Person *ptr = (Person *)malloc(sizeof(Person));

    char *pagebuff = (char *)malloc(PAGE_SIZE);
    readPage(rcrd, pagebuff, pagenum);

    // get data_offset from Header Area
    int offset;
    int len;
    memcpy(&offset, pagebuff + 4 + recordnum * 8, sizeof(int));
    memcpy(&len, pagebuff + 4 + recordnum * 8 + 4, sizeof(int));

    char *rcrdbuff = (char *)malloc(len);
    memcpy(rcrdbuff, pagebuff + HEADER_AREA_SIZE + offset, len);
    // pack 함수를 통해 그 정보를 Person구조체에 저장
    unpack(rcrdbuff, ptr);

    printf("id=%s\n", ptr->id);
    printf("name=%s\n", ptr->name);
    printf("age=%s\n", ptr->age);
    printf("addr=%s\n", ptr->addr);
    printf("phone=%s\n", ptr->phone);
    printf("email=%s\n", ptr->email);
  }
}
// record file의 모든 값과 record들을 확인하는 함수
void Checking_rcrdfile(FILE *rcrdptr)
{
  int HR_totalpage;
  int HR_totalrcrd;
  int HR_delpage;
  int HR_delrcrd;
  fseek(rcrdptr, 0, SEEK_SET);
  fread(&HR_totalpage, sizeof(int), 1, rcrdptr);
  fread(&HR_totalrcrd, sizeof(int), 1, rcrdptr);
  fread(&HR_delpage, sizeof(int), 1, rcrdptr);
  fread(&HR_delrcrd, sizeof(int), 1, rcrdptr);

  printf("================ record file =====================\n");
  printf(" %d / %d / %d / %d\n", HR_totalpage, HR_totalrcrd, HR_delpage, HR_delrcrd);
  for (int i = 0; i < HR_totalpage; i++)
  {
    printf("-------------------------------------------------\n");
    printf("page : %d\n", i);
    int totalrcrd;
    int offset;
    int len;
    fseek(rcrdptr, HEADER_RECORD_SIZE + i * PAGE_SIZE, SEEK_SET);
    fread(&totalrcrd, sizeof(int), 1, rcrdptr);
    for (int j = 0; j < totalrcrd; j++)
    {
      fseek(rcrdptr, HEADER_RECORD_SIZE + i * PAGE_SIZE + 4 + j * 8, SEEK_SET);
      fread(&offset, sizeof(int), 1, rcrdptr);
      fread(&len, sizeof(int), 1, rcrdptr);
      char *temp = (char *)malloc(sizeof(char) * len);
      fseek(rcrdptr, HEADER_RECORD_SIZE + i * PAGE_SIZE + HEADER_AREA_SIZE + offset, SEEK_SET);
      fread(temp, sizeof(char), len, rcrdptr);
      printf("rcrdnum : %d / offset : %d / len : %d\n", j, offset, len);
      printf("<%s>\n", temp);
    }
  }
}

// index file의 모든 값과 record들을 확인하는 함수
void Checking_idxfile(FILE *idx)
{
  int total_rcrd;
  fseek(idx, 0, SEEK_SET);
  fread(&total_rcrd, sizeof(int), 1, idx);
  printf("\n=================idxfile==================\n");
  printf("total record in index : %d\n",total_rcrd);
  for (int i = 0; i < total_rcrd; i++)
  {
    char id[14];
    int pagenum, rcrdnum;
    fseek(idx, IDX_HEADER_SIZE + IDX_DATA_SIZE * i, SEEK_SET);
    fread(id, sizeof(char), 13, idx);
    fread(&pagenum, sizeof(int), 1, idx);
    fread(&rcrdnum, sizeof(int), 1, idx);
    printf("[%d]/ id:<%s> pagenum:<%d> rcrdnum:<%d>\n", i, id, pagenum, rcrdnum);
  }
}
int main(int argc, char *argv[])
{
  FILE *fp = NULL; // 레코드 파일의 파일 포인터
  FILE *idx = NULL;
  if ((fp = fopen(argv[2], "rb")) == NULL)
  {
    fprintf(stderr, "Error - fopen\n");
    exit(1);
  }

  switch (argv[1][0])
  {
  case 'i':
    idx = fopen(argv[3], "w+");
    if (idx == NULL || argc != 4)
    {
      if(idx == NULL)
        fprintf(stderr, "Error - fopen\n");
      else
        fprintf(stderr,"Error - Unexpected input error(argc!=4)\n");
      exit(1);
    }
    createIndex(idx, fp);
    break;
  case 'b':
    idx = fopen(argv[3], "rb");
    if (idx == NULL || argc != 5)
    {
      if(idx == NULL)
        fprintf(stderr, "Error - fopen\n");
      else
        fprintf(stderr,"Error - Unexpected input error(argc!=5)\n");
      exit(1);
    }
    Interface_binarysearch(fp, idx, argv[4]);
    break;
  default:
    printf("Unexpected input\n");
    break;
  }
  return 0;
}