#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <pthread.h>


typedef struct{
        char list_of_rules[100][100];  // stores all the Rules
        char list_requests[100][100];
        int count;             //Stores all the requests sent to it
    } Firewall;
    
void list_requests(){
    // command is R
    // write the code to list all the requests
    Firewall* request;
    for(int i = 0; i<request->count;i++){
        printf("%s\n",request->list_requests[i]);
    }      

}

void list_rules(){
    //to list all the rules
    Firewall* request;
    for(int i = 0; i<request->count;i++){
        printf("Rule: %s\n",request->list_of_rules[i]);
    }      
}

void add_rule(const char* rule){
    // command is A <rule>
    // write code to add the rule 
    Firewall* request;
   // int count = request->count;
    if(request->count < 100){
        strcpy(request->list_of_rules[request->count],rule);
        request->count++;
        printf("Rule Added\n");
    }
    
}

void parseIPAddress(const char *IP){
     char* token ; 
     char* temp = strdup(IP);

     token = strtok(temp,".");
     while (token != NULL)
     {
        if(strchr(token,'-')==NULL){
            int num = atoi(token);
            if(num<0||num>255){
                printf("Invalid Rule\n");
                break;
            }
        }
        else{
           
           char* range_temp = strdup(token);
           char* first_part = strtok(range_temp,"-");
           char* second_part = strtok(NULL,"-");

           int first_num = atoi(first_part);
           int second_num = atoi(second_part);
           
           if( first_num<second_num){
               if(first_num>=0 && first_num <= 255 && second_num<=255){
                printf("Rule Valid\n");
               }
               else{
                printf("Invalid Rule\n");
               }
           }
           else{
            printf("Invalid Rule\n");
           }
           
           free(range_temp);
        }
        token = strtok(NULL, ".");
     }
     free(temp);    
}

void parsePort(char* port){
   
   if(strchr(port,'-')!=NULL){
      
      char* range_temp = strdup(port);
      char* first_part = strtok(range_temp,"-");
      char* second_part = strtok(NULL,"-");

      int first_num = atoi(first_part);
      int second_num = atoi(second_part);

      if(first_num<second_num){
        if(first_num>=0 && first_num<= 65535 && second_num <=65535){
            printf("Rule Valid\n");
        }
        else{
            printf("Invalid Rule\n");
        }
      }
      else{
        printf("Invalid Rule");
      }

     free(range_temp);
   }
   else{
    int num = atoi(port);
    if(num>=0 && num<=65535){
        printf("Rule Valid\n");
    }
    else{
        printf("Invalid Rule\n");
     }
   }

}

void check_rule(char rule[]){
    // command is C <IPAddress> <Ports>
    // validate whether the rule is valid or not and add it if needed
    // call the parseIp address and the parse port methods to verify the rules 
     char* token ;
     int loop_count = 1;

     token = strtok(rule," ");
     while(token!=NULL){
        if(strcmp(token,"C")==0){
            loop_count++;
        }
        else if (loop_count==2)
        {
            parseIPAddress(token);
            loop_count++;
        }
        else if (loop_count == 3)
        {
            parsePort(token);
            loop_count++;
        }
        
        token = strtok(NULL," ");
     }

}

void delete_rule(char rule[]){
    // command is D <rule>
    // deletes a valid rule from the Server   
    Firewall* request;
    int is_found = -1;

    //find the index of the string to be deleted
    for (int i = 0; i<request->count;i++){
        if(strcmp(request->list_of_rules,rule)==0){
            is_found=i;
            break;
        }
    }

    if (is_found != -1){
        for( int i =0; i< request->count -1;i++){
            strcpy(request->list_of_rules[i],request->list_of_rules[i+1]);
        }
        request->count--;
        printf("Rule deleted");
    }
    else{
        printf("Rule not found");
    }


}

void runserver(){
 printf("Server has started\n");   
 char line[256];
 fgets(line,256,stdin);
 switch (line[0])
        {
        case 'A':
            add_rule(line);
            break;
        
        case 'D':
            delete_rule(line);
            break;
        
        case 'L':
            list_rules();
            break;
        
        case 'R':
            list_requests();
            break;
        
        case 'C':
            check_rule(line);
            break;
        
        default:
            printf("Illegal request");
            break;
        }


}

int main (int argc, char ** argv) {
    /* to be written */
    runserver();
    printf ("Server to be written\n");
    return 0;
    
}
 