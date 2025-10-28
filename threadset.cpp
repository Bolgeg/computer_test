#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadSet;

class ThreadSetThreadParameters
{
	public:
	ThreadSet*threadSet;
	int threadNumber;
};

void threadLoopFunction(ThreadSetThreadParameters);

class ThreadSet
{
	public:
	int numberOfThreads=0;
	
	vector<std::thread> threads;
	bool threadsShouldWork=false;
	bool threadsShouldStop=false;
	std::condition_variable threadsMutexCondition;
	vector<bool> threadFinished;
	std::condition_variable mainThreadWaitMutexCondition;
	std::mutex threadsMutex;
	
	std::function<void(int,int)> functionToExecute;
	
	void initialize(int _numberOfThreads=0)
	{
		numberOfThreads=_numberOfThreads;
		
		if(numberOfThreads==0)
		{
			numberOfThreads=std::thread::hardware_concurrency();
		}
		
		threads.resize(numberOfThreads);
		threadFinished.resize(numberOfThreads,false);
		for(int i=0;i<threads.size();i++)
		{
			ThreadSetThreadParameters param;
			param.threadSet=this;
			param.threadNumber=i;
			threads[i]=std::thread(threadLoopFunction,param);
		}
	}
	~ThreadSet()
	{
		threadsShouldStop=true;
		threadsMutexCondition.notify_all();
		for(int i=0;i<threads.size();i++)
		{
			threads[i].join();
		}
	}
	void threadLoop(int threadNumber)
	{
		while(true)
		{
			{
				std::unique_lock<std::mutex> lock(threadsMutex);
				threadsMutexCondition.wait(lock,[this,threadNumber]
					{
						return (threadsShouldWork && threadFinished[threadNumber]==false) || threadsShouldStop;
					});
				if(threadsShouldStop)
				{
					return;
				}
			}
			
			functionToExecute(threadNumber,numberOfThreads);
			
			{
				std::unique_lock<std::mutex> lock(threadsMutex);
				threadFinished[threadNumber]=true;
			}
			mainThreadWaitMutexCondition.notify_one();
		}
	}
	void executeInAllThreadsAndWaitToFinish(std::function<void(int,int)> _functionToExecute)
	{
		{
			std::unique_lock<std::mutex> lock(threadsMutex);
			
			functionToExecute=_functionToExecute;
			
			for(int i=0;i<threadFinished.size();i++)
			{
				threadFinished[i]=false;
			}
			threadsShouldWork=true;
		}
		threadsMutexCondition.notify_all();
		{
			std::unique_lock<std::mutex> lock(threadsMutex);
			mainThreadWaitMutexCondition.wait(lock,[this]
				{
					for(int i=0;i<threadFinished.size();i++)
					{
						if(threadFinished[i]==false) return false;
					}
					return true;
				});
		}
		{
			std::unique_lock<std::mutex> lock(threadsMutex);
			threadsShouldWork=false;
			for(int i=0;i<threadFinished.size();i++)
			{
				threadFinished[i]=false;
			}
		}
	}
};

void threadLoopFunction(ThreadSetThreadParameters param)
{
	param.threadSet->threadLoop(param.threadNumber);
}

