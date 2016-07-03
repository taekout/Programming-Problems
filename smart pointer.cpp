// code a smart pointer class.
#include <iostream>
#include <exception>
#include <atomic>

using namespace std;

template<typename T>
class SmartPointer
{
	T * fPtr;
	long * fCount;

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
		if (*fCount == 1 && fPtr) {
			delete fPtr;
		}
		(*fCount)--;
	}


	T * operator =(const T * r) {
		return r;
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
	}

	return 0;
}