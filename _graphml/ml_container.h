#ifndef ML_CONTAINER_H
#define ML_CONTAINER_H

#include <cstdint>

namespace nsGraphML {

    enum EdgeType { directed, undirected };
    enum GraphObject { graph, node, edge, all };

    struct Key {
        char* id; // 8 Bytes
        char* attrname; // 8 Bytes
        char* attrtype; // 8 Bytes
        char* _default; // 8 Bytes
        enum GraphObject _for; // n.def. in Standart

        ~Key(void) {
            delete[] id;
            delete[] attrname;
            delete[] attrtype;
            delete[] _default;
        }
    }; // min 32 Bytes + enum...

    struct Data {
        char* key; // 8 Bytes
        char* value; // 8 Bytes

        ~Data(void) {
            delete[] key;
            delete[] value;
        }
    }; // 16 Bytes

    struct Node {
        // id des Knotens
        char* id; // 8 Bytes
        // Array Data-Objekte
        struct Data* datas; // 8 Bytes
        // fortlaufende Nummer
        uint8_t no; // 1 Byte
        unsigned:7; // 7 Padding Bytes

        ~Node(void) {
            delete[] id;
            delete[] datas;
        }
    }; // 24 Bytes

    struct Edge {
        // id der Kante
        char* id; // 8 Bytes
        // id Quellknoten
        char* source; // 8 Bytes
        // id Zielknoten
        char* target; // 8 Bytes
        // Array Data-Objekte
        struct Data* datas; // 8 Bytes
        // Gerichtet/Ungerichtet
        enum EdgeType type; // n. def. in Standart

        ~Edge(void) {
            delete[] id;
            delete[] source;
            delete[] target;
            delete[] datas;
        }
    };
}

#endif // ml_container.h