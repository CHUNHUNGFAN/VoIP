#define WIN32_LEAN_AND_MEAN
#define LENGTH 5
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

const int DATASIZE = 22050 * 2 * LENGTH; //SamplesPerSec * byte per sample * duration time
char soundBuffer[DATASIZE];

void get_audio(WAVEFORMATEX audio_format, WAVEHDR audio_header);
void save_file(WAVEFORMATEX audio_format, WAVEHDR audio_header);


int main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s server-name\n", argv[0]);
		system("pause");
		return 1;
	}

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		system("pause");
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		system("pause");
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			system("pause");
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		system("pause");
		return 1;
	}

	// Send an initial buffer
	/*iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		system("pause");
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);*/

	// Open wav file with mode "rb" and send an initial buffer
	FILE *wav_file;
	wav_file = fopen("D:\\bird.wav", "rb");
	fseek(wav_file, 0, SEEK_END);
	unsigned long filesize = ftell(wav_file);
	char *buffer = (char*)malloc(sizeof(char)*filesize);
	rewind(wav_file);
	// store read data into buffer
	fread(buffer, sizeof(char), filesize, wav_file);

	// send buffer to client
	int sent = 0;
	while (sent < filesize)
	{
		iResult = send(ConnectSocket, buffer + sent, filesize - sent, 0);
		if (iResult == -1)
			break;  // ERROR

		sent += iResult;
	}

	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	printf("Bytes Sent: %ld\n", iResult);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		system("pause");
		return 1;
	}

	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	WAVEFORMATEX audio_format;  //structure of wave format      
	audio_format.wFormatTag = WAVE_FORMAT_PCM;
	audio_format.nChannels = 1;
	audio_format.nSamplesPerSec = 22050;
	audio_format.wBitsPerSample = 16;
	audio_format.cbSize = 0;
	audio_format.nBlockAlign = (audio_format.nChannels * audio_format.wBitsPerSample) / 8;
	audio_format.nAvgBytesPerSec = (audio_format.nSamplesPerSec*audio_format.nBlockAlign);

	WAVEHDR audio_header;
	audio_header.dwFlags = 0;
	audio_header.dwUser = 0;
	audio_header.dwLoops = 0;
	audio_header.dwBytesRecorded = 0;
	audio_header.lpData = (LPSTR)soundBuffer;
	audio_header.dwBufferLength = DATASIZE;

	get_audio(audio_format, audio_header);
	save_file(audio_format, audio_header);

	system("pause");
	return 0;
}

void get_audio(WAVEFORMATEX audio_format, WAVEHDR audio_header)
{
	HWAVEIN audio_in;
	int log;
	log = waveInOpen(&audio_in, WAVE_MAPPER, &audio_format, NULL, NULL, NULL);
	//open input device

	if (log == MMSYSERR_NOERROR)
		printf("waveInOpen success\n");
	else
		printf("waveInOpen fail\n");

	//prepare buffer header
	log = waveInPrepareHeader(audio_in, &audio_header, sizeof(WAVEHDR));

	if (log == MMSYSERR_NOERROR)
		printf("waveInPrepareHeader success\n");
	else
		printf("waveInPrepareHeader fail\n");
	//send buffer to input device
	log = waveInAddBuffer(audio_in, &audio_header, sizeof(WAVEHDR));

	if (log == MMSYSERR_NOERROR)
		printf("waveInAddBuffer success\n");
	else
		printf("waveInAddBuffer fail\n");

	printf("Press Enter to start recording\n");
	getchar();
	if (!waveInStart(audio_in))	//start recording										
		printf("recording start ...\n");
	else
		printf("recording fail...\n");
	Sleep(LENGTH * 1000);

	if (!waveInStop(audio_in))	//stop recording										
		printf("recording stop\n");
	else
		printf("recording stop fail\n");

	//clean buffer header
	if (waveInUnprepareHeader(audio_in, &audio_header, sizeof(WAVEHDR)))
		printf("UnPrepare Header fail\n");
	else
		printf("UnPrepare Header success\n");



	if (waveInClose(audio_in) == MMSYSERR_NOERROR)
		printf("waveInClose success\n");
	else
		printf("waveInClose fail\n");
}

