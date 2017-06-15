#define _CRT_SECURE_NO_WARNINGS
#define BUFSIZE 4
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <cstring>
#include <typeinfo>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

struct Chunk
{
	char ChunkID[4];
	int ChunkSize;
	char Format[4];
};

struct Subchunk1
{
    char SubChunk1ID[4];
    int SubChunk1Size;
    short AudioFormat;
    short NumChannel;
    int SampleRate;
    int ByteRate;
    short BlockAlign;
    short BytePerSample;
};

struct Subchunk2
{
    char SubChunk2ID[4];
    int SubChunk2Size;
};

struct blank
{
    short blak;
};

int main(int argc, char* argv[])
{
	Chunk Ch;
    Subchunk1 S1;
    Subchunk2 S2;
    blank bk;
	char fn[20] = "a.out";
	if (argc == 2)
		strcpy(fn, argv[1]);

	FILE *fp = fopen(fn, "rb");

	fread(&Ch, sizeof(Ch), 1, fp);
    
    string str_chunkid(Ch.ChunkID);
    string b="\0";
    str_chunkid.replace(4,5,b);

    string str_format(Ch.Format);
    str_format.replace(4,5,b);
    
    cout << "chunk ID :" << str_chunkid << endl;
    cout << "chunk Size:" << Ch.ChunkSize << "(0x" << hex << Ch.ChunkSize << ")" << endl;
    cout << "Format:" << str_format << endl;
    
    
    fread(&S1,sizeof(S1),1,fp);
    
    cout << "SubChunk1ID:" << S1.SubChunk1ID << endl;
    cout << "SubChunk1Size:" << dec << S1.SubChunk1Size << endl;
    cout << "AudioFormat:" << S1.AudioFormat << endl;
    cout << "NumChannels:" << S1.NumChannel << endl;
    cout << "SampleRate:" << dec << S1.SampleRate << endl;
    cout << "ByteRate:" << S1.ByteRate << endl;
    cout << "BlockAlign:" << S1.BlockAlign << endl;
    cout << "BytePerSample:" << S1.BytePerSample << endl;
    
    if(S1.SubChunk1Size == 18){
        fread(&bk,sizeof(bk),1,fp);
        fread(&S2,sizeof(S2),1,fp);
    }
    else
        fread(&S2,sizeof(S2),1,fp);
    
    string str_chunk2id(S2.SubChunk2ID);
    str_chunk2id.replace(4,2,b);
    
    cout << "SubChunk2ID:" << str_chunk2id << endl;
    cout << "SubChunk2Size:" << S2.SubChunk2Size << "(0x" << hex << S2.SubChunk2Size << ")"<< endl;
  
    char * buffer;
    char * buffer_2;
    size_t result;
    size_t result_2;
    buffer = (char*) malloc (S2.SubChunk2Size/2);
    buffer_2 = (char*) malloc (S2.SubChunk2Size/2);
    result = fread(buffer,S2.SubChunk2Size/2,1,fp);
    result_2 = fread(buffer_2,S2.SubChunk2Size/2,1,fp);
    
    FILE *output=fopen("output.wav","wb");
    fwrite(&Ch,sizeof(Ch),1,output);
    if(S1.SubChunk1Size == 18){
        fwrite(&S1,sizeof(S1),1,output);
        fwrite(&bk,sizeof(bk),1,output);
        fwrite(&S2,sizeof(S2),1,output);
        fwrite(buffer_2,S2.SubChunk2Size/2,1,output);
        fwrite(buffer,S2.SubChunk2Size/2,1,output);
    }
    else{
        fwrite(&S1,sizeof(S1),1,output);
        fwrite(&S2,sizeof(S2),1,output);
        fwrite(buffer_2,S2.SubChunk2Size/2,1,output);
        fwrite(buffer,S2.SubChunk2Size/2,1,output);
    }
    fclose(output);
    fclose(fp);
	return 0;
}
