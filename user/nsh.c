#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/fcntl.h"

typedef enum
{
	redirect_none,
	redirect_input,	  //"<"
	redirect_output,	//">"
	redirect_input_output	//"< something >"
}Redirection_Flag;

typedef enum
{
	pipeline_none,
	pipeline_left,	//" |"
	pipeline_right	//"| "
}Pipeline_Flag;

typedef enum
{
	cm_none,
	cm_grep,
	cm_echo,
	cm_ls,
	cm_wc,
	cm_nsh,
	cm_find,
	cm_xargs,
	cm_cat
}Command_Type;


char* jump_blank(char *s);
char* get_argument(char* buffer, char* src);
char* get_arguments(char* buffer, char* src);
char simple_shell(char *argv);

int main(int argc, char *argv[])
{
	char argv_buffer[MAXARG + MAXARG];
	int argv_length;
	
	while (1)
	{
		// collect arguments
		printf("@ ");
		gets(argv_buffer, MAXARG + MAXARG);
		
		argv_length = strlen(argv_buffer);
		if (argv_length == 0)	// #1
			break;
		// delete the '\n' from the end of input sequence, and replace it with '\0'	
		argv_buffer[argv_length - 1] = '\0';
		
		simple_shell(argv_buffer);
  }
  exit(0);
}

// skip the blanks in front of the first argument at the string
char* jump_blank(char *s)
{
  while(*s != '\0')
  {
  	if(*s == ' ')
  	{
  		++s;
  	}
  	else
  		break;
  }
  return s;
}

// get one argument, it will also update the source string
char* get_argument(char* buffer, char* src)
{
	int i;
	src = jump_blank(src);
	i = 0;
	while ((*src != '\0')&&(*src != ' '))
	{
		buffer[i] = *src;
		++src;
		++i;
	}
	buffer[i] = '\0';
	return src;
}

// get arguments, it will also update the source string
char* get_arguments(char* buffer, char* src)
{
	int i;
	src = jump_blank(src);
	i = 0;
	while (*src != '\0')
	{
		buffer[i] = *src;
		++src;
		++i;
		if (((*src == '>')||(*src == '|'))&&(buffer[--i] == ' '))	// "something >"
			break;
	}
	buffer[i] = '\0';
	return src;
}


