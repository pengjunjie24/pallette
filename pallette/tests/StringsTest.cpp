
#include <iostream>
#include <pallette/Strings.h>
using namespace pallette;
using namespace std;

void regexSplitTest(string testInStr, string matchStr)
{
    cout << "testInStr: " << testInStr << endl;
    cout << "match " << matchStr << endl;
    vector<std::string> vecStr;
    utils::regexSplit(vecStr, testInStr, matchStr);
    for (auto value : vecStr)
    {
        cout << value << endl;
    }
    cout << "vecStr size: " << vecStr.size() << endl;
    cout << endl;
}

int main()
{
    string testInStr = "I am student, I you are teacher!";

    string matchStr = "I am";
    regexSplitTest(testInStr, matchStr);
    matchStr = "!";
    regexSplitTest(testInStr, matchStr);
    matchStr = "a!";
    regexSplitTest(testInStr, matchStr);
    matchStr = "e";
    regexSplitTest(testInStr, matchStr);
    matchStr = testInStr;
    regexSplitTest(testInStr, matchStr);

    string formatString;
    utils::stringFormat(formatString, "aa %d bb%c cc%d ee%f ff%s",
        1, 'c', 3, 3.14, "who are you");
    cout << formatString << endl;

    return 0;
}
