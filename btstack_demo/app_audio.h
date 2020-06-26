#include "Mmsystem.h"
#include "Digitalv.h"
#include "math.h"
#pragma comment(lib, "winmm.lib")

//#define ENABLED_RECODE_DATA_TO_FILE

typedef struct record_data_ring_buffer_t {
	char  *m_buf;
	INT m_buf_len;
	INT m_data_len;
	INT m_read_index;
	INT m_write_index;
}record_data_ring_buffer;


class WaveOut
{
private:
    HANDLE hEventPlay;
    HWAVEOUT hWaveOut;
    WAVEHDR wvHeader[2];
    CHAR *bufCaching;
	INT m_frameLen;
    INT bufUsed;
    INT iCurPlaying; /* index of current playing in 'wvHeader'. */
    BOOL hasBegan;
public:
    WaveOut();
    ~WaveOut();
    int open(DWORD nSamplesPerSec, WORD wBitsPerSample, WORD nChannels);
    void close();
	void pause();
	void restart();
    int push(const CHAR* buf, int size); /* push buffer into 'bufCaching', if fulled, play it. */
    int flush(); /* play the buffer in 'bufCaching'. */
private:
    int play(const CHAR* buf, int size);
};

class WaveIn
{
public:
	INT iCurRecording;
#ifdef ENABLED_RECODE_DATA_TO_FILE
	FILE *record_file;
#endif
	WAVEHDR wvHeader[2];
	INT m_frameLen;
	record_data_ring_buffer m_record_buf;
private:
	HWAVEIN hWaveIn;

public:
	WaveIn();
	~WaveIn();
	int open(DWORD nSamplesPerSec, WORD wBitsPerSample, WORD nChannels);
	void close();
	int record_start();
	int record_stop();
private:
#ifdef ENABLED_RECODE_DATA_TO_FILE
	int record_file_open();
	void record_file_close();
#endif
};

void app_audio_init();