// output: 1 (fail), 0 (succeed)
char simple_shell(char *argv)
{
	char* s;
	char argv2_buffer[MAXARG];
	int cpid;
	Command_Type command = cm_none;
	
	char argument_buffer[MAXARG];
	char file_buffer[MAXARG];
	char file2_buffer[MAXARG];
	char *argv2[4];
	Redirection_Flag redirect_flag = redirect_none;
	Pipeline_Flag pipeline_flag = pipeline_none;
	int pipe_buffer[2];
	
	s = jump_blank(argv);	// skip any blank
	if (*s == '\0')	
	{
		return 0;
	}
	
	while (1)
	{
		s = get_argument(argv2_buffer, s);	// 1: get command
		
		if (strcmp(argv2_buffer, "grep") == 0)
		command = cm_grep;
		else if (strcmp(argv2_buffer, "echo") == 0)
		command = cm_echo;
		else if (strcmp(argv2_buffer, "ls") == 0)
		command = cm_ls;
		else if (strcmp(argv2_buffer, "wc") == 0)
		command = cm_wc;
		else if (strcmp(argv2_buffer, "nsh") == 0)
		command = cm_nsh;
		else if (strcmp(argv2_buffer, "find") == 0)
		command = cm_find;
		else if (strcmp(argv2_buffer, "xargs") == 0)
		command = cm_xargs;
		else if (strcmp(argv2_buffer, "cat") == 0)
		command = cm_cat;
		//printf("argv2_buffer is [%s]\n", argv2_buffer);
		if (*s != '\0')	// 2: get argument
		{
			if (command == cm_echo)
				s = get_arguments(argument_buffer, s);
			else
				s = get_argument(argument_buffer, s);
			
			// pipeline: '|'
			if ((argument_buffer[0] == '|')&&(argument_buffer[1] == '\0'))	//	or (strcmp(argument_buffer, "|") == 0)
			{
				pipeline_flag = pipeline_left;
				// get file
				get_arguments(file_buffer, s);
				if (file_buffer[0] == '\0')
				{
					printf("nsh: grep failed, need more arguments\n");
					return 1;
				}
				pipe(pipe_buffer);
			}// I/O redirection: '>'
			else if ((argument_buffer[0] == '>')&&(argument_buffer[1] == '\0'))	 //	or (strcmp(argument_buffer, ">") == 0)
			{
				redirect_flag = redirect_output;
				s = get_argument(file_buffer, s);
				if (file_buffer[0] == '\0')
				{
					printf("nsh: echo failed, need more arguments\n");
					return 1;
				}
			}	// I/O redirection: '<'
			else if ((argument_buffer[0] == '<')&&(argument_buffer[1] == '\0'))	//	or (strcmp(argument_buffer, "<") == 0)
			{
				redirect_flag = redirect_input;
				s = get_argument(file_buffer, s);
				if (file_buffer[0] == '\0')
				{
					printf("nsh: grep failed, need more arguments\n");
					return 1;
				}
			}
			
		}
		else	// some command don't need argument
		{
			if ((command != cm_ls)&&(command != cm_wc)&&(command != cm_nsh)&&( (command != cm_cat)&&(pipeline_flag != pipeline_right)) )
			{
				printf("nsh: grep failed, need more arguments\n");
				return 1;
			}
		}
		
		
		if ((*s != '\0')&&( ((pipeline_flag != pipeline_left)&&(pipeline_flag != pipeline_right)) || (command == cm_xargs) ))	// 3: get extra argument
		{
			char temp_buffer[MAXARG];
			
			s = get_argument(temp_buffer, s);
			// I/O redirection: '<'
			if ((temp_buffer[0] == '<')&&(temp_buffer[1] == '\0'))	//	or (strcmp(temp_buffer, "<") == 0)
			{
				redirect_flag = redirect_input;
				// get file, here it will overwrite file_buffer
				s = get_argument(file_buffer, s);
				
				if (file_buffer[0] == '\0')
				{
					printf("nsh: grep failed, need more arguments\n");
					return 1;
				}
			}
			// I/O redirection: '>'
			else if ((temp_buffer[0] == '>')&&(temp_buffer[1] == '\0'))	 //	or (strcmp(temp_buffer, ">") == 0)
			{
				if (redirect_flag == redirect_input)
				{
					redirect_flag = redirect_input_output;
					s = get_argument(file2_buffer, s);
					if (file2_buffer[0] == '\0')
					{
						printf("nsh: echo failed, need more arguments\n");
						return 1;
					}
				}
				else
				{
					redirect_flag = redirect_output;
					s = get_argument(file_buffer, s);	// get file, here it will overwrite file_buffer
					if (file_buffer[0] == '\0')
					{
						printf("nsh: echo failed, need more arguments\n");
						return 1;
					}
				}
			}
			// pipeline: '|'
			else if ((temp_buffer[0] == '|')&&(temp_buffer[1] == '\0'))	//	or (strcmp(temp_buffer, "|") == 0)
			{
				pipeline_flag = pipeline_left;
				// get file
				get_arguments(file_buffer, s);
				if (file_buffer[0] == '\0')
				{
					printf("nsh: grep failed, need more arguments\n");
					return 1;
				}
				pipe(pipe_buffer);
			}
			
			// pipe and redirects or pipe after extra argument
			if ((*s != '\0')&&(pipeline_flag != pipeline_left))
			{
				char temp_buffer[MAXARG + 100];
				s = get_argument(temp_buffer, s);
				// pipeline: '|'
				if ((temp_buffer[0] == '|')&&(temp_buffer[1] == '\0'))	//	or (strcmp(temp_buffer, "|") == 0)
				{
					pipeline_flag = pipeline_left;
					// get file
					get_arguments(temp_buffer, s);
					//printf("|: temp_buffer = %s\n", temp_buffer);
					if (temp_buffer[0] == '\0')
					{
						printf("nsh: grep failed, need more arguments\n");
						return 1;
					}
					pipe(pipe_buffer);
				}	// I/O redirection: '>'
				else if ((temp_buffer[0] == '>')&&(temp_buffer[1] == '\0'))	 //	or (strcmp(temp_buffer, ">") == 0)
				{
					if (redirect_flag == redirect_input)
					{
						redirect_flag = redirect_input_output;
						s = get_argument(file2_buffer, s);
						if (file2_buffer[0] == '\0')
						{
							printf("nsh: echo failed, need more arguments\n");
							return 1;
						}
					}
				}
			}
			
			if ((redirect_flag == redirect_none)&&(pipeline_flag == pipeline_none))
			{
				if (command == cm_grep)
				{
					strcpy(file_buffer, temp_buffer);
				}
			}
			
		}
		else	// some command don't need extra argument
		{
			if ((command != cm_echo)&&(command != cm_ls)&&(command != cm_find)&&(command != cm_cat)\
			&&(pipeline_flag != pipeline_left)&&(pipeline_flag != pipeline_right))
			{
				printf("nsh: grep failed, need more arguments\n");
				return 1;
			}
		}
		
		argv2[0] = argv2_buffer;
		argv2[3] = 0;	// end?
		// for argv2[1]
		if ((command == cm_ls)||(command == cm_wc)||(command == cm_nsh))
		{
			argv2[1] = 0;
		}
		else if (command == cm_cat)
		{
			if ((redirect_flag == redirect_input)||(pipeline_flag == pipeline_right))
				argv2[1] = 0;
			else
				argv2[1] = argument_buffer;
		}
		else
			argv2[1] = argument_buffer;
		// for argv2[2]
		if (command == cm_grep)
		{
			if ((redirect_flag == redirect_none)&&(pipeline_flag == pipeline_none))
				argv2[2] = file_buffer;
			else if ((redirect_flag == redirect_input)||(redirect_flag == redirect_input_output)||(pipeline_flag == pipeline_right))
				argv2[2] = 0;
		}
		else if ((command == cm_echo)||(command == cm_ls)||(command == cm_wc)||(command == cm_nsh)||(command == cm_cat))
		{
			argv2[2] = 0;
		}
		else if (command == cm_find)
		{
			if (redirect_flag == redirect_none)
				argv2[2] = file_buffer;
		}
		else if (command == cm_xargs)
			argv2[2] = file_buffer;
		cpid = fork();
		if (cpid > 0)	// Parent Process 
		{ 
			if (pipeline_flag == pipeline_left)
			{
				pipeline_flag = pipeline_right;
				close(pipe_buffer[1]);	// close write end for next child to read the pipeline
				redirect_flag = redirect_none;	// finish redirection
				continue;
			}
			else
			{
				wait(0);	// parent waits for child
				//printf("exec succeed\n");
				if (pipeline_flag == pipeline_right)
				{
					wait(0);	// pipeline will create two children
					close(pipe_buffer[0]);	// pipeline finish
				}
			}
			break;
		} 
		else if (cpid == 0)	// Child Process 
		{ 
			if (redirect_flag == redirect_input)
			{
				close(0);	// close standard input 0 and redirect it to file_buffer
				open(file_buffer, O_RDONLY);
			}
			else if (redirect_flag == redirect_output)
			{
				close(1);	// close standard output 1 and redirect it to file_buffer
				open(file_buffer, O_CREATE | O_WRONLY);
			}
			else if (redirect_flag == redirect_input_output)
			{
				close(0);	// close standard input 0 and redirect it to file_buffer
				open(file_buffer, O_RDONLY);
				close(1);	// close standard output 1 and redirect it to file2_buffer
				open(file2_buffer, O_CREATE | O_WRONLY);
			}
			
			if (pipeline_flag == pipeline_left)
			{
				close(1);	// close standard output 1 and redirect it to pipeline (note here printf() will be invalid)
				dup(pipe_buffer[1]);
				close(pipe_buffer[0]); close(pipe_buffer[1]);
			}
			else if (pipeline_flag == pipeline_right)
			{
				close(0);	// close standard input 0 and redirect it to pipeline
				dup(pipe_buffer[0]);
				close(pipe_buffer[0]);
			}
			
			exec(argv2[0], argv2);
			printf("exec error\n");
			return 1;
		} 
		else 
		{
			printf("1:Fork failed\n");
			return 1;
		}
	
	}
	
	return 0;	// succeed
	
}


