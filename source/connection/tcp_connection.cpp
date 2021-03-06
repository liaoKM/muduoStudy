#include"../include/tcp_connection.h"
#include"../include/eventloop.h"
#include"../include/channel.h"
#include"../include/socket.h"
#include<unistd.h>
#include<functional>
namespace muduo
{
    TcpConnection::TcpConnection(EventLoop* loop,int sockfd,const sockaddr_in localAddr,const sockaddr_in peerAddr)
        :loop_(loop),
        state_(kConnecting),
        reading_(true),
        socket_(new Socket(sockfd)),
        channel_(new Channel(loop,sockfd)),
        localAddr_(localAddr),
        peerAddr_(peerAddr),
        highWaterMark_(64*1024*1024)

    {
        //todo bind call back to channel;
        socket_->setKeepAlive(true);
    }
    TcpConnection::~TcpConnection()
    {

    }
    void TcpConnection::send(void* const message, size_t len)
    {
        if(state_==kConnected)
        {
            if(loop_->isInLoopThread())
            {
                sendInLoop(message,len);
            }
            else
            {
                loop_->queueInLoop(std::bind(&TcpConnection::sendInLoop,shared_from_this(),message,len));
            }
        }
    }
    void TcpConnection::send(Buffer* message)
    {
        //todo 
        void* data;
        size_t len;
        message->getContent(data,len);
        send(data,len);
    }
    void TcpConnection::sendInLoop(void* const message, size_t len)
    {
        assert(loop_->isInLoopThread());
        size_t nwrote=0;
        size_t remaining=len;
        bool faultError = false;
        if(state_==kDisconnected)
        {
            //log warning
            return;
        }

        //no thing in output buffer (app level),which means that write buffer in socket is not full
        //try to write socket firstly
        if(outputBuffer_.getUsedSize()==0)
        {
            assert(!channel_->isWriting());
            assert(channel_->fd()==socket_->getFd());
            nwrote=::write(socket_->getFd(),message,len);
            if(nwrote>=0)
            {
                remaining-=nwrote;
                if(remaining==0)
                {
                    //??? push writeCompleteCallback into task queue
                    //why do not call it immediately?
                    writeCompleteCallback_(shared_from_this());
                }
            }
            else//nwrote<0 error happened
            {
                nwrote=0;
                //todo log error
                if(errno!=EWOULDBLOCK)
                {
                    if(errno==EPIPE||errno==ECONNRESET)
                    {
                        faultError=true;
                    }
                }
                
            }
        }

        if(!faultError&&remaining>0)
        {
            //todo check highwatermark
            outputBuffer_.push_back((char*)message+nwrote,remaining);
            if(!channel_->isWriting())//????????????socket_??????????????????????????????
            {
                channel_->enableWriting();
            }
        }


    }

    void TcpConnection::startRead()
    {
        loop_->runInLoop(std::bind(&TcpConnection::startReadInLoop,shared_from_this()));
    }
    void TcpConnection::startReadInLoop()
    {
        assert(loop_->isInLoopThread());
        if(!reading_)
        {
            assert(!channel_->isReading());
            channel_->enableReading();
            reading_=true;
        }
    }
   
    void TcpConnection::stopReadInLoop()
    {
        assert(loop_->isInLoopThread());
        if(reading_)
        {
            assert(channel_->isReading());
            channel_->disableReading();
            reading_=false;
        }
    }
    void TcpConnection::stopRead()
    {
        loop_->runInLoop(std::bind(&TcpConnection::stopReadInLoop,shared_from_this()));
    }
    void TcpConnection::handleError()
    {
        
    }
    void TcpConnection::handleWrite()
    {
        assert(loop_->isInLoopThread());
        if(channel_->isWriting())
        {
            ssize_t len=::write(channel_->fd(),outputBuffer_.getUsedPointer(),outputBuffer_.getUsedSize());
            if(len>0)
            {
                outputBuffer_.pop(len);
                if(outputBuffer_.getUsedSize()==0)
                {
                    channel_->disableWriting();
                    if(writeCompleteCallback_)
                    {
                        loop_->queueInLoop(std::bind(writeCompleteCallback_,shared_from_this()));
                    }
                    if(state_==kDisconnecting)//????????????????????????buffer??????????????????
                    {
                        shutdownInLoop();
                    }
                }
            }
            else
            {
                //log error
            }
        }
        else
        {
            //fd close,no more writing
        }
    }
    void TcpConnection::handleRead(time_t receiveTime)
    {
        assert(loop_->isInLoopThread());
        int readErr;
        ssize_t n = inputBuffer_.readFd(channel_->fd(), readErr);
        if(n>0)
        {
            messageCallback_(shared_from_this(),&inputBuffer_,receiveTime);
        }
        else if(n==0)//????????????,????????????????????????????????????buffer????????????
        {
            handleClose();
        }
        else
        {
            //log readErr
            handleError();
        }
    }
    //todo handleClose handleError
    void TcpConnection::handleClose()
    {
        assert(loop_->isInLoopThread());
        assert(state_==kConnected||state_==kDisconnecting);
        state_=kDisconnected;//??????????????????socket??????????????????this??????
        channel_->disableAll();
        closeCallback_(shared_from_this());
    }

    void TcpConnection::connectEstablished()
    {
        assert(loop_->isInLoopThread());
        assert(state_==kConnecting);
        state_=kConnected;
        channel_->tie(shared_from_this());
        channel_->enableReading();
        if(connectionCallback_)
            connectionCallback_(shared_from_this());
    }
    void TcpConnection::connectDestroyed()
    {
        assert(loop_->isInLoopThread());
        if (state_ == kConnected)
        {
            state_=(kDisconnected);
            channel_->disableAll();
            if(connectionCallback_)
                connectionCallback_(shared_from_this());
        }
    }

    void TcpConnection::shutdown() //?????????????????????????????????????????????
    {
        if (state_ == kConnected)
        {
            state_=kDisconnecting;
            // FIXME: shared_from_this()?
            loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
        }
    } // NOT thread safe, no simultaneous calling
    void TcpConnection::shutdownInLoop() 
    {
        assert(loop_->isInLoopThread());
        if(!channel_->isWriting())//??? ouputbuffer?????????????????????????????????
        {
            socket_->shutdownWrite();
        }
        //?????? push into queue ?????????loop?????????try to shutdown
        //????????????writehandle???(check state_=kDisconnecting)??????shutdownInLoop??????
    }
    void TcpConnection::forceClose(){}
    void TcpConnection::forceCloseWithDelay(double seconds){}
    void TcpConnection::setTcpNoDelay(bool on){}


}