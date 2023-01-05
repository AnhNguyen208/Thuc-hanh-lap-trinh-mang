#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct Account {
    char username[20];
    char password[20];
    int status;
} Account;

typedef struct node
{
    Account infor;
    int SignIn;
    struct node* next;
} node;

struct node* head = NULL;
struct node* current = NULL;

void printList() {
    struct node* ptr = head;
    printf("\n[ ");

    while (ptr != NULL) {
        printf("(%s,%s,%d) \n", ptr->infor.username, ptr->infor.password, ptr->infor.status);
        ptr = ptr->next;
    }

    printf(" ]");
}

void insertFirst(Account account) {
    struct node* link = (struct node*)malloc(sizeof(struct node));

    strcpy(link->infor.username, account.username);
    strcpy(link->infor.password, account.password);
    link->infor.status = account.status;
    link->SignIn = 0;

    link->next = head;
    head = link;
}

struct node* deleteFirst() {
    struct node* tempLink = head;
    head = head->next;
    return tempLink;
}

bool isEmpty() {
    return head == NULL;
}

void wrileFile()
{
    FILE* outfile;
    outfile = fopen("account.txt", "w");
    struct node* temp = head;
    while (temp != NULL)
    {
        fprintf(outfile, "%s %s %d\n", temp->infor.username, temp->infor.password, temp->infor.status);
        temp = temp->next;
    }
    fclose(outfile);
}

void readFile()
{
    FILE* file;
    file = fopen("account.txt", "r");
    Account temp;
    if (file == NULL)
    {
        fprintf(stderr, "\nCouldn't Open File'\n");
        exit(1);
    }
    while (fscanf(file, "%s %s %d", temp.username, temp.password, &temp.status) != EOF)
    {
        insertFirst(temp);
    }
    fclose(file);
}

node* checkUsername(char* username)
{
    node* temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->infor.username, username) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

int main()
{
    readFile();
    int choice, num = 0;
    Account account;
    while (1)
    {
        printf("--------USER MANAGEMENT PROGRAM--------\n");
        printf("---------------------------------------\n");
        printf("1. Register\n");
        printf("2. Sign in\n");
        printf("3. Search\n");
        printf("4. Sign out\n");
        printf("Your choice (1-4, other to quit):\n");
        scanf("%d", &choice);
        fflush(stdin);
        if (choice == 1) {
            printf("Input Username: ");
            scanf("%s", account.username);
            fflush(stdin);
            if (checkUsername(account.username) != NULL) {
                printf("Account existed\n\n");
            }
            else {
                printf("Input Password: ");
                scanf("%s", account.password);
                fflush(stdin);
                account.status = 1;
                insertFirst(account);
            }
        }
        else if (choice == 2) {
            printf("Input Username: ");
            scanf("%s", account.username);
            fflush(stdin);
            if (checkUsername(account.username) == NULL) {
                printf("Cannot find account\n\n");
            }
            else {
                while (1) {
                    printf("Input Password: ");
                    scanf("%s", account.password);
                    fflush(stdin);
                    if (strcmp(checkUsername(account.username)->infor.password, account.password) != 0)
                    {
                        num++;
                        if (num < 3)
                            printf("Password is incorrect\n");
                        else
                        {
                            printf("Password is incorrect. Account is blocked\n\n");
                            checkUsername(account.username)->infor.status = 0;
                            num = 0;
                            break;
                        }
                    }
                    else
                    {
                        printf("Hello %s\n\n", checkUsername(account.username)->infor.username);
                        checkUsername(account.username)->SignIn = 1;
                        num = 0;
                        break;
                    }
                }
            }
        }
        else if (choice == 3) {
            printf("Input Username: ");
            scanf("%s", account.username);
            fflush(stdin);
            if (checkUsername(account.username) == NULL) {
                printf("Cannot find account\n\n");
            }
            else {
                if (checkUsername(account.username)->infor.status == 0) {
                    printf("Account is blocked\n\n");
                }
                else {
                    printf("Account is active\n\n");
                }
            }
        }
        else if (choice == 4) {
            printf("Input Username: ");
            scanf("%s", account.username);
            fflush(stdin);
            if (checkUsername(account.username) == NULL) {
                printf("Cannot find account\n\n");
            }
            else {
                if (checkUsername(account.username)->SignIn == 1) {
                    printf("Goodbye %s\n\n", account.username);
                }
                else {
                    printf("Account is not sign in\n\n");
                }
            }
        }
        else {
            wrileFile();
            node* temp = deleteFirst();
            free(temp);
            exit(0);
        }
    }
}