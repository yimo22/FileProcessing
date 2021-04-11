/*
  학부 : 컴퓨터학부
  학번 : 20172612
  성명 : 김영도

  주어진 파일에서 <오프셋>을 포함하여 오른편에 존재하는 
  <삭제 바이트 수>만큼의 데이터를 삭제한다.
  만약 <삭제 바이트 수>만큼의 데이터가 존재하지 않는 경우
  파일의 마지막 바이트까지 삭제한다.
  삭제 후 삭제 데이터를 기준으로 전후 데이터가 병합되어야 한다.
  즉, 삭제 데이터의 공간은 파일에서 사라져야 한다.

  a.out <파일명> <오프셋> <삭제 바이트 수>
*/

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FILESIZE 1024
int main(int args, char *argv[])
{
  int fp = open(argv[1], O_RDWR);
  if (fp == -1)
  {
    perror("error");
    exit(1);
  }
  int offset = atoi(argv[2]);
  int Del_bytes = atoi(argv[3]);
  int PastePoint = lseek(fp, offset, SEEK_SET);
  lseek(fp, offset + Del_bytes, SEEK_SET);
  int re = 0;
  // Back Part
  char *buff = (char *)malloc(sizeof(char) * FILESIZE);
  while ((re = read(fp, buff, FILESIZE)) > 0)
  {
    // save Seeking Point
    int ReadPoint = lseek(fp, 0, SEEK_CUR);
    lseek(fp, PastePoint, SEEK_SET);
    // Overwrite
    write(fp, buff, strlen(buff));
    // Renew SeekPoint(PastePoint)
    PastePoint = lseek(fp, 0, SEEK_CUR);
    // Back to SeekPoint(ReadPoint)
    lseek(fp, ReadPoint, SEEK_SET);
  }
  int filesize = lseek(fp, 0, SEEK_END);
  // Resize size of file
  truncate(argv[1], filesize - Del_bytes);
  close(fp);

  return 0;
}
