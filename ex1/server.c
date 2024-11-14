#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<signal.h>
#include<ctype.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10
// # define PORT 2200

typedef struct RuleNode {
    char rule[100];            
    struct RuleNode *next;     
} RuleNode;

typedef struct RequestNode {
    char request[100];         
    struct RequestNode *next;  
} RequestNode;

typedef struct {
    RuleNode *rules_head;       
    RequestNode *requests_head;
} Firewall;

Firewall firewall = {.rules_head = NULL, .requests_head = NULL};
pthread_mutex_t firewall_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to add a rule to the linked list
void add_rule(const char *rule) {
    pthread_mutex_lock(&firewall_mutex);
    RuleNode *new_node = (RuleNode *)malloc(sizeof(RuleNode));
    if (!new_node) {
        printf("Memory allocation failed for rule.\n");
        pthread_mutex_unlock(&firewall_mutex);
        return;
    }
    strcpy(new_node->rule, rule + 2);  // Skipping the command part
    new_node->next = firewall.rules_head;
    firewall.rules_head = new_node;
    pthread_mutex_lock(&firewall_mutex);
    printf("Rule Added\n");
}

// Function to list all rules in the linked list
void list_rules() {
    pthread_mutex_lock(&firewall_mutex);
    RuleNode *current = firewall.rules_head;
    if (current == NULL) {
        printf("No rules available.\n");
        
        return;
    }
    int rule_num = 1;
    while (current) {
        printf("Rule %d: %s\n", rule_num++, current->rule);
        current = current->next;
    }
    pthread_mutex_unlock(&firewall_mutex);
}

// Function to delete a specific rule from the linked list
void delete_rule(const char *rule) {
    pthread_mutex_lock(&firewall_mutex);
    RuleNode *current = firewall.rules_head;
    RuleNode *previous = NULL;
    
    while (current != NULL && strcmp(current->rule, rule + 2) != 0) {
        previous = current;
        current = current->next;
    }
    
    if (current == NULL) {
        printf("Rule not found.\n");
        pthread_mutex_unlock(&firewall_mutex);
        return;
    }
    
    if (previous == NULL) { 
        firewall.rules_head = current->next;
    } else {
        previous->next = current->next;
    }
    free(current);
    pthread_mutex_unlock(&firewall_mutex);
    printf("Rule Deleted\n");
}

// Function to add a request to the linked list
void add_request(const char *request) {
    pthread_mutex_lock(&firewall_mutex);
    RequestNode *new_node = (RequestNode *)malloc(sizeof(RequestNode));
    if (!new_node) {
        printf("Memory allocation failed for request.\n");
         pthread_mutex_unlock(&firewall_mutex);
        return;
    }
    strncpy(new_node->request, request, sizeof(new_node->request)-1); //ensures that the whole string is being copied into the linked list
    new_node->request[sizeof(new_node->request)-1] = '\0'; 

    new_node->next = firewall.requests_head;
    firewall.requests_head = new_node;
     pthread_mutex_unlock(&firewall_mutex);

}

// Function to list all requests in the linked list
void list_requests() {
    pthread_mutex_lock(&firewall_mutex);
    RequestNode *current = firewall.requests_head;
    if (current == NULL) {
        printf("No requests available.\n");
        
        return;
    }
    while (current) {
        printf("%s\n", current->request);
        current = current->next;
    }
    pthread_mutex_unlock(&firewall_mutex);
}

// Helper function to parse IP address
bool parseIPAddress(const char *IP) {
    pthread_mutex_lock(&firewall_mutex);
    char *token;
    char *temp = strdup(IP);

    token = strtok(temp, ".");
    while (token != NULL) {
        if (strchr(token, '-') == NULL) {
            int num = atoi(token);
            if (num < 0 || num > 255) {
                printf("Invalid Rule\n");
                free(temp);
                pthread_mutex_unlock(&firewall_mutex);
                return false;
            }
        } else {
            char *range_temp = strdup(token);
            char *first_part = strtok(range_temp, "-");
            char *second_part = strtok(NULL, "-");

            int first_num = atoi(first_part);
            int second_num = atoi(second_part);

            if (first_num < 0 || first_num > second_num || second_num > 255) {
                printf("Invalid Rule\n");
                free(temp);
                free(range_temp);
                 pthread_mutex_unlock(&firewall_mutex);
                return false;
            }
            free(range_temp);
        }
        token = strtok(NULL, ".");
    }
    free(temp);
    pthread_mutex_unlock(&firewall_mutex);
    return true;
}

