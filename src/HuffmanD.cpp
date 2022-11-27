#include <bits/stdc++.h>

using namespace std;
using namespace std::chrono;

class Node
{
public:
    Node(bool flag, unsigned long long int val, Node *left, Node *right)
        : flag(flag), val(val), left(left), right(right)
    {
    }

    Node(bool flag, unsigned long long int val)
        : flag(flag), val(val)
    {
    }

public:
    bool flag;              // mark if it's end node, 1 if it's end node, 0 is not. REMOVABLE?
    unsigned long long val; // node val
    Node *left = nullptr;
    Node *right = nullptr;
};

class HuffmanDecoder
{
private:
    vector<char> inputStream;
    Node *head = new Node(false, NULL);

public:
    explicit HuffmanDecoder(const vector<char> &inputStream)
        : inputStream(inputStream)
    {
    }

    pair<vector<char>&, vector<char>&> splitContent()
    {
        // Length of first section
        int *lengthOfEncodeMapPtr = reinterpret_cast<int *>(&inputStream[0]);
        int lengthOfEncodeMap = *lengthOfEncodeMapPtr;
        // Remove header
        inputStream.erase(inputStream.begin(), inputStream.begin() + 4);
        // Dictionary
        vector<char> encodeCodeMapVector(inputStream.begin(), inputStream.begin() + lengthOfEncodeMap);
        inputStream.erase(inputStream.begin(), inputStream.begin() + lengthOfEncodeMap);
        // inputStream: Encoded data
        return {encodeCodeMapVector, inputStream};
    }

    map<unsigned long long, string>& getEncodeMapTree(vector<char>& original)
    {
        // assert(original.size() % 7 == 0 && "Bruh you fed me the WRONG VECTOR");
        map<unsigned long long, string> ret;
        // [01010101 0101]

        for (int i = 0; i < original.size(); i += 7)
        {
            auto *originalCode = reinterpret_cast<unsigned short *>(&original[i]);
            char prefix_zeros = original[i + 2];
            auto *outputStringCode = reinterpret_cast<int *>(&original[i + 3]);
            int outputStringCodeLiteral = *outputStringCode;
            string result;

            while (outputStringCodeLiteral)
            {
                result.push_back((outputStringCodeLiteral & 1) + '0');
                outputStringCodeLiteral >>= 1;
            }
            for (int j = 0; j < prefix_zeros; j++)
                result.push_back('0');
            reverse(result.begin(), result.end());
            ret[(unsigned long long)*originalCode] = result;
        }

        // while (!original.empty())
        // {
        //     vector<char> currentBuffer(original.begin(), original.begin() + 7);
        //     original.erase(original.begin(), original.begin() + 7);

        //     auto *originalCode = reinterpret_cast<unsigned short *>(&currentBuffer[0]);
        //     char prefix_zeros = currentBuffer[2];
        //     auto *outputStringCode = reinterpret_cast<int *>(&currentBuffer[3]);
        //     int outputStringCodeLiteral = *outputStringCode;
        //     string result;
        //     while (outputStringCodeLiteral)
        //     {
        //         // result.insert(result.begin(), ((outputStringCodeLiteral & 1) + '0'));
        //         result.push_back((outputStringCodeLiteral & 1) + '0');
        //         outputStringCodeLiteral >>= 1;
        //     }
        //     for (int i = 0; i < prefix_zeros; i++)
        //         result.push_back('0');
        //     reverse(result.begin(), result.end());
        //     ret[(unsigned long long)*originalCode] = result;
        // }
        return ret;
    }

