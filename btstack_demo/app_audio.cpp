#include "stdafx.h"
#include "app_audio.h"


#ifdef __cplusplus
extern "C" {
#endif

	#include "app.h"

#ifdef __cplusplus
}
#endif


#define MAX_RECORD_DATA_LEN (16000*2*4)
#define MAX_INSTANCE_NUM 2
static WaveIn g_WaveIn[MAX_INSTANCE_NUM];
static WaveOut g_WaveOut[MAX_INSTANCE_NUM];
static audio_operation m_opt;


static DWORD CALLBACK RecordCallBack(HWAVEIN hWaveIn, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	//这个WaveIn就是你的音频采集类
	WaveIn *mWaveIn = (WaveIn *)dwInstance;

	switch (uMsg)
	{
	case WIM_OPEN:
		printf("WIM_OPEN\n");
		break;
	case WIM_DATA:
	{
		char *data = mWaveIn->wvHeader[mWaveIn->iCurRecording].lpData;
		int record_len = mWaveIn->wvHeader[mWaveIn->iCurRecording].dwBytesRecorded;
		//while (mWaveIn->m_record_buf.m_buf_len - mWaveIn->m_record_buf.m_data_len < record_len)
			//Sleep(10);
		memcpy(mWaveIn->m_record_buf.m_buf + mWaveIn->m_record_buf.m_write_index, data, record_len);
		mWaveIn->m_record_buf.m_data_len += record_len;
		mWaveIn->m_record_buf.m_write_index = (mWaveIn->m_record_buf.m_write_index + record_len) % mWaveIn->m_record_buf.m_buf_len;
#ifdef ENABLED_RECODE_DATA_TO_FILE		
		fwrite(mWaveIn->wvHeader[mWaveIn->iCurRecording].lpData, mWaveIn->wvHeader[mWaveIn->iCurRecording].dwBytesRecorded, 1, mWaveIn->record_file);
#endif
		memset(mWaveIn->wvHeader[mWaveIn->iCurRecording].lpData, 0, mWaveIn->m_frameLen);
		mWaveIn->wvHeader[mWaveIn->iCurRecording].dwBytesRecorded = 0;
		waveInAddBuffer(hWaveIn, &mWaveIn->wvHeader[mWaveIn->iCurRecording], sizeof(WAVEHDR));

		mWaveIn->iCurRecording = !mWaveIn->iCurRecording;
	}
		break;
	case WIM_CLOSE:
		printf("WIM_CLOSE\n");
		break;
	default:
		break;
	}
	return 0;
}

WaveOut::WaveOut() : hWaveOut(NULL)
{
	wvHeader[0].dwFlags = 0;
	wvHeader[1].dwFlags = 0;
	m_frameLen = 0;
	hEventPlay = CreateEvent(NULL, FALSE, FALSE, NULL);
}
WaveOut::~WaveOut()
{
	close();
	CloseHandle(hEventPlay);
}
int WaveOut::open(DWORD nSamplesPerSec, WORD wBitsPerSample, WORD nChannels)
{
	WAVEFORMATEX wfx;

	m_frameLen = (nSamplesPerSec * (wBitsPerSample / 8) * nChannels)/25;//1s/20
	wvHeader[0].lpData = (CHAR*)malloc(m_frameLen);
	wvHeader[1].lpData = (CHAR*)malloc(m_frameLen);
	wvHeader[0].dwBufferLength = m_frameLen;
	wvHeader[1].dwBufferLength = m_frameLen;
	bufCaching = (CHAR*)malloc(m_frameLen);

	if (!bufCaching || !hEventPlay || !wvHeader[0].lpData || !wvHeader[1].lpData)
	{
		return -1;
	}
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = nChannels;
	wfx.nSamplesPerSec = nSamplesPerSec;
	wfx.wBitsPerSample = wBitsPerSample;
	wfx.cbSize = 0;
	wfx.nBlockAlign = wfx.wBitsPerSample * wfx.nChannels / 8;
	wfx.nAvgBytesPerSec = wfx.nChannels * wfx.nSamplesPerSec * wfx.wBitsPerSample / 8;

	/* 'waveOutOpen' will call 'SetEvent'. */
	if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD_PTR)hEventPlay, 0, CALLBACK_EVENT))
	{
		return -1;
	}

	waveOutPrepareHeader(hWaveOut, &wvHeader[0], sizeof(WAVEHDR));
	waveOutPrepareHeader(hWaveOut, &wvHeader[1], sizeof(WAVEHDR));

	if (!(wvHeader[0].dwFlags & WHDR_PREPARED) || !(wvHeader[1].dwFlags & WHDR_PREPARED))
	{
		return -1;
	}

	bufUsed = 0;
	iCurPlaying = 0;
	hasBegan = 0;

	return 0;
}

