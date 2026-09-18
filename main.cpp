#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <limits>
#include <cctype>

using namespace std;

class Transaction {
private:
    int id;
    string date;
    string type;
    string category;
    string description;
    double amount;

public:
    Transaction(int id, string date, string type, string category,
                string description, double amount)
        : id(id), date(date), type(type), category(category),
          description(description), amount(amount) {}

    int getId() const {
        return id;
    }

    string getDate() const {
        return date;
    }

    string getType() const {
        return type;
    }

    string getCategory() const {
        return category;
    }

    string getDescription() const {
        return description;
    }

    double getAmount() const {
        return amount;
    }

    void setDate(const string& value) {
        date = value;
    }

    void setType(const string& value) {
        type = value;
    }

    void setCategory(const string& value) {
        category = value;
    }

    void setDescription(const string& value) {
        description = value;
    }

    void setAmount(double value) {
        amount = value;
    }

    void display() const {
        cout << left
             << setw(5) << id
             << setw(13) << date
             << setw(10) << type
             << setw(16) << category
             << setw(28) << description
             << "RM "
             << fixed
             << setprecision(2)
             << amount
             << '\n';
    }
};


// Convert text to lowercase
string toLowerCopy(string text) {

    transform(
        text.begin(),
        text.end(),
        text.begin(),
        [](unsigned char c) {
            return static_cast<char>(tolower(c));
        }
    );

    return text;
}


// Get validated integer
int getIntInRange(
    const string& prompt,
    int minValue,
    int maxValue
) {

    while (true) {

        cout << prompt;

        string input;
        getline(cin, input);

        stringstream ss(input);

        int value;
        char extra;

        if (
            ss >> value &&
            !(ss >> extra) &&
            value >= minValue &&
            value <= maxValue
        ) {
            return value;
        }

        cout << "Invalid input. Please enter a number from "
             << minValue
             << " to "
             << maxValue
             << ".\n";
    }
}


// Get validated positive amount
double getPositiveDouble(const string& prompt) {

    while (true) {

        cout << prompt;

        string input;
        getline(cin, input);

        stringstream ss(input);

        double value;
        char extra;

        if (
            ss >> value &&
            !(ss >> extra) &&
            value > 0
        ) {
            return value;
        }

        cout << "Invalid amount. Please enter a value greater than 0.\n";
    }
}


// Prevent empty input
string getNonEmptyLine(const string& prompt) {

    while (true) {

        cout << prompt;

        string value;
        getline(cin, value);

        if (!value.empty()) {
            return value;
        }

        cout << "Input cannot be empty.\n";
    }
}


// Select transaction type
string chooseType() {

    cout << "\nTransaction Type\n";
    cout << "1. Income\n";
    cout << "2. Expense\n";

    int choice =
        getIntInRange("Select type: ", 1, 2);

    if (choice == 1) {
        return "Income";
    }

    return "Expense";
}


// Select category
string chooseCategory(const string& type) {

    if (type == "Expense") {

        cout << "\nExpense Category\n";

        cout << "1. Food\n";
        cout << "2. Transport\n";
        cout << "3. Shopping\n";
        cout << "4. Bills\n";
        cout << "5. Entertainment\n";
        cout << "6. Education\n";
        cout << "7. Health\n";
        cout << "8. Other\n";

        int choice =
            getIntInRange(
                "Select category: ",
                1,
                8
            );

        vector<string> categories = {
            "Food",
            "Transport",
            "Shopping",
            "Bills",
            "Entertainment",
            "Education",
            "Health",
            "Other"
        };

        return categories[choice - 1];
    }


    cout << "\nIncome Category\n";

    cout << "1. Salary\n";
    cout << "2. Allowance\n";
    cout << "3. Freelance\n";
    cout << "4. Investment\n";
    cout << "5. Other\n";

    int choice =
        getIntInRange(
            "Select category: ",
            1,
            5
        );

    vector<string> categories = {
        "Salary",
        "Allowance",
        "Freelance",
        "Investment",
        "Other"
    };

    return categories[choice - 1];
}


// Handle commas inside CSV data
string escapeCSV(const string& field) {

    if (
        field.find_first_of(",\"") ==
        string::npos
    ) {
        return field;
    }

    string escaped = "\"";

    for (char c : field) {

        if (c == '"') {
            escaped += "\"\"";
        }
        else {
            escaped += c;
        }
    }

    escaped += '"';

    return escaped;
}


