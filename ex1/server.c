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
     char* copy = strdup(IP);
     char * token = strtok(copy,".");
     while(token !=NULL){
        if(strchr(token,'-')!=NULL){
            char * new_copy  = strdup(token);
            char* new_token= strtok(new_copy,"-");
            int first_num,second_num;
            int count =1;
            while(new_token !=NULL){
                int num = atoi(new_token);
                if(!(num>=0 && num<=255)){
                  printf("Illegal IP address or port specified");
                  break;
                }
                else if(count!=2){
                    first_num=num;
                    count++;
                    new_token = strtok(NULL,"-");
                }
                else if(count ==2){
                    second_num = num;
                }
                
            }
            if (first_num<second_num){
                  printf("Illegal IP address or port specified");
                  break;
                }

        }
        else{
            int num = atoi(token);
            if(!(num >=0 && num<=255)){
                printf("Illegal IP address or port specified");
                break;
            }
        }
        token = strtok(NULL,".");
        
     }
     free(copy);
}



void check_rule(const char rule[]){
    // command is C <IPAddress> <Ports>
    // validate whether the rule is valid or not and add it if needed
    char* copy = strdup(rule);
    char* token = strtok(copy," ");
    int loop_count =1;
    while(token!=NULL){
       if(strcmp(token,'A')==0){
        token = strtok(NULL," ");
        loop_count++;
        continue;
       }
       else if (loop_count == 2)
       {
        parseIPAddress(token);
        token = strtok(NULL," ");
        loop_count++;
       }
       else if(loop_count ==3){
        parsePortNumber(token);
        token = strtok(NULL," ");
        break;
       }
       
       
    }
    
    free(copy);

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
 