// Helper function to parse port
bool parsePort(const char *port) {
    pthread_mutex_lock(&firewall_mutex);
    if (strchr(port, '-') != NULL) {
        char *range_temp = strdup(port);
        char *first_part = strtok(range_temp, "-");
        char *second_part = strtok(NULL, "-");

        int first_num = atoi(first_part);
        int second_num = atoi(second_part);

        if (first_num < 0 || first_num > second_num || second_num > 65535) {
            printf("Invalid Rule\n");
            free(range_temp);
            pthread_mutex_unlock(&firewall_mutex);
            return false;
        }
        free(range_temp);
        pthread_mutex_unlock(&firewall_mutex);
    } else {
        int num = atoi(port);
        if (num < 0 || num > 65535) {
            printf("Invalid Rule\n");
            pthread_mutex_unlock(&firewall_mutex);
            return false;
        }
    }
    pthread_mutex_unlock(&firewall_mutex);
    return true;
}

// Function to check and validate a rule
void check_rule(char rule[]) {
    pthread_mutex_lock(&firewall_mutex);
    char *token;
    int loop_count = 1;
    bool valid_ip = false;
    bool valid_port = false;

    token = strtok(rule, " ");
    while (token != NULL) {
        if (loop_count == 2) {
            valid_ip = parseIPAddress(token);
        } else if (loop_count == 3) {
            valid_port = parsePort(token);
        }
        loop_count++;
        token = strtok(NULL, " ");
    }

    if (valid_ip && valid_port) {
        add_rule(rule);
        add_request(rule);
        printf("Connection accepted\n");
        pthread_mutex_unlock(&firewall_mutex);
    } else {
        printf("Connection rejected\n");
        pthread_mutex_unlock(&firewall_mutex);
    }
}

void runserver() {
    printf("Server has started\n");
    while (true) {
        char line[256];
        fgets(line, sizeof(line), stdin);
        switch (line[0]) {
            case 'A':
                add_rule(line);
                add_request(line);
                break;
            case 'D':
                delete_rule(line);
                add_request(line);
                break;
            case 'L':
                list_rules();
                add_request(line);
                break;
            case 'R':
                list_requests();
                add_request(line);
                break;
            case 'C':
                check_rule(line);
                add_request(line);
                break;
            default:
                printf("Illegal request\n");
                break;
        }
    }
}

// fo rhandling client request via network mode
void handle_client_command(int client_sock, const char *command) {
    char response[BUFFER_SIZE] = {0};
    
    switch (command[0]) {
        case 'A':
            add_rule(command);
            add_request(command);
            snprintf(response, sizeof(response), "Rule added\n");
            break;
        case 'D':
            delete_rule(command);
            add_request(command);
            snprintf(response, sizeof(response), "Rule deleted\n");
            break;
        case 'L':
            list_rules();
            add_request(command);
            snprintf(response, sizeof(response), "Rules listed\n");
            break;
        case 'R':
            list_requests();
            add_request(command);
            snprintf(response, sizeof(response), "Requests listed\n");
            break;
        case 'C':
            add_request(command);
            snprintf(response, sizeof(response), "Connection checked\n");
            break;
        default:
            snprintf(response, sizeof(response), "Illegal request\n");
            break;
    }

    send(client_sock, response, strlen(response), 0);
}



void *client_handler(void *client){
    int client_sock = *(int*)client;
    free(client);

    char buffer[BUFFER_SIZE];
    while(1){
        memset(buffer, 0 , BUFFER_SIZE);
        int bytes_recieved = recv(client_sock, buffer, sizeof(buffer),0);
        if (bytes_recieved <= 0) {
            printf("Client disconnected\n");
            close(client_sock);
            break;
        }


        buffer[bytes_recieved] = '\0';
        printf("Received command: %s\n", buffer);
        handle_client_command(client_sock, buffer);
    }

    return NULL;
}


void start_socket_server(int port){
    int server_sock;
    struct sockaddr_in server_addr;

    if ((server_sock = socket(AF_INET, SOCK_STREAM,0))<0){
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", port);


    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int *client_sock = malloc(sizeof(int));

        if ((*client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("Accept failed");
            free(client_sock);
            continue;
        }

        printf("Client connected\n");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, client_handler, client_sock);
        pthread_detach(thread_id);
    
    }

    close(server_sock);
    
}

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "-i") == 0) {
        runserver();
    } else if (argc == 3) {
        int port = atoi(argv[2]);
        printf("Starting server in socket mode on port %d\n", port);
        start_socket_server(port);
    } else {
        fprintf(stderr, "Usage: %s -i | %s <port>\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    pthread_mutex_destroy(&firewall_mutex);
    
    return 0;
}
