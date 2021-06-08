#pragma once

#include <iostream>
#include <stack>
#include <vector>

namespace ariel{
    template<typename T>
    class BinaryTree
    {
        private:
            class Node
            {
                public:
                    T value;
                    Node* left;
                    Node* right;
                    Node(T v) : value(v){left = nullptr; right = nullptr;}
                    Node(const Node& n):value(n.value)
                    {
                        if(n.left != nullptr){left = new Node(*n.left);}
                        else{left = nullptr;}
                        if(n.right != nullptr){right = new Node(*n.right);}
                        else{right = nullptr;}                        
                    }
                    Node(const Node&& n) noexcept : value(n.value) 
                    {
                        left = n.left;
                        right = n.right;
                        n.left = nullptr;
                        n.right = nullptr;
                    }
                    Node& operator=(const Node& other)
                    {
                        if(this == &other){return *this;}
                        value = other.value;
                        left = new Node(*other.left);
                        right = new Node(*other.right);
                        return *this;
                    }
                    Node& operator=(Node&& other) noexcept
                    {
                        left = other.left;
                        right = other.right;
                        other.left = nullptr;
                        other.right = nullptr;
                        return *this;
                    }
                    bool hasLeft(){return left != nullptr;}
                    bool hasRight(){return right != nullptr;}
                    ~Node(){}
            };

            Node* head;
            void freetree(Node* n)
            {
                if(n == nullptr){return;}
                freetree(n->left);
                n->left = nullptr;
                freetree(n->right);
                n->right = nullptr;
                delete n;
            }

        public:
            BinaryTree(){head = nullptr;}
            BinaryTree(const BinaryTree& other)
            {
                if(other.head == nullptr){head = nullptr;return;}
                head = new Node(*other.head);
            }
            BinaryTree(BinaryTree&& other) noexcept
            {
                head = other.head;
                other.head = nullptr;
            }
            ~BinaryTree(){freetree(head);}

            BinaryTree& add_root(T value);
            BinaryTree& add_left(T existing_node, T new_node);
            BinaryTree& add_right(T existing_node, T new_node);

            BinaryTree& operator=(const BinaryTree& other)
            {
                if(this == &other){return *this;}
                if(head == nullptr){head = new Node(*other.head);}
                else
                {
                    if(head != nullptr){freetree(head);}
                    head = new Node(*other.head);
                }
                return *this;
            }

            BinaryTree& operator=(BinaryTree&& other) noexcept
            {
                if(head != nullptr){freetree(head);}
                head = other.head;
                other.head = nullptr;
                return *this;
            }

            void print()
            {
                print_all(head);
            }
            void print_all(Node* n)
            {
                if(n == nullptr){return;}
                std::cout << n->value;
                print_all(n->left);
                print_all(n->right);
            }

            Node* find(Node* current, T val)
            {
                if(current == nullptr)
                {
                    return nullptr;
                }
                if(current->value == val)
                {
                    return current;
                }
                Node* left = find(current->left, val);
                if(left != nullptr)
                {
                    return left;
                }
                return find(current->right, val);
            }

            class preorder_iterator{
                private:
                    std::stack<Node*> st;
                    void next()
                    {
                        if(st.empty()){return;}
                        // If current node has left child, return the left child.
                        if(st.top()->hasLeft()){st.push(st.top()->left);}
                        else
                        {
                            // While the current node doesn't have a right child, go back to it's parents.
                            // Or until we finished all the nodes.
                            while (!st.empty() && !st.top()->hasRight()){st.pop();}

                            // If the stack isn't empty, then a node with right child has left in the stack.
                            if(!st.empty())
                            {
                                Node* n = st.top();
                                st.pop();
                                st.push(n->right);
                            }
                        }
                    }
                public:    
                    preorder_iterator(Node* ptr = nullptr)
                    {
                        if(ptr != nullptr){st.push(ptr);}
                    }
                    preorder_iterator& operator++()
                    {
                        next();
                        return *this;
                    }
                    preorder_iterator operator++(int)
                    {
                        preorder_iterator tmp = *this;
                        next();
                        return tmp;
                    }
                    bool operator==(const preorder_iterator& it)
                    {
                        if(!st.empty() && !it.st.empty()){return st.top() == it.st.top();}
                        return st.empty() && it.st.empty();
                    }
                    bool operator!=(const preorder_iterator& it)
                    {
                        if(!st.empty() && !it.st.empty()){return st.top() != it.st.top();}
                        return !(st.empty() && it.st.empty());
                    }
                    T& operator*(){return st.top()->value;}
                    T* operator->(){return &st.top()->value;}
            };

