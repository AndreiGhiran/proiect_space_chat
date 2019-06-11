/* servTCPIt.c - Exemplu de server TCP iterativ
   Asteapta un nume de la clienti; intoarce clientului sirul
   "Hello nume".
   
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

/* definire FIFO */
#define FromMars "FromMars.txt"
#define ToMars "ToMars.txt"

/* portul folosit */
#define PORT 2026

/* portul folosit */
#define PORT_SAT 2027

/* codul de eroare returnat de anumite apeluri */
extern int errno;

void cl_msg_rec(int client)
{
      char msg[900];		//mesajul primit de la client 
      char msgrasp[900];  
      FILE * fd;
      while(1)
 {
      fd=fopen(FromMars,"a");
      /* s-a realizat conexiunea, se astepta mesajul */
      bzero (msg, 900);
      bzero(msgrasp, 900);
      printf ("[server-client]Asteptam mesajul...\n");
      fflush (stdout);
      
      /* citirea mesajului */
      if (read (client, msg, 900) <= 0)
	{
	  perror ("[server-client]Eroare la read() de la client.\n");
	  close (client);	/* inchidem conexiunea cu clientul */
      exit(1);
	  continue;		/* continuam sa ascultam */

	}
	 if(strcmp(msg,"quit\n")==0)
      {
      FILE * fde;
      fde=fopen(ToMars,"a");
      fputs("/quit\n",fde);
      fclose(fde);
      printf ("[server-client]Mesajul a fost receptionat...%s\n", msg);
      strcat(msgrasp,"Mars s-a deconectat!\n");
      fputs(msgrasp,fd);
      fclose(fd);
      bzero(msg,900);
      break;
      }
      printf ("[server-client]Mesajul a fost receptionat...%s\n", msg);
      strcat(msgrasp,"Mars: ");
      strcat(msgrasp,msg);
      fputs(msgrasp,fd);
      fclose(fd);
      bzero(msg,900);
   } 

}

void cl_msg_send(int client)
{

        FILE * fd;
        char msg[900];
        bzero(msg,900);
        fd=fopen(ToMars,"r");
        while(fgets(msg, 900, fd))
            {   
                
                printf("[server-client] %s\n",msg);
                if (write (client, msg, 900) <= 0)
	                {
	                    perror ("[server-client]Eroare la write() catre client.\n");
	                    continue;		/* continuam sa ascultam */
	                }
                    else
	                printf ("[server-client]Mesajul a fost trasmis cu succes.\n");
                    bzero(msg,900);

            }
        fclose(fd);
        fd=fopen(ToMars,"w");
        fclose(fd); 
}

void sat_msg_rec(int satelit)
{
      char msg[900];		//mesajul primit de la client 
      char msgrasp[900];  
      FILE * fd;
      while(1)
 {
      fd=fopen(ToMars,"a");
      /* s-a realizat conexiunea, se astepta mesajul */
      bzero (msg, 900);
      printf ("[server-satelit]Asteptam mesajul...\n");
      fflush (stdout);
      
      /* citirea mesajului */
      if (read (satelit, msg, 900) <= 0)
	{
	 // perror ("[server-satelit]Eroare la read() de la satelit.\n");
      //fputs("[EROARE]:Satelitul este in afara zonei de acoperire.\n",fd);
	  close (satelit);	/* inchidem conexiunea cu satelitul */
      exit(1);
	  continue;		/* continuam sa ascultam */

	}
	  
      printf ("[server-satelit]Mesajul a fost receptionat...%s\n", msg);    
      fputs(msg,fd);
      fclose(fd);
      bzero(msg,900);
   } 

}

void sat_msg_send(int satelit)
{

        FILE * fd;
        char msg[900];
        bzero(msg,900);
        fd=fopen(FromMars,"r");
          while( fgets(msg, 900, fd))
            {
                printf("[server-satelit] %s\n",msg);
                if (write (satelit, msg, 900) <= 0)
	                {
	                    perror ("[server-satelit]Eroare la write() catre satelit.\n");
	                    continue;		/* continuam sa ascultam */
	                }
                    else
	                printf ("[server-satelit]Mesajul a fost trasmis cu succes.\n");
                   bzero(msg,900);
            }
        fclose(fd);
        fd=fopen(FromMars,"w");
        fclose(fd);
        write(satelit,"/EOF",900);        
}

