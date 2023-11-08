#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <cstring>

using namespace std;

const string PEACE_TERMS_PATH = "./textfiles/peace_terms.txt";
const string WAR_TERMS_PATH = "./textfiles/war_terms.txt";
const string WAR_AND_PEACE_PATH = "./textfiles/war_and_peace.txt";

//special characters to ignore
const string delimiters = " ;.!-_+?,\"/()[]*:~'";

auto readFileAsVector = [](const string& filename)
{
    ifstream file(filename);
    vector<string> content;

    if(file.is_open())
    {
        for_each(istream_iterator<string>(file), istream_iterator<string>(), [&content](const string& line) 
        {
            content.push_back(line);
        });
    }
    else
    {
        cerr << "ERR: Unable to open file!" << endl;
    }

    return content;
};

auto isDelimiter = [](const string& delimiters, const char input)
{
    return delimiters.find(input) != string::npos;
};

auto tokenizeStringIntoWords = [](const vector<string> input, const string& delimiters)
{
    vector<string> tokens;
    string token;

    //Is this loop ok?
    for (const string& str : input) {
        for (char ch : str) {
            if (isDelimiter(delimiters, ch)) {
                if (!token.empty()) {
                    tokens.push_back(token);
                }
                token.clear();
            } else {
                token += ch;
            }
        }

        // Add the last token if not empty
        if (!token.empty()) {
            tokens.push_back(token);
        }

        token.clear(); // Clear token for the next string in the input vector
    }

    return tokens;
};

auto splitIntoChapters = [](const vector<string> bookContent)
{
    vector<vector<string>> chapters;
    vector<string> chapter;
    //flag value to ignore the headers/introduction
    bool pastHeader = false;

    for_each(bookContent.begin(), bookContent.end(), [&](auto word)
    {
        //check for chapter keyword
        if(strcmp(word.c_str(), "CHAPTER") == 0)
        {
            pastHeader = true;
            //push_back current chapter
            if(!chapter.empty())
            {
                chapters.push_back(chapter);
            }
            chapter.clear();
            chapter.push_back(word);
        }
        else
        {
            //fill current chapter
            if(pastHeader)
            {
                chapter.push_back(word);
            }
        }
    });

    //push_back last chapter
    if (!chapter.empty()) 
    {
        chapters.push_back(chapter);
    }

    return chapters;
};


int main()
{
    vector<string> peaceTerms = readFileAsVector(PEACE_TERMS_PATH);
    vector<string> warTerms = readFileAsVector(WAR_TERMS_PATH);
    vector<string> contentBook = readFileAsVector(WAR_AND_PEACE_PATH);

    auto chapters = splitIntoChapters(tokenizeStringIntoWords(contentBook, delimiters));

    return 0;
}

/*for_each(contentPeaceTerms.begin(), contentPeaceTerms.end(), [](auto line) 
    {
        cout << line << endl;
    });
    */