#ifndef CORE_TEMPLATES_FILTER_H_
#define CORE_TEMPLATES_FILTER_H_

#include <vector>

template<class T>
class Filter
{

protected:

    std::vector<T> m_data{};

    virtual void algorithm() {
    };

public:

    virtual ~Filter() = default;

    Filter<T>& operator<<(const std::vector<T>& data)
    {
        m_data = data;

        algorithm();

        return *this;
    }

    operator std::vector<T>() const
    {
        return m_data;
    }
};

#endif /* CORE_TEMPLATES_FILTER_H_ */
