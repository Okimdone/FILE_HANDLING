#include <stdio.h>  //fgets()
#include <string.h> //strlen()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  //open
#include <unistd.h> //access()
#include <errno.h>  //errno;
//#include <stdlib.h> //exit()

#define MAXFILENAMELENGTH 255
#define MAX_FILE_NAME 30
#define MAX_FILE_NUMBER 20
#define BUFFER_SIZE 4096

int p_menu(void);
void create_file(void);
void write_file(void);	void get_text_into_file(int );
void read_file(void);
void delete_file(void);
int get_file_name(char *, int);
int get_stdin_string(char *, int);
//cp functions
/*CP main fontion*/
void CP();
/*Menu pour choisir la 'copier' voulu, elle retourne le choit*/
int menu_cp(void); 
void cp2(void);
void cp_append(void);
void cp3(void);
void copy_in_to_out(int infd, int outfd);

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
			CP();
			break;
		case 6:
			goto end_for;
		}
	}
end_for:;
}

int p_menu(void)
{
	int choix = 0; char c;
	do
	{
		printf("\n\t===============>MENU<===============\n");
		printf(" _______________________________________________________\n");
		printf("|\t\t|\t                  \t\t|\n");
		printf("|\t1\t|\tCreer un fichier      \t\t|\n");
		printf("|\t2\t|\tecrire dans un fichier\t\t|\n");
		printf("|\t3\t|\tlire un fichier       \t\t|\n");
		printf("|\t4\t|\tsupprimer un fichier  \t\t|\n");
		printf("|\t5\t|\tcopier un ficher      \t\t|\n");
		printf("|\t6\t|\tQuitter               \t\t|\n");
		printf("|_______________________________________________________|\n");

		printf("Entrez votre choix : ");
		scanf("%d", &choix);
		//clear input
		while((c=getchar()) != '\n' && c != EOF ){}
	} while (choix < 1 || choix > 6);
	return choix;
}

