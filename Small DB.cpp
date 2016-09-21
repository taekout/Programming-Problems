#include <map>
#include <exception>
#include <memory>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

using namespace std;

class Database;

/*
DataMap : abstract for map class
*/
template<typename K>
class DataMap
{
public:

    virtual void set(const K & var, long value) = 0;
    virtual void unset(const K & var) = 0;
    virtual bool get(const K & var, long & value) = 0;

    virtual void increment(const K & var) = 0;
    virtual void decrement(const K & var) = 0;
};

/*
ValueMap : map class that has mapping for values. can be used to look up a variable.
*/
template<typename K>
class ValueMap : public DataMap<K>
{
    unordered_map<K /*var*/, long /*value*/> values;
public:
    // return: if the var existed or not.
    virtual void set(const K & var, long newValue) {
        long oldValue;
        bool exist = get(var, oldValue);
        if (exist) {
            values.erase(var);
        }
        values.insert(make_pair(var, newValue));
    }

    virtual void unset(const K & var) {
        long garbage;
        bool exist = get(var, garbage);
        if (exist) {
            values.erase(var);
        }
    }

    virtual bool get(const K & var, long & value) {
        auto it = values.find(var);
        if (it != values.end()) {
            value = it->second;
            return true;
        }
        return false;
    }

    virtual void increment(const K & var) {
        auto it = values.find(var);
        if (it != values.end()) {
            it->second++;
        }
        else
            values.insert(make_pair(var, 1));
    }

    virtual void decrement(const K & var) {
        auto it = values.find(var);
        if (it != values.end()) {
            it->second--;
            if (it->second == 0) {
                values.erase(it);
            }
        }
        else {
            //throw "Cannot decrement what does not exist.";
        }
    }
};


class Command  {
public:
    virtual void run(Database * db) = 0;
};

class Set : public Command { // Set command class.
    string fVar;
    long fValue;
public:
    Set(const string & var, long newValue) : fVar(var), fValue(newValue) {}
    virtual void run(Database * db);
};

class UnSet : public Command { // Unset command class.
    string fVar;
public:
    UnSet(const string & var) : fVar(var) {}
    virtual void run(Database * db);
};

class Get : public Command { // Get command class.
    string fVar;
public:
    Get(const string & var) : fVar(var) {}
    virtual void run(Database * db);
};

class NumEqualTo : public Command { // NumEqualTo command class.
    long fValue;
public:
    NumEqualTo(long value) : fValue(value) {}
    virtual void run(Database * db);
};

class Begin : public Command {
public:
    virtual void run(Database * db);
};

class NoOperation : public Command {
public:
    virtual void run(Database * db);
};

class Rollback : public Command {
public:
    void run(Database * db);
};

class Commit : public Command {
public:
    void run(Database * db);
};

class End : public Command {
public:
    void run(Database * db);
};

class Parser
{
protected:
    void ignoreWhiteSpace(string str, size_t & index) {
        while (str.length() > index && str[index] == ' ') {
            index++;
        }
    }

public:
    string getName(string str, size_t & index) {
        ignoreWhiteSpace(str, index);

        size_t startPos = index;
        string name;
        while (str.length() > index && str[index] != ' ') {
            index++;
        }
        name = str.substr(startPos, index - startPos);
        return std::move(name);
    }

    long getNumber(string str, size_t index) {
        ignoreWhiteSpace(str, index);

        size_t startPos = index;
        string value;
        while (str.length() > index && str[index++] != ' ') {
            index++;
        }
        value = str.substr(startPos, index - startPos);
        return std::stol(value);
    }
};

class CommandFactory {
    static CommandFactory * fFactory;
public:

    // Singleton. - Memory leakage seems inevitable. Is there a better way?
    static CommandFactory * getInstnace() {
        if (fFactory == nullptr) {
            fFactory = new CommandFactory();
        }
        return fFactory;
    }

    Command * getCommand(const string & cmdStr) {
        if (cmdStr.empty()) return new NoOperation;

        if (!cmdStr.compare(0, 3, "SET")) {
            size_t index = 3;
            Parser parser;

            string var = parser.getName(cmdStr, index);
            long value = parser.getNumber(cmdStr, index);
            return new Set(var, value);
        }
        else if (!cmdStr.compare(0, 5, "UNSET")) {
            size_t index = 5;
            Parser parser;

            string var = parser.getName(cmdStr, index);
            return new UnSet(var);
        }
        else if (!cmdStr.compare(0, 3, "GET")) {
            size_t index = 3;
            Parser parser;

            string var = parser.getName(cmdStr, index);
            return new Get(var);
        }
        else if (!cmdStr.compare(0, 10, "NUMEQUALTO")) {
            size_t index = 10;
            Parser parser;

            long value = parser.getNumber(cmdStr, index);
            return new NumEqualTo(value);
        }
        else if (!cmdStr.compare(0, 5, "BEGIN")) {
            return new Begin;
        }
        else if (!cmdStr.compare(0, 8, "ROLLBACK")) {
            return new Rollback;
        }
        else if (!cmdStr.compare(0, 6, "COMMIT")) {
            return new Commit;
        }
        else if (!cmdStr.compare(0, 3, "END")) {
            return new End;
        }
        return nullptr;
    }
};

CommandFactory * CommandFactory::fFactory = nullptr;

class NoTransactionException : public std::runtime_error {
public:
    NoTransactionException() : runtime_error("no more transaction.") {}
    virtual const char* what() const throw() {
        return runtime_error::what();
    }
};

template<typename K>
class Undo
{
public:
    struct UndoValue {
        UndoValue(long value, bool valid) : fUndoValue(value), fValid(valid) {}
        long fUndoValue;
        bool fValid;
    };
    typedef unordered_map< K, UndoValue > Transaction;

