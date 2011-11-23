#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define NAME_LENGTH_MAX 256
#define BUFFER_SIZE 1024
#define MAX_COUNTER_FILE 1024

int numbers = 0;

void get_file_info(int,char*);
void sort(int ,struct dirent **);


int main(int argc, char** argv)
{

	char buffer[BUFFER_SIZE];

	if((getcwd(buffer,sizeof(buffer)) ==NULL))
	{
		fprintf(stdout,"%s:%d:Get current directory failed:%s\n",__FILE__,__LINE__,strerror(errno));
		exit(1);
	}
	DIR* pdir;
	if((pdir = opendir(buffer)) ==NULL )
	{
		fprintf(stderr,"%s:%d:cannot open current path %s:%s\n",__FILE__,__LINE__,buffer,strerror(errno));
		exit(1);
	}
//	fprintf(stdout,"%d:Errorr-1\n",__LINE__);
	/*Create a array of struct dirent * to storage all the file information*/
	/* */
	struct dirent* c_dirent[MAX_COUNTER_FILE];

	int i;
	int counter;

	for(i = 0; i < MAX_COUNTER_FILE ;i++)
	{
		c_dirent[i] = NULL;
	}

	i = 0;

	while((c_dirent[i] = readdir(pdir)) != NULL)
	{
		counter ++;
		i ++;
	}
//	struct dirent* c_dirent;
	/*-------------------------------------------------------------------------------*/
	/*make a sort for the element of struct dirent ** according to the direct->d_name*/
	/*-------------------------------------------------------------------------------*/
	sort(counter,c_dirent);
	
	char file_name[BUFFER_SIZE];

 	/*printf the information to stdout one by one*/
	for(i = 0; i < counter ; i++ )
	{
		memcpy(file_name ,c_dirent[i]->d_name,strlen(c_dirent[i]->d_name)+1);
		
		int flags;
		if(argc < 2)
		{
			flags = 0;
		}
		else
		{
			int ch ;
			while((ch = getopt(argc,argv,"ahli")) != -1)
			{
				switch(ch)
				{		
				case 'a':
					flags = flags | 1;
				break;
				case 'h':
					flags = flags | 2;
				break;
				case 'i':
					flags = flags | 4;
				break;
				case 'l':
					flags = flags | 8;
				break;
				default:
				break ;
				}
			}	
		}
	
		get_file_info(flags,file_name);
	}
		

	closedir(pdir);
	return 0;

}

