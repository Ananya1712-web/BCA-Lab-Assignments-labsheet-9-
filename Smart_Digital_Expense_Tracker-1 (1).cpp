// Lab Sheet 09: Smart Digital Expense Tracker with Budget Prediction
// BCA I Semester (Session 2026-2027)
//
// Beginner-friendly C++ project using structures, arrays, functions,
// loops, strings, searching, sorting and file handling.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

const int MAX_EXPENSES = 1000;

struct Expense
{
    int id;
    string username;
    string date;        // Format: YYYY-MM-DD
    string month;       // Format: YYYY-MM
    string category;
    string description;
    double amount;
};

struct Budget
{
    string username;
    string month;
    double amount;
};

Expense expenses[MAX_EXPENSES];
int expenseCount = 0;
string currentUser;
int nextExpenseId = 1;

// Function declarations
void loadExpenses();
void saveExpenses();
void registerUser();
bool loginUser();
void addExpense();
void viewExpenseHistory();
void categoryWiseExpense();
void monthlyExpenseReport();
void setBudget();
void budgetWarningAndPrediction();
void searchExpense();
void deleteExpense();
int daysInMonth(const string &month);
double getMonthlyTotal(const string &month);
double getMonthlyBudget(const string &month);
void mainMenu();

// Load saved expenses from the file.
void loadExpenses()
{
    expenseCount = 0;
    ifstream file("expenses.txt");
    string line;

    while (getline(file, line) && expenseCount < MAX_EXPENSES)
    {
        stringstream ss(line);
        string idText, amountText;
        Expense e;

        getline(ss, idText, '|');
        getline(ss, e.username, '|');
        getline(ss, e.date, '|');
        getline(ss, e.month, '|');
        getline(ss, e.category, '|');
        getline(ss, e.description, '|');
        getline(ss, amountText, '|');

        if (!idText.empty() && !amountText.empty())
        {
            e.id = stoi(idText);
            e.amount = stod(amountText);
            expenses[expenseCount++] = e;

            if (e.id >= nextExpenseId)
                nextExpenseId = e.id + 1;
        }
    }
    file.close();
}

// Save all expenses to the file.
void saveExpenses()
{
    ofstream file("expenses.txt");
    for (int i = 0; i < expenseCount; i++)
    {
        file << expenses[i].id << '|'
             << expenses[i].username << '|'
             << expenses[i].date << '|'
             << expenses[i].month << '|'
             << expenses[i].category << '|'
             << expenses[i].description << '|'
             << fixed << setprecision(2) << expenses[i].amount << '\n';
    }
    file.close();
}

// Register a new user and save the account in users.txt.
void registerUser()
{
    string username, password, savedUser, savedPassword;
    cout << "\n--- User Registration ---\n";
    cout << "Create username: ";
    cin >> username;
    cout << "Create password: ";
    cin >> password;

    ifstream inputFile("users.txt");
    while (inputFile >> savedUser >> savedPassword)
    {
        if (savedUser == username)
        {
            cout << "This username already exists. Please try another.\n";
            inputFile.close();
            return;
        }
    }
    inputFile.close();

    ofstream outputFile("users.txt", ios::app);
    outputFile << username << " " << password << '\n';
    outputFile.close();

    cout << "Registration successful!\n";
}

// Check username and password.
bool loginUser()
{
    string username, password, savedUser, savedPassword;
    cout << "\n--- Login ---\n";
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    ifstream file("users.txt");
    while (file >> savedUser >> savedPassword)
    {
        if (savedUser == username && savedPassword == password)
        {
            currentUser = username;
            file.close();
            cout << "Login successful. Welcome, " << currentUser << "!\n";
            return true;
        }
    }
    file.close();

    cout << "Invalid username or password.\n";
    return false;
}

// Add a new daily expense.
void addExpense()
{
    if (expenseCount >= MAX_EXPENSES)
    {
        cout << "Expense storage is full.\n";
        return;
    }

    Expense e;
    e.id = nextExpenseId++;
    e.username = currentUser;

    cout << "\n--- Add Daily Expense ---\n";
    cout << "Enter date (YYYY-MM-DD): ";
    cin >> e.date;

    if (e.date.length() != 10 || e.date[4] != '-' || e.date[7] != '-')
    {
        cout << "Invalid date format. Please use YYYY-MM-DD.\n";
        nextExpenseId--;
        return;
    }

    e.month = e.date.substr(0, 7);

    cout << "Enter category (Food/Travel/Books/Shopping/Other): ";
    cin >> e.category;

    cout << "Enter short description (one word, e.g. Lunch): ";
    cin >> e.description;

    cout << "Enter amount: Rs. ";
    cin >> e.amount;

    if (cin.fail() || e.amount <= 0)
    {
        cout << "Please enter a valid positive amount.\n";
        cin.clear();
        cin.ignore(10000, '\n');
        nextExpenseId--;
        return;
    }

    expenses[expenseCount++] = e;
    saveExpenses();
    cout << "Expense added and saved successfully!\n";
}

