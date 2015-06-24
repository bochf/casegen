#include <unistd.h>
#include <iostream>

using namespace std;

int main() {
  cout << "start" << endl;
  usleep(1 * 1000 * 1000);
  cout << "end" << endl;

	size_t i = 100;
	size_t j = 1000;
	int x = i - j;
	cout << i << " = " << j << " = " << x << endl;
	x = j - i;
	cout << i << " = " << j << " = " << x << endl;
  return 0;
}
