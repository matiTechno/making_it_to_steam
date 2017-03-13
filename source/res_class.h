#ifndef RES_CLASS_H
#define RES_CLASS_H

#include <functional>
#include <assert.h>

// take care of throwing exceptions
// in derived class
template <typename T>
class Res_class
{
public:
    Res_class(std::function<void(T)> deleter):
        was_moved(false),
        deleter(deleter)
    {
        assert(deleter);
    }

    ~Res_class()
    {
        if(!was_moved)
            deleter(id);
    }

    // no copy operations
    Res_class(const Res_class&) = delete;
    Res_class& operator=(const Res_class&) = delete;

    Res_class(Res_class&& rhs):
        id(rhs.id),
        was_moved(false),
        deleter(rhs.deleter)
    {
        rhs.was_moved = true;
    }

    Res_class& operato(Res_class&& rhs)
    {
        if(this == &rhs)
            return *this;

        deleter(id);
        id = rhs.id;
        rhs.was_moved = true;
        return *this;
    }

protected:
    T id;

private:
    bool was_moved;
    std::function<void(T)> deleter;
};

#endif // RES_CLASS_H
