#ifndef REQUESTIDGENERATOR_H
#define REQUESTIDGENERATOR_H
#include <QObject>

using RequestId = quint64;

class RequestIdGenerator
{
public:
    static RequestId next()
    {
        static std::atomic<RequestId> counter{1};
        return counter.fetch_add(1, std::memory_order_relaxed);
    }
    const static RequestId NO_REQUEST_ID = 0;
};
#endif // REQUESTIDGENERATOR_H
