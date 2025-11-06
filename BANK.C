// Reworked console frontend (Green theme) + bug fixes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

#define MAX_LINE 256

// Structure to store user data
struct UserData 
{
    char username[50];
    char password[50];
    char firstName[20];
    char lastName[20];
    char fatherName[20];
    char motherName[20];
    char address[50];
    char accountType[20];
    int dateOfBirth;
    char adharNumber[20];
    char phoneNumber[15];
    long int accountBalance;
    struct UserData* next;
};

// Structure to store loan details (for reading file)
struct loan
{
    char user[50];
    long int loan;
    char permissions[50];
    long int amount;
    struct loan* next;
};

// Global linked lists
struct UserData* userList = NULL;
struct loan* userViewList = NULL;

// ---------- UI Helpers (Green theme) ----------
void setColor(WORD color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void drawHeader(const char* title)
{
    system("cls");
    setColor(10); // light green
    printf("==============================================\n");
    printf("    %s\n", title);
    printf("==============================================\n\n");
    setColor(7); // reset to normal
}

void pauseAndReturn()
{
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
    getchar();
}

// ---------- File / data helpers ----------
int isUsernameExists(const char* username);
void saveUserDataToFile(const struct UserData* user);
void loadUserDataFromFile();
void rewriteAllUsersToFile();

// ---------- Core functions (prototypes) ----------
void createAccount();
void accountCreated();
void login();
void loginsu();
void displayloginboard(const char* username);
void Displayuserdata(const char* username);
void checkBalance(const char* username);
void addMoney(const char* username);
void withdrawMoney(const char* username);
void transferMoney(const char* username);
void TransactionHistory(const char* username, long int amount, const char* transactionType, const char* touser);
void ViewTransaction(const char* username);
void logout();
void printUserDataFromFile(const char* username);

void adminlogin();
void adminloginboard();
void viewallusers();
void applyForLoan(const char* username);
void approveloan();
void addloanMoney(const char* username, long int amt);
void resetLoan(const char* username);

// ---------- Main ----------
int main() 
{
    loadUserDataFromFile();

    while (1)
    {
        drawHeader("WELCOME TO SD Global Banking (Console)");
        printf("1. CREATE A BANK ACCOUNT\n");
        printf("2. ALREADY A USER? SIGN IN\n");
        printf("3. ADMIN LOGIN\n");
        printf("4. EXIT\n\n");
        printf("ENTER YOUR CHOICE: ");
        int choice = 0;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n'); // clear newline

        switch (choice) 
        {
            case 1:
                drawHeader("CREATE ACCOUNT");
                printf("NOTE: USERNAME and PASSWORD - max 50 characters\n\n");
                createAccount();
                break;
            case 2:
                drawHeader("USER LOGIN");
                login();
                break;
            case 3:
                drawHeader("ADMIN LOGIN");
                adminlogin();
                break;
            case 4:
                drawHeader("EXIT");
                printf("Goodbye!\n");
                setColor(7);
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
                Sleep(800);
        }
    }
    return 0;
}

// ---------- Implementation ----------

int isUsernameExists(const char* username) 
{
    struct UserData* temp = userList;
    while (temp) {
        if (strcmp(temp->username, username) == 0)
            return 1;
        temp = temp->next;
    }
    return 0;
}

void saveUserDataToFile(const struct UserData* user) 
{
    FILE* file = fopen("user_data.txt", "a");
    if (!file) {
        printf("Error: Could not open user_data.txt for appending.\n");
        return;
    }

    fprintf(file, "Username: %s\n", user->username);
    fprintf(file, "Password: %s\n", user->password);
    fprintf(file, "First Name: %s\n", user->firstName);
    fprintf(file, "Last Name: %s\n", user->lastName);
    fprintf(file, "Father's Name: %s\n", user->fatherName);
    fprintf(file, "Mother's Name: %s\n", user->motherName);
    fprintf(file, "Address: %s\n", user->address);
    fprintf(file, "Account Type: %s\n", user->accountType);
    fprintf(file, "Date of Birth: %d\n", user->dateOfBirth);
    fprintf(file, "Adhar Number: %s\n", user->adharNumber);
    fprintf(file, "Phone Number: %s\n", user->phoneNumber);
    fprintf(file, "Account Balance: %ld\n", user->accountBalance);
    fprintf(file, "\n");

    fclose(file);
}

// Rewrites all users from linked list to user_data.txt
void rewriteAllUsersToFile()
{
    FILE* file = fopen("user_data.txt", "w");
    if (!file) {
        printf("Error: Could not open user_data.txt for writing.\n");
        return;
    }
    struct UserData* temp = userList;
    // write in reverse order to keep creation order roughly similar
    // First count nodes
    int n = 0;
    for (temp = userList; temp; temp = temp->next) n++;
    // store pointers
    struct UserData** arr = (struct UserData**)malloc(sizeof(struct UserData*) * n);
    temp = userList;
    for (int i = 0; i < n; ++i) { arr[i] = temp; temp = temp->next; }
    for (int i = 0; i < n; ++i) {
        struct UserData* u = arr[i];
        fprintf(file, "Username: %s\n", u->username);
        fprintf(file, "Password: %s\n", u->password);
        fprintf(file, "First Name: %s\n", u->firstName);
        fprintf(file, "Last Name: %s\n", u->lastName);
        fprintf(file, "Father's Name: %s\n", u->fatherName);
        fprintf(file, "Mother's Name: %s\n", u->motherName);
        fprintf(file, "Address: %s\n", u->address);
        fprintf(file, "Account Type: %s\n", u->accountType);
        fprintf(file, "Date of Birth: %d\n", u->dateOfBirth);
        fprintf(file, "Adhar Number: %s\n", u->adharNumber);
        fprintf(file, "Phone Number: %s\n", u->phoneNumber);
        fprintf(file, "Account Balance: %ld\n", u->accountBalance);
        fprintf(file, "\n");
    }
    free(arr);
    fclose(file);
}

void loadUserDataFromFile() 
{
    FILE* file = fopen("user_data.txt", "r");
    if (!file) return;

    char line[MAX_LINE];
    struct UserData* curr = NULL;

    while (fgets(line, sizeof(line), file) != NULL) 
    {
        if (strncmp(line, "Username:", 9) == 0) 
        {
            curr = (struct UserData*)malloc(sizeof(struct UserData));
            memset(curr, 0, sizeof(struct UserData));
            sscanf(line, "Username: %s", curr->username);

            if (fgets(line, sizeof(line), file)) sscanf(line, "Password: %s", curr->password);
            if (fgets(line, sizeof(line), file)) sscanf(line, "First Name: %s", curr->firstName);
            if (fgets(line, sizeof(line), file)) sscanf(line, "Last Name: %s", curr->lastName);
            if (fgets(line, sizeof(line), file)) sscanf(line, "Father's Name: %s", curr->fatherName);
            if (fgets(line, sizeof(line), file)) sscanf(line, "Mother's Name: %s", curr->motherName);
            if (fgets(line, sizeof(line), file)) sscanf(line, "Address: %s", curr->address);
            if (fgets(line, sizeof(line), file)) sscanf(line, "Account Type: %s", curr->accountType);
            if (fgets(line, sizeof(line), file)) sscanf(line, "Date of Birth: %d", &curr->dateOfBirth);
            if (fgets(line, sizeof(line), file)) sscanf(line, "Adhar Number: %s", curr->adharNumber);
            if (fgets(line, sizeof(line), file)) sscanf(line, "Phone Number: %s", curr->phoneNumber);
            if (fgets(line, sizeof(line), file)) sscanf(line, "Account Balance: %ld", &curr->accountBalance);

            curr->next = userList;
            userList = curr;
        }
    }
    fclose(file);
}

// ---------- Account creation ----------
void createAccount() 
{
    struct UserData* newUser = (struct UserData*)malloc(sizeof(struct UserData));
    memset(newUser, 0, sizeof(struct UserData));

    printf("ENTER DETAILS TO CREATE YOUR ACCOUNT\n\n");
    printf("FIRST NAME: ");
    scanf("%19s", newUser->firstName);
    printf("LAST NAME: ");
    scanf("%19s", newUser->lastName);
    printf("FATHER'S NAME: ");
    scanf("%19s", newUser->fatherName);
    printf("MOTHER'S NAME: ");
    scanf("%19s", newUser->motherName);
    printf("ADDRESS (single token): ");
    scanf("%49s", newUser->address);
    printf("ACCOUNT TYPE: ");
    scanf("%19s", newUser->accountType);
    printf("DATE OF BIRTH (DDMMYYYY): ");
    scanf("%d", &newUser->dateOfBirth);
    printf("ADHAR NUMBER: ");
    scanf("%19s", newUser->adharNumber);
    printf("PHONE NUMBER: ");
    scanf("%14s", newUser->phoneNumber);

    do {
        printf("USERNAME (50 CHAR MAX): ");
        scanf("%49s", newUser->username);
        if (isUsernameExists(newUser->username)) {
            printf("Username already exists. Please choose a different one.\n");
        } else break;
    } while (1);

    printf("PASSWORD: ");
    // read password with getch masking
    char ch;
    int idx = 0;
    while (1) {
        ch = getch();
        if (ch == 13) break; // enter
        if (idx < 49 && ch >= 32) {
            newUser->password[idx++] = ch;
            putchar('*');
        }
    }
    newUser->password[idx] = '\0';
    printf("\n");

    newUser->accountBalance = 0;
    newUser->next = userList;
    userList = newUser;

    // create initial loan entry if not present
    FILE* loanFile = fopen("loan_data.txt", "a");
    if (loanFile) {
        fprintf(loanFile, "Username: %s\n", newUser->username);
        fprintf(loanFile, "Loan: 0\n");
        fprintf(loanFile, "Permission: -\n");
        fprintf(loanFile, "Amount: 0\n\n");
        fclose(loanFile);
    }

    saveUserDataToFile(newUser);
    accountCreated();
}

void accountCreated() 
{
    drawHeader("ACCOUNT CREATED");
    printf("YOUR ACCOUNT HAS BEEN CREATED SUCCESSFULLY.\n");
    Sleep(800);
    printf("Press Enter to go to login...");
    while (getchar() != '\n');
    getchar();
    login();
}

// ---------- Login ----------
void login() 
{
    char username[50] = {0};
    char password[50] = {0};

    printf("USERNAME: ");
    scanf("%49s", username);
    printf("PASSWORD: ");
    char ch;
    int idx = 0;
    while (1) {
        ch = getch();
        if (ch == 13) break;
        if (idx < 49 && ch >= 32) {
            password[idx++] = ch;
            putchar('*');
        }
    }
    password[idx] = '\0';
    printf("\n");

    struct UserData* current = userList;
    while (current) {
        if (strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0) {
            loginsu();
            displayloginboard(username);
            return;
        }
        current = current->next;
    }
    drawHeader("LOGIN FAILED");
    printf("Invalid username or password.\n");
    Sleep(800);
}

void loginsu() 
{
    drawHeader("LOGIN SUCCESS");
    printf("LOGIN SUCCESSFUL. Redirecting...\n");
    Sleep(800);
}

// ---------- User dashboard ----------
void displayloginboard(const char* username) 
{
    while (1) {
        drawHeader("HOME - USER DASHBOARD");
        printf("Logged in as: %s\n\n", username);
        printf("1. CHECK USER DETAILS\n");
        printf("2. CHECK BALANCE\n");
        printf("3. ADD MONEY\n");
        printf("4. WITHDRAW MONEY\n");
        printf("5. TRANSFER MONEY\n"); 
        printf("6. TRANSACTION DETAILS\n");
        printf("7. APPLY FOR LOAN\n");
        printf("8. LOG OUT\n");
        printf("9. EXIT\n\n");
        printf("ENTER YOUR CHOICE: ");

        int choice = 0;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1: Displayuserdata(username); break;
            case 2: checkBalance(username); break;
            case 3: addMoney(username); break;
            case 4: withdrawMoney(username); break;
            case 5: transferMoney(username); break;
            case 6: ViewTransaction(username); break;
            case 7: applyForLoan(username); break;
            case 8: logout(); return; // back to main
            case 9: drawHeader("EXIT"); printf("Goodbye!\n"); exit(0);
            default: printf("Invalid choice. Try again.\n"); Sleep(700); break;
        }
    }
}