void sig_wait(int sig)
{
  wait(1);
}

int main ()
{
  struct sockaddr_in server;
  struct sockaddr_in server2;	// structura folosita de server
  struct sockaddr_in from;
  struct sockaddr_in sat;
  	
  char msg[900];		//mesajul primit de la client 
  char msgrasp[900];        //mesaj de raspuns pentru client
  int sd;
  int sds;			//descriptorul de socket 
  int optval=1; 

  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la client socket().\n");
      return errno;
    }

  if ((sds = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la satelit socket().\n");
      return errno;
    }

  /*setam pentru socket optiunea SO_REUSEADDR */ 
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));
  setsockopt(sds, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));

  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&server2, sizeof (server2));
  bzero (&from, sizeof (from));
  bzero (&sat, sizeof (sat));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server2.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server2.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server2.sin_port = htons (PORT_SAT);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  if (bind (sds, (struct sockaddr *) &server2, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 5) == -1)
    {
      perror ("[server]Eroare la listen() la client.\n");
      return errno;
    }
  if (listen (sds, 5) == -1)
    {
      perror ("[server]Eroare la listen() la satelit.\n");
      return errno;
    }
  //Signal
        if( signal (SIGCHLD, sig_wait)==SIG_ERR )
     {
      perror ("signal()");
      return 1;
     }
   int pid_s=fork();
   if(pid_s<0)
    {
    perror ("[server]Eroare la pid_s=fork(). \n");
    return errno;
    }

   if(pid_s==0)
    {
     while(1)
     {
      int satelit;
      int length = sizeof (from);

      printf ("[server]Asteptam satelitul la portul %d...\n", PORT_SAT);
      satelit = accept (sds, (struct sockaddr *) &from, &length);

      if (satelit < 0)
	{
	  perror ("[server]Eroare la accept() la satelit.\n");
	  continue;
	}

       int pid1=fork();

      if(pid1<0)
    {
    perror ("[server]Eroare la pid1=fork(). \n");
    return errno;
    }

      if(pid1==0)
     {

      sat_msg_rec(satelit);
      close(satelit);
      exit(1);
     }
      int pid2=fork();
     
      if(pid2<0)
    {
    perror ("[server]Eroare la pid2=fork(). \n");
    return errno;
    }

      if(pid2==0)
      {
        sat_msg_send(satelit);
        close(satelit);
        exit(1);
       }

     }
     exit(1);
    }
  /* servim in mod iterativ clientii... */
  while (1)
    {
      int client;
      int length = sizeof (from);

      printf ("[server]Asteptam client la portul %d...\n",PORT);
      fflush (stdout);

      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      client = accept (sd, (struct sockaddr *) &from, &length);
 
      /* eroare la acceptarea conexiunii de la un client */
      if (client < 0)
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}
       FILE * fd;
       fd=fopen(FromMars,"a");
       fputs("Mars s-a conectat!\n",fd);
       fclose(fd);
       int pid=fork();
         
      if(pid<0)
    {
    perror ("[server]Eroare la pid=fork(). \n");
    return errno;
    }

      if(pid==0)
     {

      cl_msg_rec(client);
      close(client);
      exit(1);
     }
      int pid2=fork();
       
      if(pid2<0)
    {
    perror ("[server]Eroare la pid2=fork(). \n");
    return errno;
    }

      if(pid2==0)
      {
       FILE * fd;
       
       while(1)
        {
         fd=fopen(ToMars,"r");
         if(fgets(msg,900,fd))
            {           
            fclose(fd);
            cl_msg_send(client);
            }
            else
            fclose(fd);
            bzero(msg,900);  
        }
       close(client);
        exit(1);
       }
    }				/* while */
}				/* main */
