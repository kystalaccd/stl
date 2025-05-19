#ifndef __EFFICIENT_LIST_HPP__
#define __EFFICIENT_LIST_HPP__

#include <stack>
#include <initializer_list>
#include <stdexcept>
#include <type_traits>
#include <iterator>

namespace stl{

    inline namespace version_0{


        template<typename T>  class he_list_iterator;
        template<typename T>  class he_list_const_iterator;

        //Highly Efficient List
        template<typename T>
        class he_list{
            template<typename V> friend class he_list;
            friend class he_list_iterator<T>;
            friend class he_list_const_iterator<T>;

        public:
            using value_type = T;
            using size_t = unsigned long long;
            using iterator = he_list_iterator<T>;
            using const_iterator = he_list_const_iterator<T>;

        private:
            template<typename U>
            struct Node{
                U val;
                Node *left;
                Node *right;
                size_t size;

                explicit Node(const U &k):val(k), left(nullptr), right(nullptr), size(1) { }
                explicit Node(U &&k):val(std::move(k)), left(nullptr), right(nullptr), size(1) { }
            };

            using node_type = Node<T>;

        public:
            he_list():root(nullptr){ }

            he_list(size_t n, const value_type &value = value_type{}):
                he_list(){
                for(size_t i=0; i<n; ++i)
                    push_back(value);
            }

            he_list(size_t n, value_type &&value):
                he_list(){
                for(size_t i=0; i<n; ++i)
                    push_back(std::move(value));
            }

            template<typename V>
            he_list(std::initializer_list<V> lst):
                he_list() {
                for(auto &val : lst)
                    push_back(std::move(val));
            }

            template<typename Iterator, typename = std::enable_if_t<
                                        std::is_convertible<
                                                typename std::iterator_traits<Iterator>::iterator_category,
                                                std::input_iterator_tag
                                                    >::value
                                                                    >
                    >
            he_list(Iterator beg, Iterator end):
                he_list(){
                for(auto it = beg; it != end; ++it)
                    push_back(*it);
            }

            he_list(const he_list &rhs): root(copy<T>(rhs.root)){ }

            template<typename V>
            he_list(const he_list<V> &rhs): root(copy<V>(rhs.root)){ }

            he_list(he_list &&rhs)noexcept:
                root(rhs.root){
                rhs.root = nullptr;
            }

            ~he_list(){
                free_mem();
            }

            he_list &operator=(const he_list &rhs){
                return operator=<T>(rhs);
            }

            template<typename V>
            he_list &operator=(const he_list<V> &rhs){
                auto tmp = copy<V>(rhs.root);
                free_mem();
                root = tmp;

                return *this;
            }

            he_list &operator=(he_list &&rhs)noexcept{
                if(this != &rhs){
                    root = rhs.root;
                    rhs.root = nullptr;
                }

                return *this;
            }

        public:
            size_t size()const{
                return root?root->size:0;
            }

            bool empty()const{
                return root;
            }

        public:
            void insert(size_t pos, const value_type &val){
                check(pos, size()+1);
                root = add_node(root, pos, val);
            }

            void insert(size_t pos, value_type &&val){
                check(pos, size()+1);
                root = add_node(root, pos, std::move(val));
            }

            void erase(size_t pos){
                check(pos, size());
                root = erase_node(root, pos);
            }

            void push_back(const value_type &val){
                root = add_node(root, size(), val);
            }

            void push_back(value_type &&val){
                root = add_node(root, size(), std::move(val));
            }

            void pop_back(){
                check(0,size());
                erase(size()-1);
            }

            void push_front(const value_type &val){
                root = add_node(root, 0, val);
            }

            void push_front(value_type &&val){
                root = add_node(root, 0, std::move(val));
            }

            void pop_front(){
                erase(0);
            }

        public:
            value_type &operator[](size_t pos){
                check(pos, size());
                auto nd = search_node(root, pos);
                return nd->val;
            }

            const value_type &operator[](size_t pos)const{
                check(pos, size());
                auto nd = search_node(root, pos);
                return nd->val;
            }

            value_type &back(){
                return const_cast<value_type&>(
                                    const_cast<const he_list *const>(this)->back());
            }

            const value_type &back()const{
                check(0,size());
                auto cur = root;
                while(cur->right)
                    cur = cur->right;

                return cur->val; 
            }

            value_type &front(){
                return const_cast<value_type&>(
                                    const_cast<const he_list *const>(this)->front());
            }

            const value_type &front()const{
                check(0,size());
                auto cur = root;
                while(cur->left)
                    cur = cur->left;
                
                return cur->val;
            }

        public:
            iterator begin(){
                return iterator(root);
            }

            iterator end(){
                return iterator();
            }

            const_iterator begin()const{
                return const_iterator(root);
            }

            const_iterator end()const{
                return const_iterator();
            }

            const_iterator cbegin()const{
                return const_iterator(root);
            }

            const_iterator cend()const{
                return const_iterator();
            }

        private:
            void check(size_t pos, size_t range)const{
                if(pos >= range)
                    throw std::runtime_error("Out of range.");
            }

            template<typename V>
            node_type *copy(const typename he_list<V>::node_type *cur){
                if(!cur)
                    return nullptr;
                
                auto nd = new node_type(cur->val);
                nd->size = cur->size;
                nd->left = copy<V>(cur->left);
                nd->right = copy<V>(cur->right);
                return nd;
            } 

            void free_mem(){
                if(!root)
                    return;

                std::stack<node_type*> stk;
                stk.push(root);
                while(!stk.empty()){
                    auto cur = stk.top(); stk.pop();
                    if(cur->right)
                        stk.push(cur->right);
                    if(cur->left) 
                        stk.push(cur->left);
                    delete cur;
                }
                root = nullptr;
            }

            node_type *left_rotate(node_type *cur){
                auto r = cur->right;
                cur->right = r->left;
                r->left = cur;
                r->size = cur->size;
                cur->size = (cur->left?cur->left->size:0) + (cur->right?cur->right->size:0) + 1;
                return r;
            }

            node_type *right_rotate(node_type *cur){
                auto l = cur->left;
                cur->left = l->right;
                l->right = cur;
                l->size = cur->size;
                cur->size = (cur->left?cur->left->size:0) + (cur->right?cur->right->size:0) + 1;
                return l;
            }

            node_type *matain(node_type *cur){
                if(!cur)
                    return nullptr;

                auto lc = cur->left, rc = cur->right;        
                auto lz = lc?lc->size:0, rz = rc?rc->size:0,
                        llz = lc&&lc->left?lc->left->size:0, lrz = lc&&lc->right?lc->right->size:0,
                        rlz = rc&&rc->left?rc->left->size:0, rrz = rc&&rc->right?rc->right->size:0;
                    
                if(llz > rz){   //LL
                    cur = right_rotate(cur);
                    cur->right = matain(cur->right);
                    cur = matain(cur);
                }
                else if(lrz > rz){  //LR
                    cur->left = left_rotate(cur->left);
                    cur = right_rotate(cur);
                    cur->left = matain(cur->left);
                    cur->right = matain(cur->right);
                    cur = matain(cur);
                }
                else if(rrz > lz){  //RR
                    cur = left_rotate(cur);
                    cur->left = matain(cur->left);
                    cur = matain(cur);
                }
                else if(rlz > lz){  //RL
                    cur->right = right_rotate(cur->right);
                    cur = left_rotate(cur);
                    cur->left = matain(cur->left);
                    cur->right = matain(cur->right);
                    cur = matain(cur);
                }

                return cur;
            }

            node_type *add_node(node_type *cur, size_t pos, const value_type &val){   //pos从0开始
                if(!cur) 
                    return new node_type(val);

                auto lmsz = cur->size - (cur->right?cur->right->size:0);
                if(pos < lmsz){
                    cur->left = add_node(cur->left, pos, val);
                }
                else{
                    cur->right = add_node(cur->right, pos-lmsz, val);
                }
                
                ++cur->size;
                return matain(cur);
            }

            node_type *add_node(node_type *cur, size_t pos, value_type &&val){   //pos从0开始
                if(!cur) 
                    return new node_type(std::move(val));

                auto lmsz = cur->size - (cur->right?cur->right->size:0);
                if(pos < lmsz){
                    cur->left = add_node(cur->left, pos, std::move(val));
                }
                else{
                    cur->right = add_node(cur->right, pos-lmsz, std::move(val));
                }
                
                ++cur->size;
                return matain(cur);
            }

            node_type *erase_node(node_type *cur, size_t pos){
                auto lsz = cur->left?cur->left->size:0, lmsz = lsz+1;
                if(pos < lsz){
                    cur->left = erase_node(cur->left, pos);
                }
                else if(pos < lmsz){
                    auto lc = cur->left, rc = cur->right;
                    if(!lc && !rc){
                        delete cur;
                        cur = nullptr;
                    }
                    else if(!rc){
                        delete cur;
                        cur = lc;
                    }
                    else if(!lc){
                        delete cur;
                        cur = rc;
                    }
                    else{
                        node_type *pre = nullptr, *ml = rc;
                        --ml->size;
                        while(ml->left){
                            pre = ml;
                            ml = ml->left;
                            -ml->size;
                        }

                        if(pre){
                            pre->left = ml->right;
                            ml->right = rc;
                        }
                        ml->left = cur->left;
                        delete cur;
                        cur = ml;
                    }
                }
                else{
                    cur->right = erase_node(cur->right, pos-lmsz);
                }

                if(cur)
                    cur->size = (cur->left?cur->left->size:0) + (cur->right?cur->right->size:0) + 1;

                return cur;
            }

            node_type *search_node(node_type *cur, size_t pos)const{
                while(cur){
                    auto lsz = cur->left?cur->left->size:0, lmsz = lsz+1;
                    if(pos < lsz){
                        cur = cur->left;
                    }
                    else if(pos < lmsz){
                        break;
                    }
                    else{
                        cur = cur->right;
                        pos -= lmsz;
                    }
                }

                return cur;
            }

        private:
            node_type *root; 
        };



        template<typename T> bool operator==(const he_list_iterator<T> &, const he_list_iterator<T> &);
        template<typename T> bool operator!=(const he_list_iterator<T> &, const he_list_iterator<T> &);
        template<typename T> bool operator==(const he_list_iterator<T> &, const he_list_const_iterator<T> &);
        template<typename T> bool operator==(const he_list_const_iterator<T> &, const he_list_iterator<T> &);
        template<typename T> bool operator==(const he_list_const_iterator<T> &, const he_list_const_iterator<T> &);
        template<typename T> bool operator!=(const he_list_iterator<T> &, const he_list_const_iterator<T> &);
        template<typename T> bool operator!=(const he_list_const_iterator<T> &, const he_list_iterator<T> &);
        template<typename T> bool operator!=(const he_list_const_iterator<T> &, const he_list_const_iterator<T> &);


        template<typename T>
        class he_list_iterator{
            friend class he_list<T>;
            friend bool operator==<T>(const he_list_iterator &, const he_list_iterator &);
            friend bool operator!=<T>(const he_list_iterator &, const he_list_iterator &);
            friend bool operator==<T>(const he_list_iterator<T> &, const he_list_const_iterator<T> &);
            friend bool operator==<T>(const he_list_const_iterator<T> &, const he_list_iterator<T> &);
            friend bool operator!=<T>(const he_list_iterator<T> &, const he_list_const_iterator<T> &);
            friend bool operator!=<T>(const he_list_const_iterator<T> &, const he_list_iterator<T> &);
            friend class he_list_const_iterator<T>;

        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;

        public:
            he_list_iterator() { }

            he_list_iterator(const he_list_iterator &rhs):stk(rhs.stk) { }

            he_list_iterator(he_list_iterator &&rhs)noexcept:stk(std::move(rhs.stk)) { }

            he_list_iterator &operator=(const he_list_iterator &rhs){
                stk = rhs.stk;
                return *this;
            }

            he_list_iterator &operator=(he_list_iterator &&rhs)noexcept{
                stk = std::move(rhs.stk);
                return *this;
            }

        public:
            T *operator->()const{
                return &(stk.top()->val);
            }

            T &operator*()const{
                return stk.top()->val;
            }

            he_list_iterator &operator++(){
                auto cur = stk.top(); stk.pop();
                auto r = cur->right; 
                while(r){
                    stk.push(r);
                    r = r->left;
                }

                return *this;
            }

            he_list_iterator operator++(int){
                auto ret = *this;
                operator++();
                return ret;
            }

        private:
            he_list_iterator(typename he_list<T>::node_type* root){
                while(root){
                    stk.push(root);
                    root = root->left;
                }
            }

        private:
            std::stack<typename he_list<T>::node_type*> stk;
        };


        template<typename T>
        class he_list_const_iterator{
            friend class he_list<T>;
            friend bool operator==<T>(const he_list_iterator<T> &, const he_list_const_iterator<T> &);
            friend bool operator==<T>(const he_list_const_iterator<T> &, const he_list_iterator<T> &);
            friend bool operator==<T>(const he_list_const_iterator<T> &, const he_list_const_iterator<T> &);
            friend bool operator!=<T>(const he_list_iterator<T> &, const he_list_const_iterator<T> &);
            friend bool operator!=<T>(const he_list_const_iterator<T> &, const he_list_iterator<T> &);
            friend bool operator!=<T>(const he_list_const_iterator<T> &, const he_list_const_iterator<T> &);

        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;

        public:
            he_list_const_iterator() { }

            he_list_const_iterator(const he_list_const_iterator &rhs):stk(rhs.stk) { }

            he_list_const_iterator(const he_list_iterator<T> &rhs):stk(rhs.stk) { }

            he_list_const_iterator(he_list_const_iterator &&rhs)noexcept:stk(std::move(rhs.stk)) { }

            he_list_const_iterator(he_list_iterator<T> &&rhs)noexcept:stk(std::move(rhs.stk)) { }

            he_list_const_iterator &operator=(const he_list_const_iterator &rhs){
                stk = rhs.stk;
                return *this;
            }

            he_list_const_iterator &operator=(const he_list_iterator<T> &rhs){
                stk = rhs.stk;
                return *this;
            }

            he_list_const_iterator &operator=(he_list_const_iterator &&rhs)noexcept{
                stk = std::move(rhs.stk);
                return *this;
            }

            he_list_const_iterator &operator=(he_list_iterator<T> &&rhs)noexcept{
                stk = std::move(rhs.stk);
                return *this;
            }

        public:
            const T *operator->()const{
                return &(stk.top()->val);
            }

            const T &operator*()const{
                return stk.top()->val;
            }

            he_list_const_iterator &operator++(){
                auto cur = stk.top(); stk.pop();
                auto r = cur->right; 
                while(r){
                    stk.push(r);
                    r = r->left;
                }

                return *this;
            }

            he_list_const_iterator operator++(int){
                auto ret = *this;
                operator++();
                return ret;
            }

        private:
            he_list_const_iterator(typename he_list<T>::node_type* root){
                while(root){
                    stk.push(root);
                    root = root->left;
                }
            }

        private:
            std::stack<typename he_list<T>::node_type*> stk;
        };


        template<typename T>
        bool operator==(const he_list_iterator<T> &lhs, const he_list_iterator<T> &rhs){
            return lhs.stk == rhs.stk;
        }

        template<typename T>
        bool operator!=(const he_list_iterator<T> &lhs, const he_list_iterator<T> &rhs){
            return lhs.stk != rhs.stk;
        }

        template<typename T>
        bool operator==(const he_list_const_iterator<T> &lhs, const he_list_iterator<T> &rhs){
            return lhs.stk == rhs.stk;
        }

        template<typename T>
        bool operator==(const he_list_iterator<T> &rhs, const he_list_const_iterator<T> &lhs){
            return lhs.stk == rhs.stk;
        }

        template<typename T>
        bool operator==(const he_list_const_iterator<T> &rhs, const he_list_const_iterator<T> &lhs){
            return lhs.stk == rhs.stk;
        }

        template<typename T>
        bool operator!=(const he_list_const_iterator<T> &lhs, const he_list_iterator<T> &rhs){
            return lhs.stk != rhs.stk;
        }

        template<typename T>
        bool operator!=(const he_list_iterator<T> &rhs, const he_list_const_iterator<T> &lhs){
            return lhs.stk != rhs.stk;
        }

        template<typename T>
        bool operator!=(const he_list_const_iterator<T> &rhs, const he_list_const_iterator<T> &lhs){
            return lhs.stk != rhs.stk;
        }


    }   //!version_0


}   //!stl


#endif  //!__EFFICIENT_LIST_HPP__