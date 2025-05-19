#ifndef __FUNCTIONAL_HPP__
#define __FUNCTIONAL_HPP__

#include <utility>
#include <cstring>

namespace stl{

    namespace version_0_0{


        template<typename Signature>
        class function;


        template<typename Res, typename... Args>
        class function<Res(Args...)>{
            struct callable_base{
                virtual Res operator()(Args... args) = 0;
                virtual callable_base *clone() const = 0;
                virtual ~callable_base() {}
            };

            template<typename F>
            struct callable : public callable_base{
                F functor;

                callable(F func):functor(func) { }

                virtual Res operator()(Args... args){
                    return functor(std::forward<Args>(args)...);
                }

                virtual callable_base *clone()const{
                    return new callable(functor);
                }
            };

            callable_base *callable_ptr;

        public:
            constexpr function():
                callable_ptr(nullptr){
            }

            ~function(){
                delete callable_ptr;
            }

            template<typename F>
            function(F f):
                callable_ptr(new callable<F>(f)) {
            }

            function(const function &rhs):
                callable_ptr(rhs.callable_ptr->clone()){
            }

            function(function &&rhs)noexcept:
                callable_ptr(rhs.callable_ptr) {
                rhs.callable_ptr = nullptr;
            }

            function &operator=(const function &rhs){
                auto tmp = rhs.callable_ptr->clone();
                delete callable_ptr;
                callable_ptr = tmp;
                return *this;
            }

            function &operator=(function &&rhs)noexcept{
                if(callable_ptr != rhs.callable_ptr){
                    delete callable_ptr;
                    callable_ptr = rhs.callable_ptr;
                    rhs.callable_ptr = nullptr;
                }
                return *this;
            }

            Res operator()(Args... args){
                return (*callable_ptr)(std::forward<Args>(args)...);
            }
        };


    }   //!version_0_0



    namespace version_0_1{

        template<typename Signature>
        class function;


        template<typename Res, typename... Args>
        class function<Res(Args...)>{
            Res (*call_fptr)(function*, Args...);
            void *(*clone_fptr)(const function*);
            void (*destruct_fptr)(function*);
        
            void *callable_ptr;

            template<typename Functor>
            static Res call(function *self, Args... args){
                return (*static_cast<Functor*>(self->callable_ptr))(std::forward<Args>(args)...);
            }

            template<typename Functor>
            static void *clone(function const *src){
                return new Functor(*static_cast<Functor*>(src->callable_ptr));
            }

            template<typename Functor>
            static void destruct(function *self){
                delete static_cast<Functor*>(self->callable_ptr);
            }

        public:
            constexpr function():
                call_fptr(nullptr),
                clone_fptr(nullptr),
                destruct_fptr(nullptr),
                callable_ptr(nullptr){
            }

            ~function(){
                destruct_fptr(this);
            }

            template<typename F>
            function(F f):
                call_fptr(call<F>),
                clone_fptr(clone<F>),
                destruct_fptr(destruct<F>),
                callable_ptr(new F(f)) {
            }

            function(const function &rhs):
                call_fptr(rhs.call_fpr),
                clone_fptr(rhs.clone_fptr),
                destruct_fptr(rhs.destruct_fptr),
                callable_ptr(clone_fptr(&rhs)) {
            }

            function(function &&rhs)noexcept:
                call_fptr(rhs.call_fptr),
                clone_fptr(rhs.clone_fptr),
                destruct_fptr(rhs.destruct_fptr),
                callable_ptr(rhs.callable_ptr){
                rhs.call_fptr = nullptr;
                rhs.clone_fptr = nullptr;
                rhs.destruct_fptr = nullptr;
                rhs.callable_ptr = nullptr;
            }

            function &operator=(const function &rhs){
                auto tmp = rhs.clone_fptr(&rhs);
                destruct_fptr(this);
                callable_ptr = tmp;
                clone_fptr = rhs.clone_fptr;
                call_fptr = rhs.call_fptr;
                destruct_fptr = rhs.destruct_fptr;
                return *this;
            }

            function &operator=(function &&rhs)noexcept{
                if(this != &rhs){
                    destruct_fptr(this);
                    call_fptr = rhs.call_fptr;
                    clone_fptr = rhs.clone_fptr;
                    destruct_fptr = rhs.destruct_fptr;
                    callable_ptr = rhs.callable_ptr;
                    rhs.call_fptr = nullptr;
                    rhs.clone_fptr = nullptr;
                    rhs.destruct_fptr = nullptr;
                    rhs.callable_ptr = nullptr;
                }

                return *this;
            }

            Res operator()(Args... args){
                return call_fptr(this, std::forward<Args>(args)...);
            }
        };

    }   //!version_0_1



