#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// copy and modified from grep.c

int matchhere(char*, char*);
int matchstar(char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')	// ?
    return matchhere(re+1, text);
  
  if(re[0]==*text)	// normal search
  {
    return matchhere(re+1, text+1);
  }
  else if(re[0]=='*')	//search for *c
  {
  	return matchstar(re+1, text);
  }
  
  // fail to match
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if((re[0] == '\0')&&(*text == '\0' || *text == '.'))	// end for match, it will skip ".c" or anything like that
    return 1;
  if(re[0] == '*')	// for c*ab and c*
    return matchstar(re+1, text);
  if(re[0] == '$' && re[1] == '\0')	// ?
    return *text == '\0';
  if(*text!='\0' && (re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(char *re, char *text)
{
  if(re[0] == '\0')	// for c*
  	return 1;
  
  while (*text!='\0')	// for c*ab
  {
  	if(matchhere(re, text))
  		return 1;
  	text++;
  }
  return 0;
}
// end of copy and modified from grep.c

// copy and modified from ls.c
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));	// align the format
  return buf;
}

void
find(char *path, char *name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  //int temp, temp2;	// debug

  if((fd = open(path, 0)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0)	// retrieves information about the object a file descriptor refers to
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
	
	//printf("st.dev=%d, st.ino=%d, st.type=%d, st.nlink=%d, st.size=%d\n", st.dev, st.ino, st.type, st.nlink, st.size);
	
  switch(st.type)
  {
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);	// load path in the buf like "."
    p = buf+strlen(buf);
    *p++ = '/';	// replace '\0' with '/' in the end of path like "./"
    while(read(fd, &de, sizeof(de)) == sizeof(de))	// read all files in the file descriptor fd refering to the path each time
    {
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);	// concatenate each file at the end of path like "./." or "./.." or "./README"...
      p[DIRSIZ] = 0;	// add '\0' in the end of path 
      if(stat(buf, &st) < 0)	// retrieves information about the object at this path 
      {
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      //printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
      
      if (match(name, p))
      {
      	//printf("find: found at %s\n", buf);
      	printf("%s\n", buf);
      }
      
      if ((st.type == T_DIR)&&(p[0]!='.'))	// find directory and exclude "./." and "./.."
      {
      	find(buf, name);	// continue to search
      }
    }
    break;
  }
  close(fd);
}
// end copy and modified from ls.c




// For example: to search sleep, it supports sleep, sl*, *ep, or sl**.
int main(int argc, char *argv[])
{
	char temp = '\0';
	
	
	if(argc < 2)	// no parameter input
  {
    find(".", &temp);
  }
	else if(argc < 3)	// received a parameter at argv[1]
	{
		find(argv[1], &temp);
  }
  else	// received two parameters at argv[1] and argv[2]
  {
  	find(argv[1], argv[2]);
  }
  
	
	
  
  exit(0);
  
  
}


