/*
 * This is written based on : https://www.youtube.com/watch?v=dM6us854Jk0
 * And also : https://www.youtube.com/watch?v=apcCVfXfcqE (This guys is cool ;D)
 */
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <sstream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Change here AND the definition of counterMap to control compress rate
const int windowSz = 2;
ofstream dbgPrint("dbgPrettyPrint.diag");

// Auto indentation
class VecCompressor
{
public:
    VecCompressor() {}
    void putValue(int value)
    {
        value <<= bitPos - 1;
        tempChar += value, bitPos--;
        if (bitPos == 0)
            ret.push_back((char)tempChar), tempChar = 0, bitPos = 8;
    }
    const vector<char> &returnResult()
    {
        ret.push_back((char)tempChar);
        return ret;
    }

private:
    vector<char> ret;
    unsigned char tempChar = 0;
    int bitPos = 8;
};

template <class T>
void printAsBinary(T input)
{
    bitset<64> x(input);
    for (int i = 0; i < 64; i++)
    {
        if (i % 8 == 0 && i != 0)
            cout << ' ';
        cout << x[i];
    }
    //    cout << endl;
}

unsigned long long charToUll(vector<char>::iterator start, vector<char>::iterator end)
{
    unsigned long long ret = 0L;
    for (auto it = start; it != end - 1; it++)
    {
        ret += (unsigned char)*it, ret <<= 8;
    }
    ret += (unsigned char)*(end - 1);
    return ret;
}

vector<char> ullToChar(unsigned long long in)
{
    vector<char> ret(8);
    for (int i = 0; i < 8; i++)
    {
        ret[i] = (char)(in & 0b11111111);
        in >>= 8;
    }
    reverse(ret.begin(), ret.end());
    return ret;
}

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

class HuffmanEncoder
{
public:
    explicit HuffmanEncoder(const vector<char> &unprocessedArray)
        : unprocessedStream(unprocessedArray)
    {
    }

    void putMap()
    {
        for (int i = 0; i < 10; i++)
        {
            cout << (int)(unsigned char)unprocessedStream[i] << ' ';
        }
        cout << endl;
        cout << "<-- putMap Begin --> " << endl;
        int windowSzTrack = 0;
        unsigned long long ret = 0L;
        int tracer = 0;
        for (const char &i : unprocessedStream)
        {
            if (tracer < 10)
            {
                cerr << (int)(unsigned char)i << ' ';
                tracer++;
            }
            if (windowSzTrack < windowSz - 1)
            {
                ret += (unsigned char)i, ret <<= 8;
                windowSzTrack++;
            }
            else
            {
                ret += (unsigned char)i;
                if (tracer < 10)
                {
                    cerr << " is " << ret << ' ';
                }
                counterMap[ret]++, ret = 0, windowSzTrack = 0;
            }
        }
        if (ret)
            counterMap[ret]++;

        //        cerr << "=============" << endl;

        cout << "<-- putMap End --> " << endl;
    }

    Node *buildTree()
    {
        auto cmpIntMinHeap = [](const pair<int, int> &lhs, const pair<int, int> &rhs)
        {
            return lhs.second > rhs.second;
        };
        priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(cmpIntMinHeap)> pqii(
            cmpIntMinHeap); // create min heap to store data in counterMap
        for (const auto iteratorTuple : counterMap)
            pqii.push(iteratorTuple);
        auto cmpNodeMinHeap = [](const Node *lhs, const Node *rhs)
        { return lhs->val > rhs->val; };
        priority_queue<Node *, vector<Node *>, decltype(cmpNodeMinHeap)> pqNode(cmpNodeMinHeap);
        Node *currentMax = nullptr;
        // Code Block here, so you don't mess up with global variables
        {
            auto top = pqii.top();
            pqii.pop();
            auto top2 = pqii.top();
            pqii.pop();
            Node *LHSNode = new Node(true, top.first);
            Node *RHSNode = new Node(true, top2.first);
            Node *newNode = new Node(false, top.second + top2.second, LHSNode, RHSNode);
            pqNode.push(newNode);
            // Maintain a max node to check if we need to build a new Node
            currentMax = newNode;
        }

        while (!pqii.empty())
        {
            auto top = pqii.top();
            pqii.pop();
            // If current item's count is larger than the count of the largest node, we want to create a new node to store it.
            if (currentMax->val > top.second && !pqii.empty())
            {
                auto top2 = pqii.top();
                pqii.pop();
                Node *LHSNode = new Node(true, top.first);
                Node *RHSNode = new Node(true, top2.first);
                Node *newNode = new Node(false, top.second + top2.second, LHSNode, RHSNode);
                pqNode.push(newNode);
                if (currentMax->val < newNode->val)
                {
                    currentMax = newNode;
                }
            }
            else
            {
                Node *LHSNode = new Node(true, top.first);
                Node *parentNodeOfBoth = new Node(false, pqNode.top()->val + top.second, LHSNode, pqNode.top());
                // As we need to combine all trees later, this step is to remove all trees that already have a parent.
                pqNode.pop();
                pqNode.push(parentNodeOfBoth);
                if (currentMax->val < parentNodeOfBoth->val)
                {
                    currentMax = parentNodeOfBoth;
                }
            }
        }
        //
        while (!pqNode.empty() && pqNode.size() != 1)
        {
            auto top1 = pqNode.top();
            pqNode.pop();
            auto top2 = pqNode.top();
            pqNode.pop();
            Node *parentNode = new Node(false, top1->val + top2->val, top1, top2);
            pqNode.push(parentNode);
        }
        return pqNode.top();
    }

