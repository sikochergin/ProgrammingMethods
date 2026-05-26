#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <vector>
#include <string>

#include "../lab1/soldier.h"

using namespace std;

/**
 * @brief Класс бинарного дерева поиска для объектов Soldier.
 *
 * Дерево строится по ключу fullName.
 * Если несколько солдат имеют одинаковое ФИО,
 * они хранятся в одном узле вектора values.
 */
class BinarySearchTree {
private:
    /**
     * @brief Узел бинарного дерева поиска.
     */
    struct Node {
        string key;              // Ключ узла: ФИО солдата
        vector<Soldier> values;  // Все солдаты с таким ФИО

        Node* left;              // Левый потомок
        Node* right;             // Правый потомок

        /**
         * @brief Конструктор узла.
         *
         * @param soldier Солдат, по которому создаётся новый узел.
         */
        Node(const Soldier& soldier)
        {
            key = soldier.getFullName();
            values.push_back(soldier);

            left = nullptr;
            right = nullptr;
        }
    };

    Node* root; // Корень дерева

    /**
     * @brief Рекурсивно добавляет солдата в дерево.
     *
     * @param node Текущий узел дерева.
     * @param soldier Солдат, которого нужно добавить.
     */
    void insert(Node*& node, const Soldier& soldier)
    {
        string key = soldier.getFullName();

        if (node == nullptr) {
            node = new Node(soldier);
            return;
        }

        if (key < node->key) {
            insert(node->left, soldier);
        }
        else if (key > node->key) {
            insert(node->right, soldier);
        }
        else {
            node->values.push_back(soldier);
        }
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
     * @brief Конструктор пустого дерева.
     */
    BinarySearchTree()
    {
        root = nullptr;
    }

    /**
     * @brief Деструктор дерева.
     *
     * Автоматически удаляет все узлы дерева,
     * чтобы не было утечек памяти.
     */
    ~BinarySearchTree()
    {
        clear(root);
    }

    /**
     * @brief Добавляет одного солдата в дерево.
     *
     * @param soldier Солдат, которого нужно добавить.
     */
    void insert(const Soldier& soldier)
    {
        insert(root, soldier);
    }

    /**
     * @brief Строит дерево по вектору солдат.
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