            class inorder_iterator{
                private:
                    std::stack<Node*> st;
                    void get_path()
                    {
                        while (st.top()->hasLeft())
                        {
                            st.push(st.top()->left);
                        }
                    }
                    void next()
                    {
                        if(st.empty()){return;}
                        if(!st.top()->hasRight())
                        {
                            st.pop();
                        }
                        else
                        {
                            Node* n = st.top();
                            st.pop();
                            st.push(n->right);
                            get_path();
                        }
                    }
                public:    
                    inorder_iterator(Node* ptr = nullptr)
                    {
                        if(ptr != nullptr)
                        {
                            st.push(ptr);
                            get_path();
                        }
                    }
                    inorder_iterator& operator++()
                    {
                        next();
                        return *this;
                    }
                    inorder_iterator operator++(int)
                    {
                        inorder_iterator tmp = *this;
                        next();
                        return tmp;
                    }
                    bool operator==(const inorder_iterator& it)
                    {
                        if(!st.empty() && !it.st.empty()){return st.top() == it.st.top();}
                        return st.empty() && it.st.empty();
                    }
                    bool operator!=(const inorder_iterator& it)
                    {
                        if(!st.empty() && !it.st.empty()){return st.top() != it.st.top();}
                        return !(st.empty() && it.st.empty());
                    }
                    T& operator*(){return st.top()->value;}
                    T* operator->(){return &st.top()->value;}
            };

            class postorder_iterator{
                private:
                    std::stack<Node*> st;
                    std::stack<Node*> visited;

                    void get_path()
                    {
                        // Im taking the most left path possible (including the only right childs)
                        // If we reach a child without left or right, then we stored the whole path.
                        while(st.top()->hasLeft() || st.top()->hasRight())
                        {
                            // Go first left.
                            if(st.top()->hasLeft()){st.push(st.top()->left);}
                            // If can't go left, then go right.
                            else if(st.top()->hasRight())
                            {
                                // Save each right turn in path, to not to go there again later.
                                visited.push(st.top());
                                st.push(st.top()->right);
                            }
                        }
                    }
                    void next()
                    {
                        if(st.empty()){return;}
                        if(!visited.empty() && st.top() == visited.top()){visited.pop();}
                        st.pop();
                        if(st.empty() || (!visited.empty() && st.top() == visited.top())){return;}
                        if(st.top()->hasRight())
                        {
                            visited.push(st.top());
                            st.push(st.top()->right);
                            get_path();
                        }
                    }
                public:
                    postorder_iterator(Node* ptr = nullptr)
                    {
                        if(ptr != nullptr)
                        {
                            st.push(ptr);
                            get_path();
                        }
                    }
                    postorder_iterator& operator++()
                    {
                        next();
                        return *this;
                    }
                    postorder_iterator operator++(int)
                    {
                        postorder_iterator tmp = *this;
                        next();
                        return tmp;
                    }
                    bool operator==(const postorder_iterator& it)
                    {
                        if(!st.empty() && !it.st.empty()){return st.top() == it.st.top();}
                        return st.empty() && it.st.empty();
                    }
                    bool operator!=(const postorder_iterator& it)
                    {
                        if(!st.empty() && !it.st.empty()){return st.top() != it.st.top();}
                        return !(st.empty() && it.st.empty());
                    }
                    T& operator*(){return st.top()->value;}
                    T* operator->(){return &st.top()->value;}
            };
            inorder_iterator begin()
            {
                return inorder_iterator(head);
            }
            inorder_iterator end()
            {
                return inorder_iterator();
            }
            preorder_iterator begin_preorder()
            {
                return preorder_iterator(head);
            }
            preorder_iterator end_preorder()
            {
                return preorder_iterator();
            }
            inorder_iterator begin_inorder()
            {
                return inorder_iterator(head);
            }
            inorder_iterator end_inorder()
            {
                return inorder_iterator();
            }
            postorder_iterator begin_postorder()
            {
                return postorder_iterator(head);
            }
            postorder_iterator end_postorder()
            {
                return postorder_iterator();
            }