    inline namespace version_0_2{   //暂不支持bind函数返回值

        template<bool enough>
        struct Placement{ };


        template<typename Signature>
        class function;


        template<typename Res, typename... Args>
        class function<Res(Args...)>{
            Res (*call_fptr)(function*, Args...);
            void (*clone_fptr)(function *, const function*);
            void (*destruct_fptr)(function*);
        
            void *callable_ptr;

            template<typename Functor>
            static Res call(function *self, Args... args){
                return (*static_cast<Functor*>(self->callable_ptr))(std::forward<Args>(args)...);
            }

            template<typename Class_,typename Arg,typename... ArgTypes>
            static Res detail_call(function *self, Arg&& obj, ArgTypes&&... args){
                typedef Res(Class_::*F)(ArgTypes...);
                F fptr = nullptr;
                std::memcpy(&fptr, self->callable_ptr, sizeof(F));
                return (obj.*fptr)(std::forward<ArgTypes>(args)...);
            }

            template<typename Class_,typename Arg,typename... ArgTypes>
            static Res detail_call(function *self, Arg *obj, ArgTypes&&... args){
                typedef Res(Class_::*F)(ArgTypes...);
                F fptr = nullptr;
                std::memcpy(&fptr, self->callable_ptr, sizeof(F));
                return (obj->*fptr)(std::forward<ArgTypes>(args)...);
            }

            template<typename Class_>
            static Res mem_call(function *self, Args... args){
                return detail_call<Class_>(self, std::forward<Args>(args)...);
            }

            template<typename Functor>
            static void clone(function *dst, function const *src){
                dst->callable_ptr = new Functor(*static_cast<Functor*>(src->callable_ptr));
            }

            template<typename Functor>
            static void destruct(function *self){
                delete static_cast<Functor*>(self->callable_ptr);
            }

            static void *addr_of_callable(function *f){
                return &f->callable_ptr;
            }

            static const void *addr_of_callable(const function *f){
                return &f->callable_ptr;
            }

            template<typename Functor>
            static Res call_placement(function *self, Args... args){
                return (*static_cast<Functor*>(addr_of_callable(self)))(std::forward<Args>(args)...);
            }

            template<typename Functor>
            static void clone_placement(function *dst, const function *src){
                new(addr_of_callable(dst)) Functor(*static_cast<const Functor*>(addr_of_callable(src)));
            }

            template<typename Functor>
            static void destruct_placement(function *self){
                static_cast<Functor*>(addr_of_callable(self))->~Functor();
            }

            template<typename F>
            void init(F f, Placement<false>){
                call_fptr = call<F>;
                clone_fptr = clone<F>;
                destruct_fptr = destruct<F>;
                callable_ptr = new F(f);
            }

            template<typename F>
            void init(F f, Placement<true>){
                call_fptr = call_placement<F>;
                clone_fptr = clone_placement<F>;
                destruct_fptr = destruct_placement<F>;
                new (addr_of_callable(this)) F(f);
            }

        public:
            constexpr function():
                call_fptr(nullptr),
                clone_fptr(nullptr),
                destruct_fptr(nullptr),
                callable_ptr(nullptr){
            }

            ~function(){
                destruct_fptr(this);
            }

            template<typename F>
            function(F f){
                init(f, Placement<(sizeof(F) <= sizeof(callable_ptr))>());
            }

            template<typename Class_, typename... ArgTypes>
            function(Res(Class_::*f)(ArgTypes...)){
                call_fptr = mem_call<Class_>;
                typedef  Res(Class_::*F)(ArgTypes...);
                clone_fptr = clone<F>;
                destruct_fptr = destruct<F>;
                callable_ptr = new F(f);
            }

            function(const function &rhs):
                call_fptr(rhs.call_fpr),
                clone_fptr(rhs.clone_fptr),
                destruct_fptr(rhs.destruct_fptr),
                callable_ptr(clone_fptr(&rhs)) {
            }

            function(function &&rhs)noexcept:
                call_fptr(rhs.call_fptr),
                clone_fptr(rhs.clone_fptr),
                destruct_fptr(rhs.destruct_fptr),
                callable_ptr(rhs.callable_ptr){
                rhs.call_fptr = nullptr;
                rhs.clone_fptr = nullptr;
                rhs.destruct_fptr = nullptr;
                rhs.callable_ptr = nullptr;
            }

            function &operator=(const function &rhs){
                auto tmp = rhs.clone_fptr(&rhs);
                destruct_fptr(this);
                callable_ptr = tmp;
                clone_fptr = rhs.clone_fptr;
                call_fptr = rhs.call_fptr;
                destruct_fptr = rhs.destruct_fptr;
                return *this;
            }

            function &operator=(function &&rhs)noexcept{
                if(this != &rhs){
                    destruct_fptr(this);
                    call_fptr = rhs.call_fptr;
                    clone_fptr = rhs.clone_fptr;
                    destruct_fptr = rhs.destruct_fptr;
                    callable_ptr = rhs.callable_ptr;
                    rhs.call_fptr = nullptr;
                    rhs.clone_fptr = nullptr;
                    rhs.destruct_fptr = nullptr;
                    rhs.callable_ptr = nullptr;
                }

                return *this;
            }

            Res operator()(Args... args){
                return call_fptr(this, std::forward<Args>(args)...);
            }
        };

    }   //!version_0_2


}   //!stl

#endif  //!__FUNCTIONAL_HPP__