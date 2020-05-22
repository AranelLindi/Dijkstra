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
            free(id);
            free(attrname);
            free(attrtype);
            free(_default);
        }
    }; // min 32 Bytes + enum...

    struct Data {
        char* key; // 8 Bytes
        char* value; // 8 Bytes

        ~Data(void) {
            free(key);
            free(value);
        }
    }; // 16 Bytes

    struct Node {
        // id des Knotens
        char* id; // 8 Bytes
        // Array Data-Objekte
        struct Data* datas; // 8 Bytes
        // Anzahl data-Elements
        uint8_t dcount; // 1 Byte
        // fortlaufende Nummer
        uint8_t no; // 1 Byte
        //unsigned:7; // 6 Padding Bytes

        ~Node(void) {
            free(id);

            // Data Objekte freigeben!
            // Dazu durch alle Data's iterieren und Destruktor aufrufen
            for (uint8_t i = 0; i < this->dcount; i++) {
                Data& data_ref = this->datas[i]; // Hilfsreferenz

                // für jede Data den Destruktor aufrufen um Member freizugeben:
                data_ref.~Data();

                // danach das gesamte Struct freigeben:
                free(&data_ref);
            }
            
            // Zum Schluss Zeigerarray freigeben:
            free(datas);
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
        // Anzahl data-Elements
        uint8_t dcount; // 1 Byte
        // Gerichtet/Ungerichtet
        enum EdgeType type; // n. def. in Standart

        ~Edge(void) {
            free(id);
            free(source);
            free(target);
            
            // Data Objekte freigeben!
            // Dazu durch alle Data's iterieren und Destruktor aufrufen
            for (uint8_t i = 0; i < this->dcount; i++) {
                Data& data_ref = this->datas[i]; // Hilfsreferenz

                // für jede Data den Destruktor aufrufen um Member freizugeben:
                data_ref.~Data();

                // danach das gesamte Struct freigeben:
                free(&data_ref);
            }
            
            // Zum Schluss Zeigerarray freigeben:
            free(datas);
        }
    };
}

#endif // ml_container.h