void Displayuserdata(const char* username)
{
    drawHeader("USER DETAILS");
    struct UserData* cur = userList;
    while (cur) {
        if (strcmp(cur->username, username) == 0) {
            printf("WELCOME, %s %s\n\n", cur->firstName, cur->lastName);
            printf("Name: %s %s\n", cur->firstName, cur->lastName);
            printf("Father's Name: %s\n", cur->fatherName);
            printf("Mother's Name: %s\n", cur->motherName);
            printf("Adhar Card Number: %s\n", cur->adharNumber);
            printf("Mobile Number: %s\n", cur->phoneNumber);
            printf("Date of Birth: %d\n", cur->dateOfBirth);
            printf("Address: %s\n", cur->address);
            printf("Account Type: %s\n", cur->accountType);
            printf("Account Balance: %ld\n", cur->accountBalance);
            break;
        }
        cur = cur->next;
    }
    printf("\nPress Enter to return to dashboard...");
    while (getchar() != '\n');
    getchar();
}

// ---------- Balance / money ops ----------
void checkBalance(const char* username) 
{
    drawHeader("BALANCE DASHBOARD");
    struct UserData* cur = userList;
    while (cur) {
        if (strcmp(cur->username, username) == 0) {
            printf("Total Balance: %ld\n", cur->accountBalance);
            break;
        }
        cur = cur->next;
    }
    printf("\nPress Enter to return to dashboard...");
    while (getchar() != '\n');
    getchar();
}