// Read CSV line safely
vector<string> parseCSVLine(
    const string& line
) {

    vector<string> fields;

    string field;

    bool inQuotes = false;

    for (
        size_t i = 0;
        i < line.size();
        ++i
    ) {

        char c = line[i];

        if (c == '"') {

            if (
                inQuotes &&
                i + 1 < line.size() &&
                line[i + 1] == '"'
            ) {

                field += '"';

                ++i;
            }
            else {

                inQuotes = !inQuotes;
            }
        }

        else if (
            c == ',' &&
            !inQuotes
        ) {

            fields.push_back(field);

            field.clear();
        }

        else {

            field += c;
        }
    }

    fields.push_back(field);

    return fields;
}


// Save all transactions to CSV
void saveTransactions(
    const vector<Transaction>& transactions,
    const string& filename =
        "transactions.csv"
) {

    ofstream file(filename);

    if (!file) {

        cout << "Warning: Could not save transaction data.\n";

        return;
    }


    file
        << "ID,Date,Type,Category,Description,Amount\n";


    for (
        const Transaction& transaction :
        transactions
    ) {

        file
            << transaction.getId()
            << ","

            << escapeCSV(
                transaction.getDate()
            )
            << ","

            << escapeCSV(
                transaction.getType()
            )
            << ","

            << escapeCSV(
                transaction.getCategory()
            )
            << ","

            << escapeCSV(
                transaction.getDescription()
            )
            << ","

            << fixed
            << setprecision(2)
            << transaction.getAmount()

            << '\n';
    }
}


// Load transactions from CSV
void loadTransactions(
    vector<Transaction>& transactions,
    int& nextId,
    const string& filename =
        "transactions.csv"
) {

    ifstream file(filename);

    if (!file) {

        return;
    }


    string line;

    // Skip header
    getline(file, line);


    int highestId = 0;


    while (getline(file, line)) {

        if (line.empty()) {

            continue;
        }


        vector<string> fields =
            parseCSVLine(line);


        if (fields.size() != 6) {

            continue;
        }


        try {

            int id =
                stoi(fields[0]);

            double amount =
                stod(fields[5]);


            transactions.emplace_back(
                id,
                fields[1],
                fields[2],
                fields[3],
                fields[4],
                amount
            );


            highestId =
                max(
                    highestId,
                    id
                );
        }

        catch (...) {

            // Ignore corrupted rows
        }
    }


    nextId =
        highestId + 1;
}


// Save budget
void saveBudget(
    double budget,
    const string& filename =
        "budget.txt"
) {

    ofstream file(filename);

    if (file) {

        file
            << fixed
            << setprecision(2)
            << budget;
    }
}


// Load budget
double loadBudget(
    const string& filename =
        "budget.txt"
) {

    ifstream file(filename);

    double budget = 0.0;


    if (
        file >> budget &&
        budget > 0
    ) {

        return budget;
    }


    return 0.0;
}


// Table header
void printTransactionHeader() {

    cout << left

         << setw(5)
         << "ID"

         << setw(13)
         << "Date"

         << setw(10)
         << "Type"

         << setw(16)
         << "Category"

         << setw(28)
         << "Description"

         << "Amount\n";


    cout
        << string(88, '-')
        << '\n';
}


// View transactions
void viewTransactions(
    const vector<Transaction>& transactions
) {

    cout
        << "\n--- Transaction History ---\n";


    if (transactions.empty()) {

        cout
            << "No transactions recorded yet.\n";

        return;
    }


    printTransactionHeader();


    for (
        const Transaction& transaction :
        transactions
    ) {

        transaction.display();
    }
}


// Add transaction
void addTransaction(
    vector<Transaction>& transactions,
    int& nextId
) {

    cout
        << "\n--- Add Transaction ---\n";


    string date =
        getNonEmptyLine(
            "Enter date (DD/MM/YYYY): "
        );


    string type =
        chooseType();


    string category =
        chooseCategory(type);


    string description =
        getNonEmptyLine(
            "Enter description: "
        );


    double amount =
        getPositiveDouble(
            "Enter amount (RM): "
        );


    transactions.emplace_back(
        nextId,
        date,
        type,
        category,
        description,
        amount
    );


    nextId++;


    saveTransactions(
        transactions
    );


    cout
        << "\nTransaction added successfully.\n";
}


