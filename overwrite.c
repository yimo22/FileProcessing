/*
  학부 : 컴퓨터학부
  학번 : 20172612
  성명 : 김영도

  주어진 파일에서 <오프셋>에서부터 <데이터>를 덮어쓴다.
  덮어쓰기를 할 때 파일의 EOF를 만나면 중단하지 않고 그대로 쓰기를 진행한다.

  a.out <파일명> <오프셋> <데이터>
*/
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int args, char *argv[])
{
  int fp = open(argv[1], O_WRONLY);
  if (fp == -1)
  {
    perror("error");
    exit(1);
  }
  int offset = atoi(argv[2]);
  lseek(fp, offset, SEEK_SET);
  write(fp, argv[3], strlen(argv[3]));

  close(fp);
  return 0;
}