    void print2DUtil(Node *root, int space)
    {
        // Base case
        if (root == nullptr)
            return;

        space += 2;

        print2DUtil(root->right, space);

        dbgPrint << endl;
        for (int i = 2; i < space; i++)
            dbgPrint << "  ";
        dbgPrint << root->val << "\n";

        print2DUtil(root->left, space);
    }

    // Wrapper over print2DUtil()
    void print2D(Node *root)
    {
        dbgPrint << "Sum of all Nodes: " << root->val << endl;
        // Pass initial space count as 0
        print2DUtil(root, 0);
    }

    void _getTreeContent(Node *currentNode, const string &nodePath, int depth)
    {
        if (currentNode->flag)
        {
            encodeCodeMap[currentNode->val] = nodePath;
            //            encodeTreeMaxDepth = max(encodeTreeMaxDepth, depth);
            return;
        }
        _getTreeContent(currentNode->left, nodePath + "0", depth + 1);
        _getTreeContent(currentNode->right, nodePath + "1", depth + 1);
    }
    // Wrapper over _getTreeContent
    void getTreeContent()
    {
        _getTreeContent(this->head, "", 1);
        //        assert(encodeTreeMaxDepth != -1 && "encodeTreeMaxDepth is still -1, what the fuck?");
    }

    // Read every $windowsSz window and output a result in the `encodedOutput` vec
    vector<char> substitute()
    {
        for (int i = 10; i > 0; i--)
            cout << (int)(unsigned char)*(unprocessedStream.end() - i) << ' ';
        VecCompressor vecCompressor;
        int windowSzTrack = 0;
        unsigned long long ret = 0L;
        int tracer = 0;
        int thisIsAVeryLongValueSoIDontFuckUp = 0;
        for (const char &i : unprocessedStream)
        {
            if (tracer < 10)
            {
                //                cout << (int)(unsigned char)i << ' ';
                tracer++;
            }
            if (windowSzTrack < windowSz - 1)
            {
                ret += (unsigned char)i, ret <<= 8;
                windowSzTrack++;
            }
            else
            {
                ret += (unsigned char)i;
                // ret is the current window's value
                //                cerr << ret << ' ';
                assert(encodeCodeMap.count(ret) && "WTF where is my content?");
                auto encodeCode = encodeCodeMap[ret];
                for (auto &a : encodeCode)
                {
                    if (thisIsAVeryLongValueSoIDontFuckUp < 10)
                        cout << '[' << a << ']' << ' ', thisIsAVeryLongValueSoIDontFuckUp++;
                    vecCompressor.putValue(a - '0');
                }
                ret = 0, windowSzTrack = 0;
            }
        }
        if (ret)
        {
            auto encodeCode = encodeCodeMap[ret];
            cout << encodeCode << endl;
            for (auto &a : encodeCode)
            {
                vecCompressor.putValue(a - '0');
            }
        }
        auto result = vecCompressor.returnResult();
        cout << "<->" << endl;
        for (int i = 10; i > 0; i--)
            cout << (int)(unsigned char)*(result.end()-i) << ' ';
        cout << endl;
        return result;
    }

    vector<char> compress()
    {
        //        for (int i = 0; i < 10; i++) {
        //            cout << (int)(unsigned char)unprocessedStream[i] << ' ';
        //        }
        //        cout << endl;
        putMap();
        ofstream dbgFile("dbg.log");
        for (auto &p : counterMap)
            dbgFile << p.first << ' ' << p.second << endl;
        this->head = buildTree();
        //        print2D(this->head);
        getTreeContent();
        ofstream encodeCodeMapDBG;
        encodeCodeMapDBG.open("encodeCodeMapDbg.log");
        //        encodeCodeMapDBG << "Max depth of tree: " << encodeTreeMaxDepth << endl;
        for (auto &p : encodeCodeMap)
        {
            encodeCodeMapDBG << p.first << ' ' << p.second << endl;
            //     // Print encode path as binary: L is 0, R is 1
            //     //            bitset<64> x(p.second);
            //     //            for (int i = 0; i < 64; i++) {
            //     //                if (i % 8 == 0 && i != 0)
            //     //                    encodeCodeMapDBG << ' ';
            //     //                encodeCodeMapDBG << x[i];
            //     //            }
            //     //            encodeCodeMapDBG << endl;
        }
        //        indentConst = ((int)encodeTreeMaxDepth / 8) + 1;
        storeMap();
        vector<char> result = substitute();
        // for (int i = 0; i < 10; i++)
        //     cout << '*' << (int)(unsigned char)result[i] << '*' << ' ';
        return result;
    }

