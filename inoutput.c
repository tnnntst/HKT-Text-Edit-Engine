// Input/Output functions
// Manages file loading/saving(.txt), editing, and undo/redo operations.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> //related to time
#include <ctype.h>
#include <sys/time.h>  // Timestamps
#include <unistd.h> // Check paths

// Our header(main structures and functions)
#include "texteditengine.h"

// Save current version for undo/redo
void addstack(Content content, stackno** stack) {
    stackno* newno = malloc(sizeof(stackno)); // Allocate memory
    if (!newno) return;

    // Copy current text into a new node
    strcpy(newno->data.text, content.text);
    newno->data.timesec = content.timesec;
    newno->data.timemisec = content.timemisec;

    // Push new version to top
    newno->next = *stack;
    *stack = newno;
}

// Pop top item, push current draft
Content* pop(stackno** stack, stackno** otherStack) {
    static Content resultst; // Use static to return a pointer

    if (!*stack) return NULL;

    stackno* topno = *stack;
    *stack = topno->next; // Move the stack pointer to the next node

    // Backup current version
    Content currentcontent;
    strcpy(currentcontent.text, currentdraft->data.text);
    currentcontent.timesec = currentdraft->data.timesec;
    currentcontent.timemisec = currentdraft->data.timemisec;

    stackno* newno = malloc(sizeof(stackno));
    if (!newno) {
        topno->next = *stack;
        *stack = topno;
        return NULL;
    }

    // Copy the current content into the new node
    strcpy(newno->data.text, currentcontent.text);
    newno->data.timesec = currentcontent.timesec;
    newno->data.timemisec = currentcontent.timemisec;
    newno->next = *otherStack;
    *otherStack = newno;

    // Prepare the popped content to be returned
    strcpy(resultst.text, topno->data.text);
    resultst.timesec = topno->data.timesec;
    resultst.timemisec = topno->data.timemisec;

    free(topno); // return memory of the popped node
    return &resultst;
}

// Let user edit the current draft
void edittext() {
    printf("\nEnter new text to append (end with a line containing only '.' or '0' to cancel)\n");

    // Save current draft before editing for undo
    Content prevcontent;
    strcpy(prevcontent.text, currentdraft->data.text);
    prevcontent.timesec = currentdraft->data.timesec;
    prevcontent.timemisec = currentdraft->data.timemisec;

    char newin[MAX_SIZE] = "", line[MAX_SIZE];

    // Get lines until '.'
    while (1) {
        if (!fgets(line, MAX_SIZE, stdin)) break;
        line[strcspn(line, "\n")] = 0; // Remove newline character

        if (strcmp(line, ".") == 0) break;       // End input with '.'
        if (strcmp(line, "0") == 0) {            // Cancel with '0'
            printf("Edit cancelled.\n");
            return;
        }

        if (strlen(newin) > 0) strcat(newin, "\n");
        strcat(newin, line);
    }

    if (strlen(newin) == 0) {
        printf("No changes made (empty input).\n");
        return;
    }

    // Combine new input with current text
    char combinetext[MAX_SIZE];
    strcpy(combinetext, currentdraft->data.text);
    if (strlen(combinetext) > 0) strcat(combinetext, "\n");

    if (strlen(combinetext) + strlen(newin) < MAX_SIZE) {
        strcat(combinetext, newin);
    }

    else {
        printf("Warning: Text truncated (exceeds maximum size).\n");
        strncat(combinetext, newin, MAX_SIZE - strlen(combinetext) - 1);
    }

    // Save old draft to undo stack
    addstack(prevcontent, &undost);
    strcpy(currentdraft->data.text, combinetext);

    // Update recent timestamp
    struct timeval times;
    gettimeofday(&times, NULL);
    currentdraft->data.timesec = times.tv_sec;
    currentdraft->data.timemisec = times.tv_usec;

    // Clear redo stack because it has a new change
    while (redost) {
        stackno* temp = redost;
        redost = redost->next;
        free(temp);
    }

    addhis(currentdraft->data, "Edit"); // Add to history
}

