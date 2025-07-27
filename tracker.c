#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 


typedef struct {
    int day;
    int month;
    int year;
    char category[50];
    double amount;
    char description[100];
} Expense;


void addExpense(Expense **expenses, int *count, int *capacity);
void listExpenses(const Expense *expenses, int count);
void saveExpenses(const Expense *expenses, int count, const char *filename);
void loadExpenses(Expense **expenses, int *count, int *capacity, const char *filename);
double calculateTotal(const Expense *expenses, int count);
void displayMenu();
void clearInputBuffer();

int main() {
    Expense *expenses = NULL; 
    int count = 0;            
    int capacity = 0;         
    const char *filename = "expenses.dat"; 

    
    loadExpenses(&expenses, &count, &capacity, filename);

    int choice;
    do {
        displayMenu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clearInputBuffer();
            choice = -1; 
            continue;
        }
        clearInputBuffer(); 

        switch (choice) {
            case 1:
                addExpense(&expenses, &count, &capacity);
                break;
            case 2:
                listExpenses(expenses, count);
                break;
            case 3:
                printf("Total expenses: %.2f\n", calculateTotal(expenses, count));
                break;
            case 4:
                saveExpenses(expenses, count, filename);
                printf("Expenses saved to %s.\n", filename);
                break;
            case 5:
                printf("Loading expenses...\n");
                loadExpenses(&expenses, &count, &capacity, filename);
                break;
            case 0:
                printf("Exiting Expense Tracker. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
        printf("\n"); 
    } while (choice != 0);

    
    free(expenses);
    expenses = NULL; 

    return 0;
}


void displayMenu() {
    printf("--- Expense Tracker Menu ---\n");
    printf("1. Add Expense\n");
    printf("2. List Expenses\n");
    printf("3. Calculate Total Expenses\n");
    printf("4. Save Expenses\n");
    printf("5. Load Expenses\n");
    printf("0. Exit\n");
    printf("----------------------------\n");
}


void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


void addExpense(Expense **expenses, int *count, int *capacity) {
   
    if (*count >= *capacity) {
        *capacity = (*capacity == 0) ? 5 : (*capacity * 2); 
        Expense *temp = realloc(*expenses, *capacity * sizeof(Expense));
        if (temp == NULL) {
            perror("Failed to reallocate memory for expenses");
            exit(EXIT_FAILURE);
        }
        *expenses = temp;
    }

    Expense newExpense;
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    newExpense.day = tm_info->tm_mday;
    newExpense.month = tm_info->tm_mon + 1; 
    newExpense.year = tm_info->tm_year + 1900;

    printf("\n--- Add New Expense ---\n");
    printf("Date (DD MM YYYY) (Current: %02d %02d %d): ", newExpense.day, newExpense.month, newExpense.year);
    printf("\nEnter Category: ");
    fgets(newExpense.category, sizeof(newExpense.category), stdin);
    newExpense.category[strcspn(newExpense.category, "\n")] = 0; 

    printf("Enter Amount: ");
    while (scanf("%lf", &newExpense.amount) != 1 || newExpense.amount <= 0) {
        printf("Invalid amount. Please enter a positive number: ");
        clearInputBuffer();
    }
    clearInputBuffer(); 

    printf("Enter Description: ");
    fgets(newExpense.description, sizeof(newExpense.description), stdin);
    newExpense.description[strcspn(newExpense.description, "\n")] = 0; 

    (*expenses)[*count] = newExpense; 
    (*count)++;
    printf("Expense added successfully!\n");
}


void listExpenses(const Expense *expenses, int count) {
    if (count == 0) {
        printf("No expenses recorded yet.\n");
        return;
    }

    printf("\n--- All Expenses ---\n");
    printf("%-12s %-15s %-10s %-30s\n", "Date", "Category", "Amount", "Description");
    printf("-------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%02d/%02d/%d   %-15s %-10.2f %-30s\n",
               expenses[i].day, expenses[i].month, expenses[i].year,
               expenses[i].category, expenses[i].amount, expenses[i].description);
    }
    printf("-------------------------------------------------------------------\n");
}


double calculateTotal(const Expense *expenses, int count) {
    double total = 0.0;
    for (int i = 0; i < count; i++) {
        total += expenses[i].amount;
    }
    return total;
}


void saveExpenses(const Expense *expenses, int count, const char *filename) {
    FILE *file = fopen(filename, "wb"); 
    if (file == NULL) {
        perror("Error opening file for saving");
        return;
    }

 
    fwrite(&count, sizeof(int), 1, file);
    
    fwrite(expenses, sizeof(Expense), count, file);

    fclose(file);
}


void loadExpenses(Expense **expenses, int *count, int *capacity, const char *filename) {
    FILE *file = fopen(filename, "rb"); 
    if (file == NULL) {
        perror("No existing expense file found. Starting with an empty tracker.");
        *count = 0; 
        return;
    }

    int loadedCount = 0;
    fread(&loadedCount, sizeof(int), 1, file);

    if (loadedCount > *capacity) {
        *capacity = loadedCount;
        Expense *temp = realloc(*expenses, *capacity * sizeof(Expense));
        if (temp == NULL) {
            perror("Failed to reallocate memory for loading expenses");
            fclose(file);
            exit(EXIT_FAILURE);
        }
        *expenses = temp;
    }

    fread(*expenses, sizeof(Expense), loadedCount, file);
    *count = loadedCount; 

    fclose(file);
    printf("%d expenses loaded successfully from %s.\n", *count, filename);
}