#include <iostream>
#include <sys/time.h> // for gettimeofday()

#include "Audio.h"
#include "FeatureExtract.h"
#include "Model.h"

using namespace std;

int main(int argc, char *argv[])
{
    struct timeval start, end;
    Audio audio;
    FeatureExtract fe(1);
    ModelConfig cfg;

    cfg.vocab_path = "stream/vocab.txt";
    cfg.wenet_path = "stream/wenet_params.bin";
    Model mm(cfg, 1);

    int i;
    for (i = 0; i < 10; i++) {
        int16_t *buff;
        int len = 1360;
        int sum = 0;
        audio.loadwav("zh.wav");
        mm.reset();
        fe.reset();

        SpeechFlag flag = S_MIDDLE;
        while (flag == S_MIDDLE) {
            flag = audio.fetch_chunck(buff, len);
            sum += len;
            fe.insert(buff, len, flag);
        }

        int j = 0;
        int ll = fe.size();

        gettimeofday(&start, NULL);
        for (j = 0; j < ll; j++) {
            Tensor<float> *buff;
            fe.fetch(buff);
            // delete buff;
            string msg = mm.forward_chunk(buff);
            cout << msg << endl;
        }

        string msg = mm.rescoring();
        cout << msg << endl;

        gettimeofday(&end, NULL);
        long seconds = (end.tv_sec - start.tv_sec);
        long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
        printf("Model initialization takes %lfs\n", (double)micros / 1000000);
    }
}