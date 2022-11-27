// #include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <tuple>
#include <string>
#include <utility>
#include <cstdint>
#include <cstring>
#include <map>
// header for read in using C
#include <stdio.h>
#include <stdlib.h>
#include "serialize.h"
// header for compression 
#include "LZSS_Compression.h"
#include "HuffmanC.cpp"
using namespace std;
using namespace jls;

string input_location, output_location;

vector<ubyte> res;

vector<node_v> vs;

vector<node_vt> vts;
vector<node_vn> vns;
vector<node_f> fs;

/* ubyte -> numbers of occurence of that input */
map<ubyte, int> input_pro;
const bool debug_pro=false;
extern bool Compress_Twice;
/**
 * @brief Reads in a 6 decimal place float as string and returns an integer that is exactly that float * 1e6.
 *
 * @param str the string representing the float.
 * @return int
 */
int strfloat_mult1M_to_int(string str) {
    int ret = 0;
    int n = str.length();
    int pos_of_dp = str.find('.');
    if (n - pos_of_dp != 7) {
        cerr << "WARNING: Encountered non-6-dp number: " << str << endl;
    }
    for (int i = 0; i < n; i++) {
        if (isdigit(str[i])) {
            ret = ret * 10 + (str[i] - '0');
        }
    }
    if (str[0] == '-') {
        ret *= -1;
    }
    return ret;
}

/**
 * @brief Converts a face string ("[integer]/[integer]/[integer]") and returns a tuple of those three integers.
 *
 * @param str
 * @return tiii
 */
tiii face_to_tuple(string str) {
    int p1 = str.find('/');
    int a = stoi(str.substr(0, p1));
    int p2 = str.find('/', p1 + 1);

    int b;
    if (p2 - p1 > 1) {
        b = stoi(str.substr(p1 + 1, p2 - p1 - 1));
    }
    else {
        b = 0;
    }
    int c = stoi(str.substr(p2 + 1));
    return make_tuple(a, b, c);
}

void readin() {
    ifstream fin(input_location);
    string node_type;
    while (fin >> node_type) {
        if (node_type == "v") {
            string s1, s2, s3;
            fin >> s1 >> s2 >> s3;
            vs.push_back(node_v(
                strfloat_mult1M_to_int(s1),
                strfloat_mult1M_to_int(s2),
                strfloat_mult1M_to_int(s3)));
        }
        else if (node_type == "vn") {
            string s1, s2, s3;
            fin >> s1 >> s2 >> s3;
            vns.push_back(node_vn(
                strfloat_mult1M_to_int(s1),
                strfloat_mult1M_to_int(s2),
                strfloat_mult1M_to_int(s3)));
        }
        else if (node_type == "vt") {
            string s1, s2;
            fin >> s1 >> s2;
            vts.push_back(node_vt(
                strfloat_mult1M_to_int(s1),
                strfloat_mult1M_to_int(s2)));
        }
        else if (node_type == "f") {
            string s1, s2, s3;
            fin >> s1 >> s2 >> s3;
            fs.push_back(node_f(
                face_to_tuple(s1),
                face_to_tuple(s2),
                face_to_tuple(s3)));
        }
    }
    fin.close();
}

void obj_output(string outfile = "") {
    if (outfile == "") {
        outfile = output_location + ".obj";
    }
    ofstream fout(outfile);
    for (auto x : vs) {
        fout << x.dump_output_obj() << "\n";
    }
    for (auto x : vts) {
        fout << x.dump_output_obj() << "\n";
    }
    for (auto x : vns) {
        fout << x.dump_output_obj() << "\n";
    }
    for (auto x : fs) {
        fout << x.dump_output_obj() << "\n";
    }
    fout << flush;
    return;
}

void generate_output() {
    // Add flags
    for (int i = 0; i < FLAG_BYTES; i++) {
        res.push_back(FLAGS[i]);
    }
    push_int(res, NUM_BLOCKS);
    push_int(res, vs.size());
    for (auto v : vs) {
        v.to_stream(res);
    }
    push_int(res, vts.size());
    for (auto vt : vts) {
        vt.to_stream(res);
    }
    push_int(res, vns.size());
    for (auto vn : vns) {
        vn.to_stream(res);
    }
    int nfs = fs.size();
    push_int(res, nfs);
    fs[0].to_stream(res);
    for (int i = 1; i < nfs; i++) {
        delta_f df(fs[i-1], fs[i]);
        df.to_stream(res);
    }
}


void debug_writeout(string outfile = output_location) {
    ofstream bout(outfile, ios::out | ios::binary);
    bout.write((char *)&res[0], res.size());
    bout << flush;
    bout.close();
    return;
}
 
 void remove_duplicate_v() {
    map<node_v, int> v_map; // node_vn -> id of first occurrence
    vector<int> id_map(vs.size()); // id of node_vn in old array -> id in new array
    vector<node_v> new_vs;
    for (int i = 0; i < (int)vs.size(); i++) {
        const node_v &v = vs[i];
        auto iter = v_map.find(v);
        int new_ind = 0;
        if (iter == v_map.end()) {
            v_map[v] = new_ind = new_vs.size();
            new_vs.push_back(v);
        } else {
            new_ind = iter -> second;
        }
        id_map[i] = new_ind;
    }
    for (int j = 0; j < (int)fs.size(); j++) {
        node_f &f = fs[j];
        for (int k = 0; k < 3; k++) {
            f.f[k] = make_tuple(id_map[get<0>(f.f[k])-1]+1, get<1>(f.f[k]), get<2>(f.f[k]));
        }
    }
    vs = new_vs;
}

