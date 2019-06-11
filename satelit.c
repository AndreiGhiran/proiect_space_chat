/* servTCPIt.c - Exemplu de server TCP iterativ
   Asteapta un nume de la clienti; intoarce clientului sirul
   "Hello nume".
   
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

/* porturile folosite */
#define PORT_E 2025
#define PORT_M 2027

/*IP-ul folosit*/
#define IP "127.0.0.1"

/* fisierele de stocare locala*/
#define Earth "Earth.txt"
#define Mars "Mars.txt"

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

void rec_msg_e(int sd)
{
    char msg[900];
    bzero(msg,900);
    FILE * fd;
    read(sd,msg,900);
   while( strcmp(msg,"/EOF") )
    {
      
     printf ("[satelit-Earth]Am receptionat cu succes de la Earth mesajul: %s\n",msg);
     fd=fopen(Mars,"a");
     fputs(msg,fd);
     fclose(fd);
     bzero(msg,900);
     read(sd,msg,900);

    }
    close(sd);

}

void send_msg_e(int sd)
{
        FILE * fd;
        char msg[900];
        bzero(msg,900);
        fd=fopen(Earth,"r");
          while( fgets(msg, 900, fd))
            {
                
                if (write (sd, msg, 900) <= 0)
	                {
	                    perror ("[satelit-Earth]Eroare la write() catre Earth.\n");
	                    break;		/* continuam sa ascultam */
	                }
                    else
	                printf ("[satelit-Earth]Am trasmis cu succes catre Earth mesajul: %s.\n",msg);
                   bzero(msg,900);
            }
        fclose(fd);
        fd=fopen(Earth,"w");
        fclose(fd); 

}

void rec_msg_m(int sd)
{
    char msg[900];
    bzero(msg,900);
    FILE * fd;
    read(sd,msg,900);
   while( strcmp(msg,"/EOF") )
    {
      
     printf ("[satelit-Mars]Am receptionat cu succes de la Mars mesajul: %s\n",msg);
     fd=fopen(Earth,"a");
     fputs(msg,fd);
     fclose(fd);
     bzero(msg,900);
     read(sd,msg,900);

    }
    close(sd);

}

void send_msg_m(int sd)
{
        FILE * fd;
        char msg[900];
        bzero(msg,900);
        fd=fopen(Mars,"r");
          while( fgets(msg, 900, fd) )
            {
                if (write (sd, msg, 900) <= 0)
	                {
	                    perror ("[satelit]Eroare la write() catre Mars.\n");
	                    break;		
	                }
                    else
	                printf ("[satelit-Mars]Am trasmis cu succes catre Mars mesajul: %s\n",msg);
                   bzero(msg,900);
            }
        
        fclose(fd);
        fd=fopen(Mars,"w");
        fclose(fd);
 
}

void sig_wait(int sig)
{
  wait(1);
}

int main (int argc, char *argv[])
{
  int sd_e,sd_m;			// descriptorul de socket
  struct sockaddr_in server_e;
  struct sockaddr_in server_m;	// structura folosita pentru conectare 

  /* exista toate argumentele in linia de comanda? */
  if (argc != 2)
    {
     printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

   if( signal (SIGCHLD, sig_wait)==SIG_ERR )
     {
      perror ("signal()");
      return 1;
     }
 
/* umplem structura folosita pentru realizarea conexiunii cu serverul */ 
  /* familia socket-ului */
  server_e.sin_family = AF_INET;
  /* adresa IP a serverului */
  server_e.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server_e.sin_port = htons (PORT_E);

/* umplem structura folosita pentru realizarea conexiunii cu serverul */ 
  /* familia socket-ului */
  server_m.sin_family = AF_INET;
  /* adresa IP a serverului */
  server_m.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server_m.sin_port = htons (PORT_M);
 while(1)
{ 
  
    /* cream socketul */
  if ((sd_e = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }
  if ((sd_m = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

 printf("[satelit]In tranzit spre Earth\n");
 sleep(5);
 printf("[satelit]Am ajuns al Earth\n");
 
  /* ne conectam la server Earth */
  if (connect (sd_e, (struct sockaddr *) &server_e,sizeof (struct sockaddr)) != -1)
    {

    int pid_e=fork();
  
      if(pid_e<0)
    {
    perror ("[server]Eroare la pid_e=fork(). \n");
    return errno;
    }

   if(pid_e==0)
  {
   //receptionam mesaje de la server
   rec_msg_e(sd_e);
  /* inchidem conexiunea, am terminat */
   close(sd_e);
   exit(1);
  }
   //seleep(1) asigura afisarea in ordine coreacta a istoricului satelitului
   sleep(1);
   //trimitem mesaje la server
   send_msg_e(sd_e);
  /* inchidem conexiunea, am terminat */
  close (sd_e);
    }
    else
      {
	   //trimite-m mesaj de eroare
       FILE * fd;
       fd=fopen(Mars,"a");
       fputs("Serverul de pe Pamant nu este disponibil momentan\n",fd);
       fclose(fd);
       perror ("[satelit]Eroare la connect() cu Earth.\n");
       }

 printf("[satelit]In tranzit spre Mars\n");
 sleep(5);
 printf("[satelit]Aa ajuns la Mars\n");

  /* ne conectam la server Mars */
   if (connect (sd_m, (struct sockaddr *) &server_m,sizeof (struct sockaddr)) != -1)
    {
    int pid_m=fork();
     
      if(pid_m<0)
    {
    perror ("[server]Eroare la pid_m=fork(). \n");
    return errno;
    }

   if(pid_m==0)
  {
    rec_msg_m(sd_m);
  /* inchidem conexiunea, am terminat */
    close(sd_m);
    exit(1);
  }
   send_msg_m(sd_m);
   sleep(1);
  /* inchidem conexiunea, am terminat */
   close(sd_m);
  }
   else
    {
       FILE * fd;
       fd=fopen(Earth,"a");
       fputs("Serverul de pe Mars nu este disponibil momentan\n",fd);
       fclose(fd);
       perror ("[client]Eroare la connect() cu Marte.\n");
    }
 }              /* while */
}				/* main */
