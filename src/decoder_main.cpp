#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <tuple>
#include <string>
#include <utility>
#include <cstdint>
#include <cstring>
// header for read in using C
#include <stdio.h>
#include <stdlib.h>

#include "serialize.h"
#include "HuffmanD.cpp"
// header for decompression 
#include <limits>
#include "LZSS_Decompression.h"
//--
using namespace std;
using namespace jls;
// Yes, I know this is bad code. This is for testing so nobody should care.
const bool debug = false;
string debug_output_location; ofstream ferr;

string input_location;
string output_location;
string file_contents;
char *res;
char ores[50000004];
int res_len;
extern bool Compress_Twice;
fixed_byte_stream fbs(NULL, 0);
int fread_bias = 0;
// vector<node_v> vs;
// vector<node_vt> vts;
// vector<node_vn> vns;
// vector<node_f> fs;

/* ubyte -> numbers of occurence of that input */
int input_pro[257];
const int input_pro_size=256;
const bool debug_pro=false;

void readin() {
    ifstream fin(input_location, ios::binary | ios::in);
    file_contents = string(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
    res = file_contents.data();
    res_len = file_contents.length();
    fin.close();
}

void reconstruct_and_output(char *data, int len, string outfile) {
    if (outfile == "") {
        outfile = output_location;
    }
    fbs = fixed_byte_stream((unsigned char*) data, len);
    ofstream fout(outfile);
    ubyte flags[FLAG_BYTES] = {};
    for (int i = 0; i < FLAG_BYTES; i++) {
        fbs >> flags[i];
    }
    int p = 0;
    int nblocks;
    fbs >> nblocks;
    // assert (nblocks == NUM_BLOCKS);
    int nv;
    fbs >> nv;
    for (int i = 0; i < nv; i++) {
        node_v v;
        fbs >> v;
        p += v.write_obj(ores + p);
        ores[p++] = '\n';
    }
    int nvt;
    fbs >> nvt;
    for (int i = 0; i < nvt; i++) {
        node_vt vt;
        fbs >> vt;
        p += vt.write_obj(ores + p);
        ores[p++] = '\n';
    }
    int nvn;
    fbs >> nvn;
    for (int i = 0; i < nvn; i++) {
        node_vn vn;
        fbs >> vn;
        p += vn.write_obj(ores + p);
        ores[p++] = '\n';
    }
    int nf;
    fbs >> nf;
    node_f f1;
    fbs >> f1;
    p += f1.write_obj(ores + p);
    ores[p++] = '\n';
    for (int i = 1; i < nf; i++) {
        delta_f df;
        fbs >> df;
        f1 += df;
        p += f1.write_obj(ores + p);
        ores[p++] = '\n';
    }
    fout.write(ores, p);
    fout.close();
}

void read_chunk_header(FILE* f,  long long* size,
                        long long* extra) {
  unsigned char buffer[8];
  fread(buffer, 1, 8, f);
  *size = read_uint32(buffer ) & 0xffffffff;
  *extra = read_uint32(buffer+4) & 0xffffffff;
}
void read_chunk_header(FILE* f,  long long* size) {
  unsigned char buffer[4];
  fread(buffer, 1, 4, f);
  *size = read_uint32(buffer) & 0xffffffff;
}
void read_chunk_header(vector<char> &input, long long *size,
                       long long *extra)
{
    unsigned char buffer[8];
    // fread(buffer, 1, 8, f);
    copy(input.begin() + fread_bias, input.begin() + fread_bias + 8, buffer);
    fread_bias += 8;
    *size = read_uint32(buffer) & 0xffffffff;
    *extra = read_uint32(buffer + 4) & 0xffffffff;
}
void read_chunk_header(vector<char> &input, long long *size)
{
    unsigned char buffer[4];
    // fread(buffer, 1, 4, f);
    copy(input.begin() + fread_bias, input.begin() + fread_bias + 4, buffer);
    fread_bias += 4;
    *size = read_uint32(buffer) & 0xffffffff;
}
/**
  Decompress a block of compressed data and returns the size of the
  output data. If error occurs, e.g. the compressed data is
  corrupted or the OUTPUT buffer is not large enough, then 0
  will be returned.
 */
 
void LZSS_Decompression(bool Compress_Twice){
 FILE* infile =fopen(input_location.c_str(),"rb");
    long long numbytes=0,numbytes2=0,chunk_size=0;
    /* Get the number of bytes */
    // fseek(infile, 0L, SEEK_END);
    // const long long numbytes = ftell(infile);
    
    // /* reset the file position indicator to 
    // the beginning of the file */
    // fseek(infile, 0L, SEEK_SET);
    read_chunk_header(infile,  &numbytes,  &numbytes2);
 
    unsigned char* buffer = new unsigned char[numbytes];
    unsigned char* buffer2 = new unsigned char[numbytes2];
 
    // FILE *f=fopen(output_location.c_str(),"wb");
    // fwrite(buffer, 1, numbytes , f);
    // fclose(f);
    if(Compress_Twice){
        long long chunk_extra;
        read_chunk_header(infile,  &chunk_extra);
         fread(buffer, sizeof(char), numbytes, infile);
        unsigned char* buffer3 = new unsigned char[chunk_extra];
        LZSS_Decoder Lzss_Decoder(buffer, numbytes, buffer2);
        chunk_size=Lzss_Decoder.Decompress();
        LZSS_Decoder Lzss_Decoder2(buffer2, numbytes2, buffer3);
        long long chunk_size2 =Lzss_Decoder2.Decompress();
        res = (char*) buffer3;
        res_len = chunk_size2;
        reconstruct_and_output(res, res_len, output_location); 
       //Reconstruct before being deleted
        delete[]buffer3;
    }
    else{
        fread(buffer, sizeof(char), numbytes, infile);
        LZSS_Decoder Lzss_Decoder(buffer, numbytes, buffer2);
        chunk_size=Lzss_Decoder.Decompress();
        res = (char*) buffer2;
        res_len = chunk_size;
        reconstruct_and_output(res, res_len, output_location); 
        //Reconstruct before being deleted
    }
    //chunk_extra no longer in use
    //cout<<numbytes<<chunk_extra<<chunk_size<<endl;
    //FILE *f=fopen(output_location.c_str(),"wb");
    //fwrite(buffer2, 1, chunk_extra , f);
    //fclose(f);
    delete[]buffer;
    delete[]buffer2;
    fclose(infile);
}


void LZSS_Decom_once(){
     FILE* infile =fopen(input_location.c_str(),"rb");
    long long numbytes=0,chunk_extra=0;
    // /* Get the number of bytes */
    // // fseek(infile, 0L, SEEK_END);
    // // const long long numbytes = ftell(infile);
    
    // // /* reset the file position indicator to 
    // // the beginning of the file */
    // // fseek(infile, 0L, SEEK_SET);
    read_chunk_header(infile,  &numbytes,  &chunk_extra);
 
    unsigned char* buffer = new unsigned char[numbytes];
    unsigned char* buffer2 = new unsigned char[chunk_extra+10];
    fread(buffer, sizeof(char), numbytes, infile);
    // // FILE *f=fopen(output_location.c_str(),"wb");
    // // fwrite(buffer, 1, numbytes , f);
    // // fclose(f);
    LZSS_Decoder Lzss_Decoder(buffer, numbytes, buffer2);
    long long chunk_size =Lzss_Decoder.Decompress();
    // //cout<<numbytes<<chunk_extra<<chunk_size<<endl;
    // //FILE *f=fopen(output_location.c_str(),"wb");
    // //fwrite(buffer2, 1, chunk_extra , f);
    // //fclose(f);
   
    // //debug_readin();

    // /* for(long long i=0;i< chunk_size;i++){
    //     res.push_back(buffer2[i]);
    // } */
    res = (char*) buffer2;
    res_len = chunk_size;
    reconstruct_and_output(res, res_len, output_location); 
    //Reconstruct before being deleted
    delete[]buffer;
    delete[]buffer2;
    fclose(infile);
}
void LZSS_Decompression_with_Huffman(bool Compress_Twice)
{
    // FILE *infile = fopen(input_location.c_str(), "rb");
    ifstream inStream(input_location, ios::in | ios::binary);
    std::vector<char> huffDin((std::istreambuf_iterator<char>(inStream)), istreambuf_iterator<char>());
    vector<char> &infile = huffDin;
    if (__builtin_expect(huffDin.back() == 2, 0))
    {
        huffDin.pop_back();
        HuffmanDecoder huffmanDecoder(huffDin);
        infile = huffmanDecoder.decompress();
    }
    else
    {
        huffDin.pop_back();
    }
    long long numbytes = 0, numbytes2 = 0, chunk_size = 0;
    /* Get the number of bytes */
    // fseek(infile, 0L, SEEK_END);
    // const long long numbytes = ftell(infile);

    // /* reset the file position indicator to
    // the beginning of the file */
    // fseek(infile, 0L, SEEK_SET);
    read_chunk_header(infile, &numbytes, &numbytes2);

    unsigned char *buffer = new unsigned char[numbytes];
    unsigned char *buffer2 = new unsigned char[numbytes2];

    // FILE *f=fopen(output_location.c_str(),"wb");
    // fwrite(buffer, 1, numbytes , f);
    // fclose(f);
    if (Compress_Twice)
    {
        long long chunk_extra;
        read_chunk_header(infile, &chunk_extra);
        // fread(buffer, sizeof(char), numbytes, infile);
        copy(infile.begin() + fread_bias, infile.begin() + fread_bias + numbytes, buffer);
        fread_bias += numbytes;
        unsigned char *buffer3 = new unsigned char[chunk_extra];
        LZSS_Decoder Lzss_Decoder(buffer, numbytes, buffer2);  

        chunk_size = Lzss_Decoder.Decompress();
        LZSS_Decoder Lzss_Decoder2(buffer2, numbytes2, buffer3);
        long long chunk_size2 = Lzss_Decoder2.Decompress();
        res = (char *)buffer3;
        res_len = chunk_size2;
        reconstruct_and_output(res, res_len, output_location);
        // Reconstruct before being deleted
    }
    else
    {
        // fread(buffer, sizeof(char), numbytes, infile);

        copy(infile.begin() + fread_bias, infile.begin() + fread_bias + numbytes, buffer);
        fread_bias += numbytes;

        LZSS_Decoder Lzss_Decoder(buffer, numbytes, buffer2);
        
        chunk_size = Lzss_Decoder.Decompress();
        res = (char *)buffer2;
        res_len = chunk_size;
        reconstruct_and_output(res, res_len, output_location);
        // Reconstruct before being deleted
    }
    // chunk_extra no longer in use
    // cout<<numbytes<<chunk_extra<<chunk_size<<endl;
    // FILE *f=fopen(output_location.c_str(),"wb");
    // fwrite(buffer2, 1, chunk_extra , f);
    // fclose(f);
}
int main(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-i")) {
            input_location = argv[++i];
        } else if (!strcmp(argv[i], "-o")) {
            output_location = argv[++i];
            if (debug) {
                debug_output_location = output_location + ".fuck.txt";
                ferr = ofstream(debug_output_location);
            }
        }
    }
    //LZSS_Decom_once();
    //LZSS_Decompression_with_Huffman( Compress_Twice);
    LZSS_Decompression(Compress_Twice);
    // readin();
    // reconstruct_and_output(res, res_len, output_location);
    return 0;
}