#ifndef TEXTEDITENGINE_H
#define TEXTEDITENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_SIZE 1000
#define COM_SIZE 50

typedef struct {
    char text[MAX_SIZE];
    time_t timesec;
    long timemisec;
} Content;

typedef struct stackno {
    Content data;
    struct stackno* next;
} stackno;

typedef struct hisno {
    Content data;
    char action[COM_SIZE];
    struct hisno* next;
} hisno;

typedef struct treeno {
    Content data;
    char ver[COM_SIZE];
    struct treeno* next;
} treeno;

extern stackno *undost, *redost; //exist in file
extern hisno *hishead;
extern treeno *header, *currentdraft;

//core.c
void defaultdraft();
void ui();
void textndraftnow();
void process(char* com);
void cleanup();
void aboutus();

//inoutput.c
void edittext();
void addstack(Content content, stackno** stack);
Content* pop(stackno** stack, stackno** otherStack);
int loadfile(const char* filename);
void savefile(const char* filename);

//manage.c
void addhis(Content content, const char* action);
void newdraft(const char* version);
void finddraftnum(int target);
void renamedraft(const char* newname);
void showdraft();
void showhis();
void clearhis();
int draftnamecheck(const char* name);

#endif // TEXTEDITENGINE_H