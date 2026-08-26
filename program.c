#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LENGTH 2048

typedef enum sex
{
    WOMAN,
    MAN
} Sex;

typedef struct person
{
    char *name;
    int index;
    Sex sex;
    int currentCouple;
    int *preferenceByIndex;
    int *indexByPreference;
    int nextCandidate;
} Person;

typedef struct menStack
{
    Person *value;
    struct menStack *previous;
} MenStack;

Person **womenMasterArray;
Person **menMasterArray;
int n;

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

Person *createPerson(char *name, int index, Sex sex)
{
    Person *newPerson = malloc(sizeof(Person));

    newPerson->name = name; // do not free (name) anywhere or this propierty will be lose, it's just a pointer copy;
    newPerson->currentCouple = -1; // null
    newPerson->index = index;
    newPerson->sex = sex;
    newPerson->preferenceByIndex = 0; // null

    return newPerson;
}

Person **peopleArrayCreate(char *personLines, int valuesCount, Sex sex)
{
    Person **peopleArray = malloc(sizeof(Person *) * valuesCount);
    int initialPosition = getValuesPosition(personLines);
    int personIndex = 0;
    char nameBuffer[64];
    int nameBufferIndex = 0;
    char current;

    for (int i = initialPosition + 1;; i++)
    {
        current = personLines[i];

        if (current != ' ' && current != '\n')
            nameBuffer[nameBufferIndex++] = current;
        else
        {
            nameBuffer[nameBufferIndex] = '\0';
            char *nameString = malloc(strlen(nameBuffer) + 1);
            strcpy(nameString, nameBuffer);
            Person *newPerson = createPerson(nameString, personIndex, sex);
            peopleArray[personIndex++] = newPerson;
            nameBufferIndex = 0;
            if (current == '\n')
                break;
        }
    }
    return peopleArray;
}

MenStack *menStackBuild(Person **menArray)
{
    MenStack *stack = 0;
    for (int i = 0; i < n; i++)
    {
        MenStack *newNode = malloc(sizeof(MenStack));
        newNode->value = menArray[i];
        newNode->previous = stack;
        stack = newNode;
    }
    return stack;
}

Person *menStackPop(MenStack **stack)
{
    if (*stack == 0)
        return 0;
    MenStack *linkToRemove = (*stack);
    Person *returnMan = linkToRemove->value;
    *stack = (*stack)->previous;
    free(linkToRemove);
    return returnMan;
}

void menStackPush(MenStack **stack, Person *man)
{
    MenStack *newLink = malloc(sizeof(MenStack));
    newLink->value = man;
    newLink->previous = *(stack);
    *stack = newLink;
}

void personPrint(Person *person)
{
    printf("Name: %s\n", person->name);
    if (person->currentCouple != -1)
        if (person->sex == WOMAN)
            printf("CurrentCouple: %s\n", menMasterArray[person->currentCouple]->name);
        else
            printf("CurrentCouple: %s\n", womenMasterArray[person->currentCouple]->name);
    if (person->index != -1)
        printf("Index: %d\n", person->index);
    if (person->preferenceByIndex != 0)
        for (int i = 0; i < n; i++)
            printf("[%d]: %d\n", i, person->preferenceByIndex[i]);
}

void peopleArrayPrint(Person **peopleArray, int n)
{
    if (peopleArray[0]->sex == WOMAN)
        printf("Women: \n");
    else
        printf("Men: \n");
    for (int i = 0; i < n; i++)
    {
        personPrint(peopleArray[i]);
    }
}

int countIntegerDigits(int number) // ints are 32 bits most of the time, one reserved to indicate the sign, it let us
                                   // 2^31 = 2.147.483.648 posible values;
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

void loadPersonPreferences(Person *ranker, Person **candidatates)
{
}