void addMoney(const char* username) 
{
    drawHeader("ADD MONEY");
    struct UserData* cur = userList;
    long int added = 0;
    while (cur) {
        if (strcmp(cur->username, username) == 0) {
            long int amt;
            printf("Enter the amount to add: ");
            if (scanf("%ld", &amt) != 1 || amt <= 0) {
                printf("Invalid amount.\n");
                while (getchar() != '\n');
                Sleep(700);
                return;
            }
            cur->accountBalance += amt;
            added = amt;
            while (getchar() != '\n');
            break;
        }
        cur = cur->next;
    }
    rewriteAllUsersToFile();
    TransactionHistory(username, added, "ADD", username);
    printf("Amount added successfully.\n");
    Sleep(700);
}

void withdrawMoney(const char* username) 
{
    drawHeader("WITHDRAW MONEY");
    struct UserData* cur = userList;
    long int withdrawn = 0;
    while (cur) {
        if (strcmp(cur->username, username) == 0) {
            long int amt;
            printf("Enter the amount to withdraw: ");
            if (scanf("%ld", &amt) != 1 || amt <= 0) {
                printf("Invalid amount.\n");
                while (getchar() != '\n');
                Sleep(700);
                return;
            }
            if (cur->accountBalance < amt) {
                printf("Insufficient balance.\n");
                while (getchar() != '\n');
                Sleep(700);
                return;
            }
            cur->accountBalance -= amt;
            withdrawn = amt;
            while (getchar() != '\n');
            break;
        }
        cur = cur->next;
    }
    rewriteAllUsersToFile();
    TransactionHistory(username, withdrawn, "WITHDRAW", username);
    printf("Amount withdrawn successfully.\n");
    Sleep(700);
}

