#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <fstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <sstream>
using namespace std;

struct Expense {
	int amount;
	string category;
	time_t date;
};

bool compareExpenses(const Expense& a, const Expense& b) {
	return a.amount > b.amount;
}

bool compareCategories(const pair<string, int>& a, const pair<string, int>& b) {
	return a.second > b.second;
}

class Card {
protected:
	string number;
	string name;
	string expirationDate;
	string cvv;
	string type;
	int money = 0;

public:
	Card() {
		for (int i = 0; i < 16; i++) {
			number += to_string(rand() % 10);
		}
	}
	Card(string nm, string ed, string c) {
		name = nm;
		expirationDate = ed;
		cvv = c;
		type = "Default";
	}
	void addMoney(int amount) {
		int choise = rand() % 2;
		if (choise == 0) {
			ofstream Stats("Statistic.txt", ios::app);
			Stats << "Money added to account: " << amount << endl;
			money += amount;
			cout << "Money added successfully. Current balance: " << money << endl;
			Stats.close();
		}
		else {
			cout << "Bank decided to not allow add money to your account. Please try again later." << endl;
			money += 0;
		}
	}
	void withdrawMoney(int amount) {
		if (money >= amount) {
			ofstream Stats("Statistic.txt", ios::app);
			Stats << "Money withdrawed from account: " << amount << endl;
			money -= amount;
			cout << "Money withdrawed successfully. Current balance: " << money << endl;
			Stats.close();
		}
		else {
			cout << "Not enough money in your account." << endl;
		}
	}
	void getStats() {
		string text;
		ifstream Stats("Statistic.txt", ios::app);
		while (getline(Stats, text)) {
			cout << text << endl;
		}
	}
	void getCardInfo() {
		cout << "Card number: " << number << " ";
		cout << "Card name: " << name << " ";
		cout << "Card expiration date: " << expirationDate << endl;
		cout << "Card CVV: " << cvv << " ";
		cout << "Card type: " << type << " ";
		cout << "Card money: " << money << " ";
		cout << endl;
	}

	string getType() const { return type; }
	string getCardNumber() const { return number; }
	string getCardName() const { return name; }
	string getExpirationDate() const { return expirationDate; }
	string getCVV() const { return cvv; }
	int getBalance() const { return money; }
	void setBalance(int balance) { money = balance; }
};

class CreditCard : public Card {
public:
	CreditCard(string nm, string ed, string c) : Card() {
		name = nm;
		expirationDate = ed;
		cvv = c;
		type = "Credit";
	}
};

class DebitCard : public Card {
public:
	DebitCard(string nm, string ed, string c) : Card() {
		name = nm;
		expirationDate = ed;
		cvv = c;
		type = "Debit";
	}
};

void saveCardsToFile(const vector<Card>& cards, const string& filename) {
	ofstream file(filename, ios::out);
	if (!file.is_open()) {
		cout << "Failed to open file for saving cards.\n";
		return;
	}

	for (const auto& card : cards) {
		file << card.getType() << ","
			<< card.getCardNumber() << ","
			<< card.getCardName() << ","
			<< card.getExpirationDate() << ","
			<< card.getCVV() << ","
			<< card.getBalance() << "\n";
	}

	file.close();
	cout << "Cards saved successfully to " << filename << ".\n";
}

vector<Card> loadCardsFromFile(const string& filename) {
	ifstream file(filename, ios::in);
	vector<Card> cards;

	if (!file.is_open()) {
		cout << "No saved cards found. Starting fresh.\n";
		return cards;
	}

	string line;
	while (getline(file, line)) {
		stringstream ss(line);
		string type, number, name, expirationDate, cvv;
		int balance;

		getline(ss, type, ',');
		getline(ss, number, ',');
		getline(ss, name, ',');
		getline(ss, expirationDate, ',');
		getline(ss, cvv, ',');
		ss >> balance;

		if (type == "Credit") {
			cards.emplace_back(CreditCard(name, expirationDate, cvv));
		}
		else if (type == "Debit") {
			cards.emplace_back(DebitCard(name, expirationDate, cvv));
		}
		else {
			cards.emplace_back(Card(name, expirationDate, cvv));
		}

		cards.back().setBalance(balance);
	}

	file.close();
	cout << "Cards loaded successfully from " << filename << ".\n";
	return cards;
}

