#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <map>
#include <numeric>

using namespace std;

const string PEACE_TERMS_PATH = "./textfiles/peace_terms.txt";
const string WAR_TERMS_PATH = "./textfiles/war_terms.txt";
const string WAR_AND_PEACE_PATH = "./textfiles/war_and_peace.txt";

//special characters to ignore
const string delimiters = " ;.!-_+?,\"/()[]*:~'";


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

// Task 4: Filter words
auto filterWords = [](const vector<string>& words, const vector<string>& filterList){
    vector<string> filteredWords;
    // copies all elements in the range [first, last] starting from first and proceeding to last. 
    copy_if(words.begin(), words.end(), back_inserter(filteredWords), [&filterList](const string& word){
        return find(filterList.begin(), filterList.end(), word) != filterList.end();
    });
    return filteredWords;
    // Functional approach weil ich eigentlich nicht den state verändere?
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
auto calculateSingleTermDensity = [](const string& word, const map<string, int>& termCount, const int windowSize) {
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
auto calculateTermDensity = [](const vector<string>& words, const map<string, int>& termCount, const int windowSize) {
    map<string, double> termDensity;

    transform(words.begin(), words.end(), inserter(termDensity, termDensity.begin()),
              [&termCount, windowSize](const string& word) {
                  return make_pair(word, calculateSingleTermDensity(word, termCount, windowSize)); // making the pair for each term with density
              });
 
    return termDensity; // returns map with term and its density
};

// wrapper that combines term count calculation and term density calculation
auto calculateTermDensityWrapper = [](const vector<string>& words, const vector<string>& termList, const int windowSize) {
    map<string, int> termCount = countTermOccurrences(words, termList); // obtain term count
    return calculateTermDensity(words, termCount, windowSize); // return density
};

auto printResults = [](const vector<string>& results){
    int i = 1;
    for_each(results.begin(), results.end(), [&i](auto chapter){
        cout << "Chapter " << i << ": " << chapter << endl;
        ++i;
    });
};

int main()
{
    vector<string> peaceTerms = readFileAsVector(PEACE_TERMS_PATH);
    vector<string> warTerms = readFileAsVector(WAR_TERMS_PATH);
    vector<string> contentBook = readFileAsVector(WAR_AND_PEACE_PATH);

    const int windowSize = 5;

    auto chapters = splitIntoChapters(tokenizeStringIntoWords(contentBook, delimiters));

    // Step 7
    auto warWords = tokenizeStringIntoWords(filterWords(contentBook, warTerms), delimiters);
    auto peaceWords = tokenizeStringIntoWords(filterWords(contentBook, peaceTerms), delimiters);

    // Step 8
    for (const auto& chapter : chapters) {
        auto warTermDensity = calculateTermDensityWrapper(chapter, warTerms, windowSize);
        auto peaceTermDensity = calculateTermDensityWrapper(chapter, peaceTerms, windowSize);
    }

    // Step 9
    vector<string> chapterCategories;

    for(const auto& chapter : chapters){
        auto warTermDensity = calculateTermDensityWrapper(chapter, warTerms, windowSize);
        auto peaceTermDensity = calculateTermDensityWrapper(chapter, peaceTerms, windowSize);

        // calc total density for each war or peace-related shit
        // 0.0 is init value for accumulation
        // [] lambda function takes current accumulated val (acc) and next element in range and sums them up.
        double warDensity = accumulate(warTermDensity.begin(), warTermDensity.end(), 0.0, [](double acc, const auto& pair) { return acc + pair.second; }); 
        double peaceDensity = accumulate(peaceTermDensity.begin(), peaceTermDensity.end(), 0.0, [](double acc, const auto& pair) { return acc + pair.second; });

        if(warDensity > peaceDensity){
            chapterCategories.push_back("war-related");
        } else {
            chapterCategories.push_back("peace-related");
        }
    }

    // Step 10
    printResults(chapterCategories);

    return 0;
}