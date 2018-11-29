#include <stdio.h>  //fgets()
#include <string.h> //strlen()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  //open
#include <unistd.h> //access()
#include <errno.h>  //errno;

#define MAXFILENAMELENGTH 255

int p_menu(void);
//int open_file(int );
void create_file(void);
void write_file(void);	void get_text_into_file(int );
void read_file(void);
void delete_file(void);
int get_file_name(char *, int);
int get_stdin_string(char *, int);

int main(void)
{
	int choix;
	for (;;)
	{
		choix = p_menu();
		switch (choix)
		{
		case 1:
			create_file();
			break;
		case 2:
			write_file();
			break;
		case 3:
			read_file();
			break;
		case 4:
			delete_file();
			break;
		case 5:
			goto end_for;
		}
	}
end_for:;
}

int p_menu(void)
{
	int choix = 0;
	do
	{
		printf("1 - Create file\n");
		printf("2 - Write into a file\n");
		printf("3 - Read from a file\n");
		printf("4 - Delete a file\n");
		printf("5 - END\n");

		printf("Enter Choice : ");
		scanf("%d", &choix);
		getchar();
	} while (choix < 1 || choix > 5);
	return choix;
}

void create_file(void)
{
	int flags = O_RDONLY | O_CREAT | O_EXCL /*| O_NOATIME Don’t update file last access time*/;
	//setting mode for creat()scanf
	int fd, mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
	char name[MAXFILENAMELENGTH]; /*setting pathname for creat()*/

	printf("_________________________________________\n\n");
	printf("Set file s name (%d char max): ", MAXFILENAMELENGTH);
	if (get_file_name(name, MAXFILENAMELENGTH) <= 0)
	{
		printf("Invalid file name!!\n");
		printf("_________________________________________\n\n");
		return;
	}

	printf("_________________________________________\n");
	umask(0);
	if ((fd = open(name, flags, mode)) != -1)
	{ //if file 'name' doesn t exist Create the file
		close(fd);
		printf("\nDone.\nFile %s, created!!\n", name);
	}
	else
	{
		if (errno == EEXIST)
		{ //if the file exists
			fprintf(stderr, "\nNothing to do here!\nthe File \"%s\", already exists delete it first!!!!\n", name);
		}
		else
		{ //if their was an error in accessing the file $name
			perror("\nOpen!");
		}
	}
	printf("_________________________________________\n\n");
}

void write_file(void)
{
	printf("_________________________________________\n\n");
	int fd, flags = O_WRONLY | O_APPEND;
	char trunc_opt, name[MAXFILENAMELENGTH];

	printf("Enter the file name to write into : ");		//Entering the name of the file
	if (get_file_name(name, MAXFILENAMELENGTH) <= 0)
	{
		printf("Invalid file name!!\n");
		printf("_________________________________________\n\n");
		return;
	}

	printf("Do you want to truncate the file?(y/n): ");	//Asking if you want to append or truncate the file 
	do
	{
		get_stdin_string(&trunc_opt, 1);
		if (trunc_opt == 'y' || trunc_opt == 'Y')
		{
			flags |= O_TRUNC;
			break;
		}
		else if (trunc_opt == 'n' || trunc_opt == 'N')
			break;
	} while (1);

	if ((fd = open(name, flags)) == -1)					//Opening the file $name 
	{
		perror("Open! :");
		return;
	}

	get_text_into_file(fd);

	close(fd);
}

void read_file(void)
{
	int fd, flags = O_RDONLY, bytes_read; 
	char name[MAXFILENAMELENGTH], buffer[1];
	printf("_________________________________________\n\n");
	
	printf("Enter the file name to read from : ");			//Entering the name of the file
	if (get_file_name(name, MAXFILENAMELENGTH) <= 0)
	{
		printf("Invalid file name!!\n");
		printf("_________________________________________\n\n");
		return;
	}
	
	if ((fd = open(name, flags)) == -1)						//Opening the file $name 
	{
		perror("Open! :");
		return;
	}

	while((bytes_read = read(fd, buffer, 1)) != 0 )		//Reading 1*bytes - stops at EOF
	{
		if(bytes_read == -1)
		{
			perror("Read : ");return;
		}
		printf("%c",*buffer);
	}

	printf("\n_________________________________________\n\n");
	close(fd);
}

void delete_file(void)
{
	printf("_________________________________________\n\n");

	char name[MAXFILENAMELENGTH];

	printf("Enter the file name the delete : ");
	if (get_file_name(name, MAXFILENAMELENGTH) <= 0)
	{
		printf("Invalid file name!!\n");
		printf("_________________________________________\n\n");
		return;
	}

	if (remove(name) == -1)
	{
		perror("Remove!");
		return;
	}
	else
		printf("The file '%s', was successfully deleted!\n", name);

	printf("_________________________________________\n\n");
}

int get_file_name(char *file_name, int size)
{
	int len;
	if ((len = get_stdin_string(file_name, size)) == -1 || len == 0) //error or 0 lenght name
		return -1;
	else
	{
		for (int i = 0; i < len; ++i)
		{
			switch (file_name[i])
			{
			case '\\':
			case '/':
			case '?':
			case '%':
			case '*':
			case ':':
			case '|':
			case '"':
			case '<':
			case '>':
				return -2;
				break;
			}
		}
	}
	return len;
}


void get_text_into_file(int txt_fd){
	char c; int bytes_writen;
	printf("Enter the text - (ctrl + D) to quit - :\n");

	while((c = getchar()) != EOF){
		if((bytes_writen = write(txt_fd , &c, 1)) == -1 ){
			perror("Write : "); return;
		}else if(bytes_writen != 1){
			fprintf(stderr, "Nothing was written!\n");
		}
	}
	printf("\n_________________________________________\n\n");
}

//get a string ended with a '\n'--> delete '\n' --> clear stdin buffer
int get_stdin_string(char *s, int size)
{
	//getting a clean $size char input from the stdin
	if (fgets(s, size + 1, stdin) == NULL)
	{ //fgets takes size chars max ended with a '\0'
		return -1;
	}
	int len = strlen(s);
	if (size == len && s[len - 1] != '\n') //if their s still more characters in the buffer stdin (detected by not having '\n')
		while (getchar() != '\n')
			; // clear the stdin buffer
	else
		s[len - 1] = '\0';
	//return the number of characters read
	return strlen(s);
}