// Try to load file (also check outside build folder)
int loadfile(const char* filename) {
    FILE* file = NULL;
    char filepath[1024] = "";

    file = fopen(filename, "r"); // Try current directory

    // If file not found, check if in main floder
    if (!file) {
        char currentworking[1024];
        if (getcwd(currentworking, sizeof(currentworking)) != NULL) {
            char* buildpo = strstr(currentworking, "/cmake-build-debug");
            if (buildpo) {
                *buildpo = '\0';
                sprintf(filepath, "%s/%s", currentworking, filename);
                file = fopen(filepath, "r");
            }
        }
    }

    if (!file) {
        printf("Error opening file! File '%s' not found.\n", filename);
        return 0;
    }

    // Read file content into memory
    char buffer[MAX_SIZE] = "", line[MAX_SIZE];
    while (fgets(line, MAX_SIZE, file)) {
        if (strlen(buffer) + strlen(line) < MAX_SIZE) {
            strcat(buffer, line);
        }

        else {
            printf("Warning: File content truncated (exceeds maximum size).\n");
            break;
        }
    }
    fclose(file);

    // Check if current draft has content
    if (strlen(currentdraft->data.text) > 0) {
        char choice;
        int validChoice = 0;
        while (!validChoice) {
            printf("Current draft already has text. (r)eplace, (a)ppend, or (0) cancel? ");
            scanf(" %c", &choice);
            while (getchar() != '\n');

            if (choice == 'r' || choice == 'R' || choice == 'a' || choice == 'A' || choice == '0') {
                validChoice = 1;
            }

            else {
                printf("\n⚠️ Invalid option. Please enter 'r', 'a', or '0'.\n");
            }
        }

        if (choice == '0') {
            printf("Load operation cancelled.\n");
            return 0;
        }

        if (choice == 'a' || choice == 'A') {
            // Append to current draft
            Content prevcontent;
            strcpy(prevcontent.text, currentdraft->data.text);
            prevcontent.timesec = currentdraft->data.timesec;
            prevcontent.timemisec = currentdraft->data.timemisec;
            addstack(prevcontent, &undost);

            char combinetext[MAX_SIZE];
            strcpy(combinetext, currentdraft->data.text);
            if (strlen(combinetext) > 0) strcat(combinetext, "\n");

            if (strlen(combinetext) + strlen(buffer) < MAX_SIZE) {
                strcat(combinetext, buffer);
            }

            else {
                printf("Warning: Text truncated (exceeds maximum size).\n");
                strncat(combinetext, buffer, MAX_SIZE - strlen(combinetext) - 1);
            }

            strcpy(currentdraft->data.text, combinetext);

            struct timeval times;
            gettimeofday(&times, NULL);
            currentdraft->data.timesec = times.tv_sec;
            currentdraft->data.timemisec = times.tv_usec;

            addhis(currentdraft->data, "Append File");
            return 1;
        }
        // Else, default to replace mode
    }

    // Replace mode
    Content prevcontent;
    strcpy(prevcontent.text, currentdraft->data.text);
    prevcontent.timesec = currentdraft->data.timesec;
    prevcontent.timemisec = currentdraft->data.timemisec;
    addstack(prevcontent, &undost);

    strcpy(currentdraft->data.text, buffer);

    struct timeval times;
    gettimeofday(&times, NULL);
    currentdraft->data.timesec = times.tv_sec;
    currentdraft->data.timemisec = times.tv_usec;

    addhis(currentdraft->data, "Load File");
    return 1;
}

// Save draft (overwrite or append)
void savefile(const char* filename) {
    char filepath[1024] = "";
    FILE* filecheck = NULL;
    int fileexist = 0;
    char savechoice = 'w'; // Start to overwrite

    filecheck = fopen(filename, "r");
    if (filecheck) {
        fileexist = 1;
        strcpy(filepath, filename);
        fclose(filecheck);
    }

    else {
        // The file might be in the main folder, not the build folder.
        char currentworking[1024];
        if (getcwd(currentworking, sizeof(currentworking)) != NULL) {
            char* buildpo = strstr(currentworking, "/cmake-build-debug");
            if (buildpo) {
                *buildpo = '\0';
                sprintf(filepath, "%s/%s", currentworking, filename);
                filecheck = fopen(filepath, "r");
                if (filecheck) {
                    fileexist = 1;
                    fclose(filecheck);
                }

                else {
                    strcpy(filepath, filename);
                }
            }

            else {
                strcpy(filepath, filename);
            }
        }
    }

    // Ask user to overwrite or append
    if (fileexist) {
        char choice;
        int validChoice = 0;
        while (!validChoice) {
            printf("File exists. (o)verwrite, (a)ppend, or (c)ancel? ");
            scanf(" %c", &choice);
            while (getchar() != '\n');

            if (choice == 'o' || choice == 'O' || choice == 'a' || choice == 'A' || choice == 'c' || choice == 'C') {
                validChoice = 1;
            }

            else {
                printf("\n⚠️ Invalid option. Please enter 'o', 'a', or 'c'.\n");
            }
        }

        if (choice == 'a' || choice == 'A') {
            savechoice = 'a';
        }

        else if (choice != 'o' && choice != 'O') {
            printf("Save cancelled.\n");
            return;
        }
    }

    else {
        // File doesn't exist, ask to create new one
        char confirm;
        int validConfirm = 0;
        while (!validConfirm) {
            printf("File doesn't exist. Create new file? (y/n): ");
            scanf(" %c", &confirm);
            while (getchar() != '\n');

            if (confirm == 'y' || confirm == 'Y' || confirm == 'n' || confirm == 'N') {
                validConfirm = 1;
            }

            else {
                printf("\n⚠️ Invalid option. Please enter 'y' or 'n'.\n");
            }
        }

        if (confirm != 'y' && confirm != 'Y') {
            printf("Save cancelled.\n");
            return;
        }
    }

    // Save content to file
    FILE* file = fopen(filepath, savechoice == 'a' ? "a" : "w");
    if (!file) {
        printf("Error opening file for writing!\n");
        return;
    }

    // If appending, last line ends in newline
    if (savechoice == 'a') {
        fseek(file, 0, SEEK_END);
        long filesize = ftell(file);
        if (filesize > 0) {
            fseek(file, -1, SEEK_END);
            char lastchar = fgetc(file);
            if (lastchar != '\n') {
                fprintf(file, "\n");
            }
        }
    }

    fprintf(file, "%s", currentdraft->data.text); // Write draft
    fclose(file);

    // Add action to history
    addhis(currentdraft->data, fileexist ?
        (savechoice == 'a' ? "Append to File" : "Save File") :
        "Create & Save File");
}