            static int get_depth(Node* n)
            {
                if(n == nullptr){return 0;}
                return 1 + max(get_depth(n->left), get_depth(n->right));
            }

            std::ostream& assist(std::ostream& os, std::vector<Node*> a, int d) const
            {
                if(d == 0){return os;}
                os<< '\n';
                for(unsigned long i = 0; i < a.size(); i+=2)
                {
                    if(a.at(i)!=nullptr){os << '|';}
                    else if(a.at(i+1)!=nullptr){os <<'*';}
                    else {os << ' ';}
                    std::string st;
                    if(a.at(i+1)!=nullptr){st = "--";}
                    else{st = "  ";}
                    for(int j = 0; j < d; j++)
                    {st += st;}
                    os << st;
                    if(a.at(i+1)!=nullptr){os << '|';}
                    else {os << ' ';}
                    for(unsigned long r = (i/2)%2; r <= d; r++)
                    {
                        std::string s = " ";
                        for(unsigned long j = 0; j < r; j++)
                        {
                            s += s;
                        }
                        os << s;
                    }
                }
                os << '\n';
                for(unsigned long i = 0; i < a.size(); i+=2)
                {
                    if(a.at(i)!=nullptr){os << a.at(i)->value;}
                    else {os << ' ';}
                    std::string st = "  ";
                    for(int j = 0; j < d; j++)
                    {st += st;}
                    os << st;
                    if(a.at(i+1)!=nullptr){os << a.at(i+1)->value;}
                    else {os << ' ';}
                    for(unsigned long r = (i/2)%2; r <= d; r++)
                    {
                        std::string s = " ";
                        for(unsigned long j = 0; j < r; j++)
                        {
                            s += s;
                        }
                        os << s;
                    }
                }
                std::vector<Node*> new_a;
                new_a.resize(2*a.size());
                for(unsigned long i = 0; i < a.size(); i++)
                {
                    unsigned long left = 2*i;
                    unsigned long right = 2*i + 1;
                    if(a.at(i) == nullptr)
                    {
                        new_a.at(left) = nullptr;
                        new_a.at(right) = nullptr;
                    }
                    else
                    {
                        new_a.at(left) = a.at(i)->left;
                        new_a.at(right) = a.at(i)->right;
                    }
                }
                assist(os,new_a,d-1);
                return os;
            }

            friend std::ostream& operator<<(std::ostream& os, const BinaryTree tree)
            {
                if(tree.head == nullptr){return os;}
                int d = get_depth(tree.head);
                if(tree.head->left == nullptr && tree.head->left == nullptr){return os;}
                std::vector<Node*> a;
                a.resize(2);
                a.at(0) = tree.head->left;
                a.at(1) = tree.head->right;
                os << tree.head->value;
                tree.assist(os, a, d-1);
                return os;
            }
    };

    template <typename T>
    BinaryTree<T>& BinaryTree<T>::add_root(T value)
    {
        if(head == nullptr)
        {
            this->head = new Node(value);
        }
        else
        {
            head->value = value;
        }
        return *this;
    }
    template <typename T>
    BinaryTree<T>& BinaryTree<T>::add_left(T existing_node, T new_node)
    {
        Node* n = find(this->head, existing_node);
        if(n == nullptr)
        {
            throw "Node with that value does not exist";
        }
        if(n->left == nullptr)
        {
            n->left = new Node(new_node);
        }
        else
        {
            (n->left)->value = new_node;
        }
        return *this;
    }
    template <typename T>
    BinaryTree<T>& BinaryTree<T>::add_right(T existing_node, T new_node)
    {
        Node* n = find(this->head, existing_node);
        if(n == nullptr)
        {
            throw "Node with that value does not exist";
        }
        if(n->right == nullptr)
        {
            n->right = new Node(new_node);
        }
        else
        {
            (n->right)->value = new_node;
        }
        return *this;
    }
}