void WaveOut::pause()
{
	waveOutPause(hWaveOut);
}

void WaveOut::restart()
{
	waveOutRestart(hWaveOut);
}
void WaveOut::close()
{
	printf("*****************************************1\n");
	if (hWaveOut) {
		flush();
		waveOutUnprepareHeader(hWaveOut, &wvHeader[0], sizeof(WAVEHDR));
		waveOutUnprepareHeader(hWaveOut, &wvHeader[1], sizeof(WAVEHDR));
		waveOutClose(hWaveOut);
		hWaveOut = NULL;
	}

	if (m_frameLen)
	{
		free(wvHeader[0].lpData);
		free(wvHeader[1].lpData);
		free(bufCaching);
		wvHeader[0].lpData = wvHeader[1].lpData = bufCaching = NULL;
		m_frameLen = 0;
	}
	
}
int WaveOut::push(const CHAR* buf, int size)
{
again:
	if (bufUsed + size < m_frameLen)
	{
		memcpy(bufCaching + bufUsed, buf, size);
		bufUsed += size;
	}
	else
	{
		memcpy(bufCaching + bufUsed, buf, m_frameLen - bufUsed);

		if (!hasBegan)
		{
			if (0 == iCurPlaying)
			{
				memcpy(wvHeader[0].lpData, bufCaching, m_frameLen);
				iCurPlaying = 1;
			}
			else
			{
				ResetEvent(hEventPlay);
				memcpy(wvHeader[1].lpData, bufCaching, m_frameLen);

				waveOutWrite(hWaveOut, &wvHeader[0], sizeof(WAVEHDR));
				waveOutWrite(hWaveOut, &wvHeader[1], sizeof(WAVEHDR));

				hasBegan = 1;
				iCurPlaying = 0;
			}
		}
		else if (play(bufCaching, m_frameLen) < 0)
		{
			return -1;
		}

		size -= m_frameLen - bufUsed;
		buf += m_frameLen - bufUsed;
		bufUsed = 0;

		if (size > 0) goto again;
	}
	return 0;
}
int WaveOut::flush()
{
	if (bufUsed > 0 && play(bufCaching, bufUsed) < 0)
	{
		return -1;
	}
	while (1)//如果文件还在没播放完则等待500ms
	{
		if ((waveOutUnprepareHeader(hWaveOut, &wvHeader[0], sizeof(WAVEHDR)) == WAVERR_STILLPLAYING) \
			|| (waveOutUnprepareHeader(hWaveOut, &wvHeader[1], sizeof(WAVEHDR)) == WAVERR_STILLPLAYING)) {
			Sleep(500);
		} else {
			break;
		}
	}
	return 0;
}
int WaveOut::play(const CHAR* buf, int size)
{
	WaitForSingleObject(hEventPlay, INFINITE);

	wvHeader[iCurPlaying].dwBufferLength = size;
	memcpy(wvHeader[iCurPlaying].lpData, buf, size);

	if (waveOutWrite(hWaveOut, &wvHeader[iCurPlaying], sizeof(WAVEHDR)))
	{
		SetEvent(hEventPlay);
		return -1;
	}
	iCurPlaying = !iCurPlaying;

	return 0;
}
WaveIn::WaveIn()
{
	wvHeader[0].dwFlags = 0;
	wvHeader[1].dwFlags = 0;
	m_frameLen = 0;
	iCurRecording = 0;
#ifdef ENABLED_RECODE_DATA_TO_FILE	
	record_file = NULL;
#endif
}
WaveIn::~WaveIn()
{
	close();
}
#ifdef ENABLED_RECODE_DATA_TO_FILE	
int WaveIn::record_file_open()
{
	//recode_file
	SYSTEMTIME sys2; //获取系统时间确保文件的保存不出现重名
	GetLocalTime(&sys2);

	//查找当前目录中有没有Voice文件夹 没有就先创建一个，有就直接存储
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString PathName(szPath);
	//获取exe目录
	CString PROGRAM_PATH = PathName.Left(PathName.ReverseFind(_T('\\')) + 1);
	//Debug目录下RecordVoice文件夹中
	PROGRAM_PATH += _T("RecordVoice\\");

	if (!(GetFileAttributes(PROGRAM_PATH) == FILE_ATTRIBUTE_DIRECTORY))
	{
		if (!CreateDirectory(PROGRAM_PATH, NULL))
		{
			AfxMessageBox(_T("Make Dir Error"));
		}
	}

	CString m_csFileName = PROGRAM_PATH;
	CString s;

	s.Format(_T("%04d%02d%02d%02d%02d%02d"), sys2.wYear, sys2.wMonth, sys2.wDay, sys2.wHour, sys2.wMinute, sys2.wSecond);
	m_csFileName.Append(s);
	m_csFileName.Append(_T(".pcm"));

	//CString to char *
#ifndef  UNICODE
	char m_file_path[1024];
	int str_len = m_csFileName.GetLength();
	char *buf = m_csFileName.GetBuffer(str_len);
	memcpy(m_file_path, buf, str_len);
	m_file_path[str_len] = '\0';
	m_csFileName.ReleaseBuffer();
#else
	int len = WideCharToMultiByte(CP_ACP, 0, m_csFileName, -1, NULL, 0, NULL, NULL);//m_csFileName.GetLength()	
	//m_file_path = new char[len + 1];
	char m_file_path[1024];
	WideCharToMultiByte(CP_ACP, 0, m_csFileName, -1, m_file_path, len, NULL, NULL);
	m_file_path[len + 1] = '\0';
#endif
	if (fopen_s(&record_file, m_file_path, "wb") != 0)
		return -1;

	return 0;
}
	
