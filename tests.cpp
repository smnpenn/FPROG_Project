#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <map>
#include <numeric>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace std;

auto printError = [](const string msg)
{
    cerr << msg << endl;
};

// Read file as Vector
auto readFileAsVector = [](const string &filename)
{
    ifstream file(filename);
    vector<string> content;

    if (file.is_open())
    {
        for_each(istream_iterator<string>(file), istream_iterator<string>(), [&content](const string &word)
        {  
            content.push_back(word); 
        });
    }
    else
    {
        printError("ERR: Unable to open file!");
    }

    return content;
};

auto isDelimiter = [](const string &delimiters, const char input)
{
    return delimiters.find(input) != string::npos;
};

// Tokenize String Into Words
auto tokenizeStringIntoWords = [](const vector<string> input, const string &delimiters)
{
    vector<string> tokens;
    string token;

    for_each(input.begin(), input.end(), [&](auto str)
    {
        for_each(str.begin(), str.end(), [&](auto ch)
        {
            if (isDelimiter(delimiters, ch))
            {
                if (!token.empty())
                {
                    tokens.push_back(token);
                }
                token.clear();
            }
            else
            {
                token += ch;
            }
        });

        // Add the last token if not empty
        if (!token.empty())
        {
            tokens.push_back(token);
        }

        token.clear(); // Clear token for the next string in the input vector
    });
    
    return tokens;
};

// Split into chapters
auto splitIntoChapters = [](const vector<string> bookContent)
{
    vector<vector<string>> chapters;
    vector<string> chapter;
    // flag value to ignore the headers/introduction
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
        } });

    // push_back last chapter
    if (!chapter.empty())
    {
        chapters.push_back(chapter);
    }

    return chapters;
};

// Task 5: Map function for counting occurrences
auto mapCountOccurrences = [](const vector<string> &words) -> vector<pair<string, int>>
{
    vector<pair<string, int>> mappedResults;
    for_each(words.begin(), words.end(), [&](auto word)
    {
        mappedResults.push_back({word, 1});
    });
    return mappedResults;
};

// Task 5: Reduce function for counting occurrences
auto reduceCountOccurrences = [](const vector<pair<string, int>> &mappedResults) -> map<string, int>
{
    map<string, int> result;
    for_each(mappedResults.begin(), mappedResults.end(), [&](auto pair)
    {
        result[pair.first] += pair.second;
    });
    return result;
};

// Task 6: Map function for calculating term density
auto mapCalculateTermDensity = [](const vector<string> &words, const map<string, int> &termCount, const int windowSize, const vector<string> &termList) -> vector<pair<string, double>>
{
    set<string> termSet(termList.begin(), termList.end()); // Convert termList to a set for faster lookup
    vector<pair<string, double>> mappedResults;

    for_each(words.begin(), words.end(), [&](auto word)
    {
        auto termIt = termCount.find(word);
        if (termIt != termCount.end() && termSet.find(word) != termSet.end())
        {
            mappedResults.push_back({word, 1.0 / windowSize});
        }
    });

    return mappedResults;
};

// Task 6: Reduce function for calculating term density
auto reduceCalculateTermDensity = [](const vector<pair<string, double>> &mappedResults) -> map<string, double>
{
    map<string, double> result;
    for_each(mappedResults.begin(), mappedResults.end(), [&](auto pair)
    {
        result[pair.first] += pair.second;
    });
    return result;
};

TEST_CASE("Read file"){

    string filename = "./textfiles/test.txt";

    vector<string> fileContent = readFileAsVector(filename);

    CHECK_EQ(fileContent[0], "Das");
    CHECK_EQ(fileContent[1], "ist");
    CHECK_EQ(fileContent[2], "ein");
    CHECK_EQ(fileContent[3], "Testfile");
    CHECK_EQ(fileContent[4], "Zum");
    CHECK_EQ(fileContent[5], "Testen");
}

TEST_CASE("Tokenize string into words"){
    string delimiters = " ;.!-_+?,\"/()[]*:~'";
    vector<string> text = {"Example, to: showcase!! that special, Characters( are) ignored."};

    vector<string> tokens = tokenizeStringIntoWords(text, delimiters);

    CHECK_EQ(tokens[0], "Example");
    CHECK_EQ(tokens[1], "to");
    CHECK_EQ(tokens[2], "showcase");
    CHECK_EQ(tokens[3], "that");
    CHECK_EQ(tokens[4], "special");
    CHECK_EQ(tokens[5], "Characters");
    CHECK_EQ(tokens[6], "are");
    CHECK_EQ(tokens[7], "ignored");
}

TEST_CASE("Split into chapters"){

    auto chapters = splitIntoChapters(readFileAsVector("./textfiles/war_and_peace.txt"));

    CHECK_EQ(chapters.size(), 365);
}

