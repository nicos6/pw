#include <stdio.h>
#include <stdlib.h>   //u_int32_t
#include <unistd.h>   //close, read, write, fseek
#include <fcntl.h>    //oflag
#include <sys/stat.h> //pt stat
#include <time.h>     //pentru functia ctime
#include <dirent.h>
#include <string.h>

int main(int argc, char *argv[])
{
  if (argc != 2){
    printf("Numar gresit de argumente\n");
    exit(-1);
  }

  char *file_input = argv[1];

  struct stat infos;
  if (stat(file_input, &infos) == -1){
    printf("Eroare functia stat");
    exit(-1);
  }

  if (S_ISDIR(infos.st_mode) == 0){
    printf("Parametrul transmis nu este director\n");
    exit(-1);
  }

  DIR *director = NULL;
  if ((director = opendir(file_input)) == NULL){
    printf("Eroare deschidere director");
    exit(-1);
  }

  int fd2;
  if (creat("statistica.txt", S_IRUSR | S_IWUSR) == -1){
    perror("Eroare creare fisier statistica");
  }

  if ((fd2 = open("statistica.txt", O_WRONLY | O_TRUNC)) == -1)
    perror("Eroare deschidere fisier statistica");

  struct dirent *entry;
  while ((entry = readdir(director)) != NULL){

    int size_path = strlen(file_input) + 1 + strlen(entry->d_name) + 1;
    char *path = malloc(size_path);

    if (path == NULL){
      perror("Eroare alocare memorie dinamic");
      exit(-1);
    }

    // calea absoluta
    snprintf(path, size_path, "%s/%s", file_input, entry->d_name);

    if (lstat(path, &infos) == -1){ //daca e legatura simbolica, ia inf despre ea, nu despre parinte
      perror("Eroare functia stat");
    }
    

    if (S_ISREG(infos.st_mode) == 1){ // daca fisierul este obisnuit
      char *extensie = strchr(path, '.');
      if (extensie != NULL && strcmp(extensie, ".bmp") == 0){ // daca e bmp file

        // deschidem imaginea
        int fd1;
        if ((fd1 = open(path, O_RDONLY)) == -1)
          perror("Eroare deschidere fisier");

        u_int32_t height = 0, width = 0;
        lseek(fd1, 18, SEEK_SET);

        if (read(fd1, &height, 4) == -1){
          perror("Eroare citire inaltime fisier bmp");
        }

        if (read(fd1, &width, 4) == -1){
          perror("Eroare citire inaltime fisier bmp");
        }
        close(fd1);

        char statistica[300];
        int size = 0;
        size = sprintf(statistica, "Nume fisier: %s\nInaltime: %d\nLatime: %d\n\n", entry->d_name, height, width);

        if (write(fd2, statistica, size) == -1){
          perror("Eroare scriere fisier output");
        }
      }
      
      else
	{
	  char statistica[300];
	  int size = 0;
	  size = sprintf(statistica, "Nume fisier: %s\n\n", entry->d_name);
	  
	  if (write(fd2, statistica, size) == -1){
	      perror("Eroare scriere fisier output");
	    }
	}
    }
    else if (S_ISLNK(infos.st_mode) == 1) { // daca e legatura
	
        char statistica[300];
        int size = 0;
        size = sprintf(statistica, "Nume legatura: %s\n\n", entry->d_name);
	
        if (write(fd2, statistica, size) == -1){
	    perror("Eroare scriere fisier output");
	  }
      }
    else if (S_ISDIR(infos.st_mode) == 1) { // daca fisierul este director
	char statistica[300];
	int size = 0;
	size = sprintf(statistica, "Nume director: %s\n\n", entry->d_name);
	
	if (write(fd2, statistica, size) == -1){
	    perror("Eroare scriere fisier output");
	  }
      }
    
    free(path);
  }
  
  close(fd2);
  
  return 0;
}
