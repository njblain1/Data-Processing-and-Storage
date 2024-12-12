#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <string>
using namespace std;

class InMemoryDB {          // from fig 1 in assignment instrucitons
private:
    unordered_map<string, int> mainStore;
    unordered_map<string, int> transactionChanges;
    bool activeTransaction = false;

public:
    int get(const string& key) {            // get(key) will return the value associated with the key or null if the key doesn’t exist. can be called anytime even when a transaction is not in progress
        if (activeTransaction && transactionChanges.count(key)) {
            return transactionChanges[key];
        }
        if (mainStore.count(key)) {
            return mainStore[key];
        }
        throw runtime_error("Key not found: " + key);
    }

    void put(const string& key, int value) {            // put(key, val) will create a new key with the provided value if a key doesn’t exist. Otherwise it will update the value of an existing key.
        if (!activeTransaction) {
            throw runtime_error("No transaction in progress");
        }
        transactionChanges[key] = value;
    }

    void begin_transaction() {          // begin_transaction() starts a new transaction. At a time only a single transaction may exist. Within a transaction you can make as many changes to as many keys as you like. However, they should not be “visible” to get(), until the transaction is committed.
        if (activeTransaction) {
            throw runtime_error("Transaction already in progress");
        }
        activeTransaction = true;
        transactionChanges.clear();
    }

    void commit() {                     // commit() applies changes made within the transaction to the main state. Allowing any future gets() to “see” the changes made within the transaction
        if (!activeTransaction) {
            throw runtime_error("No transaction in progress");
        }
        for (const auto& [key, value] : transactionChanges) {
            mainStore[key] = value;
        }
        activeTransaction = false;
        transactionChanges.clear();
    }

    void rollback() {               // rollback() should abort all the changes made within the transaction and everything should go back to the way it was before.
        if (!activeTransaction) {
            throw runtime_error("No transaction in progress");
        }
        activeTransaction = false;
        transactionChanges.clear();
    }
};
// end fig 1

// start fig 2
int main() {
    try {
        InMemoryDB db;

        try {
            db.get("A");                    // should return null, because A doesn’t exist in the DB yet
        }
        catch (const exception& e) {
            cout << e.what() << endl;
        }

        try {
            db.put("A", 5);             // should throw an error because a transaction is not in progress
        }
        catch (const exception& e) {
            cout << e.what() << endl;
        }

        db.begin_transaction();                     // starts a new transaction
        db.put("A", 5);                  // set’s value of A to 5, but its not committed yet
        try {
            cout << db.get("A") << endl;        // should return null, because updates to A are not committed yet
        }
        catch (const exception& e) {
            cout << e.what() << endl;
        }

        db.put("A", 6);                 // update A’s value to 6 within the transaction
        db.commit();                            // commits the open transaction
        cout << db.get("A") << endl;        // should return 6, that was the last value of A to be committed

        try {
            db.commit();                        // throws an error, because there is no open transaction
        }
        catch (const exception& e) {
            cout << e.what() << endl;
        }

        try {
            db.rollback();                      // throws an error because there is no ongoing transaction
        }
        catch (const exception& e) {
            cout << e.what() << endl;
        }

        try {
            cout << db.get("B") << endl;        // should return null because B does not exist in the database
        }
        catch (const exception& e) {
            cout << e.what() << endl;
        }

        db.begin_transaction();            // starts a new transaction
        db.put("B", 10);        // Set key B’s value to 10 within the transaction
        db.rollback();                   // Rollback the transaction - revert any changes made to B

        try {
            cout << db.get("B") << endl;        // Should return null because changes to B were rolled back
        }
        catch (const exception& e) {
            cout << e.what() << endl;
        }
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}