void save_file(WAVEFORMATEX audio_format, WAVEHDR audio_header)
{
	//saving as wave file
	MMCKINFO wav_file_header1;
	MMCKINFO wav_file_header2;
	HMMIO wav_file;
	MMRESULT result;
	long wav_log;

	//create wav file
	wav_file = mmioOpen("wavfile.wav", NULL, MMIO_CREATE | MMIO_WRITE | MMIO_EXCLUSIVE | MMIO_ALLOCBUF);
	if (wav_file == NULL)
		printf("mmioOpen fail\n");

	ZeroMemory(&wav_file_header1, sizeof(MMCKINFO));
	wav_file_header1.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	result = mmioCreateChunk(wav_file, &wav_file_header1, MMIO_CREATERIFF);  //create RIFF chunk 
	if (result == MMSYSERR_NOERROR)
		printf("mmioCreateChunk: wav_file_header1 %s ok\n", result);
	else
		printf("mmioCreateChunk: wav_file_header1 fail\n");


	ZeroMemory(&wav_file_header2, sizeof(MMCKINFO));
	wav_file_header2.ckid = mmioFOURCC('f', 'm', 't', ' ');
	wav_file_header2.cksize = sizeof(WAVEFORMATEX)+audio_format.cbSize;

	//create fmt chunk
	result = mmioCreateChunk(wav_file, &wav_file_header2, 0);
	if (result == MMSYSERR_NOERROR)
		printf("mmioCreateChunk wav_file_header2  %s ok\n", result);
	else
		printf("mmioCreateChunk wav_file_header2 fail\n");

	wav_log = mmioWrite(wav_file, (char*)&audio_format, sizeof(WAVEFORMATEX)+audio_format.cbSize);
	if (wav_log == -1)
		printf("mmioWrite audio_format fail\n");
	else
		printf("mmioWrite write %d bytes\n", wav_log);

	result = mmioAscend(wav_file, &wav_file_header2, 0);
	if (result == MMSYSERR_NOERROR)
		printf("mmioAscend wav_file_header2 %s ok\n", result);
	else
		printf("mmioAscend wav_file_header2 %s fail\n", result);

	wav_file_header2.ckid = mmioFOURCC('d', 'a', 't', 'a');

	//create data Chunk
	result = mmioCreateChunk(wav_file, &wav_file_header2, 0);
	if (result == MMSYSERR_NOERROR)
		printf("mmioCreateChunk wav_file_header2 %s ok\n", result);
	else
		printf("mmioCreateChunk wav_file_header2 %s fail\n", result);
	printf("audio_header %d BytesRecorded\n", audio_header.dwBytesRecorded);

	wav_log = mmioWrite(wav_file, (char*)audio_header.lpData, DATASIZE);

	if (wav_log == -1)
		printf("mmioWrite audio data fail\n");
	else
		printf("mmioWrite success : write %d bytes\n", wav_log);

	result = mmioAscend(wav_file, &wav_file_header2, 0);
	if (result == MMSYSERR_NOERROR)
		printf("mmioAscend wav_file_header2 %s ok\n", result);
	else
		printf("mmioAscend wav_file_header2 %s fail\n", result);

	result = mmioAscend(wav_file, &wav_file_header1, 0);
	if (result == MMSYSERR_NOERROR)
		printf("mmioAscend wav_file_header1 %s ok\n", result);
	else
		printf("mmioAscend wav_file_header1 %s fail\n", result);

	result = mmioClose(wav_file, 0);	//close the wav file 
	if (result == MMIOERR_CANNOTWRITE){
		printf("mmioClose fail\n");
	}
}