void WaveIn::record_file_close()
{
	if (record_file) {
		fclose(record_file);
		record_file = NULL;
	}	
}
#endif

int WaveIn::open(DWORD nSamplesPerSec, WORD wBitsPerSample, WORD nChannels)
{
	WAVEFORMATEX wfx;

	m_frameLen = (nSamplesPerSec * (wBitsPerSample / 8) * nChannels)/20;
	wvHeader[0].lpData = (CHAR*)malloc(m_frameLen);
	wvHeader[1].lpData = (CHAR*)malloc(m_frameLen);
	wvHeader[0].dwBufferLength = m_frameLen;
	wvHeader[1].dwBufferLength = m_frameLen;

	if (!wvHeader[0].lpData || !wvHeader[1].lpData)
	{
		return -1;
	}
#ifdef ENABLED_RECODE_DATA_TO_FILE	
	if (record_file_open())
		return -1;
#endif

	memset(&m_record_buf, 0, sizeof(record_data_ring_buffer));
	m_record_buf.m_buf = new char[MAX_RECORD_DATA_LEN];
	m_record_buf.m_buf_len = MAX_RECORD_DATA_LEN;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = nChannels;
	wfx.nSamplesPerSec = nSamplesPerSec;
	wfx.wBitsPerSample = wBitsPerSample;
	wfx.cbSize = 0;
	wfx.nBlockAlign = wfx.wBitsPerSample * wfx.nChannels / 8;
	wfx.nAvgBytesPerSec = wfx.nChannels * wfx.nSamplesPerSec * wfx.wBitsPerSample / 8;

	//if (waveInOpen(&hWaveIn, WAVE_MAPPER, &wfx, (DWORD)RecordCallBack, NULL, CALLBACK_FUNCTION))
	if (waveInOpen(&hWaveIn, WAVE_MAPPER, &wfx, DWORD(RecordCallBack), DWORD(this), CALLBACK_FUNCTION))
	{
		return -1;
	}

	wvHeader[0].dwBytesRecorded = wvHeader[1].dwBytesRecorded = 0;
	wvHeader[0].dwUser = wvHeader[1].dwUser = 0;
	wvHeader[0].dwFlags = wvHeader[1].dwFlags = 0;
	wvHeader[0].dwLoops = wvHeader[1].dwLoops = 1;
	wvHeader[0].lpNext = wvHeader[1].lpNext = NULL;
	wvHeader[0].reserved = wvHeader[1].reserved = 0;

	//waveInReset(hWaveIn);
	waveInPrepareHeader(hWaveIn, &wvHeader[0], sizeof(WAVEHDR));
	waveInPrepareHeader(hWaveIn, &wvHeader[1], sizeof(WAVEHDR));

	//将两个wHdr添加到waveIn中去
	waveInAddBuffer(hWaveIn, &wvHeader[0], sizeof(WAVEHDR));
	waveInAddBuffer(hWaveIn, &wvHeader[1], sizeof(WAVEHDR));

	iCurRecording = 0;

	return 0;
}
void WaveIn::close()
{
	printf("*****************************************1\n");
	if (hWaveIn) {
		waveInStop(hWaveIn);
		waveInUnprepareHeader(hWaveIn, &wvHeader[0], sizeof(WAVEHDR));
		waveInUnprepareHeader(hWaveIn, &wvHeader[1], sizeof(WAVEHDR));
		waveInClose(hWaveIn);
		hWaveIn = NULL;
	}

	if (m_frameLen)
	{
		free(wvHeader[0].lpData);
		free(wvHeader[1].lpData);
		wvHeader[0].lpData = wvHeader[1].lpData = NULL;
		m_frameLen = 0;
	}

	if (m_record_buf.m_buf) {
		delete[] m_record_buf.m_buf;
		m_record_buf.m_buf = NULL;
	}
#ifdef ENABLE_RECODE_DATA_TO_FILE
	record_file_close();
#endif
}
int WaveIn::record_start()
{
	waveInStart(hWaveIn);
	return 0;
}