// Search transactions
void searchTransactions(
    const vector<Transaction>& transactions
) {

    if (transactions.empty()) {

        cout
            << "\nNo transactions available.\n";

        return;
    }


    cout
        << "\n--- Search Transactions ---\n";

    cout
        << "1. Search by category\n";

    cout
        << "2. Search by description\n";

    cout
        << "3. Search by type\n";


    int choice =
        getIntInRange(
            "Select search method: ",
            1,
            3
        );


    string keyword =
        toLowerCopy(
            getNonEmptyLine(
                "Enter search keyword: "
            )
        );


    bool found = false;


    printTransactionHeader();


    for (
        const Transaction& transaction :
        transactions
    ) {

        string value;


        if (choice == 1) {

            value =
                transaction.getCategory();
        }

        else if (choice == 2) {

            value =
                transaction.getDescription();
        }

        else {

            value =
                transaction.getType();
        }


        if (
            toLowerCopy(value)
                .find(keyword)
            != string::npos
        ) {

            transaction.display();

            found = true;
        }
    }


    if (!found) {

        cout
            << "No matching transactions found.\n";
    }
}


// Find transaction by ID
Transaction* findTransactionById(
    vector<Transaction>& transactions,
    int id
) {

    for (
        Transaction& transaction :
        transactions
    ) {

        if (
            transaction.getId()
            == id
        ) {

            return &transaction;
        }
    }


    return nullptr;
}


// Edit transaction
void editTransaction(
    vector<Transaction>& transactions
) {

    if (transactions.empty()) {

        cout
            << "\nNo transactions available to edit.\n";

        return;
    }


    viewTransactions(
        transactions
    );


    int id =
        getIntInRange(
            "\nEnter transaction ID to edit: ",
            1,
            numeric_limits<int>::max()
        );


    Transaction* transaction =
        findTransactionById(
            transactions,
            id
        );


    if (transaction == nullptr) {

        cout
            << "Transaction ID not found.\n";

        return;
    }


    cout
        << "\nEditing transaction:\n";


    printTransactionHeader();

    transaction->display();


    cout
        << "\nEnter the new details.\n";


    string date =
        getNonEmptyLine(
            "Enter date (DD/MM/YYYY): "
        );


    string type =
        chooseType();


    string category =
        chooseCategory(type);


    string description =
        getNonEmptyLine(
            "Enter description: "
        );


    double amount =
        getPositiveDouble(
            "Enter amount (RM): "
        );


    transaction->setDate(
        date
    );

    transaction->setType(
        type
    );

    transaction->setCategory(
        category
    );

    transaction->setDescription(
        description
    );

    transaction->setAmount(
        amount
    );


    saveTransactions(
        transactions
    );


    cout
        << "\nTransaction updated successfully.\n";
}


// Delete transaction
void deleteTransaction(
    vector<Transaction>& transactions
) {

    if (transactions.empty()) {

        cout
            << "\nNo transactions available to delete.\n";

        return;
    }


    viewTransactions(
        transactions
    );


    int id =
        getIntInRange(
            "\nEnter transaction ID to delete: ",
            1,
            numeric_limits<int>::max()
        );


    auto position =
        find_if(
            transactions.begin(),
            transactions.end(),

            [id](
                const Transaction& transaction
            ) {

                return
                    transaction.getId()
                    == id;
            }
        );


    if (
        position ==
        transactions.end()
    ) {

        cout
            << "Transaction ID not found.\n";

        return;
    }


    cout
        << "\nTransaction selected:\n";


    printTransactionHeader();

    position->display();


    string confirmation =
        toLowerCopy(
            getNonEmptyLine(
                "Delete this transaction? (Y/N): "
            )
        );


    if (
        confirmation == "y" ||
        confirmation == "yes"
    ) {

        transactions.erase(
            position
        );


        saveTransactions(
            transactions
        );


        cout
            << "Transaction deleted successfully.\n";
    }

    else {

        cout
            << "Deletion cancelled.\n";
    }
}


