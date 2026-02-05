#include <type_traits>

template<typename F>
concept Callable = std::is_invocable_v<F>;

template<Callable F>
class ScopeExit {
    F m_cb;
public:
    explicit ScopeExit(F cb): m_cb(cb) {}

    ~ScopeExit() {
        this->m_cb();
    }
};