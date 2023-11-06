#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

using namespace std;

const string PEACE_TERMS_PATH = "./textfiles/peace_terms.txt";
const string WAR_TERMS_PATH = "./textfiles/war_terms.txt";
const string WAR_AND_PEACE_PATH = "./textfiles/war_and_peace.txt";

auto readFileAsVector = [](const string& filename)
{
    ifstream file(filename);
    vector<string> content;

    if(file.is_open())
    {
        for_each(istream_iterator<string>(file),
                      istream_iterator<string>(),
                      [&content](const string& line) {
                          content.push_back(line);
                      });
    }else
    {
        cerr << "ERR: Unable to open file!" << endl;
    }

    return content;
};

auto tokenizeStringIntoWords = [](const string input)
{

};

int main()
{
    vector<string> contentPeaceTerms = readFileAsVector(PEACE_TERMS_PATH);

    for_each(contentPeaceTerms.begin(), contentPeaceTerms.end(), [](auto line) 
    {
        cout << line << endl;
    });

    return 0;
}