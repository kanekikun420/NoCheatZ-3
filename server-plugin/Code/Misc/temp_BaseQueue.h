#include <list>
#include <string>
//#include <mutex>
//#include <condition_variable>

#ifndef MESSAGEQUEUE
#define MESSAGEQUEUE

template <typename dataT>
class BaseQueue
{
public:
	BaseQueue(){};
	~BaseQueue(){};

	/* Add data to the end of the queue */
	void Push(dataT data)
	{
		//std::unique_lock<std::mutex> lck(m_mtx);
		m_queue.push_back(data);
		//m_cond.notify_all();
	};

	/* Get data on top of the queue and pop it
	   Remember to delete if data is a temporary pointer */
	dataT GetNext(bool kill_ptr = false)
	{
		//std::unique_lock<std::mutex> lck(m_mtx);
		//while(m_queue.size() == 0) m_cond.wait(lck);
		dataT nextData = m_queue.front();
		//m_queue.pop_front();
		return nextData;
	};

	/* Get the size of the queue */
	size_t GetCount()
	{
		//std::unique_lock<std::mutex> lck(m_mtx);
        return m_queue.size();
	};

private:
	std::list<dataT> m_queue;
	//std::mutex m_mtx;
	//std::condition_variable m_cond;
};

#endif
