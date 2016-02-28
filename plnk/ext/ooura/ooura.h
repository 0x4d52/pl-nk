


typedef struct OouraFFT
{
    int n;
    int* ip;
    float* w;
} OouraFFT;


OouraFFT* ooura_create (int n);
void ooura_destroy (OouraFFT* o);
void ooura_forward (OouraFFT* o, float* io);
void ooura_inverse (OouraFFT* o, float* io);




