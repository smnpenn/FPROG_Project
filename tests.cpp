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

// Read file as Vector
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

// Split into chapters
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

// Tokenize String Into Words
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

/*
// Task 5: Count occurrences
auto countOccurrences = [](const vector<string>& words) -> map<string, int>{
    return accumulate(words.begin(), words.end(), map<string, int>{}, [](auto& accu, const string& word) {
        accu[word]++;
        return accu;
    });
};
*/

// Task 6: Calculate Term Density
// Counts occurences of each word in set words

auto countTermOccurrences = [](const vector<string>& words, const vector<string>& termList) {
    // Checks if each word in vector is present in termList
    return accumulate(words.begin(), words.end(), map<string, int>{}, [&termList](auto& acc, const string& word) {
        if (find(termList.begin(), termList.end(), word) != termList.end()) {
            // if word present ->  increment count for term in map
            acc[word]++;
        }
        return acc;
    });
};

// calculate single term density based on single term and windowSize
auto calculateSingleTermDensity = [](const string& word, const map<string, int>& termCount, int windowSize) {
    auto it = termCount.find(word);
    if (it != termCount.end()) { 
        // if term is found in map -> return density of term
        // it->second = occurences divided by windowSize
        return it->second / static_cast<double>(windowSize);
    } else {
        return 0.0;
    }
};

// calculates term density for each term using singleTermDesnity and store res in termDensity map
auto calculateTermDensity = [](const vector<string>& words, const map<string, int>& termCount, int windowSize) {
    map<string, double> termDensity;

    transform(words.begin(), words.end(), inserter(termDensity, termDensity.begin()),
              [&termCount, windowSize](const string& word) {
                  return make_pair(word, calculateSingleTermDensity(word, termCount, windowSize)); // making the pair for each term with density
              });
 
    return termDensity; // returns map with term and its density
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

TEST_CASE("Count term occurrences"){
    string delimiters = " ;.!-_+?,\"/()[]*:~'";
    vector<string> tokens = tokenizeStringIntoWords({"Hallo das ist ein Hallo und ein Hallo"}, delimiters);
    vector<string> wordList = {"Hallo", "das", "ein"};

    auto wordOcc = countTermOccurrences(tokens, wordList);

    CHECK_EQ(wordOcc["Hallo"], 3);
    CHECK_EQ(wordOcc["das"], 1);
    CHECK_EQ(wordOcc["ein"], 2);
}

TEST_CASE("Calculate term density"){
    string delimiters = " ;.!-_+?,\"/()[]*:~'";
    vector<string> tokens1 = tokenizeStringIntoWords({"Hallo das ist ein Hallo und ein Hallo"}, delimiters);
    vector<string> tokens2 = tokenizeStringIntoWords({"Hallo das hat das nicht das eine so große Density"}, delimiters);
    vector<string> wordList = {"Hallo", "das", "ein"};

    auto tokens1Dens = calculateTermDensity(tokens1, countTermOccurrences(tokens1, wordList), 5);
    auto tokens2Dens = calculateTermDensity(tokens2, countTermOccurrences(tokens2, wordList), 5);

    CHECK_LT(tokens2Dens["Hallo"], tokens1Dens["Hallo"]);
    CHECK_LT(tokens1Dens["das"], tokens2Dens["das"]);
    CHECK_LT(tokens2Dens["ein"], tokens1Dens["ein"]);
}

TEST_CASE("Split into chapters"){

    auto chapters = splitIntoChapters(readFileAsVector("./textfiles/war_and_peace.txt"));

    CHECK_EQ(chapters.size(), 365);
}