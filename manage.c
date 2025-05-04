// Functions for history, drafts and version management

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>

// Include our header after standard headers
#include "texteditengine.h"

// Add something to history
void addhis(Content content, const char* action) {
    hisno* newno = malloc(sizeof(hisno));
    if (!newno) {
        return;
    }

    strcpy(newno->data.text, content.text);
    struct timeval times;
    gettimeofday(&times, NULL);
    newno->data.timesec = times.tv_sec;
    newno->data.timemisec = times.tv_usec;
    strcpy(newno->action, action);
    newno->next = hishead;
    hishead = newno;
}

// Make a new draft
void newdraft(const char* version) {
    treeno* newdraftno = malloc(sizeof(treeno));
    if (!newdraftno) {
        return;
    }

    strcpy(newdraftno->data.text, "");
    struct timeval times;
    gettimeofday(&times, NULL);
    newdraftno->data.timesec = times.tv_sec;
    newdraftno->data.timemisec = times.tv_usec;
    strcpy(newdraftno->ver, version);

    newdraftno->next = header;
    header = newdraftno;
    currentdraft = newdraftno;
    addhis(currentdraft->data, "Create Draft");
}

// Search for the draft using number
void finddraftnum(int target) {
    treeno* current = header;
    int count = 1;

    while (current && count < target) {
        current = current->next;
        count++;
    }

    if (current && count == target) {
        currentdraft = current;
    }
}

