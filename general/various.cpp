// Enthält gemischten Code. So lange bis es Sinn macht, diesen zu trennen:


#include <cstring> // Stringoperationen (strlen)
#include <cstdlib> // Speicheroperationen


// kopert ein Array in ein anderes, allokiert entsprechend Speicher und gibt Zeiger auf kopiertes Objekt zurück
char* copyCharArray(char* value) {
    if (value == nullptr) return nullptr; // ist Zeichenlänge 0 bzw. zeigt value auf nullptr, abbrechen

    int length = strlen(value); // Länge von value

    if (length == 0) return nullptr; // bei Länge von 0 braucht ebenfalls nichts getan zu werden


    // Annahme: Es wird ebenfalls das Stringabschlusszeichen '\0' mitkopiert. Ansonsten hier noch ein Feld zu length addieren und einfügen


    char* alloc = (char*)calloc(length, sizeof(char));

    // Kopieren:
    strcpy(alloc, value);

    // Zurückgeben, fertig!
    return alloc;
}