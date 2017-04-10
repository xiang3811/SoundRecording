// audiomaketest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <stdio.h>  
#include <windows.h>  
#pragma comment(lib, "winmm.lib")  


/**/
//RandomAccessFile fopen(String path) throws IOException {
//	File f = new File(path);
//
//	if (f.exists()) {
//		f.delete();
//	}
//	else {
//		File parentDir = f.getParentFile();
//		if (!parentDir.exists()) {
//			parentDir.mkdirs();
//		}
//	}
//
//	RandomAccessFile file = new RandomAccessFile(f, "rw");
//	// 16K、16bit、单声道
//	/* RIFF header */
//	file.writeBytes("RIFF"); // riff id
//	file.writeInt(0); // riff chunk size *PLACEHOLDER*
//	file.writeBytes("WAVE"); // wave type
//
//							 /* fmt chunk */
//	file.writeBytes("fmt "); // fmt id
//	file.writeInt(Integer.reverseBytes(16)); // fmt chunk size
//	file.writeShort(Short.reverseBytes((short)1)); // format: 1(PCM)
//	file.writeShort(Short.reverseBytes((short)1)); // channels: 1
//	file.writeInt(Integer.reverseBytes(16000)); // samples per second
//	file.writeInt(Integer.reverseBytes((int)(1 * 16000 * 16 / 8))); // BPSecond
//	file.writeShort(Short.reverseBytes((short)(1 * 16 / 8))); // BPSample
//	file.writeShort(Short.reverseBytes((short)(1 * 16))); // bPSample
//
//														  /* data chunk */
//	file.writeBytes("data"); // data id
//	file.writeInt(0); // data chunk size *PLACEHOLDER*
//
//	Log.d(TAG, "wav path: " + path);
//	return file;
//}
//
//private void fwrite(RandomAccessFile file, byte[] data, int offset, int size) throws IOException {
//	file.write(data, offset, size);
//	Log.d(TAG, "fwrite: " + size);
//}
//
//private void fclose(RandomAccessFile file) throws IOException {
//	try {
//		file.seek(4); // riff chunk size
//		file.writeInt(Integer.reverseBytes((int)(file.length() - 8)));
//
//		file.seek(40); // data chunk size
//		file.writeInt(Integer.reverseBytes((int)(file.length() - 44)));
//
//		Log.d(TAG, "wav size: " + file.length());
//
//	}
//	finally {
//		file.close();
//	}
//}
//
class CAudioRecording
{
private:
	HWAVEIN hWaveIn;  //输入设备
	WAVEFORMATEX waveform; //采集音频的格式，结构体
	
	WAVEHDR wHdr1; //采集音频时包含数据缓存的结构体
public:
	CAudioRecording() :hWaveIn(0) { Init(); }
	~CAudioRecording() {
		if(hWaveIn)
			waveInClose(hWaveIn);
	}
	bool Init() {
		HANDLE          wait;
		waveform.wFormatTag = WAVE_FORMAT_PCM;//声音格式为PCM
		waveform.nSamplesPerSec = 8000;//采样率，16000次/秒
		waveform.wBitsPerSample = 16;//采样比特，16bits/次
		waveform.nChannels = 1;//采样声道数，2声道
		waveform.nAvgBytesPerSec = 16000;//每秒的数据率，就是每秒能采集多少字节的数据
		waveform.nBlockAlign = 2;//一个块的大小，采样bit的字节数乘以声道数
		waveform.cbSize = 0;//一般为0

		wait = CreateEvent(NULL, 0, 0, NULL);
		//使用waveInOpen函数开启音频采集
		MMRESULT error = waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);
		return error == 0;
	}
	int getWaveBuff(BYTE pBuff[], size_t size){
		wHdr1.lpData = (LPSTR)pBuff;
		wHdr1.dwBufferLength = size;
		wHdr1.dwBytesRecorded = 0;
		wHdr1.dwUser = 0;
		wHdr1.dwFlags = 0;
		wHdr1.dwLoops = 1;
		waveInPrepareHeader(hWaveIn, &wHdr1, sizeof(WAVEHDR));//准备一个波形数据块头用于录音
		waveInAddBuffer(hWaveIn, &wHdr1, sizeof(WAVEHDR));//指定波形数据块为录音输入缓存
		waveInStart(hWaveIn);//开始录音
		Sleep(1000);//等待声音录制1s
		waveInReset(hWaveIn);//停止录音

		return wHdr1.dwBytesRecorded;
	}

};


FILE *pf;
int _tmain(int argc, _TCHAR* argv[])
{
	CAudioRecording audio;

	BYTE *pBuffer1;//采集音频时的数据缓存
	//建立两个数组（这里可以建立多个数组）用来缓冲音频数据
	DWORD bufsize = 1024 * 100;//每次开辟10k的缓存存储录音数据
	int i = 20;
	fopen_s(&pf, "录音测试.pcm", "wb");
	while (i--)//录制20左右秒声音，结合音频解码和网络传输可以修改为实时录音播放的机制以实现对讲功能
	{
		pBuffer1 = new BYTE[bufsize];
		int lenth = audio.getWaveBuff(pBuffer1, bufsize);
		fwrite(pBuffer1, 1, lenth, pf);
		delete pBuffer1;
		printf("%ds  ", i);
	}
	fclose(pf);
	return 0;
}
