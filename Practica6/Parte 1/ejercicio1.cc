#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
//
class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int fileIndex = open("ONE_W", O_RDWR | O_CREAT);

        alloc_data(MAX_NAME*sizeof(char) + 2 * sizeof(int16_t));
        char * tmp = _data;
        memcpy(tmp, name, sizeof(char) * MAX_NAME);
        write(fileIndex, tmp, sizeof(char) * MAX_NAME);
        tmp += sizeof(char) * MAX_NAME; //se adelanta 
        memcpy(tmp, &x, sizeof(int16_t));
        write(fileIndex, tmp, sizeof(int16_t));
        tmp += sizeof(int16_t); //se adelanta 
        memcpy(tmp, &y, sizeof(int16_t));
        write(fileIndex, tmp, sizeof(int16_t));

        close(fileIndex);
    }

    int from_bin(char * data)
    {
        //int fileIndex = open(fileName, O_RDWR);

        char * tmp = data;
        
        //read(fileIndex, tmp, sizeof(char) * MAX_NAME);
        memcpy(name, tmp, sizeof(char) * MAX_NAME);
        tmp+=sizeof(char) * MAX_NAME;
        //read(fileIndex, tmp, sizeof(int16_t));
        memcpy(&x, tmp, sizeof(int16_t));
        tmp+=sizeof(int16_t);
        //read(fileIndex, tmp, sizeof(int16_t));
        memcpy(&y, tmp, sizeof(int16_t));
        tmp+=sizeof(int16_t);

        //close(fileIndex);

        //memcpy(name, data, sizeof(char) * MAX_NAME);
        //memcpy(&x, data, sizeof(int16_t));
        //memcpy(&y, data, sizeof(int16_t));

        return 0;
    }

    void dumpInfo(){
        std::cout << name << " " << x << " " << y << std::endl;
    }

private:
    static const size_t MAX_NAME = 80;
    char name[MAX_NAME];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    one_w.to_bin();
    one_r.from_bin(one_w.data());
    one_r.dumpInfo();


    //Serializar y escribir one_w en un fichero
    //Leer el fichero en un buffer y "deserializar" en one_r
    //Mostrar el contenido one_r

    return 0;
}
