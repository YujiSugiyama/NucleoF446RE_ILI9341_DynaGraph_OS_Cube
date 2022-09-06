#ifndef MODEL_HPP
#define MODEL_HPP

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();
    void putData(float* wave, int n);
    void pause(int active);
    int sineActive;

protected:
    ModelListener* modelListener;
private:
    float waveBuf[15];      // sizeof(wave.val) x 3buffers = 5 x 3 = 15
};

#endif // MODEL_HPP