void addExpense(vector<Expense>& expenses) {
	Expense expense;
	cout << "Enter the amount: ";
	cin >> expense.amount;
	cout << "Enter the category: ";
	cin >> expense.category;

	expense.date = time(nullptr);

	expenses.push_back(expense);
	cout << "Expense added successfully!" << endl;
}

vector<Expense> filterExpensesByDate(const vector<Expense>& expenses, int days) {
	vector<Expense> filtered;
	time_t now = time(nullptr);

	for (const auto& expense : expenses) {
		double diff = difftime(now, expense.date) / (60 * 60 * 24);
		if (diff <= days) {
			filtered.push_back(expense);
		}
	}
	return filtered;
}

void generateReportByCategory(const vector<Expense>& expenses) {
	if (expenses.empty()) {
		cout << "No expenses to report." << endl;
		return;
	}

	map<string, int> categoryTotals;

	for (const auto& expense : expenses) {
		categoryTotals[expense.category] += expense.amount;
	}

	cout << "Expense Report by Category:" << endl;
	for (const auto& pair : categoryTotals) {
		cout << "Category: " << pair.first << ", Total: " << pair.second << endl;
	}
}

void saveToFile(const string& filename, const string& content) {
	ofstream file(filename, ios::app);
	if (file.is_open()) {
		file << content << endl;
		file.close();
		cout << "Report saved to " << filename << endl;
	}
	else {
		cout << "Failed to open file: " << filename << endl;
	}
}

void generateTopExpenses(const vector<Expense>& expenses, int days, int topN, const string& filename) {
	vector<Expense> filtered = filterExpensesByDate(expenses, days);
	sort(filtered.begin(), filtered.end(), compareExpenses);
	string report = "Top " + to_string(topN) + " Expenses for the last " + to_string(days) + " days:\n";
	for (int i = 0; i < min(topN, (int)filtered.size()); ++i) {
		report += "Amount: " + to_string(filtered[i].amount) + ", Category: " + filtered[i].category + "\n";
	}
	cout << report;
	saveToFile(filename, report);
}

void generateTopCategories(const vector<Expense>& expenses, int days, int topN, const string& filename) {
	vector<Expense> filtered = filterExpensesByDate(expenses, days);
	map<string, int> categoryTotals;
	for (const auto& expense : filtered) {
		categoryTotals[expense.category] += expense.amount;
	}
	vector<pair<string, int>> sortedCategories(categoryTotals.begin(), categoryTotals.end());
	sort(sortedCategories.begin(), sortedCategories.end(), compareCategories);
	string report = "Top " + to_string(topN) + " Categories for the last " + to_string(days) + " days:\n";
	for (int i = 0; i < min(topN, (int)sortedCategories.size()); ++i) {
		report += "Category: " + sortedCategories[i].first + ", Total: " + to_string(sortedCategories[i].second) + "\n";
	}
	cout << report;
	saveToFile(filename, report);
}

void generateReportAndRankings(const vector<Expense>& expenses) {
	cout << "Generating reports..." << endl;
	generateReportByCategory(filterExpensesByDate(expenses, 1));
	generateReportByCategory(filterExpensesByDate(expenses, 7));
	generateReportByCategory(filterExpensesByDate(expenses, 30));

	cout << "Generating ranking..." << endl;
	generateTopExpenses(expenses, 7, 3, "TopExpenses_Week.txt");
	generateTopExpenses(expenses, 30, 3, "TopExpenses_Month.txt");
	generateTopCategories(expenses, 7, 3, "TopExpenses_Week.txt");
	generateTopCategories(expenses, 30, 3, "TopExpenses_Month.txt");
}

bool isCreditCard(const Card& card) {
	return card.getType() == "Credit";
}

bool isDebitCard(const Card& card) {
	return card.getType() == "Debit";
}

