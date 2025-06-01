// btree.h
#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <sstream>
#include <vector>
#include "node.h"

using namespace std;

template <typename TK>
class BTree {
private:
    Node<TK>* root;
    int M;
    int n;

public:
    BTree(int _M) : root(nullptr), M(_M), n(0) {}

    ~BTree() {
        clear();
    }
    bool search(TK key) {
        return root ? root->search(key) : false;
    }

    void insert(TK key) {
        if (!root) {
            root = new Node<TK>(M);
            root->keys[0] = key;
            root->count = 1;
            root->leaf = true;
            n++;
            return;
        }
        if (root->isFull()) {
            Node<TK>* s = new Node<TK>(M);
            s->leaf = false;
            s->children[0] = root;
            s->splitChild(0);
            root = s;
        }
        if (root->insertNonFull(key)) n++;
    }

    void remove(TK key) {
        if (!root) return;

        bool removed = root->remove(key);
        if (removed) {
            n--; // Decrementar contador solo si se eliminó
            if (root->count == 0 && !root->leaf) {
                Node<TK>* old_root = root;
                root = root->children[0];
                delete old_root;
            }
        }
    }

    int height() {
        if (!root) return 0;
        int h = 0;
        Node<TK>* current = root;
        while (current) {
            h++;
            if (current->leaf) break;
            current = current->children[0];
        }
        return h;
    }

    string toString(const string& sep) {
        if (!root) return "";
        ostringstream oss;
        root->inorder(oss, sep);
        string s = oss.str();
        if (!s.empty() && s.substr(s.length() - sep.length()) == sep)
            s = s.substr(0, s.length() - sep.length());
        return s;
    }

    vector<TK> rangeSearch(TK begin, TK end) {
        vector<TK> result;
        if (root) root->rangeSearch(begin, end, result);
        return result;
    }

    TK minKey() {
        if (!root) throw runtime_error("Árbol vacío");
        Node<TK>* current = root;
        while (!current->leaf) {
            current = current->children[0]; // Ir al primer hijo
        }
        return current->keys[0]; // Primera clave
    }

    TK maxKey() {
        if (!root) throw runtime_error("Árbol vacío");
        Node<TK>* current = root;
        while (!current->leaf) {
            current = current->children[current->count]; // Ir al último hijo
        }
        return current->keys[current->count - 1]; // Última clave
    }

    void clear() {
        if (root) {
            delete root;  // Esto activará la cadena de destructores
            root = nullptr;
            n = 0;
        }
    }
    int size() { return n; }

    static BTree* build_from_ordered_vector(vector<TK> elements, int grado) {
        BTree<TK>* tree = new BTree<TK>(grado);
        for (auto val : elements) {
            tree->insert(val);
        }
        return tree;
    }

    bool check_properties() {
        if (!root) return true;
        vector<Node<TK>*> nodos;
        vector<int> niveles;
        int nivel = -1;
        nodos.push_back(root);
        niveles.push_back(0);
        while (!nodos.empty()) {
            Node<TK>* nodo = nodos.back(); nodos.pop_back();
            int profundidad = niveles.back(); niveles.pop_back();
            if (nodo != root && nodo->count < (M / 2 - 1)) return false;
            for (int i = 1; i < nodo->count; i++) {
                if (nodo->keys[i - 1] > nodo->keys[i]) return false;
            }
            if (nodo->leaf) {
                if (nivel == -1) nivel = profundidad;
                else if (nivel != profundidad) return false;
            }
            if (!nodo->leaf) {
                for (int i = nodo->count; i >= 0; i--) {
                    if (!nodo->children[i]) return false;
                    nodos.push_back(nodo->children[i]);
                    niveles.push_back(profundidad + 1);
                }
                for (int i = 0; i < nodo->count; i++) {
                    Node<TK>* left = nodo->children[i];
                    Node<TK>* right = nodo->children[i + 1];
                    if (left->keys[left->count - 1] >= nodo->keys[i]) return false;
                    if (right->keys[0] <= nodo->keys[i]) return false;
                }
            }
        }
        return true;
    }
};

#endif