void get_file_info(int flags,char* file_name)
{
	struct stat s;
	if(lstat(file_name,&s) < 0)
	{
		fprintf(stdout,"cannot get information from file %s  :%s\n",file_name,strerror(errno));
		//exit(1);
	}

	char permission[] = "-rwxrwxrwx";

	int mode = s.st_mode ;
	int i;
	/* have not carry out setuid bit*/
	for(i = 0; i < 9 ; i++)
	{
		if( (mode & 0x01) ==0 )
		{
			permission[9 - i] = '-';
		}
		mode = mode >> 1;
	}
  /*
           S_IFMT     0170000   bit mask for the file type bit fields
           S_IFSOCK   0140000   socket
           S_IFLNK    0120000   symbolic link
           S_IFREG    0100000   regular file
           S_IFBLK    0060000   block device
           S_IFDIR    0040000   directory
           S_IFCHR    0020000   character device
           S_IFIFO    0010000   FIFO
           S_ISUID    0004000   set UID bit
           S_ISGID    0002000   set-group-ID bit (see below)
           S_ISVTX    0001000   sticky bit (see below)
           S_IRWXU    00700     mask for file owner permissions
           S_IRUSR    00400     owner has read permission
           S_IWUSR    00200     owner has write permission
           S_IXUSR    00100     owner has execute permission
           S_IRWXG    00070     mask for group permissions
           S_IRGRP    00040     group has read permission
           S_IWGRP    00020     group has write permission
           S_IXGRP    00010     group has execute permission
           S_IRWXO    00007     mask for permissions for others (not in group)
           S_IROTH    00004     others have read permission
           S_IWOTH    00002     others have write permission
           S_IXOTH    00001     others have execute permission
   */
	int file_type;
	file_type = s.st_mode & S_IFMT ;
	
	file_type = file_type >>12 ;
	char filetype_s[32];

	char* filetype_l[32];
	{
		for(i = 0;i < 32 ; i++)
			filetype_l[i] = NULL;
	}

	filetype_l[01] = "FIFO";
	filetype_l[02] = "character device";
	filetype_l[04] = "directory";
	filetype_l[06] = "block devece";
	filetype_l[010] = "regular file";
	filetype_l[012] = "symbolic link";
	filetype_l[014] = "sorket";

	filetype_s[01] = 'p';
	filetype_s[02] = 'c';
	filetype_s[04] = 'd';
	filetype_s[06] = 'b';
	filetype_s[010] = '-';
	filetype_s[012] = 'l';
	filetype_s[014] = 's';
	
	permission[0] = filetype_s[file_type];


	struct passwd* pwd ;
	struct group* grp;
	struct tm *t;

	pwd = getpwuid(s.st_uid);
	grp = getgrgid(s.st_gid);
	t = localtime(&s.st_mtime);

	if(flags == 0 || flags == 2)
	{
		if(file_name[0] != '.')
		{
			fprintf(stdout,"%-16s",file_name);
			numbers ++;
			if(numbers % 5 == 0)
			{
				fprintf(stdout,"\n");
			}
		}

	}
	else if(flags == 1 || flags == 3 )
	{
		fprintf(stdout,"%-16s",file_name);
		numbers ++;
		if(numbers % 5 == 0 )
		{
			fprintf(stdout,"\n");
		}
	}
	else if(flags == 4 || flags == 6)
	{
		if(file_name[0] != '.')
		{
			fprintf(stdout,"%-6d %-16s",(int)s.st_ino,file_name);
			numbers++;
			if(numbers % 4 == 0)
			{
				fprintf(stdout,"\n");
			}
		}
	}
	else if(flags == 5 || flags == 7)
	{
		fprintf(stdout,"%-6d %-16s",(int)s.st_ino,file_name);
		numbers++;
		if(numbers % 4 == 0)
		{
			fprintf(stdout,"\n");
		}
	}
	else if(flags == 8)
	{
		if(file_name[0] != '.')
		{
			fprintf(stdout,"%s %2d %4s %4s %8d %4d-%02d-%02d %02d:%02d %s\n",permission,s.st_nlink,pwd->pw_name,grp->gr_name,(int)s.st_size,t->tm_year+1900,t->tm_mon + 1,t->tm_mday,t->tm_hour,t->tm_min,file_name);
		}
	}
	else if(flags == 9)
	{
		fprintf(stdout,"%s %2d %4s %4s %8d %4d-%02d-%02d %02d:%02d %s\n",permission,s.st_nlink,pwd->pw_name,grp->gr_name,(int)s.st_size,t->tm_year+1900,t->tm_mon + 1,t->tm_mday,t->tm_hour,t->tm_min,file_name);
	}
	else if(flags == 10)
	{
		if(file_name[0] != '.')
		{
			fprintf(stdout,"%s %2d %4s %4s %8dK %4d-%02d-%02d %02d:%02d %s\n",permission,s.st_nlink,pwd->pw_name,grp->gr_name,(int)s.st_size,t->tm_year+1900,t->tm_mon + 1    ,t->tm_mday,t->tm_hour,t->tm_min,file_name);

		}
	}
	else if(flags == 11)
	{
		fprintf(stdout,"%s %2d %4s %4s %8dK %4d-%02d-%02d %02d:%02d %s\n",permission,s.st_nlink,pwd->pw_name,grp->gr_name,(int)s.st_size,t->tm_year+1900,t->tm_mon + 1,t->tm_mday,t->tm_hour,t->tm_min,file_name);
	}
	else if(flags == 12)
	{
		if(file_name[0] != '.')
		{
			fprintf(stdout,"%-d %s %2d %4s %4s %8d %4d-%02d-%02d %02d:%02d %s\n",(int)s.st_ino,permission,s.st_nlink,pwd->pw_name,grp->gr_name,(int)s.st_size,t->tm_year+1900,t->tm_mon + 1,t->tm_mday,t->tm_hour,t->tm_min,file_name);
		}
	}
	else if(flags == 13)
	{
		fprintf(stdout,"%-d %s %2d %4s %4s %8d %4d-%02d-%02d %02d:%02d %s\n",(int)s.st_ino,permission,s.st_nlink,pwd->pw_name,grp->gr_name,(int)s.st_size,t->tm_year+1900,t->tm_mon + 1,t->tm_mday,t->tm_hour,t->tm_min,file_name);

	}
	else if(flags == 14)
	{
		if(file_name[0] != '.')
		{
			fprintf(stdout,"%-d %s %2d %4s %4s %8dK %4d-%02d-%02d %02d:%02d %s\n",(int)s.st_ino,permission,s.st_nlink,pwd->pw_name,grp->gr_name,(int)s.st_size,t->tm_year+1900,t->tm_mon + 1,t->tm_mday,t->tm_hour,t->tm_min,file_name);
		}
	}
	else if(flags == 15)
	{
		fprintf(stdout,"%-d %s %2d %4s %4s %8dK %4d-%02d-%02d %02d:%02d %s\n",(int)s.st_ino,permission,s.st_nlink,pwd->pw_name,grp->gr_name,(int)s.st_size,t->tm_year+1900,t->tm_mon + 1,t->tm_mday,t->tm_hour,t->tm_min,file_name);
	}


//	fprintf(stdout,"%s %2d %4s %4s %8d %4d-%02d-%02d %02d:%02d %-16s %s\n",permission,s.st_nlink,pwd->pw_name,grp->gr_name,(int)s.st_size,t->tm_year+1900,t->tm_mon + 1,t->tm_mday,t->tm_hour,t->tm_min,filetype_l[file_type],file_name);



}

