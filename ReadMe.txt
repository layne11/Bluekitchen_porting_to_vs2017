当首次打开工程时（没有.vs文件夹），请将x64的配置修改为x86，不然会报没有预编译头的错误


a2dp音频传输会保存一个avdtp_sink.wav的文件，推荐使用GoleWave软件先格式化一下，格式如下：
文件类型：Raw（snd，raw）
	音质：PCM singed 16 bit，little endian，stereo
速率(Hz)：44100