#include "compressor.h"
#include <utility>
#include <unordered_map>
#include <map>
#include <vector>
#include <algorithm>
// Implement Encode Here if Needed
using namespace std;

template<typename T1, typename T2>
void printDict(unordered_map<T1, T2> dict){
  cout << "Printing Dictionary" << endl;
  for(typename unordered_map<T1, T2>::iterator i = dict.begin(); i != dict.end(); ++i){
    cout << i->first << " : " << i->second << endl;
  }
}

bool tupleCMP(const pair<char, int> &p1, const pair<char, int> &p2){
  return p1.first < p2.first;
}

string BWTEncode(const string &source){
  int length = source.size() + 1;
  char END = '\0';
  string target = source + END;
  vector<string> rotations(length);
  // Build rotation array
  for(int i = 0; i < length; ++i){
    char front = target[0];
    for(int j = 0; j < length-1; ++j){
      target[j] = target[j+1];
    }
    target[length-1] = front;
    rotations[i] = target;
  }

  //Lexico sort
  sort(rotations.begin(), rotations.end());

  //Take the last char from each rotation
  string encoded = "";
  for(vector<string>::iterator it = rotations.begin(); it != rotations.end(); ++it){
    encoded += *(it->end()-1);
  }
  // cout << encoded << endl;
  return encoded;
}

string BWTDecode(const string &source){
  int len = source.length();
  // cout << source << endl;
  vector<pair<char, int> > A(len);
  for(int i = 0; i < len; ++i){
    A[i] = make_pair(source[i], i);
  }

  stable_sort(A.begin(), A.end(), tupleCMP);
  vector<int> N(len);
  for(int i = 0; i < len; ++i){
    N[i] = A[i].second;
  }

  string decoded = "";
  int index = 0;

  for(int i = 0; i < len; ++i){
    index = N[index];
    if(source[index] != '\0'){
      decoded += source[index];
    }
  }
  return decoded;
}

unsigned int Encode::Compress( const string &source, unsigned char *& result ){
  const int MAX_DICT_SIZE = 256;
  result = new unsigned char[2]();
  unsigned int result_size = 2;
  unsigned int result_items = 0;
//  string BWT_source = BWTEncode(source);
  unordered_map<string, unsigned int> dict;
  unsigned int size = source.length();
  string target = source;
  if(size < 25000){
    target = BWTEncode(source);
    ++size;
  }

  // add ASCII charset to dict
  dict.insert(make_pair(string(1, 0), 0));
  for(unsigned short int i = 32; i < 127; ++i){
    dict.insert(make_pair(string(1, i), i));
  }

  string w = "";
  unsigned short int code = 128;

  for(unsigned int i = 0; i < size; ++i){
    string next = string(1, target[i]);

    //Reset the dict
    if(code == MAX_DICT_SIZE){
      dict.clear();
      // add ASCII charset to dict
      dict.insert(make_pair(string(1, 0), 0));
      for(unsigned short int i = 32; i < 127; ++i){
        dict.insert(make_pair(string(1, i), i));
      }
      code = 128;
    }

    unordered_map<string, unsigned int>::iterator found = dict.find(w + next);

    //Item not found
    if(found == dict.end()){
      //Need to increase array size
      if(result_items + 1 == result_size){
        int oldLen = result_size;
        result_size *= 2;
        unsigned char * newArr = new unsigned char[result_size]();
        for(int i = 0; i < oldLen; ++i){
          newArr[i] = result[i];
        }

        delete [] result;
        result = newArr;
      }

      result[result_items++] = dict[w];
      dict.insert(make_pair(w+next, code++));

      w = next;
    }
    else{
      //Exists in dict, try next char
      w += next;
    }
  }

  // Adjust for last character
  if(!w.empty()){
    result[result_items++] = dict[w];
  }
  return result_items;
}

string Encode::Decompress( unsigned char * codedText, unsigned int length ) {
  unsigned short int code = 128;
  const int MAX_DICT_SIZE = 256;
  unordered_map<unsigned char, string> dict;

  // add ASCII charset to dict
  dict.insert(make_pair(0, string(1, 0)));
  for(unsigned short int i = 32; i < 127; ++i){
    dict.insert(make_pair(i, string(1, i)));
  }

  string s = dict[codedText[0]];
  string output = s;
  for(unsigned int i = 1; i < length; ++i){
    string s_prev = s;
    unsigned char currentCode = codedText[i];

    //Reset the dict
    if(code == MAX_DICT_SIZE){
      dict.clear();
      // add ASCII charset to dict
      dict.insert(make_pair(0, string(1, 0)));
      for(unsigned short int i = 32; i < 127; ++i){
        dict.insert(make_pair(i, string(1, i)));
      }
      code = 128;
    }

    unordered_map<unsigned char, string>::iterator found = dict.find(currentCode);

    //Found the item
    if(found != dict.end()){
      s = found->second;
    }else if(currentCode == code){
      s = s_prev + s_prev[0];
    }else{
      cerr << "ERROR" << endl;
      cerr << s << endl;
      cerr << int(currentCode) << endl;
      break;
    }

    output += s;
    dict.insert(make_pair(code++, s_prev + s[0]));
  }


  int size = output.length();
  if(size < 25000){
    output = BWTDecode(output);
  }

  return output;
}
