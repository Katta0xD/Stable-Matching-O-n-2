#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGHT 2048

typedef struct menStack
{
    struct man *value;
    struct menStack *previous;
} MenStack;

typedef struct woman
{
    char *name;
    int currentCouple;
    int index;
    int *preferenceArray;
} Woman;

typedef struct man
{
    char *name;
    int currentCouple;
    int index;
    int *preferenceArray;
} Man;

FILE *fp;
Woman **womenMasterArray;
Man **menMasterArray;
MenStack *ms;
int n;

Woman *createWoman(char *name, int index)
{
    Woman *newWoman = malloc(sizeof(Woman));

    newWoman->name = name; // do not free (name) anywhere or this propierty will be lose, it's just a pointer copy;
    newWoman->currentCouple = -1; // null
    newWoman->index = index;
    newWoman->preferenceArray = 0; // null

    return newWoman;
}

Man *createMan(char *name, int index)
{
    Man *newMan = malloc(sizeof(Man));

    newMan->name = name;        // do not free (name) anywhere or this propierty will be lose, it's just a pointer copy;
    newMan->currentCouple = -1; // null
    newMan->index = index;
    newMan->preferenceArray = 0; // null

    return newMan;
}

int getValuesPosition(char *Line)
{
    for (int i = 0;; i++)
        if (Line[i] == ' ')
            return i;
}

int countValues(char *line)
{
    char current;
    int n = 0;
    for (int i = 0; (current = line[i]) != '\n'; i++)
        if (current == ' ')
            n++;
    return n;
}

Woman **createWomenArray(char *womenLine, int valuesCount)
{
    Woman **womenArray = malloc(sizeof(Woman *) * valuesCount);
    int initialPosition = getValuesPosition(womenLine);
    int womenIndex = 0;
    char nameBuffer[64];
    int nameBufferIndex = 0;
    char current;

    for (int i = initialPosition + 1;; i++)
    {
        current = womenLine[i];

        if (current != ' ' && current != '\n')
            nameBuffer[nameBufferIndex++] = current;
        else
        {
            nameBuffer[nameBufferIndex] = '\0';
            char *nameString = malloc(strlen(nameBuffer) + 1);
            strcpy(nameString, nameBuffer);
            Woman *newWoman = createWoman(nameString, womenIndex);
            womenArray[womenIndex++] = newWoman;
            nameBufferIndex = 0;
            if (current == '\n')
                break;
        }
    }
    return womenArray;
}

Man **createMenArray(char *menLine, int valuesCount)
{
    Man **menArray = malloc(sizeof(Man *) * valuesCount);
    int initialPosition = getValuesPosition(menLine);
    int manIndex = 0;
    char nameBuffer[64];
    int nameBufferIndex = 0;
    char current;

    for (int i = initialPosition + 1;; i++)
    {
        current = menLine[i];

        if (current != ' ' && current != '\n')
            nameBuffer[nameBufferIndex++] = current;
        else
        {
            nameBuffer[nameBufferIndex] = '\0';
            char *nameString = malloc(strlen(nameBuffer) + 1);
            strcpy(nameString, nameBuffer);
            Man *newMan = createMan(nameString, manIndex);
            menArray[manIndex++] = newMan;
            nameBufferIndex = 0;
            if (current == '\n')
                break;
        }
    }
    return menArray;
}

MenStack *menStackBuild(Man **menArray)
{
    MenStack *stack = malloc(sizeof(MenStack));
    stack->value = menMasterArray[n - 1];
    for (int i = n - 2; i >= 0; i--)
    {
        MenStack *previous = malloc(sizeof(MenStack));
        previous->value = menMasterArray[i];
        stack->previous = previous;

        stack = previous;
    }
    stack->previous = 0;
    return stack;
}

Man *menStackPop(MenStack **stack)
{
    if (*stack == 0)
        return 0;
    MenStack *linkToRemove = (*stack);
    Man *returnMan = linkToRemove->value;
    *stack = (*stack)->previous;
    free(linkToRemove);
    return returnMan;
}

void menStackPush(MenStack **stack, Man *man)
{
    MenStack *newLink = malloc(sizeof(MenStack));
    newLink->value = man;
    newLink->previous = *(stack);
    *stack = newLink;
}

