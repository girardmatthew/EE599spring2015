fid = fopen('/tmp/testAudio.dat', 'r');
data = fread(fid, inf, 'float');
fclose(fid);
