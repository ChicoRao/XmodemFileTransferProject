// For purposes of grading, please
//
// 1) clean the project
// 2) add this file, main.cpp, to the src folder in your Ensc351Part2 project.
// 3) rename main() to myMain() in Ensc351Part2.cpp
// 4) and rename Ensc351Part2.cpp to Ensc351Part2-renamed-main.cpp

#include <exception>

int myMain();

int main()
{
    try
    {
    	return myMain();
    }
    catch(std::exception const& e)
    {  /* LOG */
       throw;
    }
    catch(...) // Catch anything else.
    {  /* LOG */
       throw;
    }
}