void womanPrint(Woman *woman)
{
    printf("Name: %s\n", woman->name);
    if (woman->currentCouple != -1)
        printf("CurrentCouple: %s\n", menMasterArray[woman->currentCouple]->name);
    if (woman->index != -1)
        printf("Index: %d\n", woman->index);
    if (woman->preferenceArray != 0)
        for (int i = 0; i < n; i++)
            printf("[%d]: %d\n", i, woman->preferenceArray[i]);
}

void womenArrayPrint(Woman **womenArray, int n)
{
    printf("Women: \n");
    for (int i = 0; i < n; i++)
    {
        womanPrint(womenArray[i]);
    }
}

void manPrint(Man *man)
{
    printf("Name: %s\n", man->name);
    if (man->currentCouple != -1)
        printf("CurrentCouple: %s\n", womenMasterArray[man->currentCouple]->name);
    if (man->index != -1)
        printf("Index: %d\n", man->index);
    if (man->preferenceArray != 0)
        for (int i = 0; i < n; i++)
            printf("[%d]: %d\n", i, man->preferenceArray[i]);
}

void menArrayPrint(Man **menArray, int n)
{
    printf("Men: \n");
    for (int i = 0; i < n; i++)
    {
        manPrint(menArray[i]);
    }
}

int countIntegerDigits(int number) // ints are 32 bits most of the time, one reserved to indicate the sign, it let us
                                   // 2^31 = 2 147 483 648 posible values;
{
    int count = 1;
    count += (number >= 10);
    count += (number >= 100);
    count += (number >= 1000);
    count += (number >= 10000);
    count += (number >= 100000);
    count += (number >= 1000000);
    count += (number >= 10000000);
    count += (number >= 100000000);
    count += (number >= 100000000);
    return count;
}

void menArrayLoadPreferences(
    FILE *src) // this method is awful because at first the approach was different and i don't want to rewrite it.
{
    char line[MAX_LINE_LENGHT];
    for (int i = 0; i < n; i++)
        menMasterArray[i]->preferenceArray = malloc(sizeof(int) * n);
    while (fgets(line, MAX_LINE_LENGHT, src) != 0 && (line[0] != '\n' && line[0] != ' ')) // 0 is null
    {
        printf("-------------------------------\n");
        printf("line:|%s", line);
        char newLine[MAX_LINE_LENGHT];
        for (int i = 0; i < n; i++)
        {
            char *match;
            int matchIndex;
            char charI[countIntegerDigits(n)];

            sprintf(charI, "%0*d", countIntegerDigits(n), i);
            match = strstr(line, womenMasterArray[i]->name);

            // printf("Woman name:|%s|", womenMasterArray[i]->name);
            // printf("match:|%s", match);
            // fflush(stdout);

            matchIndex = strlen(line) - strlen(match);

            snprintf(newLine, matchIndex + 1, "%s ", line); // the +1 is to make room for the EOF character
            snprintf(newLine + matchIndex, strlen(charI) + 1, "%s",
                     charI); // the +1 is to make room for the EOF character

            sprintf(newLine + matchIndex + strlen(charI), "%s", line + matchIndex + strlen(womenMasterArray[i]->name));

            strcpy(line, newLine);
        }
        for (int i = 0; i < n; i++)
        {
            int currentCharIndex = 0;
            while (line[currentCharIndex++] != ' ')
                ;
            for (int j = 0; j < n; j++)
            {
                currentCharIndex++;
                menMasterArray[i]->preferenceArray[j] = atoi(&line[currentCharIndex]);

                printf("%s", &line[currentCharIndex]);
                ;

                for (int k = 1; k < countIntegerDigits(n); k++)
                    currentCharIndex++;
            }
        }
        printf("newline:|%s", line);
    }
}