    void _rebuildTree(Node *currentNode, const string &currentRoute, const unsigned long long &finalValue)
    {
        if (currentRoute.empty())
        {
            assert(currentNode->val == NULL);
            currentNode->flag = true, currentNode->val = finalValue;
            return;
        }
        string newRoute(currentRoute.begin() + 1, currentRoute.end());
        assert(newRoute != currentRoute && &newRoute != &currentRoute);
        if (currentRoute[0] - '0')
        {
            if (currentNode->right == nullptr)
            {
                Node *newNode = new Node(false, NULL);
                currentNode->right = newNode;
                _rebuildTree(currentNode->right, newRoute, finalValue);
            }
            else
                _rebuildTree(currentNode->right, newRoute, finalValue);
        }
        else
        {
            if (currentNode->left == nullptr)
            {
                Node *newNode = new Node(false, NULL);
                currentNode->left = newNode;
                _rebuildTree(currentNode->left, newRoute, finalValue);
            }
            else
                _rebuildTree(currentNode->left, newRoute, finalValue);
        }
    }

    void rebuildTree(const map<unsigned long long, string> &inputMap)
    {
        for (const auto &p : inputMap)
        {
            _rebuildTree(head, p.second, p.first);

            Node *currentHead = head;
            // '1' - '0' = 1
            // for (int i = 0; i<p.second.length(); i++)
            for (auto c : p.second)
            {
                if (c - '0')
                    currentHead = currentHead->right;
                else
                    currentHead = currentHead->left;

                assert(currentHead != nullptr);
            }
            assert(currentHead != nullptr && currentHead->val == p.first);
        }
    }

    vector<char>& literal(vector<char> &input) const
    {

        vector<char> ret;

        // optimize
        // char[0b10101010] -> int[1 0 1 0 1 0 1 0]
        // vector<bool> inputButBool;
        // for (auto a : input)
        // {
        //     auto currentU8 = (unsigned char)*reinterpret_cast<char *>(&a);
        //     for (int i = 0; i < 8; i++)
        //     {
        //         inputButBool.push_back(currentU8 & 0b10000000), currentU8 <<= 1;
        //     }
        // }
        // Node *currentHead = head;
        // for (const auto &a : inputButBool)
        // {
        //     if (a)
        //         currentHead = currentHead->right;
        //     else
        //         currentHead = currentHead->left;
        //     if (currentHead->flag)
        //     {
        //         auto result = currentHead->val;
        //         ret.push_back((char)(unsigned char)((result & 0xFF00) >> 8));
        //         ret.push_back((char)(unsigned char)(result & 0xFF));
        //         currentHead = head;
        //     }
        // }
        Node *currentHead = head;
        for (auto &a : input)
        {
            auto currentU8 = (unsigned char)*reinterpret_cast<char *>(&a);
            for (int i = 0; i < 8; i++)
            {
                const bool a = (currentU8 & (1 << (7 - i)));
                if (a)
                    currentHead = currentHead->right;
                else
                    currentHead = currentHead->left;
                if (currentHead->flag)
                {
                    auto result = currentHead->val;
                    ret.push_back((char)(unsigned char)((result & 0xFF00) >> 8));
                    ret.push_back((char)(unsigned char)(result & 0xFF));
                    currentHead = head;
                }
            }
        }

        return ret;
    }

    vector<char>& decompress()
    {

        auto splittedContent = splitContent();

        // multithread
        // ~70ms
        auto encodeMapTree = getEncodeMapTree(splittedContent.first);

        rebuildTree(encodeMapTree);

        // 37
        auto result = literal(splittedContent.second);
        if (result.back() == 0)
            result.pop_back();
        return result;
    }
};

// int main(int argc, char *argv[])
// {

//     // Read in

//     string input_location = "outStream.obj", output_location = "rebuildStream.obj";
//     for (int i = 0; i < argc; i++)
//     {
//         if (!strcmp(argv[i], "-i"))
//             input_location = argv[++i];
//         else if (!strcmp(argv[i], "-o"))
//             output_location = argv[++i];
//     }

//     ifstream inStream(input_location, ios::in | ios::binary);
//     std::vector<char> inputFile((std::istreambuf_iterator<char>(inStream)), istreambuf_iterator<char>());

//     // start

//     HuffmanDecoder huffmanDecoder(inputFile);
//     auto result = huffmanDecoder.decompress();
//     ofstream rebuildStream(output_location, ios::binary | ios::out);
//     rebuildStream.write((char *)&result[0], result.size());

//     return 0;
// }