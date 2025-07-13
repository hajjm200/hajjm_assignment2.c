#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct movie {
    char* title;
    int year;
    char* languages;
    float rating;
    struct movie* next;
};

/* This function should:
  -  Open the file.
  -  Skip the header line.
  -  For each line, parse title, year, languages, rating.
  -  Create a struct movie* using malloc.
  -  Append to the linkekd list.
  -  Update *count.
*/
struct movie* processFile(char* filename, int* count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    char line[1024];
    fgets(line, sizeof(line), file); // Skip header line

    struct movie* head = NULL;
    struct movie* tail = NULL;
    *count = 0;

    while (fgets(line, sizeof(line), file)) {
        struct movie* newNode = malloc(sizeof(struct movie));

        // Tokenize the line
        char* saveptr;
        char* token = strtok_r(line, ",", &saveptr);
        newNode->title = strdup(token);

        token = strtok_r(NULL, ",", &saveptr);
        newNode->year = atoi(token);

        token = strtok_r(NULL, ",", &saveptr);
        newNode->languages = strdup(token);

        token = strtok_r(NULL, ",", &saveptr);
        newNode->rating = strtof(token, NULL);

        newNode->next = NULL;

        // Append to the linked list
        if (head == NULL) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }

        (*count)++;
    }

    fclose(file);
    return head;
}



/*
  -  Loop until user selects Exit (4).
  -  Prompt for choice (1-4).
  -  Handle invalid input with:
*/
void showMenu(struct movie* head) {
    int choice = 0;

    while (1) {
        printf("\n1. Show movies released in the specified year\n");
        printf("2. Show highest rated movie for each year\n");
        printf("3. Show the title and year of release of all movies in a specific language\n");
        printf("4. Exit from the program\n\n");

        printf("Enter a choice from 1 to 4: ");
        scanf("%d", &choice);

        if (choice == 1) {
            int year;
            printf("Enter the year for which you want to see movies: ");
            scanf("%d", &year);
            showMoviesByYear(head, year);
        } else if (choice == 2) {
            showHighestRatedByYear(head);
        } else if (choice == 3) {
            char language[21];
            printf("Enter the language for which you want to see movies: ");
            scanf("%s", language);
            showMoviesByLanguage(head, language);
        } else if (choice == 4) {
            break;
        } else {
            printf("You entered an incorrect choice. Try again.\n");
        }
    }
}


void showMoviesByYear(struct movie* head, int year) {
    int found = 0;
    struct movie* curr = head;

    while (curr != NULL) {
        if (curr->year == year) {
            printf("%s\n", curr->title);
            found = 1;
        }
        curr = curr->next;
    }

    if (!found) {
        printf("No data about movies released in the year %d\n", year);
    }
}

#include <float.h> // for FLT_MIN

void showHighestRatedByYear(struct movie* head) {
    struct movie* curr = head;

    // Loop through all years from 1900 to 2021
    for (int year = 1900; year <= 2021; year++) {
        struct movie* best = NULL;
        float highestRating = FLT_MIN;

        curr = head;
        while (curr != NULL) {
            if (curr->year == year && curr->rating > highestRating) {
                highestRating = curr->rating;
                best = curr;
            }
            curr = curr->next;
        }

        if (best != NULL) {
            printf("%d %.1f %s\n", best->year, best->rating, best->title);
        }
    }
}

void showMoviesByLanguage(struct movie* head, char* language) {
    struct movie* curr = head;
    int found = 0;

    while (curr != NULL) {
        // The languages are stored in a string like: [English;French]
        // We need to remove the brackets and split by semicolon

        char langCopy[256];
        strncpy(langCopy, curr->languages, sizeof(langCopy));
        langCopy[sizeof(langCopy) - 1] = '\0'; // safety null terminator

        // Remove [ and ] from beginning and end
        if (langCopy[0] == '[') {
            memmove(langCopy, langCopy + 1, strlen(langCopy));
        }
        if (langCopy[strlen(langCopy) - 1] == ']') {
            langCopy[strlen(langCopy) - 1] = '\0';
        }

        // Tokenize the language string
        char* saveptr;
        char* token = strtok_r(langCopy, ";", &saveptr);
        while (token != NULL) {
            if (strcmp(token, language) == 0) {
                printf("%d %s\n", curr->year, curr->title);
                found = 1;
                break; // Stop checking this movie once a match is found
            }
            token = strtok_r(NULL, ";", &saveptr);
        }

        curr = curr->next;
    }

    if (!found) {
        printf("No data about movies released in %s\n", language);
    }
}

void freeMovieList(struct movie* head) {
    struct movie* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->title);
        free(temp->languages);
        free(temp);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    int count = 0;
    struct movie* list = processFile(argv[1], &count);
    printf("Processed file %s and parsed data for %d movies\n", argv[1], count);
    showMenu(list);
    // Free memory here
    freeMovieList(list);
    return 0;
}
