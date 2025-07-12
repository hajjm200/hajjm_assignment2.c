#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LANGUAGES 5
#define MAX_LANGUAGE_LENGTH 21

// Define the struct for a movie
struct movie {
    char* title;
    int year;
    char languages[MAX_LANGUAGES][MAX_LANGUAGE_LENGTH];
    int languageCount;
    float rating;
    struct movie* next;
};

// Function declarations
void printMenu();
int getMenuChoice();
struct movie* createMovie(char* line);
struct movie* loadMovies(const char* filename, int* movieCount);
void showMoviesByYear(struct movie* list, int year);
void showHighestRatedEachYear(struct movie* list);
void showMoviesByLanguage(struct movie* list, char* language);
void handleUserChoice(int choice, struct movie* list);
void freeMovies(struct movie* list);

// Print the menu options
void printMenu() {
    printf("\n1. Show movies released in the specified year\n");
    printf("2. Show highest rated movie for each year\n");
    printf("3. Show the title and year of release of all movies in a specific language\n");
    printf("4. Exit from the program\n\n");
}

// Ask the user to enter a menu choice
int getMenuChoice() {
    int choice;
    printf("Enter a choice from 1 to 4: ");
    scanf("%d", &choice);
    return choice;
}

// Create a movie struct from a CSV line
struct movie* createMovie(char* line) {
    struct movie* m = malloc(sizeof(struct movie));
    if (!m) {
        fprintf(stderr, "Memory allocation failed for movie struct.\n");
        exit(1);
    }
    m->next = NULL;

    char* token = strtok(line, ",");
    if (!token) {
        fprintf(stderr, "Missing title.\n");
        free(m);
        return NULL;
    }
    m->title = malloc(strlen(token) + 1);
    strcpy(m->title, token);

    token = strtok(NULL, ",");
    if (!token) {
        fprintf(stderr, "Missing year.\n");
        free(m->title);
        free(m);
        return NULL;
    }
    m->year = atoi(token);

    token = strtok(NULL, ",");
    if (!token) {
        fprintf(stderr, "Missing languages.\n");
        free(m->title);
        free(m);
        return NULL;
    }

    m->languageCount = 0;
    char* lang = strtok(token + 1, ";]");
    while (lang && m->languageCount < MAX_LANGUAGES) {
        strncpy(m->languages[m->languageCount], lang, MAX_LANGUAGE_LENGTH - 1);
        m->languages[m->languageCount][MAX_LANGUAGE_LENGTH - 1] = '\0';
        m->languageCount++;
        lang = strtok(NULL, ";]");
    }

    token = strtok(NULL, "\n");
    if (!token) {
        fprintf(stderr, "Missing rating.\n");
        free(m->title);
        free(m);
        return NULL;
    }
    m->rating = strtof(token, NULL);

    return m;
}


// Read and process all movies from a CSV file
struct movie* loadMovies(const char* filename, int* movieCount) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    struct movie* head = NULL;
    struct movie* tail = NULL;

    getline(&line, &len, file); // skip header

    while ((read = getline(&line, &len, file)) != -1) {
 struct movie* m = createMovie(line);
if (m != NULL) {
    (*movieCount)++;
    if (!head) {
        head = m;
        tail = m;
    } else {
        tail->next = m;
        tail = m;
    }
}
        if (!head) {
            head = m;
            tail = m;
        } else {
            tail->next = m;
            tail = m;
        }
    }

    free(line);
    fclose(file);
    return head;
}

// Show all movies released in a given year
void showMoviesByYear(struct movie* list, int year) {
    int found = 0;
    while (list) {
        if (list->year == year) {
            printf("%s\n", list->title);
            found = 1;
        }
        list = list->next;
    }
    if (!found) {
        printf("No data about movies released in the year %d\n", year);
    }
}

// Show the highest rated movie for each year
void showHighestRatedEachYear(struct movie* list) {
    struct movie* top[122] = {NULL}; // for years 1900 to 2021

    while (list) {
        int index = list->year - 1900;
        if (!top[index] || list->rating > top[index]->rating) {
            top[index] = list;
        }
        list = list->next;
    }

    for (int i = 0; i < 122; i++) {
        if (top[i]) {
            printf("%d %.1f %s\n", top[i]->year, top[i]->rating, top[i]->title);
        }
    }
}

// Show all movies released in a specific language
void showMoviesByLanguage(struct movie* list, char* language) {
    int found = 0;
    while (list) {
        for (int i = 0; i < list->languageCount; i++) {
            if (strcmp(list->languages[i], language) == 0) {
                printf("%d %s\n", list->year, list->title);
                found = 1;
                break;
            }
        }
        list = list->next;
    }
    if (!found) {
        printf("No data about movies released in %s\n", language);
    }
}

// Handle the user's menu selection
void handleUserChoice(int choice, struct movie* list) {
    if (choice == 1) {
        int year;
        printf("Enter the year for which you want to see movies: ");
        scanf("%d", &year);
        showMoviesByYear(list, year);
    } else if (choice == 2) {
        showHighestRatedEachYear(list);
    } else if (choice == 3) {
        char language[21];
        printf("Enter the language for which you want to see movies: ");
        scanf("%s", language);
        showMoviesByLanguage(list, language);
    } else if (choice != 4) {
        printf("You entered an incorrect choice. Try again.\n");
    }
}

// Free memory used by the linked list
void freeMovies(struct movie* list) {
    while (list) {
        struct movie* temp = list;
        list = list->next;
        free(temp->title);
        free(temp);
    }
}

// MAIN function: clean and simple
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies.csv\n");
        return 1;
    }

    int movieCount = 0;
    struct movie* movieList = loadMovies(argv[1], &movieCount);
    printf("Processed file %s and parsed data for %d movies\n", argv[1], movieCount);

    int choice;
    do {
        printMenu();
        choice = getMenuChoice();
        handleUserChoice(choice, movieList);
    } while (choice != 4);

    freeMovies(movieList);
    return 0;
}
