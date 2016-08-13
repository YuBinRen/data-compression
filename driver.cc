#include "compressor.h"

using namespace std;

int main() {
  Encode en = Encode();

  unsigned char * result = NULL;
 
  string source2 = "";
  getline(cin, source2);
  unsigned int len = en.Compress(source2, result);
  cout << "Starting Length: " << source2.length() << endl;
  cout << "Compressed Length: " << len << endl;
  
  string output = en.Decompress(result, len);
//  cout << source2 << endl;
//  cout << output << endl;
  cout << ((output == source2) ? "Good" : "Bad") << endl;



  delete [] result;
}
