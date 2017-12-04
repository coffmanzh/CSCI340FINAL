/*
**************************************
*
*   Client.c
*
*   CS340 Final Project
*
*       Description: Password storage
*       for multipel users implementing 
*       a MultiTreaded server, Network 
*       layers, and mutex locks
*
*   Team Name: Neam Tame
*   Cass Outlaw
*   Zach Coffman
*   
*
**************************************
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <sqlite3.h> 

char message[100];
 
static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: \n", (const char*)data);
   
   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      strncpy(message, (argv[i] ? argv[i] : "NULL"), sizeof(message)+1);
   }
   //printf("%s", message);

   
   printf("\n");
   return 0;
}

int main(){

  sqlite3_stmt* stmt = NULL;

  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  char *sql;
  const char* data = "Callback function called";

 
  
  char auth_code[100] = "307";
  char str[100];
  int listen_fd, comm_fd;
  //char message[100];

  struct sockaddr_in servaddr;

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  bzero( &servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htons(INADDR_ANY);
  servaddr.sin_port = htons(22000);

  bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

  listen(listen_fd, 10);

  comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
  printf("Client connected on Port: 22000\n");
  while(1){

    bzero( str, 100);


    // reads client input
    read(comm_fd,str,100);

    // will give connection if client has correct code
    //printf("%s\n", str);
    //printf("%s\n", auth_code);
    //printf("%i\n", strcmp(str, auth_code));


    // Correct code
    if (strcmp(str, auth_code) == 10){

      //printf("Access Granted");
      strncpy(message, "Access Granted\n", sizeof(message) +1);
      write(comm_fd, message, strlen(message)+1);
      printf("Correct auth_code provided by client\n");

      //DATABASE
      /* Open database */
      rc = sqlite3_open("database.db", &db);

      if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        //return(0);
      } else {
        fprintf(stdout, "Opened database successfully\n");
      }

      while(1){
        //Database request
        printf("Client database request : ");
        read(comm_fd, str, 100);
        printf("%s", str);
        //printf("%i\n", strcmp(str, "exit"));
        
        

        // Tests inputs from table
        // Get data from table

        if(strcmp(str, "get") == 10){
          printf("Entered in the Get server logic and wants ");
          
          // fetch a row's status
          read(comm_fd, str, 100);
          printf("%s\n", str);
          //rintf("\n%i", strcmp(str, "exit"));
          if(strcmp(str, "User1") == 10){
            sql = "SELECT PASSWORDS.PASS from PASSWORDS where PASSWORDS.USER = 'User1'";
          
          }else if(strcmp(str, "User2") ==10){
            sql = "SELECT PASSWORDS.PASS from PASSWORDS where PASSWORDS.USER = 'User2'";
          
          }else if(strcmp(str, "User3") ==10){
            sql = "SELECT PASSWORDS.PASS from PASSWORDS where PASSWORDS.USER = 'User3'";
          
          }else if(strcmp(str, "User4") ==10){
            sql = "SELECT PASSWORDS.PASS from PASSWORDS where PASSWORDS.USER = 'User4'";
          
          }else{
            printf("User did not enter existing database entry\n");
            strcpy(message, "Invalid Input");
            write(comm_fd, message, strlen(message)+1);
            sql = "Invalid Input";
          }
             
        }else if(strcmp(str, "edit") == 10){
          printf("Entered in the insert server logic and editing\n ");
          read(comm_fd, str, 100);
          sql = str;  
          
        }else if(strcmp(str, "exit") == 10){
          printf("User Terminated session\n");
          break;

        }else if(strcmp(str, "help") == 10){
          read(comm_fd, str,100);
          printf("User is dumb, and needed help\n");
          sql ="No Input";

        }else{

          printf("User is dumb and did not enter valid command \n");
          sql = "invalid Command";

          //break;
        }

        /* Execute SQL statement */
        rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

        if( rc != SQLITE_OK ) {
          fprintf(stderr, "SQL error: %s\n", zErrMsg);
          sqlite3_free(zErrMsg);
          write(comm_fd, zErrMsg, strlen(zErrMsg)+1);
        } else {
          fprintf(stdout, "Operation done successfully\n");
        }

        //strncpy(message, "fetching\n", sizeof(message) +1);
        write(comm_fd, message, strlen(message)+1);
          
        
        //break;

      }

      }
      // incorrect code kicks client off the network
    else{

      printf("Terminating Client socket\n");
      strncpy(message, "Access Denied\n", sizeof(message) +1);
      write(comm_fd, message, strlen(message)+1);
      break;
    }

  }


  sqlite3_close(db);

  return 0;
      
 
}