// Check for draft name
int draftnamecheck(const char* name) {
    treeno* current = header;
    while (current) {
        if (strcmp(current->ver, name) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// Show all drafts and thier options
void showdraft() {
    treeno* current = header;
    int count = 1;

    printf("-----------------------------------------------------------------------------------------------\n");
    printf("| NUM |      DRAFT NAME      |      TIMESTAMP     |           TEXT PREVIEW                    |\n");
    printf("-----------------------------------------------------------------------------------------------\n");

    while (current) {
        char timestr[25];
        struct tm* timeinfo = localtime(&current->data.timesec);
        strftime(timestr, 25, "%Y-%m-%d %H:%M:%S", timeinfo);

        // Create contebt preview
        char preview[31] = "(empty)";
        if (strlen(current->data.text) > 0) {
            strncpy(preview, current->data.text, 30);
            preview[30] = '\0';

            //Replace new lines with spaces
            for (int j = 0; preview[j]; j++) {
                if (preview[j] == '\n') {
                    preview[j] = ' ';
                }
            }

            if (strlen(current->data.text) > 30) {
                strcat(preview, "...");
            }
        }

        // Create draft name with (current) to show the current draft
        char draftname[30] = "";
        strcpy(draftname, current->ver);
        if (current == currentdraft) {
            strcat(draftname, " (current)");
        }

        printf("| %-3d | %-20s | %-18s | %-40s |\n", count, draftname, timestr, preview);
        printf("-----------------------------------------------------------------------------------------------\n");

        current = current->next;
        count++;
    }

    // Draft count
    int draftcount = 0;
    treeno* countcheck = header;
    while (countcheck) {
        draftcount++;
        countcheck = countcheck->next;
    }

    // Show choices with input validination
    char choice;
    int validChoice = 0;

    while (!validChoice) {
        // Only show switch option if there's more than one draft
        if (draftcount > 1) {
            printf("\nOptions: (n)ew draft, (s)witch draft, (r)ename draft, (0) Return to menu: ");
        }

        else {
            printf("\nOptions: (n)ew draft, (r)ename draft, (0) Return to menu: ");
        }

        scanf(" %c", &choice);
        while (getchar() != '\n'); // Clear buffer

        if (choice == 'n' || choice == 'N' || choice == 'r' || choice == 'R' || choice == '0' ||
            ((choice == 's' || choice == 'S') && draftcount > 1)) {
            validChoice = 1;
        }

        else if ((choice == 's' || choice == 'S') && draftcount <= 1) {
            printf("\n--------------------------------------------------\n");
            printf("⚠️ There are no other drafts to switch to.\n");
            printf("--------------------------------------------------\n");
            printf("Please create a new draft first.\n");
            validChoice = 0;
        }

        else {
            printf("\n--------------------------------------------------\n");
            printf("⚠️ Invalid option. Please try again.\n");
            printf("--------------------------------------------------\n");
        }
    }

    if (choice == 'n' || choice == 'N') {
        // Create a new draft
        printf("Enter name for new draft: ");
        char newdraftname[COM_SIZE];
        int nameok = 0;

        while (!nameok) {
            fgets(newdraftname, COM_SIZE, stdin);
            newdraftname[strcspn(newdraftname, "\n")] = 0;

            if (!draftnamecheck(newdraftname)) {
                nameok = 1;
                continue;
            }

            // If the name already exists, give a prompt to try another
            printf("\n--------------------------------------------------\n");
            printf("⚠️ Draft name '%s' already exists. Please choose another name.\n", newdraftname);
            printf("--------------------------------------------------\n");
            printf("Enter name for new draft: ");
        }

        newdraft(newdraftname);
        printf("\n--------------------------------------------------\n");
        printf("✅ Created new draft: %s\n", newdraftname);
        printf("--------------------------------------------------\n");
    }
    else if (choice == 's' || choice == 'S') {
        //Switch draft
        //Only accesable if there are more than 1 draft
        int draftnum;
        int validInput = 0;

        while (!validInput) {
            printf("Enter draft num to switch to (0 to cancel): ");
            if (scanf("%d", &draftnum) != 1) {
                // Clear input buffer if anything other name a number was given
                while (getchar() != '\n');
                printf("\n--------------------------------------------------\n");
                printf("⚠️ Invalid input. Please enter a number.\n");
                printf("--------------------------------------------------\n");
                continue;
            }
            while (getchar() != '\n'); // Clear remaining buffer

            if (draftnum == 0) {
                printf("\n--------------------------------------------------\n");
                printf("❌ Switch draft cancelled.\n");
                printf("--------------------------------------------------\n");
                return;
            }

            if (draftnum < 0) {
                printf("\n--------------------------------------------------\n");
                printf("⚠️ Invalid draft number. Please enter a positive number.\n");
                printf("--------------------------------------------------\n");
                continue; // Loops back to re-input instead of returning
            }

            if (draftnum > draftcount) {
                printf("\n--------------------------------------------------\n");
                printf("⚠️ Invalid draft number. Maximum draft number is %d.\n", draftcount);
                printf("--------------------------------------------------\n");
                continue; // Loops back to re-input instead of returning
            }

            // If we're here then the input is valid
            validInput = 1;
        }

        finddraftnum(draftnum);
        addhis(currentdraft->data, "Switch Draft");

        printf("\n--------------------------------------------------\n");
        printf("✅ Switched to draft: %s\n", currentdraft->ver);
        printf("--------------------------------------------------\n");
    }
    else if (choice == 'r' || choice == 'R') {
        //Rename draft, you can select which one you want to rename
        int draftnum;
        int validInput = 0;
        treeno* draftToRename = currentdraft; //Default is "currentdraft"

        // Asks which draft to rename
        while (!validInput) {
            printf("Enter draft num to rename (0 to cancel): ");
            if (scanf("%d", &draftnum) != 1) {
                // Clear input buffer if anything other name a number was given
                while (getchar() != '\n');
                printf("\n--------------------------------------------------\n");
                printf("⚠️ Invalid input. Please enter a number.\n");
                printf("--------------------------------------------------\n");
                continue;
            }
            while (getchar() != '\n');

            if (draftnum == 0) {
                printf("\n--------------------------------------------------\n");
                printf("❌ Rename draft cancelled.\n");
                printf("--------------------------------------------------\n");
                return;
            }

            if (draftnum < 0) {
                printf("\n--------------------------------------------------\n");
                printf("⚠️ Invalid draft number. Please enter a positive number.\n");
                printf("--------------------------------------------------\n");
                continue; // Loops back to re-input instead of returning
            }

            if (draftnum > draftcount) {
                printf("\n--------------------------------------------------\n");
                printf("⚠️ Invalid draft number. Maximum draft number is %d.\n", draftcount);
                printf("--------------------------------------------------\n");
                continue; // Loops back to re-input instead of returning
            }

             // If we're here then the input is valid
            validInput = 1;
        }

        // Find the draft that is chosen to be renamed
        treeno* current = header;
        int count = 1;

        while (current && count < draftnum) {
            current = current->next;
            count++;
        }

        if (current && count == draftnum) {
            draftToRename = current;
        }

        printf("Current draft name: %s\n", draftToRename->ver);
        printf("Enter new name for draft: ");
        char newdraftname[COM_SIZE];
        int nameok = 0;

        while (!nameok) {
            fgets(newdraftname, COM_SIZE, stdin);
            newdraftname[strcspn(newdraftname, "\n")] = 0;

            // Skip if the name was the same as the old one
            if (strcmp(draftToRename->ver, newdraftname) == 0) {
                printf("\n--------------------------------------------------\n");
                printf("ℹ️ The new name is the same as the current name.\n");
                printf("--------------------------------------------------\n");
                return;
            }

            if (!draftnamecheck(newdraftname)) {
                nameok = 1;
                continue;
            }

            // If the name already exists, give a prompt to try another
            printf("\n--------------------------------------------------\n");
            printf("⚠️ Draft name '%s' already exists. Please choose another name.\n", newdraftname);
            printf("--------------------------------------------------\n");
            printf("Enter new name for draft: ");
        }

        // Confirm rename
        char confirm;
        int validConfirm = 0;

        while (!validConfirm) {
            printf("Rename draft from '%s' to '%s'? (y/n): ", draftToRename->ver, newdraftname);
            scanf(" %c", &confirm);
            while (getchar() != '\n');

            if (confirm == 'y' || confirm == 'Y' || confirm == 'n' || confirm == 'N') {
                validConfirm = 1;
            }

            else {
                printf("\n--------------------------------------------------\n");
                printf("⚠️ Invalid option. Please enter 'y' or 'n'.\n");
                printf("--------------------------------------------------\n");
            }
        }

        if (confirm != 'y' && confirm != 'Y') {
            printf("\n--------------------------------------------------\n");
            printf("❌ Rename draft cancelled.\n");
            printf("--------------------------------------------------\n");
            return;
        }

        // Save the old name
        char oldname[COM_SIZE];
        strcpy(oldname, draftToRename->ver);

        // Update the name of the select draft with the name chosen
        strcpy(draftToRename->ver, newdraftname);

        // Add to history
        char actiondesc[COM_SIZE];
        sprintf(actiondesc, "Rename Draft (%s to %s)", oldname, newdraftname);
        addhis(draftToRename->data, actiondesc);

        printf("\n--------------------------------------------------\n");
        printf("✅ Renamed draft: '%s' to '%s'\n", oldname, newdraftname);
        printf("--------------------------------------------------\n");
    }
}

// Display history with an option to clear
void showhis() {
    // Count history items
    int count = 0;
    hisno* current = hishead;
    while (current) {
        count++;
        current = current->next;
    }

    if (!count) {
        printf("⚠️ No history found.\n");
        return;
    }

    // Create an array to sort
    hisno** historyar = malloc(count * sizeof(hisno*));
    if (!historyar) {
        return;
    }

    // Fill array
    current = hishead;
    for (int i = 0; i < count; i++) {
        historyar[i] = current;
        current = current->next;
    }

    // Sort all history by timestamps
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (historyar[j]->data.timesec > historyar[j + 1]->data.timesec ||
                (historyar[j]->data.timesec == historyar[j + 1]->data.timesec &&
                 historyar[j]->data.timemisec > historyar[j + 1]->data.timemisec)) {
                hisno* temp = historyar[j];
                historyar[j] = historyar[j + 1];
                historyar[j + 1] = temp;
            }
        }
    }

    // Display histroy
    printf("-----------------------------------------------------------------------------------------------\n");
    printf("|  NUM  |      TIME           |      ACTION        |               Text                       |\n");
    printf("-----------------------------------------------------------------------------------------------\n");

    int maxen = (count > 10) ? 10 : count;
    for (int i = 0; i < maxen; i++) {
        char timestr[25];
        struct tm* timeinfo = localtime(&historyar[i]->data.timesec);
        strftime(timestr, 25, "%Y-%m-%d %H:%M:%S", timeinfo);

        // Create content preview
        char preview[31] = "(empty)";
        if (strlen(historyar[i]->data.text) > 0) {
            strncpy(preview, historyar[i]->data.text, 30);
            preview[30] = '\0';

            // Replace the new lines
            for (int j = 0; preview[j]; j++) {
                if (preview[j] == '\n') {
                    preview[j] = ' ';
                }
            }

            if (strlen(historyar[i]->data.text) > 30) {
                strcat(preview, "...");
            }
        }

        printf("|  %-3d  | %-18s | %-18s | %-40s |\n",
               i + 1, timestr, historyar[i]->action, preview);
        printf("-----------------------------------------------------------------------------------------------\n");
    }

    free(historyar);

    // Show choices with input validination
    char choice;
    int validChoice = 0;

    while (!validChoice) {
        printf("\nOptions: (c)lear history, (0) Return to menu: ");
        scanf(" %c", &choice);
        while (getchar() != '\n');

        if (choice == 'c' || choice == 'C' || choice == '0') {
            validChoice = 1;
        }

        else {
            printf("\n--------------------------------------------------\n");
            printf("⚠️ Invalid option. Please try again.\n");
            printf("--------------------------------------------------\n");
        }
    }

    if (choice == 'c' || choice == 'C') {
        // Valid clear confirmation
        char confirm;
        int validConfirm = 0;

        while (!validConfirm) {
            printf("⚠️ Are you sure you want to clear history and text? This will reset all content and disable undo/redo (y/n): ");
            scanf(" %c", &confirm);
            while (getchar() != '\n'); // Clear buffer

            if (confirm == 'y' || confirm == 'Y' || confirm == 'n' || confirm == 'N') {
                validConfirm = 1;
            }

            else {
                printf("\n--------------------------------------------------\n");
                printf("⚠️ Invalid option. Please enter 'y' or 'n'.\n");
                printf("--------------------------------------------------\n");
            }
        }

        if (confirm == 'y' || confirm == 'Y') {
            clearhis();
            printf("\n--------------------------------------------------\n");
            printf("✅ History and current text has been cleared. Undo/Redo functionality has been reset.\n");
            printf("--------------------------------------------------\n");
        }

        else {
            printf("\n--------------------------------------------------\n");
            printf("❌ Clear operation cancelled.\n");
            printf("--------------------------------------------------\n");
        }
    }
}

// Clear history and text
void clearhis() {
    // Clear history
    while (hishead) {
        hisno* temp = hishead;
        hishead = hishead->next;
        free(temp);
    }

    // Clear stacks
    while (undost) {
        stackno* temp = undost;
        undost = undost->next;
        free(temp);
    }

    while (redost) {
        stackno* temp = redost;
        redost = redost->next;
        free(temp);
    }

    // Clear text
    strcpy(currentdraft->data.text, "");

    // Update the timestamp
    struct timeval times;
    gettimeofday(&times, NULL);
    currentdraft->data.timesec = times.tv_sec;
    currentdraft->data.timemisec = times.tv_usec;

    // Add to history
    addhis(currentdraft->data, "Clear All");
}