    virtual void begin() = 0;
    virtual void set(const K & var) = 0;
    virtual void doUndo() = 0;
    virtual void reset() = 0;
};

template<typename K>
class UndoImpl : public Undo<K>
{
    Database * fDatabase;
    stack<Transaction> fTransactions;
public:
    UndoImpl(Database * db) : fDatabase(db) {}

    virtual void begin() {
        // create a new transaction.
        Transaction t;
        fTransactions.push(t);
    }

    virtual void set(const K & var) {
        // save the history data in the current transaction.
        if (fTransactions.empty())
            return;

        Transaction & t = fTransactions.top();
        auto it = t.find(var);
        if (it == t.end()) // first time to set the var during the current transaction. Then remember the value in DB before change.
        {
            long oldValueBeforeChange;
            if (fDatabase->get(var, oldValueBeforeChange)) {
                t.insert(make_pair(var, UndoValue(oldValueBeforeChange, true))); // I made it too complicated. But really just calling insert is fine though. Because insert won't let the data come in if duplicate key.
            }
            else { // variable did not exist. Then put null. So later we can revert back to null.
                t.insert(make_pair(var, UndoValue(-1, false)));
            }
        }
    }

    virtual void doUndo() {
        if (fTransactions.empty())
            throw NoTransactionException();

        // Based on the latest transaction, revert data in database and wipe the latest transaction.
        Transaction & t = fTransactions.top();
        for (auto it = t.begin(); it != t.end(); it++) {
            if (it->second.fValid == false) {
                Command * c = new UnSet(it->first);
                c->run(fDatabase);
                delete c;
            }
            else {
                Command * c = new Set(it->first, it->second.fUndoValue);
                c->run(fDatabase);
                delete c;
            }
        }

        fTransactions.pop();
    }

    virtual void reset() {
        // wipe all existing transactions.
        if (fTransactions.empty())
            throw NoTransactionException();

        while (!fTransactions.empty()) {
            fTransactions.pop();
        }
    }
};

class Database
{
    unique_ptr< DataMap<string /*variable*//*values*/> > fValues;
    unique_ptr< DataMap<long/*values*//*frequency*/> > fFreqs;

    unique_ptr< Undo<string> > fUndoManager;

public:
    Database() : fValues(new ValueMap<string>), fFreqs(new ValueMap<long>), fUndoManager(new UndoImpl<string>(this)) {
    }
    virtual ~Database() {}

    void run(Command * c);
    void interpretAndRun(const string & cmdStr);

    void set(string var, long value);
    void unset(string var);
    bool get(string var, long & outValue);
    long freq(long value);
    Undo<string> * getUndo();
};

int main() {
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    Database db;

    string line;

    string filename;
    std::cin >> filename;

    fstream in(filename);
    if (!in.is_open()) {
        system("pwd");
        system("cd");
        return -1;
    }


    while (!in.eof()) {
        getline(in, line);
        cout << line << endl;

        db.interpretAndRun(line);

    }
    cout << "END OF PROGRAM" << endl;
    return 0;
}



void Database::run(Command * c) {
    c->run(this);
}

// Return if it should continue.
void Database::interpretAndRun(const string & cmdStr) {
    unique_ptr<Command> cmd;
    cmd.reset(CommandFactory::getInstnace()->getCommand(cmdStr));
    run(cmd.get());
}

void Database::set(string var, long value)
{
    // Save value. Change value and maintain num_equals data accordingly.
    long oldValue;
    if (fValues->get(var, oldValue)) {
        fFreqs->decrement(oldValue);
    }
    fValues->set(var, value);
    fFreqs->increment(value);
}

void Database::unset(string var) {
    // delete value. maintain num_equals data accordingly.
    long oldValue;
    if (fValues->get(var, oldValue)) {
        fFreqs->decrement(oldValue);
        fValues->unset(var);
    }
}

bool Database::get(string var, long & outValue)
{
    return fValues->get(var, outValue);
}

long Database::freq(long value)
{
    long freq = 0;
    if (fFreqs->get(value, freq)) {
        return freq;
    }
    return 0;
}

Undo<string> * Database::getUndo() {
    return fUndoManager.get();
}

void Set::run(Database * db) {
    // set DataMap data and save the previous value in history.
    Undo<string> * undo = db->getUndo();
    undo->set(fVar);
    db->set(fVar, fValue);
}

void UnSet::run(Database * db) {
    // unset DataMap data and save the previous value in history.
    Undo<string> * undo = db->getUndo();
    undo->set(fVar);
    db->unset(fVar);
}

void Get::run(Database * db) {
    // get - print value.
    long value;
    if (db->get(fVar, value)) {
        cout << "> " << value << endl;
    }
    else
        cout << "> NULL" << endl;
}

void NumEqualTo::run(Database * db) {
    // print freq.
    long freq = db->freq(fValue);
    cout << "> " << freq << endl;
}

void Begin::run(Database * db) {
    // create history.
    Undo<string> * undo = db->getUndo();
    undo->begin();
}

void NoOperation::run(Database * db) {
}

void Rollback::run(Database * db) {
    // recover DataMap data from the latest history.
    // kill the history.
    try {
        Undo<string> * undo = db->getUndo();
        undo->doUndo();
    }
    catch (const NoTransactionException &) {
        cout << "> NO TRANSACTION" << endl;
    }
}

void Commit::run(Database * db) {
    // wipe history and transaction.
    try {
        Undo<string> * undo = db->getUndo();
        undo->reset();
    }
    catch (const NoTransactionException &) {
        cout << "> NO TRANSACTION" << endl;
    }
}

void End::run(Database * db) {
    exit(0);
}
