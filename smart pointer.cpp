// code a smart pointer class.
#include <iostream>
#include <exception>
//#include <atomic> ==> This class is not thread-safe. So I should use atomic long for fCount.

using namespace std;

template<typename T>
class SmartPointer
{
    T * fPtr;
    long * fCount;

protected:
    void remove() {
        (*fCount)--;
        if (*fCount == 0 && fPtr) {
            delete fPtr;
            delete fCount;
            fCount = nullptr;
            fPtr = nullptr;
            cout << "deleted" << endl;
        }
    }

public:

    SmartPointer(T * ptr) {
        if (ptr == nullptr) throw "pointer is null.";

        fPtr = ptr;
        fCount = new long(1);
    }

    SmartPointer(const SmartPointer & r) {
        fCount = r.fCount;
        fPtr = r.fPtr;
        (*fCount)++;
    }

    virtual ~SmartPointer() {
        remove();
    }

    SmartPointer<T> & operator = (const SmartPointer<T> & r) {
        if (r == this) return *this;

        fPtr = r.fPtr;
        fCount = r.fCount;
        fCount++;
        return *this;
    }

    void print() {
        cout << fPtr << " : " << *fCount << endl;
    }
};


int main()
{
    {
        SmartPointer<int> sp(new int(1));
        SmartPointer<int> sp2 = sp;
        SmartPointer<int> sp3 = new int(3);

        sp.print();
        sp2.print();
        sp3.print();
    }

    return 0;
}