void transferMoney(const char* username) 
{
    drawHeader("TRANSFER MONEY");
    char toUser[50];
    struct UserData* sender = NULL;
    struct UserData* cur = userList;
    while (cur) {
        if (strcmp(cur->username, username) == 0) { sender = cur; break; }
        cur = cur->next;
    }
    if (!sender) { printf("Sender not found.\n"); Sleep(700); return; }

    printf("Your current balance: %ld\n", sender->accountBalance);
    printf("TO (username of receiver): ");
    scanf("%49s", toUser);
    struct UserData* receiver = userList;
    while (receiver) {
        if (strcmp(receiver->username, toUser) == 0) break;
        receiver = receiver->next;
    }
    if (!receiver) { printf("Receiver username not found.\n"); while (getchar() != '\n'); Sleep(700); return; }

    long int amountToTransfer;
    printf("Enter the amount to transfer: ");
    if (scanf("%ld", &amountToTransfer) != 1 || amountToTransfer <= 0) {
        printf("Invalid amount.\n");
        while (getchar() != '\n');
        Sleep(700);
        return;
    }
    if (sender->accountBalance < amountToTransfer) {
        printf("Insufficient balance for the transfer.\n");
        while (getchar() != '\n');
        Sleep(700);
        return;
    }

    sender->accountBalance -= amountToTransfer;
    receiver->accountBalance += amountToTransfer;
    rewriteAllUsersToFile();
    TransactionHistory(username, amountToTransfer, "TRANSFER_OUT", toUser);
    TransactionHistory(toUser, amountToTransfer, "TRANSFER_IN", username);

    printf("Amount transferred successfully.\n");
    Sleep(700);
}

