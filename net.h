#ifndef KHAKI_NET_H
#define KHAKI_NET_H

#include "buffer.h"
#include "EventLoop.h"
#include "channel.h"
#include "log.h"

#include <arpa/inet.h>
#include <string.h>
#include <memory>
#include <mutex>

namespace khaki {

	class TcpClient;
	class TimeWheel;
	typedef std::shared_ptr<TcpClient> TcpClientPtr;
	typedef std::function<void(const TcpClientPtr& con)> Callback;

	typedef std::shared_ptr<TimeWheel> TimeWheelPtr;

	class IpAddr {
	public:
		IpAddr(struct sockaddr_in& addr);
		IpAddr(std::string& host, int port);

		~IpAddr();

		std::string getIp() const;
		int getPort() const;
		struct sockaddr_in& getAddr();
	private:
		struct sockaddr_in addr_;
	};

	////////////////////////////////////
	class TcpServer;
	class TcpClient : public std::enable_shared_from_this<TcpClient> {
	public:
		TcpClient( EventLoop* loop, TcpServer* server, std::shared_ptr<TimeWheel>& sp );
		~TcpClient();

		void handleRead(const TcpClientPtr& con) ;
		void handleWrite(const TcpClientPtr& con) ;

		void setReadCallback(const Callback& cb) { readcb_ = cb; }
		void setWriteCallback(const Callback& cb) { writecb_ = cb; }

		void send(char* buf);
		char* getBuf() { return buf; }
		void registerChannel(int fd);
		void closeClient();
		int getFd();
		int getLastTime();
		void updateTimeWheel();

	private:
		EventLoop* loop_;
		TcpServer* server_;
		std::shared_ptr<Channel> channel_;
		std::weak_ptr<TimeWheel> time_wheel_;
		Callback readcb_, writecb_;
		char buf[1024];

		int last_read_time_;

		Buffer readBuf_, writeBuf_;
	};

	////////////////////////////////////
	class TcpServer : public noncopyable {
	public:
		
		TcpServer( EventLoop* loop, std::string host, int port );
		~TcpServer();

		void start();

		EventLoop* getEventLoop() { return loop_; }  
		void handlerRead(const Callback& cb) { readcb_ = cb; }
		void handlerWrite(const Callback& cb) { writecb_ = cb; }

		void send(char* buf);

		int getOnlineNum();
		void addClient(std::shared_ptr<TcpClient>& sp);
		void delClient(int fd);
	private:
		void newConnect( int fd, IpAddr& addr );
		void handleAccept();
		void handleTimeWheel();

		EventLoop* loop_;
		Channel* listen_;
		Channel* time_wheel_;

		IpAddr addr_;
		Callback readcb_, writecb_, newcb_;
		std::shared_ptr<TimeWheel> time_wheel;
		std::mutex mtx_;
		std::map<int, std::weak_ptr<TcpClient>> sSessionList; 
	};
}

#endif