void create_file(void)
{
	int flags = O_RDONLY | O_CREAT | O_EXCL /*| O_NOATIME Don’t update file last access time*/;
	//setting mode for creat()scanf
	int fd, mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
	char name[MAXFILENAMELENGTH]; /*setting pathname for creat()*/

	printf("_________________________________________\n\n");
	printf("Entrez le nom du fichier (%d char max): ", MAXFILENAMELENGTH);
	if (get_file_name(name, MAXFILENAMELENGTH) <= 0)
	{
		printf("Nom de fichier non valide!!\n");
		printf("_________________________________________\n\n");
		return;
	}

	printf("_________________________________________\n");
	umask(0);
	if ((fd = open(name, flags, mode)) != -1)
	{ //si le fichier 'name' n'existe pas Créez le fichier
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
		{ //s'il y a eu une erreur d'accès au fichier $name
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

	printf("Entrez le nom du fichier dans lequel on écrira : ");		//Entrer le nom du fichier
	if (get_file_name(name, MAXFILENAMELENGTH) <= 0)
	{
		printf("Invalid file name!!\n");
		printf("_________________________________________\n\n");
		return;
	}

	printf("Voulez-vous tronquer le fichier?(o/n): ");	//Demander si vous souhaitez ajouter à ou tronquer le fichier
	do
	{
		get_stdin_string(&trunc_opt, 1);
		if (trunc_opt == 'o' || trunc_opt == 'O')
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
	
	printf("Entrez le nom du fichier à lire : ");			//Entering the name of the file
	if (get_file_name(name, MAXFILENAMELENGTH) <= 0)
	{
		printf("Nom de fichier non valide!!\n");
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
			perror("Read :");return;
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

	printf("Entrez le nom du fichier à supprimer : ");
	if (get_file_name(name, MAXFILENAMELENGTH) <= 0)
	{
		printf("Nom de fichier non valide!!\n");
		printf("_________________________________________\n\n");
		return;
	}

	if (remove(name) == -1)
	{
		perror("Remove!");
		return;
	}
	else
		printf("Le fichier '%s', a été supprimé avec succès!\n", name);

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
	printf("Entrer le texte - (ctrl + D) pour quitter - :\n");

	while((c = getchar()) != EOF){
		if((bytes_writen = write(txt_fd , &c, 1)) == -1 ){
			perror("Write : "); return;
		}else if(bytes_writen != 1){
			fprintf(stderr, "Rien n'a été écrit!\n");
		}
	}
	printf("\n_________________________________________\n\n");
}

//obtenir une chaîne terminée par '\n'--> supprimer '\n' --> effacer le buffer stdin
int get_stdin_string(char *s, int size)
{
	//obtenir une entrée brute de caractère $size à partir du stdin
	if (fgets(s, size + 1, stdin) == NULL)
	{ //fgets prend les caractères de taille max terminés par une '\0'
		return -1;
	}
	int len = strlen(s);
	if (size == len && s[len - 1] != '\n') //s'il y a encore plus de caractères dans le tampon stdin (détecté en n'ayant pas '\n')
		while (getchar() != '\n')
			; // effacer le tampon stdin
	else
		s[len - 1] = '\0';
	//retourne le nombre de caractères lus
	return strlen(s);
}

int menu_cp(void){
    printf("\n\t================>Coping Program<================\n");
    printf(" _______________________________________________________________\n");
    printf("|\t\t|                                            \t|\n");
    printf("|\t1\t|Copier un fichier dans [path]/nomfichier   \t|\n");
    printf("|\t2\t|Joindre un fichier dans [path]/nomfichier  \t|\n");
    printf("|\t3\t|Copier deux fichiers dans [path]/nomfichier\t|\n");
    printf("|_______________________________________________________________|\n");
    int x;char c;
    do{
        printf("Choix : ");
        scanf("%d", &x); while((c=getchar()) != '\n' && c != EOF ){}
    }while(x < 1 || x > 3);	return x;
}

void CP(){
	int choix = menu_cp();
    switch(choix){
        case 1:
            cp2();
			break;
        case 2:
            cp_append();
			break;
        case 3:
            cp3();
			break; 
    } 
}

void cp2(void){
    char in_name[MAX_FILE_NAME], out_name[MAX_FILE_NAME];
    printf("Nom du inFile  : ");	get_file_name(in_name , MAX_FILE_NAME);
    printf("Nom du outFile : ");	get_file_name(out_name, MAX_FILE_NAME);
    
	//openning file1 for read only
    int infd, outfd;
	infd=open(in_name, O_RDONLY);
	if(infd==-1){
		perror("inFile :");return;
	}

    //openning file2 for writeing (Create / erase if it exists)
	int flags= O_WRONLY | O_CREAT | O_TRUNC; 
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	outfd = open(out_name, flags, mode);
	if(outfd==-1){
		perror("Outfiles :");return;
	}

	//Coping process
	copy_in_to_out(infd, outfd);

	//closing files
	if(close(infd)==-1)
		perror("in F close");

	if(close(outfd)==-1)
		perror("out F close");
}

void cp_append(void){
    char in_name[MAX_FILE_NAME], out_name[MAX_FILE_NAME];
    printf("Nom du inFile  : ");	get_file_name(in_name , MAX_FILE_NAME);
    printf("Nom du outFile : ");	get_file_name(out_name, MAX_FILE_NAME);
  
	//openning file1 for read only
    int infd, outfd;
	infd=open(in_name, O_RDONLY);
	if(infd==-1){
		perror("inFile :");return;
	}

	//openning file2 for appending
	int flags= O_WRONLY | O_CREAT | O_APPEND ;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	outfd = open(out_name, flags, mode);
	if(outfd==-1){
		perror("Outfiles :");return;
	}

	//Coping process
	copy_in_to_out(infd, outfd);

	//closing files
	if(close(infd)==-1)
		perror("in F close");

	if(close(outfd)==-1)
		perror("out F close");

}

void cp3(void){
    char in1_name[MAX_FILE_NAME], in2_name[MAX_FILE_NAME], out_name[MAX_FILE_NAME];
    printf("Nom du premier inFile : " );		get_file_name(in1_name, MAX_FILE_NAME);
    printf("Nom du deuxième inFile : ");		get_file_name(in2_name, MAX_FILE_NAME);
    printf("Nom du OutFile: ");					get_file_name(out_name, MAX_FILE_NAME);
	
	//openning file1 for read only
    int in1fd, in2fd, outfd;
	if(( in1fd=open(in1_name, O_RDONLY) ) == -1){
		perror("inFile 1 :");return;
	}

	//openning file2 for read only
	if(( in2fd=open(in2_name, O_RDONLY) ) == -1){
		perror("inFile 2 :");return;
	}

	//openning file2 for appending
	int flags= O_WRONLY | O_CREAT | O_TRUNC | O_APPEND ; 
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	if(( outfd = open(out_name, flags, mode) ) == -1){
		perror("Outfiles :");return;
	}

	//Coping file1 into outfile
	copy_in_to_out(in1fd, outfd);
	//Coping file2 into outfile
	copy_in_to_out(in2fd, outfd);

	if(close(in1fd)==-1)
		perror("intfile 1 close");
	if(close(in2fd)==-1)
		perror("intfile 2 close");
	if(close(outfd)==-1)
		perror("outfile close");
}

void copy_in_to_out(int infd, int outfd){
    //coping the content of file1 into file2
	char buf[BUFFER_SIZE];
	ssize_t rdbytes;						//read bytes
	while((rdbytes = read(infd, buf, BUFFER_SIZE)) != 0){
		if(rdbytes == -1){	
			//if read() was not interrupted
			if(errno != EAGAIN && errno !=EINTR){ 
				perror("reading inFile");
				return;
			}
		}
		//writing into out file
		if( write(outfd, buf, strlen(buf)) == -1)
		{	
			perror("writting error"); return;
		}
	}
}