void womenArrayLoadPreferences(
    FILE *src) // this method is awful because at first the approach was different and i don't want to rewrite it.
{
    char line[MAX_LINE_LENGHT];
    for (int i = 0; i < n; i++)
        womenMasterArray[i]->preferenceArray = malloc(sizeof(int) * n);
    while (fgets(line, MAX_LINE_LENGHT, src) && (line[0] != '\n' && line[0] != ' ')) // 0 is null
    {
        printf("-------------------------------\n");
        printf("line:|%s", line);
        char newLine[MAX_LINE_LENGHT];
        for (int i = 0; i < n; i++)
        {
            char *match;
            int matchIndex;
            char charI[countIntegerDigits(n)];

            sprintf(charI, "%0*d", countIntegerDigits(n), i);
            match = strstr(line, menMasterArray[i]->name);

            // printf("Woman name:|%s|", womenMasterArray[i]->name);
            // printf("match:|%s", match);
            // fflush(stdout);

            matchIndex = strlen(line) - strlen(match);

            snprintf(newLine, matchIndex + 1, "%s ", line); // the +1 is to make room for the EOF character
            snprintf(newLine + matchIndex, strlen(charI) + 1, "%s",
                     charI); // the +1 is to make room for the EOF character

            sprintf(newLine + matchIndex + strlen(charI), "%s", line + matchIndex + strlen(menMasterArray[i]->name));

            strcpy(line, newLine);
        }
        for (int i = 0; i < n; i++)
        {
            int currentCharIndex = 0;
            while (line[currentCharIndex++] != ' ')
                ;
            for (int j = 0; j < n; j++)
            {
                currentCharIndex++;
                womenMasterArray[i]->preferenceArray[j] = atoi(&line[currentCharIndex]);
                for (int k = 1; k < countIntegerDigits(n); k++)
                    currentCharIndex++;
            }
        }
        printf("newLine:|%s", line);
    }
}

short manProposesToWoman(MenStack *ms, Man *man, Woman *woman)
{
    short accepted = 0;
    if (woman->currentCouple < 0 ||
        woman->preferenceArray[man->index] <
            woman->preferenceArray[woman->currentCouple]) // using lazy evaluation here to avoid accesing off memory
    {
        accepted = 1;
        if (woman->currentCouple > 0)
        {
            menStackPush(&ms, menMasterArray[woman->currentCouple]);
        }
        woman->currentCouple = man->index;
        man->currentCouple = woman->index;
    }
    return accepted;
}

void algGaleShapley(MenStack *ms)
{
    Man *suitor;
    while ((suitor = menStackPop(&ms)))
    {
        for (int i = 0; i < n; i++)
        {
            if (manProposesToWoman(ms, suitor, womenMasterArray[suitor->preferenceArray[i]]))
                break;
        }
    }
}

void printMatching()
{
    printf("Matching---------------------");
    for (int i = 0; i < n; i++)
    {
        printf("%s <3 %s\n", menMasterArray[i]->name, womenMasterArray[menMasterArray[i]->currentCouple]->name);
    }
}

int main()
{
    fp = fopen("input", "r");
    char line[MAX_LINE_LENGHT];

    fgets(line, MAX_LINE_LENGHT, fp);
    n = countValues(line);
    menMasterArray = createMenArray(line, n);
    menArrayPrint(menMasterArray, n);

    fgets(line, MAX_LINE_LENGHT, fp);
    womenMasterArray = createWomenArray(line, n);
    womenArrayPrint(womenMasterArray, n);

    fgets(line, MAX_LINE_LENGHT, fp);
    menArrayLoadPreferences(fp);
    menArrayPrint(menMasterArray, n);
}

// int main()
// {
//     fp = fopen("input", "r");
//     char line[MAX_LINE_LENGHT];
//
//     fgets(line, MAX_LINE_LENGHT, fp);
//     n = countValues(line);
//     menMasterArray = createMenArray(line, n);
//     menArrayPrint(menMasterArray, n);
//
//     fgets(line, MAX_LINE_LENGHT, fp);
//     womenMasterArray = createWomenArray(line, n);
//     womenArrayPrint(womenMasterArray, n);
//
//     fgets(line, MAX_LINE_LENGHT, fp); // eating a line for debuggin porpuses
//
//     menArrayLoadPreferences(fp);
//     menArrayPrint(menMasterArray, n);
//
//
//     womenArrayLoadPreferences(fp);
//     womenArrayPrint(womenMasterArray, n);
//
//     MenStack *ms = menStackBuild(menMasterArray);
//
//     algGaleShapley(ms);
//
//     printMatching();
//
//     return EXIT_SUCCESS;
// }
