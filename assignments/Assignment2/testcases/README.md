# The distwc mapreduce test case

**Notes:** Just because your MapReduce library passes all the test cases doesn't necessary mean your library is concurrency stable. Testing for race conditions and concurrency stability is very difficult and it's best to read over your code for correctness.

## How to use
Place your MapReduce and ThreadPool library in the same directory as this README. There should be a Makefile that will only compile the MapReduce and ThreadPool library as an object file. This is so that the different test cases just need to link the library with the driver code and doesn't need to recompile for every test case.

Before running the test cases you need to build your MapReduce and ThreadPool library. Type make in the terminal at this directory location to build your libraries. 
After the libraries have been built you can change directory to one of the 4 different test cases located in this directory. The four test cases are called easy, medium, hard, and the aptly named death-incarnate.
These test cases get progressively more difficult as it give larger and larger files to process and threads to manage. 
Your MapReduce library should be efficient and shouldn't take hours completing the most difficult test case (death-incarnate). 

### Running a test case
Change directory to one of the test cases, you should start with easy first, then move your way up to medium, hard, then finally death-incarnate. Once in the selected test case type 
```bash
./test.sh
```
in the terminal to start the test case. You may need to chmod the bash script in order for it to execute.

Once the MapReduce job is completed it will run the outputted files into a validation program.
The validation program will output either "Congratulation, your mapreduce library is concurrency stable" or give you a diff between your output to the correct output. 
