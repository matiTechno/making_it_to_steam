#ifndef KEY_INPUT_HPP
#define KEY_INPUT_HPP

#include <set>
//#include <queue>

//struct Key_Event
//{
//    enum Action
//    {
//        press,
//        release,
//        repeat
//    };

//    Action action;
//    int key;
//};

class Key_input
{
public:
    //void start_new_frame();

    void press_event(int code/*, bool is_repeat*/);
    // don't call this for 'repeat' action
    void release_event(int code);

    //bool was_pressed(int code) const;
    bool is_pressed(int code) const;
    //bool was_released(int code) const;
    //bool was_repeated(int code) const;
    // returns false if all events were polled
    //bool poll_event(Key_Event& key_event) const;

private:
    //std::set<int> set_was_pressed;
    std::set<int> set_is_pressed;
    //std::set<int> set_was_released;
    //std::set<int> set_was_repeated;
    //mutable std::queue<Key_Event> event_queue;
};

//inline void Key_input::start_new_frame()
//{
//    set_was_pressed.clear();
//    set_was_released.clear();
//    set_was_repeated.clear();
//    event_queue = std::queue<Key_Event>();
//}

inline void Key_input::press_event(int code/*, bool is_repeat*/)
{
    //    if(!is_repeat)
    //    {
    set_is_pressed.emplace(code);
    //set_was_pressed.emplace(code);
    //event_queue.push({Key_Event::press, code});
    //}
    //    else
    //    {
    //set_was_repeated.emplace(code);
    //event_queue.push({Key_Event::repeat, code});
    //    }
}

inline void Key_input::release_event(int code)
{
    set_is_pressed.erase(code);
    //set_was_released.emplace(code);
    //event_queue.push({Key_Event::release, code});
}

//inline bool Key_input::was_pressed(int code) const
//{
//    auto it = set_was_pressed.find(code);
//    if(it == set_was_pressed.end())
//        return false;
//    return true;
//}

inline bool Key_input::is_pressed(int code) const
{
    auto it = set_is_pressed.find(code);
    if(it == set_is_pressed.end())
        return false;
    return true;
}

//inline bool Key_input::was_released(int code) const
//{
//    auto it = set_was_released.find(code);
//    if(it == set_was_released.end())
//        return false;
//    return true;
//}

//inline bool Key_input::was_repeated(int code) const
//{
//    auto it = set_was_repeated.find(code);
//    if(it == set_was_repeated.end())
//        return false;
//    return true;
//}

//inline bool Key_input::poll_event(Key_Event& key_event) const
//{
//    if(event_queue.empty())
//        return false;

//    key_event = event_queue.front();
//    event_queue.pop();
//    return true;
//}

#endif // KEY_INPUT_HPP
