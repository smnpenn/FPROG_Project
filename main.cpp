#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <map>
#include <numeric>
#include <future>
#include <set>



using namespace std;

struct ChapterResult
{
    int chapterIndex;
    string category;
};

const string PEACE_TERMS_PATH = "./textfiles/peace_terms.txt";
const string WAR_TERMS_PATH = "./textfiles/war_terms.txt";
const string WAR_AND_PEACE_PATH = "./textfiles/war_and_peace.txt";

// special characters to ignore
const string delimiters = " ;.!-_+?,\"/()[]*:~'";

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

/*
auto calculateTermDensity = [](const vector<string> &words, const map<string, int> &termCount, const int windowSize)
{
    const int numThreads = 4; // Adjust threads as needed.
    const size_t chunkSize = words.size() / numThreads;

    vector<future<map<string, double>>> futures;

    for (int i = 0; i < numThreads; ++i)
    {
        auto start = words.begin() + i * chunkSize;
        auto end = (i == numThreads - 1) ? words.end() : start + chunkSize;

        futures.push_back(async([start, end, &termCount, windowSize]()
                                {
            map<string, double> termDensity;

            for (auto it = start; it != end; ++it) {
                const string& word = *it;
                auto termIt = termCount.find(word);
                if (termIt != termCount.end()) {
                    termDensity[word] += 1.0 / windowSize;
                }
            }

            return termDensity; }));
    }

    map<string, double> result;

    for (auto &future : futures)
    {
        const auto &threadResult = future.get();
        for (const auto &pair : threadResult)
        {
            result[pair.first] += pair.second;
        }
    }

    return result;
};

// calculate single term density based on single term and windowSize

auto calculateSingleTermDensity = [](const string &word, const map<string, int> &termCount, const int windowSize)
{
    auto it = termCount.find(word);
    if (it != termCount.end())
    {
        // if term is found in map -> return density of term
        // it->second = occurences divided by windowSize
        return it->second / static_cast<double>(windowSize);
    }
    else
    {
        return 0.0;
    }
};
*/

auto printResults = [](const vector<string> &results)
{
    int i = 1;
    for_each(results.begin(), results.end(), [&i](auto chapter)
    {
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

    // Step 7: Map-reduce for counting occurrences
    std::mutex termCountMutex;       // Declare a mutex for termCount
    std::map<std::string, int> termCount; // Declare termCount map

    auto mappedOccurrences = mapCountOccurrences(contentBook);
    {
        std::lock_guard<std::mutex> lock(termCountMutex); // Lock the mutex during the critical section
        termCount = reduceCountOccurrences(mappedOccurrences);
    }

    std::vector<std::future<ChapterResult>> chapterResults;

    for (size_t i = 0; i < chapters.size(); ++i)
    {
        auto processChapter = [i, &chapters, &termCount, &warTerms, &peaceTerms, windowSize]() -> ChapterResult {
            // Map
            auto mappedWarTermDensity = mapCalculateTermDensity(chapters[i], termCount, windowSize, warTerms);
            auto mappedPeaceTermDensity = mapCalculateTermDensity(chapters[i], termCount, windowSize, peaceTerms);

            // Reduce
            auto warTermDensity = reduceCalculateTermDensity(mappedWarTermDensity);
            auto peaceTermDensity = reduceCalculateTermDensity(mappedPeaceTermDensity);

            double warDensity = std::accumulate(warTermDensity.begin(), warTermDensity.end(), 0.0,
                                               [](double acc, const auto &pair)
                                               { return acc + pair.second; });

            double peaceDensity = std::accumulate(peaceTermDensity.begin(), peaceTermDensity.end(), 0.0,
                                                 [](double acc, const auto &pair)
                                                 { return acc + pair.second; });

            // Decide chapter category
            std::string category = (warDensity > peaceDensity) ? "war-related" : "peace-related";

            return {static_cast<int>(i), category};
        };

        chapterResults.push_back(std::async(std::launch::async, processChapter));
    }

    // Wait for all futures to be ready and collect results
    std::vector<ChapterResult> results;
    for (auto &future : chapterResults)
    {
        results.push_back(future.get());
    }

    // Sort results based on chapter index
    std::sort(results.begin(), results.end(), [](const auto &a, const auto &b) {
        return a.chapterIndex < b.chapterIndex;
    });

    // Print results
    for (const auto &result : results)
    {
        std::cout << "Chapter " << result.chapterIndex + 1 << ": " << result.category << std::endl;
    }

    return 0;
}