    vector<char> storeMap()
    {
        vector<char> encodeMapResultOutput;
        unsigned short count = 0;
        for (const auto &p : encodeCodeMap)
        {
            if ((int)p.first < count)
                cout << "Oops " << p.first << ' ' << count << endl;
            count = p.first;
            assert(count <= 65535 && count >= 0);
            char *dataCount = reinterpret_cast<char *>(&count);

            unsigned char prefix_zero = 0;
            for (const auto &c : p.second)
            {
                if (c == '0')
                    prefix_zero++;
                else
                    break;
            }

            encodeMapResultOutput.insert(encodeMapResultOutput.end(), dataCount, dataCount + sizeof(unsigned short));
            string currentString = p.second;
            int currentResult = 0;
            for (char i : currentString)
            {
                currentResult <<= 1, currentResult += (i - '0');
            }

            encodeMapResultOutput.push_back((char)prefix_zero);
            char *data = reinterpret_cast<char *>(&currentResult);
            encodeMapResultOutput.insert(encodeMapResultOutput.end(), data, data + 4);
            auto *returnBackDebug = reinterpret_cast<int *>(data);
            assert(currentString.size() < 32);
            assert(currentResult == *returnBackDebug);
        }
        ofstream outfile("encodeMapResultOutput.bin", ios::out | ios::binary);
        outfile.write(&encodeMapResultOutput[0], encodeMapResultOutput.size());
        return encodeMapResultOutput;
    }

    void saveAsFile(const string &fileName)
    {
        ofstream outfile(fileName, ios::out | ios::binary);
        vector<char> outputBufferVec;
        auto mapResult = storeMap();
        unsigned int lengthOfMapResult = mapResult.size();
        char *lengthOfMapResultData = reinterpret_cast<char *>(&lengthOfMapResult);
        outputBufferVec.insert(outputBufferVec.end(), lengthOfMapResultData, lengthOfMapResultData + sizeof(int));
        outputBufferVec.insert(outputBufferVec.end(), mapResult.begin(), mapResult.end());
        auto literal = compress();
        outputBufferVec.insert(outputBufferVec.end(), literal.begin(), literal.end());
        cout << "Map Result: " << lengthOfMapResult << " Literal Len: " << literal.size() << endl;
        outfile.write(&outputBufferVec[0], outputBufferVec.size());
    }

    vector<char> outputWholeVec()
    {
        vector<char> outputBufferVec;
        auto mapResult = storeMap();
        int lengthOfMapResult = mapResult.size();
        char *lengthOfMapResultData = reinterpret_cast<char *>(&lengthOfMapResult);
        outputBufferVec.insert(outputBufferVec.end(), lengthOfMapResultData, lengthOfMapResultData + sizeof(int));
        outputBufferVec.insert(outputBufferVec.end(), mapResult.begin(), mapResult.end());
        auto literal = compress();
        outputBufferVec.insert(outputBufferVec.end(), literal.begin(), literal.end());
        cout << "Map Result: " << lengthOfMapResult << " Literal Len: " << literal.size() << endl;
        return outputBufferVec;
    }

private:
    const vector<char> unprocessedStream;
    map<unsigned long long, unsigned long long> counterMap; // track occurrence of each element in window
    Node *head = nullptr;
    map<unsigned long long, string> encodeCodeMap;
    //    int encodeTreeMaxDepth = -1;

    // Sequence output
    //    vector<char>& encodedOutput;
};

// int main()
// {
//     ifstream inStream("huffmanC.obj", ios::in | ios::binary);
//     std::vector<char> inputVecUnprocessed((std::istreambuf_iterator<char>(inStream)), istreambuf_iterator<char>());
//     // inputVecUnprocessed.reserve(inStream.tellg());

//     // std::copy(istream_iterator<char>(inStream), istream_iterator<char>(),
//     //           back_inserter(inputVecUnprocessed));

//     HuffmanEncoder huffmanEncoder(inputVecUnprocessed);
//     auto result = huffmanEncoder.compress();
//     auto resultVector = huffmanEncoder.outputWholeVec();

//     ofstream outStream("outStream.obj", ios::binary | ios::out);
//     outStream.write((char *)&resultVector[0], resultVector.size());

//     //    ofstream outDecompStream("outDecomped.obj", ios::binary | ios::out);
//     //    outDecompStream.write((char*)&deCompedRes[0], deCompedRes.size());
// }