// Display all expenses of the logged-in user.
void viewExpenseHistory()
{
    cout << "\n--- Expense History (sorted by date) ---\n";
    Expense userExpenses[MAX_EXPENSES];
    int userCount = 0;

    for (int i = 0; i < expenseCount; i++)
    {
        if (expenses[i].username == currentUser)
            userExpenses[userCount++] = expenses[i];
    }

    if (userCount == 0)
    {
        cout << "No expenses found.\n";
        return;
    }

    // Sorting records by date in ascending order.
    sort(userExpenses, userExpenses + userCount,
         [](const Expense &a, const Expense &b)
         {
             return a.date < b.date;
         });

    cout << left << setw(6) << "ID" << setw(14) << "Date"
         << setw(14) << "Category" << setw(18) << "Description"
         << "Amount (Rs.)\n";
    cout << string(68, '-') << '\n';

    for (int i = 0; i < userCount; i++)
    {
        cout << left << setw(6) << userExpenses[i].id
             << setw(14) << userExpenses[i].date
             << setw(14) << userExpenses[i].category
             << setw(18) << userExpenses[i].description
             << fixed << setprecision(2) << userExpenses[i].amount << '\n';
    }
}

// Calculate and display spending in each category for a month.
void categoryWiseExpense()
{
    string month;
    cout << "\nEnter month (YYYY-MM): ";
    cin >> month;

    string categories[5] = {"Food", "Travel", "Books", "Shopping", "Other"};
    double totals[5] = {0, 0, 0, 0, 0};

    for (int i = 0; i < expenseCount; i++)
    {
        if (expenses[i].username == currentUser && expenses[i].month == month)
        {
            for (int j = 0; j < 5; j++)
            {
                if (expenses[i].category == categories[j])
                    totals[j] += expenses[i].amount;
            }
        }
    }

    cout << "\nCategory-wise Expense for " << month << '\n';
    for (int i = 0; i < 5; i++)
        cout << left << setw(12) << categories[i] << " Rs. "
             << fixed << setprecision(2) << totals[i] << '\n';
}

// Return total expenses for a particular month.
double getMonthlyTotal(const string &month)
{
    double total = 0;
    for (int i = 0; i < expenseCount; i++)
    {
        if (expenses[i].username == currentUser && expenses[i].month == month)
            total += expenses[i].amount;
    }
    return total;
}

// Show the total amount spent in a selected month.
void monthlyExpenseReport()
{
    string month;
    cout << "\nEnter month (YYYY-MM): ";
    cin >> month;

    cout << "\n--- Monthly Expense Report ---\n";
    cout << "Month: " << month << '\n';
    cout << "Total expense: Rs. " << fixed << setprecision(2)
         << getMonthlyTotal(month) << '\n';
}

