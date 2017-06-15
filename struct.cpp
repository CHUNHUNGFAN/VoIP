#define _CRT_SECURE_NO_WARNINGS
#define BUFSIZE 4
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;
struct Header
{
	char ChunkID[4];
	int ChunkSize;
	char Format[4];
	char SubChunk1ID[4];
	int SubChunk1Size;
	short AudioFormat;
	short NumChannel;
	int SampleRate;
	int ByteRate;
	short BlockAlign;
	short BytePerSample;
	char SubChunk2ID[4];
	int SubChunk2Size;
};

int main(int argc, char* argv[])
{
	Header h;
	char fn[20] = "a.out";
	if (argc == 2)
		strcpy(fn, argv[1]);

	FILE *fp = fopen(fn, "rb");
	
	fread(&h, sizeof(h), 1, fp);
	
	cout << "chunk ID :" << h.ChunkID << endl;
	cout << "chunk Size:" << h.ChunkSize << "(0x" << hex << h.ChunkSize << ")" << endl;
	cout << "Format:" << h.Format << endl;
	cout << "SubChunk1ID:" << h.SubChunk1ID << endl;
	cout << "SubChunk1Size:" << dec << h.SubChunk1Size << endl;
	cout << "AudioFormat:" << h.AudioFormat << endl;
	cout << "NumChannels:" << h.NumChannel << endl;
	cout << "SampleRate:" << dec << h.SampleRate << endl;
	cout << "ByteRate:" << h.ByteRate << endl;
	cout << "BlockAlign:" << h.BlockAlign << endl;
	cout << "BytePerSample:" << h.BytePerSample << endl;
	cout << "SubChunk2ID:" << h.SubChunk2ID << endl;
	cout << "SubChunk2Size:" << h.SubChunk2Size << endl;
	fclose(fp);
	return 0;
}
