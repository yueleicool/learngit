#!/usr/bin/python3
#coding=utf-8
import select  
import socket  
  
response = b''  
  
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
serversocket.bind(('0.0.0.0', 8080))  
serversocket.listen(1)  
# 因为socket默认是阻塞的，所以需要使用非阻塞（异步）模式。  
serversocket.setblocking(0)  
  
# 创建一个epoll对象  
epoll = select.epoll()  
# 在服务端socket上面注册对读event的关注。一个读event随时会触发服务端socket去接收一个socket连接  
epoll.register(serversocket.fileno(), select.EPOLLIN)  
  
try:  
    # 字典connections映射文件描述符（整数）到其相应的网络连接对象  
    connections = {}  
    requests = {}  
    responses = {}  
    while True:  
        # 查询epoll对象，看是否有任何关注的event被触发。参数“1”表示，我们会等待1秒来看是否有event发生。  
        # 如果有任何我们感兴趣的event发生在这次查询之前，这个查询就会带着这些event的列表立即返回  
        events = epoll.poll(1)  
        # event作为一个序列（fileno，event code）的元组返回。fileno是文件描述符的代名词，始终是一个整数。  
        for fileno, event in events:  
            # 如果是服务端产生event,表示有一个新的连接进来  
            if fileno == serversocket.fileno():  
                connection, address = serversocket.accept()  
                print('client connected:', address)  
                # 设置新的socket为非阻塞模式  
                connection.setblocking(0)  
                # 为新的socket注册对读（EPOLLIN）event的关注  
                epoll.register(connection.fileno(), select.EPOLLIN)  
                connections[connection.fileno()] = connection  
                # 初始化接收的数据  
                requests[connection.fileno()] = b''  
  
            # 如果发生一个读event，就读取从客户端发送过来的新数据  
            elif event & select.EPOLLIN:  
                print("------recvdata---------")  
                # 接收客户端发送过来的数据  
                requests[fileno] += connections[fileno].recv(1024)  
                # 如果客户端退出,关闭客户端连接，取消所有的读和写监听  
                if not requests[fileno]:  
                    connections[fileno].close()  
                    # 删除connections字典中的监听对象  
                    del connections[fileno]  
                    # 删除接收数据字典对应的句柄对象  
                    del requests[connections[fileno]]  
                    print(connections, requests)  
                    epoll.modify(fileno, 0)  
                else:  
                    # 一旦完成请求已收到，就注销对读event的关注，注册对写（EPOLLOUT）event的关注。写event发生的时候，会回复数据给客户端  
                    epoll.modify(fileno, select.EPOLLOUT)  
                    # 打印完整的请求，证明虽然与客户端的通信是交错进行的，但数据可以作为一个整体来组装和处理  
                    print('-' * 40 + '\n' + requests[fileno].decode())  
  
            # 如果一个写event在一个客户端socket上面发生，它会接受新的数据以便发送到客户端  
            elif event & select.EPOLLOUT:  
                print("-------send data---------")  
                # 每次发送一部分响应数据，直到完整的响应数据都已经发送给操作系统等待传输给客户端  
                byteswritten = connections[fileno].send(requests[fileno])  
                requests[fileno] = requests[fileno][byteswritten:]  
                if len(requests[fileno]) == 0:  
                    # 一旦完整的响应数据发送完成，就不再关注写event  
                    epoll.modify(fileno, select.EPOLLIN)  
  
            # HUP（挂起）event表明客户端socket已经断开（即关闭），所以服务端也需要关闭。  
            # 没有必要注册对HUP event的关注。在socket上面，它们总是会被epoll对象注册  
            elif event & select.EPOLLHUP:  
                print("end hup------")  
                # 注销对此socket连接的关注  
                epoll.unregister(fileno)  
                # 关闭socket连接  
                connections[fileno].close()  
                del connections[fileno]  
finally:  
    # 打开的socket连接不需要关闭，因为Python会在程序结束的时候关闭。这里显式关闭是一个好的代码习惯  
    epoll.unregister(serversocket.fileno())  
    epoll.close()  
    serversocket.close()  