// Sort transactions
void sortTransactions(
    vector<Transaction>& transactions
) {

    if (transactions.empty()) {

        cout
            << "\nNo transactions available to sort.\n";

        return;
    }


    cout
        << "\n--- Sort Transactions ---\n";

    cout
        << "1. Amount: Low to High\n";

    cout
        << "2. Amount: High to Low\n";

    cout
        << "3. Category: A to Z\n";

    cout
        << "4. ID: Low to High\n";


    int choice =
        getIntInRange(
            "Select sort option: ",
            1,
            4
        );


    if (choice == 1) {

        sort(
            transactions.begin(),
            transactions.end(),

            [](
                const Transaction& a,
                const Transaction& b
            ) {

                return
                    a.getAmount()
                    < b.getAmount();
            }
        );
    }


    else if (choice == 2) {

        sort(
            transactions.begin(),
            transactions.end(),

            [](
                const Transaction& a,
                const Transaction& b
            ) {

                return
                    a.getAmount()
                    > b.getAmount();
            }
        );
    }


    else if (choice == 3) {

        sort(
            transactions.begin(),
            transactions.end(),

            [](
                const Transaction& a,
                const Transaction& b
            ) {

                return
                    a.getCategory()
                    < b.getCategory();
            }
        );
    }


    else {

        sort(
            transactions.begin(),
            transactions.end(),

            [](
                const Transaction& a,
                const Transaction& b
            ) {

                return
                    a.getId()
                    < b.getId();
            }
        );
    }


    saveTransactions(
        transactions
    );


    cout
        << "\nTransactions sorted successfully.\n";


    viewTransactions(
        transactions
    );
}


// Financial analytics
void financialAnalytics(
    const vector<Transaction>& transactions
) {

    if (transactions.empty()) {

        cout
            << "\nNo transaction data available.\n";

        return;
    }


    double totalIncome = 0.0;

    double totalExpenses = 0.0;

    double largestExpense = 0.0;

    string largestExpenseDescription =
        "None";

    int expenseCount = 0;


    map<string, double>
        categorySpending;


    for (
        const Transaction& transaction :
        transactions
    ) {

        if (
            transaction.getType()
            == "Income"
        ) {

            totalIncome +=
                transaction.getAmount();
        }


        else if (
            transaction.getType()
            == "Expense"
        ) {

            totalExpenses +=
                transaction.getAmount();


            expenseCount++;


            categorySpending[
                transaction.getCategory()
            ]
            += transaction.getAmount();


            if (
                transaction.getAmount()
                > largestExpense
            ) {

                largestExpense =
                    transaction.getAmount();


                largestExpenseDescription =
                    transaction.getDescription();
            }
        }
    }


    double balance =
        totalIncome -
        totalExpenses;


    double averageExpense =
        0.0;


    if (expenseCount > 0) {

        averageExpense =
            totalExpenses /
            expenseCount;
    }


    double savingsRate =
        0.0;


    if (totalIncome > 0) {

        savingsRate =
            (
                balance /
                totalIncome
            )
            * 100.0;
    }


    string highestCategory =
        "None";

    double highestCategoryAmount =
        0.0;


    for (
        const auto& entry :
        categorySpending
    ) {

        if (
            entry.second
            > highestCategoryAmount
        ) {

            highestCategoryAmount =
                entry.second;


            highestCategory =
                entry.first;
        }
    }


    cout
        << "\n=============================================\n";

    cout
        << "              FINANCIAL ANALYTICS\n";

    cout
        << "=============================================\n";


    cout
        << fixed
        << setprecision(2);


    cout
        << "Total Income:          RM "
        << totalIncome
        << '\n';


    cout
        << "Total Expenses:        RM "
        << totalExpenses
        << '\n';


    cout
        << "Current Balance:       RM "
        << balance
        << '\n';


    cout
        << "Average Expense:       RM "
        << averageExpense
        << '\n';


    cout
        << "Savings Rate:          "
        << savingsRate
        << "%\n";


    cout
        << "\nSPENDING BY CATEGORY\n";


    cout
        << string(45, '-')
        << '\n';


    if (categorySpending.empty()) {

        cout
            << "No expense data available.\n";
    }


    else {

        for (
            const auto& entry :
            categorySpending
        ) {

            cout
                << left
                << setw(20)
                << entry.first

                << "RM "

                << right
                << setw(10)
                << entry.second

                << '\n';
        }


        cout
            << left;


        cout
            << "\nHighest Spending Category: "
            << highestCategory
            << " (RM "
            << highestCategoryAmount
            << ")\n";


        cout
            << "Largest Single Expense:   "
            << largestExpenseDescription
            << " (RM "
            << largestExpense
            << ")\n";
    }


    cout
        << "=============================================\n";
}


