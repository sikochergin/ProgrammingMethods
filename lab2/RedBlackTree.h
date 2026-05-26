#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <vector>
#include <string>

#include "../lab1/soldier.h"

using namespace std;

/**
 * @brief Класс красно-чёрного дерева для объектов Soldier.
 *
 * Дерево строится по ключу fullName.
 * Если несколько солдат имеют одинаковое ФИО,
 * они хранятся в одном узле вектора values.
 */
class RedBlackTree {
private:
    /**
     * @brief Цвет узла красно-чёрного дерева.
     */
    enum Color {
        RED,
        BLACK
    };

    /**
     * @brief Узел красно-чёрного дерева.
     */
    struct Node {
        string key; // Ключ узла: ФИО солдата
        vector<Soldier> values; // Все солдаты с таким ФИО
        Color color; // Цвет узла: RED или BLACK
        Node* parent; // Родительский узел
        Node* left; // Левый потомок
        Node* right; // Правый потомок

        /**
         * @brief Конструктор узла.
         *
         * Новый узел изначально создаётся красным.
         *
         * @param soldier Солдат, по которому создаётся новый узел.
         */
        Node(const Soldier& soldier)
        {
            key = soldier.getFullName();
            values.push_back(soldier);

            color = RED;

            parent = nullptr;
            left = nullptr;
            right = nullptr;
        }
    };

    Node* root; // Корень дерева

    /**
     * @brief Выполняет левый поворот вокруг узла x.
     *
     * @param x Узел, вокруг которого выполняется поворот.
     */
    void rotateLeft(Node* x)
    {
        Node* y = x->right;

        x->right = y->left;

        if (y->left != nullptr) {
            y->left->parent = x;
        }

        y->parent = x->parent;

        if (x->parent == nullptr) {
            root = y;
        }
        else if (x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }

        y->left = x;
        x->parent = y;
    }

    /**
     * @brief Выполняет правый поворот вокруг узла x.
     *
     * @param x Узел, вокруг которого выполняется поворот.
     */
    void rotateRight(Node* x)
    {
        Node* y = x->left;

        x->left = y->right;

        if (y->right != nullptr) {
            y->right->parent = x;
        }

        y->parent = x->parent;

        if (x->parent == nullptr) {
            root = y;
        }
        else if (x == x->parent->right) {
            x->parent->right = y;
        }
        else {
            x->parent->left = y;
        }

        y->right = x;
        x->parent = y;
    }

    /**
     * @brief Исправляет красно-чёрные свойства после вставки.
     *
     * @param node Новый добавленный узел.
     */
    void fixInsert(Node* node)
    {
        while (node != root && node->parent->color == RED) {
            Node* parent = node->parent;
            Node* grandparent = parent->parent;

            if (parent == grandparent->left) {
                Node* uncle = grandparent->right;

                if (uncle != nullptr && uncle->color == RED) {
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    grandparent->color = RED;

                    node = grandparent;
                }
                else {
                    if (node == parent->right) {
                        node = parent;
                        rotateLeft(node);

                        parent = node->parent;
                        grandparent = parent->parent;
                    }

                    parent->color = BLACK;
                    grandparent->color = RED;
                    rotateRight(grandparent);
                }
            }
            else {
                Node* uncle = grandparent->left;

                if (uncle != nullptr && uncle->color == RED) {
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    grandparent->color = RED;

                    node = grandparent;
                }
                else {
                    if (node == parent->left) {
                        node = parent;
                        rotateRight(node);

                        parent = node->parent;
                        grandparent = parent->parent;
                    }

                    parent->color = BLACK;
                    grandparent->color = RED;
                    rotateLeft(grandparent);
                }
            }
        }

        root->color = BLACK;
    }

    /**
     * @brief Рекурсивно ищет всех солдат с заданным ФИО.
     *
     * @param node Текущий узел дерева.
     * @param key ФИО, которое нужно найти.
     * @return vector<Soldier> Вектор найденных солдат.
     */
    vector<Soldier> search(Node* node, const string& key) const
    {
        if (node == nullptr) {
            return vector<Soldier>();
        }

        if (key < node->key) {
            return search(node->left, key);
        }
        else if (key > node->key) {
            return search(node->right, key);
        }
        else {
            return node->values;
        }
    }

    /**
     * @brief Рекурсивно очищает память дерева.
     *
     * @param node Текущий узел, который нужно удалить.
     */
    void clear(Node* node)
    {
        if (node == nullptr) {
            return;
        }

        clear(node->left);
        clear(node->right);

        delete node;
    }

public:
    /**
     * @brief Конструктор пустого красно-чёрного дерева.
     */
    RedBlackTree()
    {
        root = nullptr;
    }

    /**
     * @brief Запрещаем копирование дерева.
     *
     * Это нужно, чтобы два объекта RedBlackTree случайно
     * не указывали на одни и те же узлы.
     */
    RedBlackTree(const RedBlackTree&) = delete;
    RedBlackTree& operator=(const RedBlackTree&) = delete;

    /**
     * @brief Деструктор дерева.
     *
     * Удаляет все динамически созданные узлы.
     */
    ~RedBlackTree()
    {
        clear(root);
    }

    /**
     * @brief Добавляет одного солдата в красно-чёрное дерево.
     *
     * Сначала вставка выполняется как в обычном бинарном дереве поиска.
     * После этого вызывается fixInsert, чтобы восстановить балансировку.
     *
     * @param soldier Солдат, которого нужно добавить.
     */
    void insert(const Soldier& soldier)
    {
        string key = soldier.getFullName();

        if (root == nullptr) {
            root = new Node(soldier);
            root->color = BLACK;
            return;
        }

        Node* current = root;
        Node* parent = nullptr;

        while (current != nullptr) {
            parent = current;

            if (key < current->key) {
                current = current->left;
            }
            else if (key > current->key) {
                current = current->right;
            }
            else {
                current->values.push_back(soldier);
                return;
            }
        }

        Node* newNode = new Node(soldier);
        newNode->parent = parent;

        if (key < parent->key) {
            parent->left = newNode;
        }
        else {
            parent->right = newNode;
        }

        fixInsert(newNode);
    }

    /**
     * @brief Строит красно-чёрное дерево по вектору солдат.
     *
     * @param soldiers Вектор солдат.
     */
    void build(const vector<Soldier>& soldiers)
    {
        for (const Soldier& soldier : soldiers) {
            insert(soldier);
        }
    }

    /**
     * @brief Ищет всех солдат с заданным ФИО.
     *
     * @param key ФИО, которое нужно найти.
     * @return vector<Soldier> Вектор найденных солдат.
     */
    vector<Soldier> search(const string& key) const
    {
        return search(root, key);
    }
};

#endif