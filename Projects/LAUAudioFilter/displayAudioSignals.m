fid = fopen('/tmp/inData0.dat'); inD1 = fread(fid, inf, 'single'); fclose(fid);
fid = fopen('/tmp/otData0.dat'); otD1 = fread(fid, inf, 'single'); fclose(fid);
fid = fopen('/tmp/inData1.dat'); inD2 = fread(fid, inf, 'single'); fclose(fid);
fid = fopen('/tmp/otData1.dat'); otD2 = fread(fid, inf, 'single'); fclose(fid);
fid = fopen('/tmp/inData2.dat'); inD3 = fread(fid, inf, 'single'); fclose(fid);
fid = fopen('/tmp/otData2.dat'); otD3 = fread(fid, inf, 'single'); fclose(fid);

subplot(131); spectrogram(otD1, 1024, 256, 1024, 44100/5);
subplot(132); spectrogram(otD2, 1024, 256, 1024, 44100/5);
subplot(133); spectrogram(otD3, 1024, 256, 1024, 44100/5);

