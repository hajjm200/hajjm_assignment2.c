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
void showHighestRatedPerYear(struct movie* list);
void showMoviesByLanguage(struct movie* list, const char* language);
void freeMovieList(struct movie* list);

// Function to display the menu
void printMenu() {
    printf("\n1. Show movies released in the specified year\n");
    printf("2. Show highest rated movie for each year\n");
    printf("3. Show the title and year of release of all movies in a specific language\n");
    printf("4. Exit from the program\n");
}

// Function to get user menu choice
int getMenuChoice() {
    int choice;
    printf("\nEnter a choice from 1 to 4: ");
    scanf("%d", &choice);
    return choice;
}

// Function to create a new movie struct from a line of CSV
struct movie* createMovie(char* line) {
    struct movie* m = malloc(sizeof(struct movie));
    if (!m) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    m->next = NULL;

    // --- Title ---
    char* token = strtok(line, ",");
    if (!token) {
        fprintf(stderr, "Missing title.\n");
        free(m);
        return NULL;
    }
    m->title = malloc(strlen(token) + 1);
    strcpy(m->title, token);

    // --- Year ---
    token = strtok(NULL, ",");
    if (!token) {
        fprintf(stderr, "Missing year.\n");
        free(m->title);
        free(m);
        return NULL;
    }
    m->year = atoi(token);

    // --- Languages ---
    token = strtok(NULL, ",");
    if (!token) {
        fprintf(stderr, "Missing languages.\n");
        free(m->title);
        free(m);
        return NULL;
    }

    char langBuf[100];
    strncpy(langBuf, token, sizeof(langBuf) - 1);
    langBuf[sizeof(langBuf) - 1] = '\0';

    m->languageCount = 0;
    char* lang = strtok(langBuf + 1, ";]");
    while (lang && m->languageCount < MAX_LANGUAGES) {
        strncpy(m->languages[m->languageCount], lang, MAX_LANGUAGE_LENGTH - 1);
        m->languages[m->languageCount][MAX_LANGUAGE_LENGTH - 1] = '\0';
        m->languageCount++;
        lang = strtok(NULL, ";]");
    }

    // --- Rating ---
    token = strtok(NULL, ",\r\n");
    if (!token) {
        fprintf(stderr, "Missing rating.\n");
        free(m->title);
        free(m);
        return NULL;
    }
    m->rating = strtof(token, NULL);

    return m;
}

// Function to read the CSV and create a linked list
struct movie* loadMovies(const char* filename, int* movieCount) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        exit(1);
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    struct movie* head = NULL;
    struct movie* tail = NULL;

    // Skip header
    getline(&line, &len, file);

    *movieCount = 0;

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
    }

    free(line);
    fclose(file);
    return head;
}

// Show movies released in a specific year
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
        printf("No data about movies released in the year %d.\n", year);
    }
}

// Show highest rated movie per year
void showHighestRatedPerYear(struct movie* list) {
    float ratings[10000] = {0.0};
    char* titles[10000] = {NULL};

    while (list) {
        int y = list->year;
        if (!titles[y] || list->rating > ratings[y]) {
            ratings[y] = list->rating;
            titles[y] = list->title;
        }
        list = list->next;
    }

    for (int i = 0; i < 10000; i++) {
        if (titles[i]) {
            printf("%d %.1f %s\n", i, ratings[i], titles[i]);
        }
    }
}

// Show movies in a specific language
void showMoviesByLanguage(struct movie* list, const char* language) {
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
        printf("No movies released in %s.\n", language);
    }
}

// Free all allocated memory
void freeMovieList(struct movie* list) {
    while (list) {
        struct movie* temp = list;
        list = list->next;
        free(temp->title);
        free(temp);
    }
}

// Main
int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <csv file>\n", argv[0]);
        return 1;
    }

    int movieCount = 0;
    struct movie* movieList = loadMovies(argv[1], &movieCount);
    printf("Processed file %s and parsed data for %d movies\n", argv[1], movieCount);

    int choice;
    do {
        printMenu();
        choice = getMenuChoice();
        if (choice == 1) {
            int year;
            printf("Enter the year for which you want to see movies: ");
            scanf("%d", &year);
            showMoviesByYear(movieList, year);
        } else if (choice == 2) {
            showHighestRatedPerYear(movieList);
        } else if (choice == 3) {
            char lang[21];
            printf("Enter the language for which you want to see movies: ");
            scanf("%s", lang);
            showMoviesByLanguage(movieList, lang);
        } else if (choice != 4) {
            printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);

    freeMovieList(movieList);
    return 0;
}
