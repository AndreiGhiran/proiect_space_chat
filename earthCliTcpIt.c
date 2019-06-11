/* cliTCPIt.c - Exemplu de client TCP
   Trimite un nume la server; primeste de la server "Hello nume".
         
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

/* portul de conectare la server*/
#define PORT 2024

/* codul de eroare returnat de anumite apeluri */
extern int errno;


void sig_wait(int sig)
{
  wait(1);
}


void send_msg(int sd)
{

  char msg[900];
 /* citirea mesajului */
  bzero (msg, 900);
  printf ("Introduceti mesajele catre Mars: \n");
  fflush (stdout);
  read (0, msg, 900);
  while(strcmp(msg,"quit\n"))
 {
  
  /* trimiterea mesajului la server */
  if (write (sd, msg, 900) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }
  bzero(msg,900);
  read (0, msg, 900);
 }
 if (write (sd, msg, 900) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }
}

void rec_msg(int sd)
{
    char msg[900];
    bzero(msg,900);
   while(read(sd,msg,900) > 0)
    {
     if(strcmp(msg,"/quit\n")==0)
      break;
     printf("%s",msg);
     bzero(msg,900);
    }

}

int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  char msg[900];		// mesajul trimis

  /* exista toate argumentele in linia de comanda? */
  if (argc != 2)
    {
      printf ("Sintaxa: %s <adresa_server> \n", argv[0]);
      return -1;
    }
    if( signal (SIGCHLD, sig_wait)==SIG_ERR )
     {
      perror ("signal()");
      return 1;
     }

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (PORT);
  
  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }
    int pid=fork();
  
if(pid<0)
    {
    perror ("[client]Eroare la fork(). \n");
    return errno;
    }

   if(pid==0)
  {
   //receptionam mesaje de la server
   rec_msg(sd);
   exit(1);
  }
   //trimitem mesaje la server
   send_msg(sd);
   
  /* inchidem conexiunea, am terminat */
  close (sd);
  return 0;
}