int WaveIn::record_stop()
{
	waveInStop(hWaveIn);
	return 0;
}


int audio_in_open(uint8_t instance, uint32_t nSamplesPerSec, uint16_t wBitsPerSample, uint16_t nChannels)
{
	int ret = 0;
	if (instance > (MAX_INSTANCE_NUM - 1))
		return  -1;
	g_WaveIn[instance].open(nSamplesPerSec, wBitsPerSample, nChannels);

	return ret;
}
int audio_in_read(uint8_t instance, uint8_t read_type, uint8_t * buf, uint16_t len)
{
	if (instance > (MAX_INSTANCE_NUM - 1))
		return  -1;
	WaveIn * mWaveIn = &g_WaveIn[instance];
	int read_len = 0;
	switch (read_type) {
	case 0://get data len
		read_len = mWaveIn->m_record_buf.m_data_len;
		break;
	case 1://get data
		if (mWaveIn->m_record_buf.m_data_len < len)
			read_len = mWaveIn->m_record_buf.m_data_len;
		else
			read_len = len;

		memcpy(buf, mWaveIn->m_record_buf.m_buf + mWaveIn->m_record_buf.m_read_index, read_len);
		mWaveIn->m_record_buf.m_data_len -= read_len;
		mWaveIn->m_record_buf.m_read_index = (mWaveIn->m_record_buf.m_read_index + read_len) % mWaveIn->m_record_buf.m_buf_len;

	default:
		break;

	}

	return read_len;
}
int audio_in_write(uint8_t instance)
{
	if (instance > (MAX_INSTANCE_NUM - 1))
		return  -1;
	return 0;
}