int main() {
	srand(time(NULL));
	vector<Card> cards = loadCardsFromFile("cards.txt");
	vector<Expense> expenses;

	while (true) {
		cout << "\n--- Personal Finance Management System ---\n";
		cout << "1. Create a card\n";
		cout << "2. Add money to a card\n";
		cout << "3. Withdraw money from a card\n";
		cout << "4. View card statistics\n";
		cout << "5. Add an expense\n";
		cout << "6. Generate reports and rankings\n";
		cout << "7. Exit\n";
		cout << "Enter your choice: ";

		int choice;
		cin >> choice;

		switch (choice) {
		case 1: {
			string name, expirationDate, cvv, cardType;
			cout << "Enter your name: ";
			cin >> name;
			cout << "Enter expiration date (MM/YY): ";
			cin >> expirationDate;
			if (expirationDate.size() != 5 || expirationDate[2] != '/') {
				cout << "Invalid expiration date format. Use MM/YY.\n";
				break;
			}
			cout << "Enter CVV: ";
			cin >> cvv;
			if (cvv.size() > 3) {
				cout << "CVV must be 3 digits.\n";
				break;
			}
			cout << "Enter card type (Credit / Debit / Default): ";
			cin >> cardType;

			if (cardType == "Credit") {
				if (any_of(cards.begin(), cards.end(), isCreditCard)) {
					cout << "You can only create one credit card.\n";
				}
				else {
					cards.emplace_back(CreditCard(name, expirationDate, cvv));
					cout << "Credit card created successfully.\n";
				}
			}
			else if (cardType == "Debit") {
				if (any_of(cards.begin(), cards.end(), isDebitCard)) {
					cout << "You can only create one debit card.\n";
				}
				else {
					cards.emplace_back(DebitCard(name, expirationDate, cvv));
					cout << "Debit card created successfully.\n";
				}
			}
			else {
				cards.emplace_back(Card(name, expirationDate, cvv));
				cout << "Default card created successfully.\n";
			}
			break;
		}
		case 2: {
			if (cards.empty()) {
				cout << "No cards available. Please create a card first.\n";
				break;
			}
			cout << "Select a card to add money:\n";
			for (int i = 0; i < cards.size(); i++) {
				cout << i + 1 << ". ";
				cards[i].getCardInfo();
			}
			int cardIndex, amount;
			cin >> cardIndex;
			cout << "Enter the amount to add: ";
			cin >> amount;
			if (cardIndex > 0 && cardIndex <= cards.size()) {
				cards[cardIndex - 1].addMoney(amount);
			}
			else {
				cout << "Invalid card selection.\n";
			}
			break;
		}
		case 3: {
			if (cards.empty()) {
				cout << "No cards available. Please create a card first.\n";
				break;
			}
			cout << "Select a card to withdraw money:\n";
			for (int i = 0; i < cards.size(); i++) {
				cout << i + 1 << ". ";
				cards[i].getCardInfo();
			}
			int cardIndex, amount;
			cin >> cardIndex;
			cout << "Enter the amount to withdraw: ";
			cin >> amount;
			if (cardIndex > 0 && cardIndex <= cards.size()) {
				cards[cardIndex - 1].withdrawMoney(amount);
			}
			else {
				cout << "Invalid card selection.\n";
			}
			break;
		}
		case 4: {
			if (cards.empty()) {
				cout << "No cards available. Please create a card first.\n";
				break;
			}
			cout << "Select a card to view statistics:\n";
			for (int i = 0; i < cards.size(); i++) {
				cout << i + 1 << ". ";
				cards[i].getCardInfo();
			}
			int cardIndex;
			cin >> cardIndex;
			if (cardIndex > 0 && cardIndex <= cards.size()) {
				cards[cardIndex - 1].getStats();
			}
			else {
				cout << "Invalid card selection.\n";
			}
			break;
		}
		case 5:
			addExpense(expenses);
			break;
		case 6:
			generateReportAndRankings(expenses);
			break;
		case 7:
			saveCardsToFile(cards, "cards.txt");
			cout << "Exiting program. Goodbye!\n";
			return 0;
		default:
			cout << "Invalid choice. Please try again.\n";
		}
	}
}