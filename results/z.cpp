#include <iostream>
using namespace std;

int main()
{
  
  int n1 = 0;
  int n2 = 0;
  for (int i=0; i < 10; i++) {
  //if (i<5) continue;
  //if (i>7) continue;
  if (i>5) {
  //if (i==7) continue;
  n1++;
  } else {
  n2++;
  }
  //cout << i << endl;   
  //cout << "Welcome to C++ Programming" << endl;
  }
  cout << "Total of non-negative intergers that are less than or equal to 5 = " << n2 << endl;
  cout << n1 << endl;

return 0;
}