// Save or update a monthly budget.
void setBudget()
{
    string month;
    double amount;
    cout << "\nEnter month (YYYY-MM): ";
    cin >> month;
    cout << "Enter monthly budget (Rs.): ";
    cin >> amount;

    if (cin.fail() || amount <= 0)
    {
        cout << "Please enter a valid positive budget.\n";
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    Budget budgets[500];
    int count = 0;
    bool updated = false;

    ifstream inputFile("budgets.txt");
    while (count < 500 && inputFile >> budgets[count].username
          >> budgets[count].month >> budgets[count].amount)
    {
        if (budgets[count].username == currentUser &&
            budgets[count].month == month)
        {
            budgets[count].amount = amount;
            updated = true;
        }
        count++;
    }
    inputFile.close();

    if (!updated && count < 500)
    {
        budgets[count].username = currentUser;
        budgets[count].month = month;
        budgets[count].amount = amount;
        count++;
    }

    ofstream outputFile("budgets.txt");
    for (int i = 0; i < count; i++)
        outputFile << budgets[i].username << ' '
                   << budgets[i].month << ' '
                   << budgets[i].amount << '\n';
    outputFile.close();

    cout << "Budget saved successfully!\n";
}

// Find the saved budget for the current user and month.
double getMonthlyBudget(const string &month)
{
    ifstream file("budgets.txt");
    Budget b;
    while (file >> b.username >> b.month >> b.amount)
    {
        if (b.username == currentUser && b.month == month)
        {
            file.close();
            return b.amount;
        }
    }
    file.close();
    return 0;
}

// Calculate number of days in a month, including leap-year February.
int daysInMonth(const string &month)
{
    int year = 0, monthNumber = 0;
    if (month.length() != 7)
        return 30;

    year = stoi(month.substr(0, 4));
    monthNumber = stoi(month.substr(5, 2));

    if (monthNumber == 2)
    {
        bool leapYear = (year % 400 == 0) ||
                        (year % 4 == 0 && year % 100 != 0);
        return leapYear ? 29 : 28;
    }

    if (monthNumber == 4 || monthNumber == 6 ||
        monthNumber == 9 || monthNumber == 11)
        return 30;

    return 31;
}

// Compare current spending with budget and estimate full-month spending.
void budgetWarningAndPrediction()
{
    string month;
    int elapsedDays;
    cout << "\nEnter month (YYYY-MM): ";
    cin >> month;
    cout << "Enter day number reached so far (1 to "
         << daysInMonth(month) << "): ";
    cin >> elapsedDays;

    int totalDays = daysInMonth(month);
    if (elapsedDays < 1 || elapsedDays > totalDays)
    {
        cout << "Invalid day number.\n";
        return;
    }

    double spent = getMonthlyTotal(month);
    double budget = getMonthlyBudget(month);

    cout << "\n--- Budget Warning and Prediction ---\n";
    cout << "Total spent: Rs. " << fixed << setprecision(2) << spent << '\n';

    if (budget == 0)
    {
        cout << "No budget set for this month. Please set a budget first.\n";
        return;
    }

    cout << "Monthly budget: Rs. " << budget << '\n';
    if (spent > budget)
        cout << "WARNING: You have exceeded your budget!\n";
    else
        cout << "You are currently within your budget.\n";

    double predicted = (spent / elapsedDays) * totalDays;
    cout << "Predicted month-end expense: Rs. " << predicted << '\n';

    if (predicted > budget)
        cout << "Prediction: You may exceed your monthly budget.\n";
    else
        cout << "Prediction: You are likely to stay within your budget.\n";
}

// Search by expense ID, category, description or date.
void searchExpense()
{
    int choice;
    cout << "\nSearch by:\n1. Expense ID\n2. Category\n3. Description\n4. Date\n";
    cout << "Enter choice: ";
    cin >> choice;

    bool found = false;
    int searchId = 0;
    string searchText;

    if (choice == 1)
    {
        cout << "Enter expense ID: ";
        cin >> searchId;
    }
    else if (choice >= 2 && choice <= 4)
    {
        cout << "Enter search text: ";
        cin >> searchText;
    }
    else
    {
        cout << "Invalid search choice.\n";
        return;
    }

    for (int i = 0; i < expenseCount; i++)
    {
        if (expenses[i].username != currentUser)
            continue;

        bool match = false;
        if (choice == 1 && expenses[i].id == searchId)
            match = true;
        else if (choice == 2 && expenses[i].category == searchText)
            match = true;
        else if (choice == 3 && expenses[i].description == searchText)
            match = true;
        else if (choice == 4 && expenses[i].date == searchText)
            match = true;

        if (match)
        {
            found = true;
            cout << "ID: " << expenses[i].id
                 << " | Date: " << expenses[i].date
                 << " | Category: " << expenses[i].category
                 << " | Description: " << expenses[i].description
                 << " | Amount: Rs. " << expenses[i].amount << '\n';
        }
    }

    if (!found)
        cout << "No matching expense found.\n";
}

// Delete an expense using its ID.
void deleteExpense()
{
    int id;
    cout << "\nEnter expense ID to delete: ";
    cin >> id;

    for (int i = 0; i < expenseCount; i++)
    {
        if (expenses[i].id == id && expenses[i].username == currentUser)
        {
            for (int j = i; j < expenseCount - 1; j++)
                expenses[j] = expenses[j + 1];

            expenseCount--;
            saveExpenses();
            cout << "Expense deleted successfully.\n";
            return;
        }
    }

    cout << "Expense ID not found.\n";
}

// Display the menu after successful login.
void mainMenu()
{
    int choice;
    do
    {
        cout << "\n========== SMART DIGITAL EXPENSE TRACKER ==========\n";
        cout << "1. Add Daily Expense\n";
        cout << "2. View Expense History\n";
        cout << "3. Category-wise Expense\n";
        cout << "4. Monthly Expense Report\n";
        cout << "5. Set Monthly Budget\n";
        cout << "6. Budget Warning and Expense Prediction\n";
        cout << "7. Search Expense\n";
        cout << "8. Delete Expense\n";
        cout << "9. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1: addExpense(); break;
            case 2: viewExpenseHistory(); break;
            case 3: categoryWiseExpense(); break;
            case 4: monthlyExpenseReport(); break;
            case 5: setBudget(); break;
            case 6: budgetWarningAndPrediction(); break;
            case 7: searchExpense(); break;
            case 8: deleteExpense(); break;
            case 9: cout << "Logged out successfully.\n"; break;
            default: cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 9);
}

int main()
{
    loadExpenses();

    int choice;
    do
    {
        cout << "\n======= SMART DIGITAL EXPENSE TRACKER =======\n";
        cout << "1. User Registration\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                registerUser();
                break;
            case 2:
                if (loginUser())
                    mainMenu();
                break;
            case 3:
                cout << "Thank you for using the Expense Tracker!\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 3);

    return 0;
}
