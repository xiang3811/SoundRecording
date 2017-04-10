// audiomaketest1.cpp : �������̨Ӧ�ó������ڵ㡣
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
//	// 16K��16bit��������
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
	HWAVEIN hWaveIn;  //�����豸
	WAVEFORMATEX waveform; //�ɼ���Ƶ�ĸ�ʽ���ṹ��
	
	WAVEHDR wHdr1; //�ɼ���Ƶʱ�������ݻ���Ľṹ��
public:
	CAudioRecording() :hWaveIn(0) { Init(); }
	~CAudioRecording() {
		if(hWaveIn)
			waveInClose(hWaveIn);
	}
	bool Init() {
		HANDLE          wait;
		waveform.wFormatTag = WAVE_FORMAT_PCM;//������ʽΪPCM
		waveform.nSamplesPerSec = 8000;//�����ʣ�16000��/��
		waveform.wBitsPerSample = 16;//�������أ�16bits/��
		waveform.nChannels = 1;//������������2����
		waveform.nAvgBytesPerSec = 16000;//ÿ��������ʣ�����ÿ���ܲɼ������ֽڵ�����
		waveform.nBlockAlign = 2;//һ����Ĵ�С������bit���ֽ�������������
		waveform.cbSize = 0;//һ��Ϊ0

		wait = CreateEvent(NULL, 0, 0, NULL);
		//ʹ��waveInOpen����������Ƶ�ɼ�
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
		waveInPrepareHeader(hWaveIn, &wHdr1, sizeof(WAVEHDR));//׼��һ���������ݿ�ͷ����¼��
		waveInAddBuffer(hWaveIn, &wHdr1, sizeof(WAVEHDR));//ָ���������ݿ�Ϊ¼�����뻺��
		waveInStart(hWaveIn);//��ʼ¼��
		Sleep(1000);//�ȴ�����¼��1s
		waveInReset(hWaveIn);//ֹͣ¼��

		return wHdr1.dwBytesRecorded;
	}

};


FILE *pf;
int _tmain(int argc, _TCHAR* argv[])
{
	CAudioRecording audio;

	BYTE *pBuffer1;//�ɼ���Ƶʱ�����ݻ���
	//�����������飨������Խ���������飩����������Ƶ����
	DWORD bufsize = 1024 * 100;//ÿ�ο���10k�Ļ���洢¼������
	int i = 20;
	fopen_s(&pf, "¼������.pcm", "wb");
	while (i--)//¼��20�����������������Ƶ��������紫������޸�Ϊʵʱ¼�����ŵĻ�����ʵ�ֶԽ�����
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