// Budget tracker
void budgetTracker(
    const vector<Transaction>& transactions,
    double& budget
) {

    cout
        << "\n--- Expense Budget Tracker ---\n";


    if (budget <= 0) {

        cout
            << "No budget has been set yet.\n";


        budget =
            getPositiveDouble(
                "Set your expense budget (RM): "
            );


        saveBudget(
            budget
        );


        cout
            << "Budget saved successfully.\n";
    }


    else {

        cout
            << fixed
            << setprecision(2);


        cout
            << "Current budget: RM "
            << budget
            << '\n';


        cout
            << "1. View budget status\n";

        cout
            << "2. Change budget\n";


        int choice =
            getIntInRange(
                "Select option: ",
                1,
                2
            );


        if (choice == 2) {

            budget =
                getPositiveDouble(
                    "Enter new budget (RM): "
                );


            saveBudget(
                budget
            );


            cout
                << "Budget updated successfully.\n";
        }
    }


    double totalExpenses =
        0.0;


    for (
        const Transaction& transaction :
        transactions
    ) {

        if (
            transaction.getType()
            == "Expense"
        ) {

            totalExpenses +=
                transaction.getAmount();
        }
    }


    double remaining =
        budget -
        totalExpenses;


    double usage =
        0.0;


    if (budget > 0) {

        usage =
            (
                totalExpenses /
                budget
            )
            * 100.0;
    }


    cout
        << "\nBUDGET STATUS\n";


    cout
        << string(35, '-')
        << '\n';


    cout
        << fixed
        << setprecision(2);


    cout
        << "Budget:       RM "
        << budget
        << '\n';


    cout
        << "Spent:        RM "
        << totalExpenses
        << '\n';


    cout
        << "Remaining:    RM "
        << remaining
        << '\n';


    cout
        << "Budget Used:  "
        << usage
        << "%\n";


    if (
        totalExpenses >
        budget
    ) {

        cout
            << "Status: OVER BUDGET by RM "
            << (
                totalExpenses -
                budget
            )
            << '\n';
    }


    else if (
        usage >= 80.0
    ) {

        cout
            << "Status: Warning - approaching budget limit.\n";
    }


    else {

        cout
            << "Status: Within budget.\n";
    }
}


// Main menu
void showMainMenu() {

    cout
        << "\n=============================================\n";

    cout
        << "                  FINTRACK\n";

    cout
        << "     Personal Finance & Analytics System\n";

    cout
        << "=============================================\n";


    cout
        << "1. Add Transaction\n";

    cout
        << "2. View Transactions\n";

    cout
        << "3. Search Transactions\n";

    cout
        << "4. Edit Transaction\n";

    cout
        << "5. Delete Transaction\n";

    cout
        << "6. Sort Transactions\n";

    cout
        << "7. Financial Analytics\n";

    cout
        << "8. Budget Tracker\n";

    cout
        << "9. Exit\n";


    cout
        << "---------------------------------------------\n";
}


int main() {

    vector<Transaction>
        transactions;


    int nextId = 1;


    double budget =
        loadBudget();


    loadTransactions(
        transactions,
        nextId
    );


    cout
        << "Welcome to FinTrack.\n";


    cout
        << transactions.size()
        << " transaction(s) loaded.\n";


    int choice;


    do {

        showMainMenu();


        choice =
            getIntInRange(
                "Enter your choice: ",
                1,
                9
            );


        switch (choice) {

            case 1:

                addTransaction(
                    transactions,
                    nextId
                );

                break;


            case 2:

                viewTransactions(
                    transactions
                );

                break;


            case 3:

                searchTransactions(
                    transactions
                );

                break;


            case 4:

                editTransaction(
                    transactions
                );

                break;


            case 5:

                deleteTransaction(
                    transactions
                );

                break;


            case 6:

                sortTransactions(
                    transactions
                );

                break;


            case 7:

                financialAnalytics(
                    transactions
                );

                break;


            case 8:

                budgetTracker(
                    transactions,
                    budget
                );

                break;


            case 9:

                saveTransactions(
                    transactions
                );


                saveBudget(
                    budget
                );


                cout
                    << "\nData saved successfully.\n";


                cout
                    << "Thank you for using FinTrack!\n";

                break;
        }

    } while (choice != 9);


    return 0;
}