int audio_in_ioctl(uint8_t instance, uint8_t ioctl)
{
	if (instance > (MAX_INSTANCE_NUM-1))
		return  -1;
	switch (ioctl) {
		case 0://start
			g_WaveIn[instance].record_stop();
			break;
		case 1:
			g_WaveIn[instance].record_start();
			break;
		default:
			break;
	}
	
	return 0;
}
int audio_in_close(uint8_t instance)
{
	if (instance > (MAX_INSTANCE_NUM - 1))
		return  -1;
	g_WaveIn[instance].close();

	return 0;
}

int audio_out_open(uint8_t instance, uint32_t nSamplesPerSec, uint16_t wBitsPerSample, uint16_t nChannels)
{
	if (instance > (MAX_INSTANCE_NUM - 1))
		return  -1;
	g_WaveOut[instance].open(nSamplesPerSec, wBitsPerSample, nChannels);

	return 0;
}
int audio_out_read(uint8_t instance)
{
	if (instance > (MAX_INSTANCE_NUM - 1))
		return  -1;
	return 0;
}
int audio_out_write(uint8_t instance, uint8_t * buf, uint16_t len)
{
	int ret = 0;
	if (instance > (MAX_INSTANCE_NUM - 1))
		return  -1;
	
	ret = g_WaveOut[instance].push((const char *)buf, len);

	return ret;
}
int audio_out_ioctl(uint8_t instance, uint8_t ioctl)
{
	if (instance > (MAX_INSTANCE_NUM - 1))
		return  -1;
	return 0;
}
int audio_out_close(uint8_t instance)
{
	if (instance > (MAX_INSTANCE_NUM - 1))
		return  -1;
	g_WaveOut[instance].close();

	return 0;
}

void app_audio_init()
{
	m_opt.aui_open = audio_in_open;
	m_opt.aui_read = audio_in_read;
	m_opt.aui_write = audio_in_write;
	m_opt.aui_close = audio_in_close;
	m_opt.aui_ioctl = audio_in_ioctl;

	m_opt.auo_open = audio_out_open;
	m_opt.auo_read = audio_out_read;
	m_opt.auo_write = audio_out_write;
	m_opt.auo_close = audio_out_close;
	m_opt.auo_ioctl = audio_out_ioctl;

	app_audio_operation_regeister(&m_opt);
}

#if 0
int app_play_test(void)
{
	WaveOut wvOut;
	FILE *file;  //音频文件
	int sampleRate = 44100;  //采样率
	int bitsPerSample = 16;
	int channels = 1;
	int DATASIZE = sampleRate * bitsPerSample*channels / 8;
	//char *waveOut = new (char)(DATASIZE);
	char *waveOut = new char[DATASIZE];

	MMRESULT result = 0;
	printf("sample rate:%d, channels:%d, bits per sample:%d\n", sampleRate, channels, bitsPerSample);
	fopen_s(&file, "C:\\Users\\soren\\Desktop\\yy\\test\\tone2.pcm", "rb");//打开文件

	if (wvOut.open(sampleRate, bitsPerSample, channels) < 0)
	{
		printf("waveout open failed.\n");
		return 0;
	}
	int nRead = 0;
	//while (ifile.read(buffer, sizeof(buffer)))
	while ((nRead = fread(waveOut, 1, DATASIZE, file)) > 0)
	{
		if (wvOut.push(waveOut, nRead) < 0)
			printf("play failed.\n");
	}
	if (wvOut.flush() < 0)
		printf("flush failed\n");
	Sleep(500);
	printf("play done.\n");
	fclose(file);//关闭文件
	delete[]waveOut;
	return 0;
}
#endif

#if 0
int app_record_test(void)
{
	WaveIn wvIn;
	int sampleRate = 44100;  //采样率
	int bitsPerSample = 16;
	int channels = 1;

	if (wvIn.open(sampleRate, bitsPerSample, channels) < 0)
	{
		printf("wavein open failed.\n");
		return 0;
	}
	wvIn.record_start();
	int i = 3;
	while (i--)
		Sleep(1000);

	wvIn.record_stop();
	return 0;
}
#endif