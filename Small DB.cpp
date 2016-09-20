#include <map>
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
    virtual void set(const K & var, long value) {
        bool exist = get(var, value);
        if (exist) {
            values.erase(var);
        }
        values.insert(make_pair(var, value));
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
            throw "Cannot decrement what does not exist.";
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


class Undo
{
    queue<Command *> toRevert;
public:
    void add(string var, long);
};

class Transaction
{
    stack<vector<Command>> history;
public:
    void add(queue<Command *> & c);
};


class Database
{
    unique_ptr< DataMap<string /*variable*//*values*/> > fValues;
    unique_ptr< DataMap<long/*values*//*frequency*/> > fFreqs;

public:
    Database() : fValues(new ValueMap<string>), fFreqs(new ValueMap<long>) {
    }
    virtual ~Database() {}

    void run(Command * c);

    // Return if it should continue.
    void interpretAndRun(const string & cmdStr);

    friend class Set;
    friend class UnSet;
    friend class Get;
    friend class NumEqualTo;
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

void Set::run(Database * db) {
    // set DataMap data and save the previous value in history.
    long oldValue;
    if (db->fValues->get(fVar, oldValue)) {
        db->fFreqs->decrement(oldValue);
    }
    db->fValues->set(fVar, fValue);
    db->fFreqs->increment(fValue);
}

void UnSet::run(Database * db) {
    // unset DataMap data and save the previous value in history.
    long oldValue;
    if (db->fValues->get(fVar, oldValue)) {
        db->fFreqs->decrement(oldValue);
        db->fValues->unset(fVar);
    }
    else {
        throw "cannot unset a var that does not exist.";
    }
}

void Get::run(Database * db) {
    // get - print value.
    long value = -1;
    if (db->fValues->get(fVar, value)) {
        cout << "> " << value << endl;
    }
    else
        cout << "> NULL" << endl;
}

void NumEqualTo::run(Database * db) {
    // print freq.
    //> 2
    long freq = -1;
    if (db->fFreqs->get(fValue, freq)) {
        cout << "> " << freq << endl;
    }
    else
        cout << "> 0" << endl;
}

void Begin::run(Database * db) {
    // create history.
}

void NoOperation::run(Database * db) {
}

void Rollback::run(Database * db) {
    // recover DataMap data from the latest history.
    // kill the history.
}

void Commit::run(Database * db) {
    // wipe history and transaction.
}

void End::run(Database * db) {
    exit(0);
}