// ---------- Transaction history (structured) ----------
void TransactionHistory(const char* username, long int amount, const char* transactionType, const char* touser) 
{
    FILE* transactionFile = fopen("transaction.txt", "a");
    if (!transactionFile) {
        printf("Error: Could not open transaction.txt for appending.\n");
        return;
    }

    // Structured format:
    // Username: <username>
    // Type: <ADD/WITHDRAW/TRANSFER_OUT/TRANSFER_IN>
    // Amount: <amount>
    // Other: <details>
    // (blank line)
    fprintf(transactionFile, "Username: %s\n", username);
    fprintf(transactionFile, "Type: %s\n", transactionType);
    fprintf(transactionFile, "Amount: %ld\n", amount);
    if (strcmp(transactionType, "TRANSFER_OUT") == 0) {
        fprintf(transactionFile, "Other: To %s\n", touser);
    } else if (strcmp(transactionType, "TRANSFER_IN") == 0) {
        fprintf(transactionFile, "Other: From %s\n", touser);
    } else {
        fprintf(transactionFile, "Other: -\n");
    }
    fprintf(transactionFile, "\n");
    fclose(transactionFile);
}

void ViewTransaction(const char* username) 
{
    drawHeader("TRANSACTION HISTORY");
    FILE* transactionFile = fopen("transaction.txt", "r");
    if (!transactionFile) {
        printf("No transactions found.\n");
        printf("\nPress Enter to return to dashboard...");
        while (getchar() != '\n');
        getchar();
        return;
    }

    char line[MAX_LINE];
    int found = 0;
    while (fgets(line, sizeof(line), transactionFile) != NULL) {
        if (strncmp(line, "Username:", 9) == 0) {
            char entryUser[60] = {0};
            sscanf(line, "Username: %59[^\n]", entryUser);
            if (strcmp(entryUser, username) == 0) {
                found = 1;
                // print the next 3 lines (Type, Amount, Other)
                printf("%s", line);
                if (fgets(line, sizeof(line), transactionFile)) printf("%s", line);
                if (fgets(line, sizeof(line), transactionFile)) printf("%s", line);
                if (fgets(line, sizeof(line), transactionFile)) printf("%s", line);
                printf("\n");
            } else {
                // skip next 3 lines to move to next block
                fgets(line, sizeof(line), transactionFile);
                fgets(line, sizeof(line), transactionFile);
                fgets(line, sizeof(line), transactionFile);
            }
        }
    }
    fclose(transactionFile);
    if (!found) printf("No transactions for user %s\n", username);
    printf("\nPress Enter to return to dashboard...");
    while (getchar() != '\n');
    getchar();
}

// ---------- Logout ----------
void logout() 
{
    drawHeader("LOGOUT");
    printf("Signing out...\n");
    Sleep(700);
    printf("Signed out successfully.\n");
    Sleep(600);
}

// ---------- Admin ----------
void adminlogin() 
{
    char adminUsername[50];
    char adminPassword[50];

    printf("USERNAME: ");
    scanf("%49s", adminUsername);
    printf("PASSWORD: ");
    char ch;
    int idx = 0;
    while (1) {
        ch = getch();
        if (ch == 13) break;
        if (idx < 49 && ch >= 32) {
            adminPassword[idx++] = ch;
            putchar('*');
        }
    }
    adminPassword[idx] = '\0';
    printf("\n");

    if (strcmp(adminUsername, "admin") == 0 && strcmp(adminPassword, "admin123") == 0) {
        adminloginboard();
    } else {
        drawHeader("ADMIN LOGIN FAILED");
        printf("Invalid admin username or password.\n");
        Sleep(700);
    }
}