void remove_duplicate_vt() {
    map<node_vt, int> vt_map; // node_vn -> id of first occurrence
    vector<int> id_map(vts.size()); // id of node_vn in old array -> id in new array
    vector<node_vt> new_vts;
    for (int i = 0; i < (int)vts.size(); i++) {
        const node_vt &vt = vts[i];
        auto iter = vt_map.find(vt);
        int new_ind = 0;
        if (iter == vt_map.end()) {
            vt_map[vt] = new_ind = new_vts.size();
            new_vts.push_back(vt);
        } else {
            new_ind = iter -> second;
        }
        id_map[i] = new_ind;
    }
    for (int j = 0; j < (int)fs.size(); j++) {
        node_f &f = fs[j];
        for (int k = 0; k < 3; k++) {
            if (get<1>(f.f[k]) != 0) {
                f.f[k] = make_tuple(get<0>(f.f[k]), id_map[get<1>(f.f[k])-1]+1, get<2>(f.f[k]));
            }
        }
    }
    vts = new_vts;
}

void remove_duplicate_vn() {
    map<node_vn, int> vn_map; // node_vn -> id of first occurrence
    vector<int> id_map(vns.size()); // id of node_vn in old array -> id in new array
    vector<node_vn> new_vns;
    for (int i = 0; i < (int)vns.size(); i++) {
        const node_vn &vn = vns[i];
        auto iter = vn_map.find(vn);
        int new_ind = 0;
        if (iter == vn_map.end()) {
            vn_map[vn] = new_ind = new_vns.size();
            new_vns.push_back(vn);
        } else {
            new_ind = iter -> second;
        }
        id_map[i] = new_ind;
    }
    for (int j = 0; j < (int)fs.size(); j++) {
        node_f &f = fs[j];
        for (int k = 0; k < 3; k++) {
            f.f[k] = make_tuple(get<0>(f.f[k]), get<1>(f.f[k]), id_map[get<2>(f.f[k])-1]+1);
        }
    }
    vns = new_vns;
}
 void write_chunk_header(FILE* f,  unsigned long long size,  unsigned long long extra) {
  unsigned char buffer[8];
  buffer[0] = size & 255;
  buffer[1] = (size >> 8) & 255;
  buffer[2] = (size >> 16) & 255;
  buffer[3] = (size >> 24) & 255;
 
  buffer[4] = extra & 255;
  buffer[5] = (extra >> 8) & 255;
  buffer[6] = (extra >> 16) & 255;
  buffer[7] = (extra >> 24) & 255;

  fwrite(buffer, 8, 1, f);
}
void write_chunk_header(FILE* f,  unsigned long long size) {
  unsigned char buffer[4];
  buffer[0] = size & 255;
  buffer[1] = (size >> 8) & 255;
  buffer[2] = (size >> 16) & 255;
  buffer[3] = (size >> 24) & 255;

  fwrite(buffer, 4, 1, f);
}
void write_chunk_header(vector<char> &input, unsigned long long size, unsigned long long extra)
{
    unsigned char buffer[8];
    buffer[0] = size & 255;
    buffer[1] = (size >> 8) & 255;
    buffer[2] = (size >> 16) & 255;
    buffer[3] = (size >> 24) & 255;

    buffer[4] = extra & 255;
    buffer[5] = (extra >> 8) & 255;
    buffer[6] = (extra >> 16) & 255;
    buffer[7] = (extra >> 24) & 255;

    // fwrite(buffer, 8, 1, f);
    copy(buffer, buffer + 8, back_inserter(input));
}
void write_chunk_header(vector<char> &input, unsigned long long size)
{
    unsigned char buffer[4];
    buffer[0] = size & 255;
    buffer[1] = (size >> 8) & 255;
    buffer[2] = (size >> 16) & 255;
    buffer[3] = (size >> 24) & 255;

    // fwrite(buffer, 4, 1, f);
    copy(buffer, buffer + 4, back_inserter(input));
}
/**
  Compress the data in the INPUT buffer and returns the size of
  output data. The size of INPUT buffer is specified by length. The
  minimum INPUT buffer size is 16.
*/