void peopleArrayLoadPreferences(FILE *src, Person **rankers, Person **candidatates, int n)
{
    char line[MAX_LINE_LENGTH];
    for (int i = 0; i < n; i++)
    {
        fgets(line, MAX_LINE_LENGTH, src);
        loadPersonPreferences(rankers[i], candidatates);
    }
}
// void peopleArrayLoadPreferences(FILE *src, Person **peopleArray) // this method is awful because at first the
// approach
//                                                                  // was different and i don't want to rewrite it.
// {
//     char line[MAX_LINE_LENGTH];
//     for (int i = 0; i < n; i++)
//     {
//         peopleArray[i]->preferenceByIndex = malloc(sizeof(int) * n);
//         peopleArray[i]->indexByPreference = malloc(sizeof(int) * n);
//     }
//     int personIndex = -1;
//     while (fgets(line, MAX_LINE_LENGTH, src) != 0 && (line[0] != '\n' && line[0] != ' ')) // 0 is null
//     {
//         personIndex++;
//         printf("-------------------------------\n");
//         printf("line:|%s", line);
//         char newLine[MAX_LINE_LENGTH];
//         for (int i = 0; i < n; i++)
//         {
//             char *match;
//             int matchIndex;
//             char charI[countIntegerDigits(n)];
//
//             sprintf(charI, "%0*d", countIntegerDigits(n), i);
//             match = strstr(line, peopleArray[i]->name);
//
//             matchIndex = strlen(line) - strlen(match);
//
//             snprintf(newLine, matchIndex + 1, "%s", line); // the +1 is to make room for the EOF character
//             snprintf(newLine + matchIndex, strlen(charI) + 2, "%s_",
//                      charI); // the +2 is to make room for the EOF character and the '_'
//
//             sprintf(newLine + matchIndex + strlen(charI) + 1, "%s",
//                     line + matchIndex + strlen(peopleArray[i]->name) + 1);
//
//             strcpy(line, newLine);
//         }
//         int currentCharIndex = 0;
//         while (newLine[currentCharIndex++] != ' ')
//             ;
//         printf("after value finding:%s", &newLine[currentCharIndex]);
//
//         for (int j = 0; j < n; j++)
//         {
//             peopleArray[personIndex]->preferenceByIndex[atoi(&newLine[currentCharIndex])] = j;
//             peopleArray[personIndex]->indexByPreference[j] = atoi(&newLine[currentCharIndex]);
//             for (int k = 0; k < countIntegerDigits(n); k++)
//                 currentCharIndex++;
//             currentCharIndex++;
//         }
//     }
//     printf("newline:|%s\n", line);
// }

short manProposesToWoman(MenStack **ms, Person *man, Person *woman)
{
    short accepted = 0;
    if (woman->currentCouple < 0 ||
        woman->preferenceByIndex[man->index] <
            woman->preferenceByIndex[woman->currentCouple]) // using short-circuit evaluation here to avoid accesing
                                                            // off memory
    {
        printf("    %s ¡<3! %s\n", man->name, woman->name);
        accepted = 1;
        if (woman->currentCouple >= 0)
        {
            menStackPush(ms, menMasterArray[woman->currentCouple]);
            menMasterArray[woman->currentCouple]->currentCouple = -1;
            printf("        pobre %s, fue abandonado\n", menMasterArray[woman->currentCouple]->name);
        }
        woman->currentCouple = man->index;
        man->currentCouple = woman->index;
    }
    return accepted;
}

void algGaleShapley(MenStack **ms)
{
    Person *suitor;
    while ((suitor = menStackPop(ms)))
    {
        for (int i = suitor->nextCandidate; i < n; i++)
        {
            if (manProposesToWoman(ms, suitor, womenMasterArray[suitor->indexByPreference[i]]))
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
    FILE *fp = fopen("input", "r");
    MenStack *ms;
    char line[MAX_LINE_LENGTH];

    fgets(line, MAX_LINE_LENGTH, fp);
    n = countValues(line);

    menMasterArray = peopleArrayCreate(line, n, MAN);
    peopleArrayPrint(menMasterArray, n);

    ms = menStackBuild(menMasterArray);

    fgets(line, MAX_LINE_LENGTH, fp);
    womenMasterArray = peopleArrayCreate(line, n, WOMAN);
    peopleArrayPrint(womenMasterArray, n);

    fgets(line, MAX_LINE_LENGTH, fp);
    peopleArrayLoadPreferences(fp, menMasterArray);
    peopleArrayPrint(menMasterArray, n);

    // fgets(line, MAX_LINE_LENGTH, fp);
    peopleArrayLoadPreferences(fp, womenMasterArray);
    peopleArrayPrint(womenMasterArray, n);

    algGaleShapley(&ms);

    printMatching();
}

// int main()
// {
//     fp = fopen("input", "r"); char line[MAX_LINE_LENGTH];
//
//     fgets(line, MAX_LINE_LENGTH, fp);
//     n = countValues(line);
//     menMasterArray = menArrayCreate(line, n);
//     menArrayPrint(menMasterArray, n);
//
//     fgets(line, MAX_LINE_LENGTH, fp);
//     womenMasterArray = womenArrayCreate(line, n);
//     womenArrayPrint(womenMasterArray, n);
//
//     fgets(line, MAX_LINE_LENGTH, fp); // eating a line for debuggin porpuses
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
