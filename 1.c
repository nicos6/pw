#include <stdio.h>
#include <stdlib.h> //u_int32_t
#include <unistd.h> //close, read, write, fseek
#include <fcntl.h> //oflag
#include <sys/stat.h> //pt stat
#include <time.h> //pentru functia ctime

int main(int argc, char *argv[]){
  if(argc != 2){
    perror("Numar gresit de argumente");
  }
  
  char *file_input = argv[1];
  
  struct stat infos;
  if(stat(file_input, &infos) == -1){
    perror("Eroare functia stat");
  }
  
  if(S_ISREG(infos.st_mode) == 0){
    perror("Tip fisier gresit");
  }
  
  int fd1;
  if( (fd1 = open(file_input, O_RDONLY)) == -1)
    perror("Eroare deschidere fisier");
  
  u_int32_t height=0, width=0;
  lseek(fd1, 18, SEEK_SET); //punem cursorul la informatiile necesare, dupa 18B (la inaltime si latime)
  
  if(read(fd1, &height, 4) == -1){
    perror("Eroare citire inaltime fisier bmp");
  }
 
  if(read(fd1, &width, 4) == -1){
    perror("Eroare citire inaltime fisier bmp");
  }

  int fd2;
  if(creat ("statistica.txt", S_IRUSR | S_IWUSR) == -1){
    perror("Eroare creare fisier statistica");
  }

  if( (fd2 = open("statistica.txt", O_WRONLY | O_TRUNC)) == -1)
    perror("Eroare deschidere fisier statistica");

  mode_t mode = infos.st_mode;
  char statistica[300];
  int size=0;
  size=sprintf(statistica, "Nume fisier: %s\nInaltime: %d\nLatime: %d\nDimensiune: %ld\nIdentificatorul utilizatorului: %d\nTimpul ultimei modificari: %sContorul de legaturi: %ld\nDrepturi de acces user: %c%c%c\nDrepturi de acces grup: %c%c%c\nDrepturi de acces altii: %c%c%c", file_input, height, width, infos.st_size, infos.st_uid, ctime(&infos.st_mtime), infos.st_nlink, ((mode & S_IRUSR) ? 'r' : '-'), ((mode & S_IWUSR) ? 'w' : '-'), ((mode & S_IXUSR) ? 'x' : '-'), ((mode & S_IRGRP) ? 'r' : '-'), ((mode & S_IWGRP) ? 'w' : '-'), ((mode & S_IXGRP) ? 'x' : '-'), ((mode & S_IROTH) ? 'r' : '-'), ((mode & S_IWOTH) ? 'w' : '-'), ((mode & S_IXOTH) ? 'x' : '-'));

  if (write(fd2, statistica, size) == -1) {
        perror("Eroare scriere fisier output");
  }
  
  close(fd1);
  close(fd2);

  return 0;
}

