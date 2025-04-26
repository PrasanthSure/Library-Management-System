#include <iostream>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

// Book Class
class Book {
public:
    string ISBN;
    string title;
    string author;
    bool isAvailable;
    
    Book(string isbn, string t, string a) : ISBN(isbn), title(t), author(a), isAvailable(true) {}
};

// BST Node for Books
class BookNode {
public:
    Book* book;
    BookNode* left;
    BookNode* right;
    
    BookNode(Book* b) : book(b), left(nullptr), right(nullptr) {}
};

// User Class
class User {
public:
    int id;
    string name;
    vector<pair<Book*, time_t>> borrowedBooks; // Book and due date
    
    User(int i, string n) : id(i), name(n) {}
};

// Hash Table for Users
class UserHashTable {
private:
    static const int TABLE_SIZE = 100;
    vector<User*>* table[TABLE_SIZE];
    
    int hashFunction(int id) {
        return id % TABLE_SIZE;
    }

public:
    UserHashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = new vector<User*>;
        }
    }
    
    void addUser(User* user) {
        int index = hashFunction(user->id);
        table[index]->push_back(user);
    }
    
    User* getUser(int id) {
        int index = hashFunction(id);
        for (auto user : *table[index]) {
            if (user->id == id) {
                return user;
            }
        }
        return nullptr;
    }
};

// Library Class
class Library {
private:
    BookNode* root;
    UserHashTable userTable;
    
    // BST operations
    BookNode* insertBookHelper(BookNode* node, Book* book) {
        if (node == nullptr) {
            return new BookNode(book);
        }
        
        if (book->ISBN < node->book->ISBN) {
            node->left = insertBookHelper(node->left, book);
        } else if (book->ISBN > node->book->ISBN) {
            node->right = insertBookHelper(node->right, book);
        }
        
        return node;
    }
    
    Book* searchBookHelper(BookNode* node, string ISBN) {
        if (node == nullptr) {
            return nullptr;
        }
        
        if (ISBN == node->book->ISBN) {
            return node->book;
        } else if (ISBN < node->book->ISBN) {
            return searchBookHelper(node->left, ISBN);
        } else {
            return searchBookHelper(node->right, ISBN);
        }
    }
    
    void displayBooksHelper(BookNode* node) {
        if (node != nullptr) {
            displayBooksHelper(node->left);
            cout << "ISBN: " << node->book->ISBN 
                 << ", Title: " << node->book->title 
                 << ", Author: " << node->book->author 
                 << ", Available: " << (node->book->isAvailable ? "Yes" : "No") 
                 << endl;
            displayBooksHelper(node->right);
        }
    }

public:
    Library() : root(nullptr) {}
    
    void addBook(string ISBN, string title, string author) {
        Book* newBook = new Book(ISBN, title, author);
        root = insertBookHelper(root, newBook);
    }
    
    Book* searchBook(string ISBN) {
        return searchBookHelper(root, ISBN);
    }
    
    void displayAllBooks() {
        displayBooksHelper(root);
    }
    
    void addUser(int id, string name) {
        User* newUser = new User(id, name);
        userTable.addUser(newUser);
    }
    
    bool borrowBook(int userId, string ISBN) {
        User* user = userTable.getUser(userId);
        Book* book = searchBook(ISBN);
        
        if (user == nullptr || book == nullptr || !book->isAvailable) {
            return false;
        }
        
        // Set due date to 14 days from now
        time_t dueDate = time(nullptr) + (14 * 24 * 60 * 60);
        user->borrowedBooks.push_back(make_pair(book, dueDate));
        book->isAvailable = false;
        return true;
    }
    
    bool returnBook(int userId, string ISBN) {
        User* user = userTable.getUser(userId);
        Book* book = searchBook(ISBN);
        
        if (user == nullptr || book == nullptr) {
            return false;
        }
        
        // Find and remove the book from user's borrowed list
        for (auto it = user->borrowedBooks.begin(); it != user->borrowedBooks.end(); ++it) {
            if (it->first->ISBN == ISBN) {
                user->borrowedBooks.erase(it);
                book->isAvailable = true;
                
                // Check if book is overdue
                time_t now = time(nullptr);
                if (now > it->second) {
                    cout << "Book was overdue! Fine: $" 
                         << ((now - it->second) / (24 * 60 * 60)) * 1.0 
                         << " ($1 per day)" << endl;
                }
                return true;
            }
        }
        
        return false;
    }
    
    void displayUserBooks(int userId) {
        User* user = userTable.getUser(userId);
        if (user == nullptr) {
            cout << "User not found!" << endl;
            return;
        }
        
        cout << "Books borrowed by " << user->name << ":" << endl;
        for (auto& entry : user->borrowedBooks) {
            Book* book = entry.first;
            time_t dueDate = entry.second;
            cout << "Title: " << book->title 
                 << ", Due: " << ctime(&dueDate);
        }
    }
};

// Main function with menu interface
int main() {
    Library library;
    int choice;
    
    do {
        cout << "\nLibrary Management System\n";
        cout << "1. Add Book\n";
        cout << "2. Search Book\n";
        cout << "3. Display All Books\n";
        cout << "4. Add User\n";
        cout << "5. Borrow Book\n";
        cout << "6. Return Book\n";
        cout << "7. Display User's Books\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Clear newline character
        
        switch (choice) {
            case 1: {
                string ISBN, title, author;
                cout << "Enter ISBN: ";
                getline(cin, ISBN);
                cout << "Enter Title: ";
                getline(cin, title);
                cout << "Enter Author: ";
                getline(cin, author);
                library.addBook(ISBN, title, author);
                break;
            }
            case 2: {
                string ISBN;
                cout << "Enter ISBN to search: ";
                getline(cin, ISBN);
                Book* book = library.searchBook(ISBN);
                if (book != nullptr) {
                    cout << "Book found:\n";
                    cout << "Title: " << book->title << ", Author: " << book->author 
                         << ", Available: " << (book->isAvailable ? "Yes" : "No") << endl;
                } else {
                    cout << "Book not found!" << endl;
                }
                break;
            }
            case 3:
                cout << "\nAll Books in Library:\n";
                library.displayAllBooks();
                break;
            case 4: {
                int id;
                string name;
                cout << "Enter User ID: ";
                cin >> id;
                cin.ignore();
                cout << "Enter Name: ";
                getline(cin, name);
                library.addUser(id, name);
                break;
            }
            case 5: {
                int userId;
                string ISBN;
                cout << "Enter User ID: ";
                cin >> userId;
                cin.ignore();
                cout << "Enter ISBN of book to borrow: ";
                getline(cin, ISBN);
                if (library.borrowBook(userId, ISBN)) {
                    cout << "Book borrowed successfully!" << endl;
                } else {
                    cout << "Failed to borrow book!" << endl;
                }
                break;
            }
            case 6: {
                int userId;
                string ISBN;
                cout << "Enter User ID: ";
                cin >> userId;
                cin.ignore();
                cout << "Enter ISBN of book to return: ";
                getline(cin, ISBN);
                if (library.returnBook(userId, ISBN)) {
                    cout << "Book returned successfully!" << endl;
                } else {
                    cout << "Failed to return book!" << endl;
                }
                break;
            }
            case 7: {
                int userId;
                cout << "Enter User ID: ";
                cin >> userId;
                library.displayUserBooks(userId);
                break;
            }
            case 8:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice! Try again.\n";
        }
    } while (choice != 8);
    
    return 0;
}