void adminloginboard() 
{
    int adminChoice;
    do 
    {
        drawHeader("ADMIN DASHBOARD");
        printf("1. LOGOUT\n");
        printf("2. VIEW ALL USERS\n"); 
        printf("3. APPROVE LOANS\n"); 
        printf("4. EXIT\n");
        printf("ENTER YOUR CHOICE: ");
        if (scanf("%d", &adminChoice) != 1) { while (getchar() != '\n'); continue; }
        while (getchar() != '\n');

        switch (adminChoice) 
        {
            case 1:
                printf("Logging out...\n");
                Sleep(700);
                break;
            case 2:
                viewallusers();
                break;
            case 3:
                approveloan();
                break;
            case 4:
                exit(0);
            default:
                printf("Invalid choice.\n");
                Sleep(600);
                break;
        }
    } while (adminChoice != 1);
}

void viewallusers() 
{
    drawHeader("ALL USERS");
    struct UserData* temp = userList;
    if (!temp) {
        printf("No users available.\n");
    }
    while (temp) {
        printf("Username: %s\n", temp->username);
        printf("Name: %s %s\n", temp->firstName, temp->lastName);
        printf("Phone: %s\n", temp->phoneNumber);
        printf("Balance: %ld\n", temp->accountBalance);
        printf("-------------------------\n");
        temp = temp->next;
    }
    printf("\nPress Enter to return to admin panel...");
    while (getchar() != '\n');
    getchar();
}

void applyForLoan(const char* username) 
{
    drawHeader("APPLY FOR LOAN");
    // We'll append the requested loan amount into loan_data.txt (simple)
    FILE* loanFile = fopen("loan_data.txt", "r+");
    if (!loanFile) {
        // create file and write entry
        loanFile = fopen("loan_data.txt", "a");
        if (!loanFile) { printf("Could not open loan_data.txt\n"); return; }
        fprintf(loanFile, "Username: %s\nLoan: 0\nPermission: -\nAmount: 0\n\n", username);
        fclose(loanFile);
    } else {
        // check if there's an entry with loan > 0
        char line[MAX_LINE];
        int found = 0;
        long existingLoan = 0;
        long pos_save = ftell(loanFile);
        rewind(loanFile);
        while (fgets(line, sizeof(line), loanFile)) {
            if (strncmp(line, "Username:", 9) == 0) {
                char uname[60];
                sscanf(line, "Username: %59s", uname);
                if (strcmp(uname, username) == 0) {
                    // read next "Loan:" line
                    if (fgets(line, sizeof(line), loanFile)) {
                        sscanf(line, "Loan: %ld", &existingLoan);
                        found = 1;
                        break;
                    }
                }
            }
        }
        fclose(loanFile);
        if (found && existingLoan > 0) {
            printf("You already have a pending loan request.\n");
            Sleep(700);
            return;
        }
    }

    long loanAmount;
    printf("Enter loan amount: ");
    if (scanf("%ld", &loanAmount) != 1 || loanAmount <= 0) {
        printf("Invalid loan amount.\n");
        while (getchar() != '\n');
        Sleep(700);
        return;
    }
    while (getchar() != '\n');

    // We'll rewrite the loan_data.txt: replace user's loan value
    FILE* in = fopen("loan_data.txt", "r");
    FILE* out = fopen("temp_loan_data.txt", "w");
    int wrote = 0;
    if (!in) {
        // create new entry
        out = fopen("temp_loan_data.txt", "w");
        fprintf(out, "Username: %s\nLoan: %ld\nPermission: Pending\nAmount: 0\n\n", username, loanAmount);
        wrote = 1;
    } else {
        char line[MAX_LINE];
        while (fgets(line, sizeof(line), in)) {
            if (strncmp(line, "Username:", 9) == 0) {
                char uname[60];
                sscanf(line, "Username: %59s", uname);
                if (strcmp(uname, username) == 0) {
                    // write updated block for this user
                    fprintf(out, "Username: %s\n", username);
                    fprintf(out, "Loan: %ld\n", loanAmount);
                    fprintf(out, "Permission: Pending\n");
                    fprintf(out, "Amount: 0\n\n");
                    // skip next 3 lines in input
                    fgets(line, sizeof(line), in);
                    fgets(line, sizeof(line), in);
                    fgets(line, sizeof(line), in);
                    wrote = 1;
                    continue;
                } else {
                    // write username line and continue copying next 3 lines
                    fprintf(out, "%s", line);
                    if (fgets(line, sizeof(line), in)) { fprintf(out, "%s", line); }
                    if (fgets(line, sizeof(line), in)) { fprintf(out, "%s", line); }
                    if (fgets(line, sizeof(line), in)) { fprintf(out, "%s", line); }
                    continue;
                }
            } else {
                fprintf(out, "%s", line);
            }
        }
        fclose(in);
    }

    if (!wrote) {
        // append new entry
        fprintf(out, "Username: %s\nLoan: %ld\nPermission: Pending\nAmount: 0\n\n", username, loanAmount);
    }
    fclose(out);
    remove("loan_data.txt");
    rename("temp_loan_data.txt", "loan_data.txt");

    printf("Loan applied successfully. Waiting for admin approval.\n");
    Sleep(700);
}

