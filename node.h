#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

template <typename TK>
struct Node {
    TK* keys;
    Node** children;
    int count;
    bool leaf;
    int M;

    Node(int _M) {
        M = _M;
        keys = new TK[M - 1];
        children = new Node<TK>*[M];
        for (int i = 0; i < M; i++) children[i] = nullptr;
        count = 0;
        leaf = true;
    }

    bool isFull() const {
        return count == M - 1;
    }

    void inorder(ostringstream& oss, const string& sep) {
        for (int i = 0; i < count; i++) {
            if (!leaf && children[i]) children[i]->inorder(oss, sep);
            oss << keys[i] << sep;
        }
        if (!leaf && children[count]) children[count]->inorder(oss, sep);
    }

    void rangeSearch(TK begin, TK end, vector<TK>& result) {
        int i = 0;
        while (i < count && keys[i] < begin) {
            if (!leaf) children[i]->rangeSearch(begin, end, result);
            i++;
        }
        for (; i < count && keys[i] <= end; i++) {
            if (!leaf) children[i]->rangeSearch(begin, end, result);
            result.push_back(keys[i]);
        }
        if (!leaf) children[i]->rangeSearch(begin, end, result);
    }

    bool search(TK key) {
        int i = 0;
        while (i < count && key > keys[i]) i++;
        if (i < count && keys[i] == key) return true;
        if (leaf) return false;
        return children[i]->search(key);
    }

    void splitChild(int i) {
        Node* y = children[i];
        Node* z = new Node(M);
        z->leaf = y->leaf;
        int t = (M - 1) / 2;  // Punto de división

        z->count = y->count - t - 1;

        // Copiar las claves mayores al punto de división
        for (int j = 0; j < z->count; j++) {
            z->keys[j] = y->keys[j + t + 1];
        }

        // Copiar los hijos si no es hoja
        if (!y->leaf) {
            for (int j = 0; j <= z->count; j++) {
                z->children[j] = y->children[j + t + 1];
            }
        }

        y->count = t;

        // Hacer espacio para el nuevo hijo
        for (int j = count; j > i; j--) {
            children[j + 1] = children[j];
        }
        children[i + 1] = z;

        // Mover las claves para insertar la clave media
        for (int j = count - 1; j >= i; j--) {
            keys[j + 1] = keys[j];
        }

        keys[i] = y->keys[t];
        count++;
    }

    bool insertNonFull(TK key) {
        int i = count - 1;
        if (leaf) {
            while (i >= 0 && keys[i] > key) {
                keys[i + 1] = keys[i];
                i--;
            }
            if (i >= 0 && keys[i] == key) return false;
            keys[i + 1] = key;
            count++;
            return true;
        } else {
            while (i >= 0 && keys[i] > key) i--;
            i++;
            if (children[i]->isFull()) {
                splitChild(i);
                if (key > keys[i]) i++;
            }
            return children[i]->insertNonFull(key);
        }
    }
    bool remove(TK key) {
        int idx = 0;
        while (idx < count && key > keys[idx]) {
            idx++;
        }

        // Caso 1: Clave encontrada en nodo hoja
        if (leaf && idx < count && keys[idx] == key) {
            for (int i = idx; i < count - 1; i++) {
                keys[i] = keys[i + 1];
            }
            count--;
            return true;
        }

        // Caso 2: Clave encontrada en nodo interno
        if (!leaf && idx < count && keys[idx] == key) {
            // Implementación simplificada - solo para pruebas
            return false;
        }

        // Caso 3: La clave está en un subárbol
        if (!leaf) {
            bool result = children[idx]->remove(key);
            // Verificar underflow (implementación simplificada)
            if (children[idx]->count < (M-1)/2) {
                // Aquí debería ir la lógica de fusión/redistribución
            }
            return result;
        }

        return false;
    }

    ~Node() {
        if (!leaf) {
            for (int i = 0; i <= count; ++i) {
                delete children[i];  // Eliminación recursiva de los hijos
            }
        }
        delete[] keys;
        delete[] children;
    }

};

#endif