void LZSS_Compression(bool Compress_Twice){
    // int compress_level=2; //default using better
    unsigned char* buffer = new unsigned char[res.size()];
    unsigned char* buffer2 = new unsigned char[res.size()];
    unsigned char* buffer3 = new unsigned char[res.size()];
    copy(res.begin(), res.end(), buffer); 
    // FILE *f=fopen("output_location","wb");
    // fwrite(buffer, 1, res.size(), f);
    // fclose(f);
    // long long chunk_size =LZSS_compress_level(compress_level, buffer, res.size(), buffer2);
    FILE *f=fopen(output_location.c_str(),"wb");
    LZSS_Encoder Lzss_Encoder(buffer, res.size(), buffer2);
 
    long long chunk_size =Lzss_Encoder.Compress();
  
    if(Compress_Twice){
         LZSS_Encoder Lzss_Encoder2(buffer2, chunk_size, buffer3);
        // long long chunk_size2 =Compress_LZ(buffer2, chunk_size, buffer3);
         long long chunk_size2 =Lzss_Encoder2.Compress(3);
        write_chunk_header(f, chunk_size2,chunk_size);
        write_chunk_header(f, res.size());
        fwrite(buffer3, 1, chunk_size2, f);
        delete [] buffer3;
    }
    else{
        write_chunk_header(f, chunk_size,res.size() );
        fwrite(buffer2, 1, chunk_size, f);
    }
    fclose(f);   
    // static_cast<void*>(res.data()));
    // debug_writeout();
    // obj_output();
    delete [] buffer;
    delete [] buffer2;
}
void LZSS_Comp_once(){
        // int compress_level=2; //default using better
     unsigned char* buffer = new unsigned char[res.size()];
     unsigned char* buffer2 = new unsigned char[res.size()];
    copy(res.begin(), res.end(), buffer); 
    // FILE *f=fopen("output_location","wb");
    // fwrite(buffer, 1, res.size(), f);
    // fclose(f);
    // long long chunk_size =fastlz_compress_level(compress_level, buffer
    //  , res.size(), buffer2);
    LZSS_Encoder Lzss_Encoder(buffer, res.size(), buffer2);
    //  long long chunk_size =Compress_LZ(buffer
    //  , res.size(), buffer2);
    long long chunk_size =Lzss_Encoder.Compress();
     FILE *f=fopen(output_location.c_str(),"wb");

     write_chunk_header(f, chunk_size,res.size() );
     //cout<<res.size()<<chunk_size<<endl;
     fwrite(buffer2, 1, chunk_size, f);
     fclose(f);   
    // static_cast<void*>(res.data()));
    // debug_writeout();
    // obj_output();

    delete [] buffer;
    delete [] buffer2;
}
void LZSS_Compression_with_Huffman(bool Compress_Twice) 
{
    // int compress_level=2; //default using better
    unsigned char *buffer = new unsigned char[res.size()];
    unsigned char *buffer2 = new unsigned char[res.size()];
    unsigned char *buffer3 = new unsigned char[res.size()];
    copy(res.begin(), res.end(), buffer);
    // FILE *f=fopen("output_location","wb");
    // fwrite(buffer, 1, res.size(), f);
    // fclose(f);
    // long long chunk_size =LZSS_compress_level(compress_level, buffer, res.size(), buffer2);
    // FILE *f = fopen(output_location.c_str(), "wb");
    vector<char> huffIn;
    LZSS_Encoder Lzss_Encoder(buffer, res.size(), buffer2);
     
    long long chunk_size = Lzss_Encoder.Compress();
    // cout<<chunk_size<<endl;
    if (Compress_Twice)
    {
        LZSS_Encoder Lzss_Encoder2(buffer2, chunk_size, buffer3);
     

        // long long chunk_size2 =Compress_LZ(buffer2, chunk_size, buffer3);
        long long chunk_size2 = Lzss_Encoder2.Compress();
        write_chunk_header(huffIn, chunk_size2, chunk_size);
        write_chunk_header(huffIn, res.size());
        // fwrite(buffer3, 1, chunk_size2, f);
        copy(buffer3, buffer3 + chunk_size2, back_inserter(huffIn));
    }
    else
    {
        write_chunk_header(huffIn, chunk_size, res.size());
        // fwrite(buffer2, 1, chunk_size, f);
        copy(buffer2, buffer2 + chunk_size, back_inserter(huffIn));
    }
    // fclose(f);
    // static_cast<void*>(res.data()));
    // debug_writeout();
    // obj_output();
    HuffmanEncoder huffmanEncoder(huffIn);
    auto lengthHuffin = huffIn.size();
    huffmanEncoder.compress();
    vector<char> output = huffmanEncoder.outputWholeVec();
    if (lengthHuffin < output.size())
    {
        huffIn.push_back(1);
        ofstream outStream(output_location, ios::binary | ios::out);
        outStream.write((char *)&huffIn[0], huffIn.size());
    }
    else
    {
        output.push_back(2);
        ofstream outStream(output_location, ios::binary | ios::out);
        outStream.write((char *)&output[0], output.size());
    }
    
}
int main(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-i")) {
            input_location = argv[++i];
        }
        else if (!strcmp(argv[i], "-o")) {
            output_location = argv[++i];
        }
    }
    readin();
    remove_duplicate_v();
    remove_duplicate_vn();
    remove_duplicate_vt();
    generate_output();
    //LZSS_Comp_once();
    LZSS_Compression(Compress_Twice); 
    //LZSS_Compression_with_Huffman(Compress_Twice);
    // debug_writeout();
    return 0;
}