void approveloan()
{
    drawHeader("APPROVE LOANS");
    FILE* in = fopen("loan_data.txt", "r");
    if (!in) {
        printf("No loan requests found.\n");
        Sleep(700);
        return;
    }

    // We'll read all entries, present pending ones to admin, write updated file.
    FILE* out = fopen("temp_loan_data.txt", "w");
    char line[MAX_LINE];
    int anyPending = 0;

    while (fgets(line, sizeof(line), in)) {
        if (strncmp(line, "Username:", 9) == 0) {
            char uname[60];
            long loanAmt = 0;
            char permission[60] = "-";
            long amountField = 0;

            sscanf(line, "Username: %59s", uname);
            // Read the following 3 lines
            fgets(line, sizeof(line), in); sscanf(line, "Loan: %ld", &loanAmt);
            fgets(line, sizeof(line), in); sscanf(line, "Permission: %59s", permission);
            fgets(line, sizeof(line), in); sscanf(line, "Amount: %ld", &amountField);

            if (loanAmt > 0 && (strcasecmp(permission, "Pending") == 0 || strcmp(permission, "-") == 0)) {
                anyPending = 1;
                printf("Username %s applied for Loan: %ld\n", uname, loanAmt);
                printf("Approve? (yes/no): ");
                char ans[10];
                scanf("%9s", ans);
                while (getchar() != '\n');

                if (strcasecmp(ans, "yes") == 0) {
                    // add money to user
                    addloanMoney(uname, loanAmt);
                    // write reset (approved -> Permission: Approved)
                    fprintf(out, "Username: %s\n", uname);
                    fprintf(out, "Loan: 0\n");
                    fprintf(out, "Permission: Approved\n");
                    fprintf(out, "Amount: %ld\n\n", loanAmt);
                    printf("Loan approved for %s and amount added to their account.\n\n", uname);
                } else {
                    // declined: reset to zero/no permission
                    fprintf(out, "Username: %s\n", uname);
                    fprintf(out, "Loan: 0\n");
                    fprintf(out, "Permission: Declined\n");
                    fprintf(out, "Amount: 0\n\n");
                    printf("Loan declined for %s.\n\n", uname);
                }
            } else {
                // write as-is (no action)
                fprintf(out, "Username: %s\n", uname);
                fprintf(out, "Loan: %ld\n", loanAmt);
                fprintf(out, "Permission: %s\n", permission);
                fprintf(out, "Amount: %ld\n\n", amountField);
            }
        }
    }

    fclose(in);
    fclose(out);

    remove("loan_data.txt");
    rename("temp_loan_data.txt", "loan_data.txt");

    if (!anyPending) {
        printf("No pending loan requests found.\n");
        Sleep(800);
    } else {
        printf("Finished processing loan requests.\n");
        Sleep(800);
    }
}

void addloanMoney(const char* username, long int amt) 
{
    struct UserData* cur = userList;
    while (cur) {
        if (strcmp(cur->username, username) == 0) {
            cur->accountBalance += amt;
            break;
        }
        cur = cur->next;
    }
    rewriteAllUsersToFile();
}

// For completeness (not used externally)
void resetLoan(const char* username) 
{
    FILE* loanFile = fopen("temp_loan_data.txt", "a+");
    if (!loanFile) return;
    fprintf(loanFile, "Username: %s\n", username);
    fprintf(loanFile, "Loan: 0\n");
    fprintf(loanFile, "Permission: -\n");
    fprintf(loanFile, "Amount: 0\n\n");
    fclose(loanFile);
}