void sort(int counter,struct dirent* c_dirent[])
{
	struct dirent* temc_dirent ;

	int i,j,k;
	int flag = 0;
	for(i= 0; i < counter ; i++ )
	{
		for(j = 1; j < counter -i ;j++)
		{
			/*sort*/
			for(k = 0; k < NAME_LENGTH_MAX ; k++ )
				/*循环比较当前元素的每个字符，当遇到不相等时退出循环，并置位标志位*/
			{
				if((isalpha(((c_dirent[i])->d_name)[k]) != 0) && (isalpha(((c_dirent[i+j])->d_name)[k]) != 0))
					/*当比较的字符都是字母时*/
				{
					if((isupper(((c_dirent[i])->d_name)[k]) != 0) && (isupper(((c_dirent[i+j])->d_name)[k]) == 0))
						/*如果比较的字符大小写有区别则判断*/
					{
							flag = ((c_dirent[i])->d_name)[k] -((c_dirent[i+j])->d_name)[k] + 32;
							if(flag == 0)
								/*如果比较的俩个字符相等，则继续比较下一个字符，直到比较出大小*/
								continue;
							else
								/*如果比较的2个字符大小不相等，退出下当前循环，判断flag*/
								break;
					}
					else if((isupper(((c_dirent[i])->d_name)[k]) == 0) && (isupper(((c_dirent[i+j])->d_name)[k]) != 0))
					{
						/*如果比较大小写有区别则判断*/
							flag = ((c_dirent[i])->d_name)[k] -((c_dirent[i+j])->d_name)[k] - 32;
							if(flag == 0)
								continue;
							else
								break;
					}
					else
						/*如果同是大写或者小写*/
					{
						flag = ((c_dirent[i])->d_name)[k] - ((c_dirent[i+j])->d_name)[k];
						break;

					}
				}

				else if((isalpha(((c_dirent[i])->d_name)[k]) == 0) && (isalpha(((c_dirent[i+j])->d_name)[k]) == 0))
					/*当比较的字符都不是字符*/
				{
					flag = ((c_dirent[i])->d_name)[k] -((c_dirent[i+j])->d_name)[k] ;
					if(flag == 0)
						/*相等则比下一个*/
						continue;
					break;
				}
				else
				{
					flag = ((c_dirent[i])->d_name)[k] - ((c_dirent[i+j])->d_name)[k];
					break;
				}
			}
			if( flag > 0 )
				/*当当前元素字符比下一个字符大，则交换2个元素的位置*/
			{
				temc_dirent = c_dirent[i] ;	
				c_dirent[i] = c_dirent[i+j] ;
				c_dirent[i+j] = temc_dirent ;
			}
			else
				/*如果当前元素字符比下一个字符小，则不操作，并比下一对元素*/
				continue;
		}
	}
}

