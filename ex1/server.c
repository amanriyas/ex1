#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

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

// Function to add a rule to the linked list
void add_rule(const char *rule) {
    RuleNode *new_node = (RuleNode *)malloc(sizeof(RuleNode));
    if (!new_node) {
        printf("Memory allocation failed for rule.\n");
        return;
    }
    strcpy(new_node->rule, rule + 2);  // Skipping the command part
    new_node->next = firewall.rules_head;
    firewall.rules_head = new_node;
    printf("Rule Added\n");
}

// Function to list all rules in the linked list
void list_rules() {
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
}

// Function to delete a specific rule from the linked list
void delete_rule(const char *rule) {
    RuleNode *current = firewall.rules_head;
    RuleNode *previous = NULL;
    
    while (current != NULL && strcmp(current->rule, rule + 2) != 0) {
        previous = current;
        current = current->next;
    }
    
    if (current == NULL) {
        printf("Rule not found.\n");
        return;
    }
    
    if (previous == NULL) { 
        firewall.rules_head = current->next;
    } else {
        previous->next = current->next;
    }
    free(current);
    printf("Rule Deleted\n");
}

// Function to add a request to the linked list
void add_request(const char *request) {
    RequestNode *new_node = (RequestNode *)malloc(sizeof(RequestNode));
    if (!new_node) {
        printf("Memory allocation failed for request.\n");
        return;
    }
    strncpy(new_node->request, request, sizeof(new_node->request)-1); //ensures that the whole string is being copied into the linked list
    new_node->request[sizeof(new_node->request)-1] = '\0'; 

    new_node->next = firewall.requests_head;
    firewall.requests_head = new_node;

}

// Function to list all requests in the linked list
void list_requests() {
    RequestNode *current = firewall.requests_head;
    if (current == NULL) {
        printf("No requests available.\n");
        return;
    }
    while (current) {
        printf("%s\n", current->request);
        current = current->next;
    }
}

// Helper function to parse IP address
bool parseIPAddress(const char *IP) {
    char *token;
    char *temp = strdup(IP);

    token = strtok(temp, ".");
    while (token != NULL) {
        if (strchr(token, '-') == NULL) {
            int num = atoi(token);
            if (num < 0 || num > 255) {
                printf("Invalid Rule\n");
                free(temp);
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
                return false;
            }
            free(range_temp);
        }
        token = strtok(NULL, ".");
    }
    free(temp);
    return true;
}

// Helper function to parse port
bool parsePort(const char *port) {
    if (strchr(port, '-') != NULL) {
        char *range_temp = strdup(port);
        char *first_part = strtok(range_temp, "-");
        char *second_part = strtok(NULL, "-");

        int first_num = atoi(first_part);
        int second_num = atoi(second_part);

        if (first_num < 0 || first_num > second_num || second_num > 65535) {
            printf("Invalid Rule\n");
            free(range_temp);
            return false;
        }
        free(range_temp);
    } else {
        int num = atoi(port);
        if (num < 0 || num > 65535) {
            printf("Invalid Rule\n");
            return false;
        }
    }
    return true;
}

// Function to check and validate a rule
void check_rule(char rule[]) {
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
    } else {
        printf("Connection rejected\n");
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

int main(int argc, char **argv) {
    runserver();
    return 0;
}
