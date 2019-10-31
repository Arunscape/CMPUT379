
/**
 * @file validation.cc
 * @author your name (you@domain.com)
 * @brief This is a validation tool for assignment 2
 * @version 0.1
 * @date 2019-10-09
 * 
 * usage: ./validation <num-files> [user_output].. [correct_output]...
 * @copyright Copyright (c) 2019
 * 
 */
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    cout << "usage: ./validation <num-files> [user_output].. [correct_output]...\n";
    return -1;
  }
  int num_files = atoi(argv[1]);

  string user_input, correct_input;
  size_t total = 0, num_correct = 0;

  for (int i = 2; i < num_files + 2; i++)
  {
    ifstream user_output_file(argv[i]);
    ifstream correct_output_file(argv[i + num_files]);
    if (!user_output_file.is_open())
      cout << argv[i] << " is not open\n";
    if (!correct_output_file.is_open())
      cout << argv[i + num_files] << " is not open\n";

    while (getline(user_output_file, user_input) && getline(correct_output_file, correct_input))
    {
      if (user_input.compare(correct_input) == 0)
      {
        total++;
        num_correct++;
      }
      else
      {
        total++;
        cout << "Input: " << user_input << endl;
        cout << "Correct: " << correct_input << endl;
      }
    }

    while(getline(user_output_file, user_input)){
      cout << "Uh oh, you have extra inputs " << user_input << endl;
      total++;
    }
    while(getline(correct_output_file, correct_input)){
      cout << "Uh oh, you don't have enough inputs " << correct_input << endl;
      total++;
    }

    user_output_file.close();
    correct_output_file.close();
  }
  if (num_correct == total)
  {
    cout << "Congratulation, your mapreduce library is concurrency stable\n";
  }
  else
  {
    cout << "Something went wrong\n Accuracy: " << (float)num_correct / total * 100.0 << "%\n";
  }
  return 0;
}