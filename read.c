/*
  학부 : 컴퓨터학부
  학번 : 20172612
  성명 : 김영도

  주어진 파일에서 <오프셋(offset= 0, 1, 2, ...)>을 포함하여 
  오른편에 존재하는 <읽기 바이트 수>만큼의 데이터를 읽어서 화면에 출력한다. 
  만약 <읽기 바이트 수>만큼의 데이터가 존재하지 않으면 
  오프셋부터 파일의 맨마지막 바이트(EOF 제외)까지 읽어서 출력한다. 
  예를 들면, <오프셋>이 7이고 <읽기 바이트 수>가 10이면 
  오프셋 7부터 16까지 데이터를 읽어서 출력한다. 
  
  a.out <파일명> <오프셋> <읽기 바이트 수>
*/
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main(int args, char *argv[])
{
  int fp = open(argv[1], O_RDONLY);
  char buff;
  if (fp == -1)
  {
    perror("error");
    return 0;
  }
  int offset = atoi(argv[2]);
  lseek(fp, offset, SEEK_SET);

  if (args == 4)
  {
    int R_bytes = atoi(argv[3]);

    for (int i = offset; i < R_bytes + offset; i++)
    {
      read(fp, &buff, 1);
      printf("%c", buff);
    }

    return 0;
  }
  else if (args == 3)
  {
    while (read(fp, &buff, 1) > 0)
    {
      printf("%c", buff);
    }
  }
  else
  {
    print("Error - Not expected input\n");
  }
  fclose(fp);
  return 0;
}